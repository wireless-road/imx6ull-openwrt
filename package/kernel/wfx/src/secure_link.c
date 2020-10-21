// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2019-2020, Silicon Laboratories, Inc.
 */

#include <linux/of.h>
#include <linux/module.h>
#include <linux/random.h>
#include <crypto/sha.h>
#include <mbedtls/md.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ccm.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>

#include "secure_link.h"
#include "wfx.h"

static char *slk_key;
module_param(slk_key, charp, 0600);
MODULE_PARM_DESC(slk_key, "secret key for secure link (expect 64 hex digits).");

static char *slk_unsecure_cmds = "";
module_param(slk_unsecure_cmds, charp, 0644);
MODULE_PARM_DESC(slk_unsecure_cmds, "list of HIF commands IDs (ie. 4,132-256) that won't be encrypted (default: empty).");

static unsigned int slk_renew_period = BIT(29);
module_param(slk_renew_period, int, 0644);
MODULE_PARM_DESC(slk_renew_period, "number of secure link messages before renewing the key (default: 2^29).");

void mbedtls_platform_zeroize(void *buf, size_t len)
{
	memset(buf, 0, len);
}

static int mbedtls_random(void *data, unsigned char *output, size_t len)
{
	get_random_bytes(output, len);

	return 0;
}

int wfx_is_secure_command(struct wfx_dev *wdev, int cmd_id)
{
	return test_bit(cmd_id, wdev->sl.commands);
}

int wfx_sl_decode(struct wfx_dev *wdev, struct hif_sl_msg *m)
{
	int ret;
	size_t clear_len = le16_to_cpu(m->len);
	size_t payload_len = round_up(clear_len - sizeof(m->len), 16);
	u8 *tag = m->payload + payload_len;
	u8 *output = (u8 *)m;
	u32 nonce[3] = { };

	WARN(m->hdr.encrypted != 0x02, "packet is not encrypted");

	// Other bytes of nonce are 0
	nonce[1] = m->hdr.seqnum;
	if (wdev->sl.rx_seqnum != m->hdr.seqnum)
		dev_warn(wdev->dev, "wrong encrypted message sequence: %d != %d\n",
				m->hdr.seqnum, wdev->sl.rx_seqnum);
	wdev->sl.rx_seqnum = m->hdr.seqnum + 1;
	if (wdev->sl.rx_seqnum == slk_renew_period)
		schedule_work(&wdev->sl.key_renew_work);

	memcpy(output, &m->len, sizeof(m->len));
	ret = mbedtls_ccm_auth_decrypt(&wdev->sl.ccm_ctxt, payload_len,
			(u8 *)nonce, sizeof(nonce), NULL, 0,
			m->payload, output + sizeof(m->len),
			tag, sizeof(struct hif_sl_tag));
	if (ret) {
		dev_err(wdev->dev, "mbedtls error: %08x\n", ret);
		return -EIO;
	}
	if (memzcmp(output + clear_len, payload_len + sizeof(m->len) - clear_len))
		dev_warn(wdev->dev, "padding is not 0\n");
	return 0;
}

int wfx_sl_encode(struct wfx_dev *wdev,
		  const struct hif_msg *input, struct hif_sl_msg *output)
{
	int payload_len =
		round_up(le16_to_cpu(input->len) - sizeof(input->len), 16);
	u8 *tag = output->payload + payload_len;
	u32 nonce[3] = { };
	int ret;

	output->hdr.encrypted = 0x1;
	output->len = input->len;
	output->hdr.seqnum = wdev->sl.tx_seqnum;
	// Other bytes of nonce are 0
	nonce[2] = wdev->sl.tx_seqnum;
	wdev->sl.tx_seqnum++;
	if (wdev->sl.tx_seqnum == slk_renew_period)
		schedule_work(&wdev->sl.key_renew_work);

	ret = mbedtls_ccm_encrypt_and_tag(&wdev->sl.ccm_ctxt, payload_len,
			(u8 *)nonce, sizeof(nonce), NULL, 0,
			(u8 *)input + sizeof(input->len), output->payload,
			tag, sizeof(struct hif_sl_tag));
	if (ret) {
		dev_err(wdev->dev, "mbedtls error: %08x\n", ret);
		return -EIO;
	}
	return 0;
}

static int wfx_sl_get_pubkey_mac(struct wfx_dev *wdev,
				 const u8 *pubkey, u8 *mac)
{
	return mbedtls_md_hmac(
			mbedtls_md_info_from_type(MBEDTLS_MD_SHA512),
			wdev->pdata.slk_key, sizeof(wdev->pdata.slk_key),
			pubkey, API_HOST_PUB_KEY_SIZE,
			mac);
}

int wfx_sl_check_pubkey(struct wfx_dev *wdev,
			const u8 *pubkey_orig, const u8 *mac)
{
	int ret;
	size_t olen;
	u8 pubkey[API_NCP_PUB_KEY_SIZE];
	u8 secret[API_HOST_PUB_KEY_SIZE];
	u8 secret_digest[SHA256_DIGEST_SIZE];
	u8 expected_mac[SHA512_DIGEST_SIZE];

	memcpy(pubkey, pubkey_orig, sizeof(pubkey));
	ret = wfx_sl_get_pubkey_mac(wdev, pubkey, expected_mac);
	if (ret)
		goto end;
	ret = memcmp(expected_mac, mac, sizeof(expected_mac));
	if (ret)
		goto end;

	// FIXME: save Qp.Y or (reset it), concat it with ncp_public_key and
	// use mbedtls_ecdh_read_public.
	memreverse(pubkey, sizeof(pubkey));
	ret = mbedtls_mpi_read_binary(&wdev->sl.edch_ctxt.Qp.X, pubkey, API_NCP_PUB_KEY_SIZE);
	if (ret)
		goto end;
	ret = mbedtls_mpi_lset(&wdev->sl.edch_ctxt.Qp.Z, 1);
	if (ret)
		goto end;

	ret = mbedtls_ecdh_calc_secret(&wdev->sl.edch_ctxt, &olen,
			secret, sizeof(secret),
			mbedtls_random, NULL);
	if (ret)
		goto end;

	memreverse(secret, sizeof(secret));
	ret = mbedtls_sha256_ret(secret, sizeof(secret), secret_digest, 0);
	if (ret)
		goto end;

	wdev->sl.rx_seqnum = 0;
	wdev->sl.tx_seqnum = 0;
	mbedtls_ccm_free(&wdev->sl.ccm_ctxt);
	// Use the lower 16 bytes of the sha256 of the secret for AES key
	ret = mbedtls_ccm_setkey(&wdev->sl.ccm_ctxt, MBEDTLS_CIPHER_ID_AES,
			secret_digest, 16 * BITS_PER_BYTE);

end:
	complete(&wdev->sl.key_renew_done);
	return 0;
}

static int wfx_sl_key_exchange(struct wfx_dev *wdev)
{
	int ret;
	size_t olen;
	u8 mac[SHA512_DIGEST_SIZE];
	u8 pubkey[API_HOST_PUB_KEY_SIZE + 2];

	mbedtls_ecdh_init(&wdev->sl.edch_ctxt);
	ret = mbedtls_ecdh_setup(&wdev->sl.edch_ctxt, MBEDTLS_ECP_DP_CURVE25519);
	if (ret)
		goto err;
	wdev->sl.edch_ctxt.point_format = MBEDTLS_ECP_PF_COMPRESSED;
	ret = mbedtls_ecdh_make_public(&wdev->sl.edch_ctxt, &olen,
				       pubkey, sizeof(pubkey),
				       mbedtls_random, NULL);
	if (ret || olen != sizeof(pubkey))
		goto err;
	memreverse(pubkey + 2, sizeof(pubkey) - 2);
	ret = wfx_sl_get_pubkey_mac(wdev, pubkey + 2, mac);
	if (ret)
		goto err;
	ret = hif_sl_send_pub_keys(wdev, pubkey + 2, mac);
	if (ret)
		goto err;
	if (wdev->poll_irq)
		wfx_bh_poll_irq(wdev);
	if (!wait_for_completion_timeout(&wdev->sl.key_renew_done, msecs_to_jiffies(500)))
		goto err;
	if (!memzcmp(&wdev->sl.ccm_ctxt, sizeof(wdev->sl.ccm_ctxt)))
		goto err;

	mbedtls_ecdh_free(&wdev->sl.edch_ctxt);
	return 0;
err:
	mbedtls_ecdh_free(&wdev->sl.edch_ctxt);
	dev_err(wdev->dev, "key negociation error\n");
	return -EIO;
}

static void wfx_sl_renew_key(struct work_struct *work)
{
	struct wfx_dev *wdev = container_of(work, struct wfx_dev, sl.key_renew_work);

	wfx_tx_lock_flush(wdev);
	mutex_lock(&wdev->hif_cmd.key_renew_lock);
	wfx_sl_key_exchange(wdev);
	mutex_unlock(&wdev->hif_cmd.key_renew_lock);
	wfx_tx_unlock(wdev);
}

static void wfx_sl_init_cfg(struct wfx_dev *wdev)
{
	DECLARE_BITMAP(sl_commands, 256);

	if (bitmap_parselist(slk_unsecure_cmds, sl_commands, 256)) {
		dev_err(wdev->dev, "ignoring malformatted list of unencrypted commands: %s\n",
			slk_unsecure_cmds);
		bitmap_zero(sl_commands, 256);
	}
	bitmap_complement(sl_commands, sl_commands, 256);
	clear_bit(HIF_CNF_ID_SET_SL_MAC_KEY, sl_commands);
	clear_bit(HIF_CNF_ID_SL_EXCHANGE_PUB_KEYS, sl_commands);
	clear_bit(HIF_IND_ID_SL_EXCHANGE_PUB_KEYS, sl_commands);
	clear_bit(HIF_IND_ID_EXCEPTION, sl_commands);
	clear_bit(HIF_IND_ID_ERROR, sl_commands);
	clear_bit(HIF_IND_ID_RX, sl_commands);
	clear_bit(HIF_CNF_ID_TX, sl_commands);
	clear_bit(HIF_CNF_ID_MULTI_TRANSMIT, sl_commands);
	hif_sl_config(wdev, sl_commands);
	bitmap_copy(wdev->sl.commands, sl_commands, 256);
}

int wfx_sl_init(struct wfx_dev *wdev)
{
	INIT_WORK(&wdev->sl.key_renew_work, wfx_sl_renew_key);
	init_completion(&wdev->sl.key_renew_done);
	if (!memzcmp(wdev->pdata.slk_key, sizeof(wdev->pdata.slk_key)))
		return -EIO;
	if (wfx_api_older_than(wdev, 2, 0)) {
		dev_info(wdev->dev, "this driver only support secure link API >= 2.0\n");
		return -EIO;
	}
	if (wdev->hw_caps.link_mode == SEC_LINK_ENFORCED) {
		bitmap_set(wdev->sl.commands, HIF_REQ_ID_SL_CONFIGURE, 1);
		if (wfx_sl_key_exchange(wdev))
			return -EIO;
		wfx_sl_init_cfg(wdev);
	} else if (wdev->hw_caps.link_mode == SEC_LINK_EVAL) {
		if (hif_sl_set_mac_key(wdev, wdev->pdata.slk_key, SL_MAC_KEY_DEST_RAM))
			return -EIO;
		if (wfx_sl_key_exchange(wdev))
			return -EIO;
		wfx_sl_init_cfg(wdev);
	} else {
		dev_info(wdev->dev, "ignoring provided secure link key since chip does not support it\n");
	}
	return 0;
}

void wfx_sl_deinit(struct wfx_dev *wdev)
{
	mbedtls_ccm_free(&wdev->sl.ccm_ctxt);
}

void wfx_sl_fill_pdata(struct device *dev, struct wfx_platform_data *pdata)
{
	const char *ascii_key = NULL;
	int ret = 0;

	if (slk_key)
		ascii_key = slk_key;
	if (!ascii_key)
		ret = of_property_read_string(dev->of_node, "slk_key", &ascii_key);
	if (ret == -EILSEQ || ret == -ENODATA)
		dev_err(dev, "ignoring malformatted key from DT\n");
	if (!ascii_key)
		return;

	ret = hex2bin(pdata->slk_key, ascii_key, sizeof(pdata->slk_key));
	if (ret) {
		dev_err(dev, "ignoring malformatted key: %s\n", ascii_key);
		memset(pdata->slk_key, 0, sizeof(pdata->slk_key));
		return;
	}
}
