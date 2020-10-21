/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2019-2020, Silicon Laboratories, Inc.
 */
#ifndef WFX_SECURE_LINK_H
#define WFX_SECURE_LINK_H

#include "hif_api_general.h"

struct wfx_dev;

#ifdef CONFIG_WFX_SECURE_LINK

#include <linux/bitmap.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ccm.h>

struct wfx_platform_data;

struct sl_context {
	unsigned int         rx_seqnum;
	unsigned int         tx_seqnum;
	struct completion    key_renew_done;
	struct work_struct   key_renew_work;
	DECLARE_BITMAP(commands, 256);
	mbedtls_ecdh_context edch_ctxt; // Only valid druing key negociation
	mbedtls_ccm_context  ccm_ctxt;
};

int wfx_is_secure_command(struct wfx_dev *wdev, int cmd_id);
int wfx_sl_decode(struct wfx_dev *wdev, struct hif_sl_msg *m);
int wfx_sl_encode(struct wfx_dev *wdev,
		  const struct hif_msg *input, struct hif_sl_msg *output);
int wfx_sl_check_pubkey(struct wfx_dev *wdev,
			const u8 *ncp_pubkey, const u8 *ncp_pubmac);
int wfx_sl_init(struct wfx_dev *wdev);
void wfx_sl_deinit(struct wfx_dev *wdev);
void wfx_sl_fill_pdata(struct device *dev, struct wfx_platform_data *pdata);

#else /* CONFIG_WFX_SECURE_LINK */

#include <linux/of.h>

struct sl_context {
};

static inline bool wfx_is_secure_command(struct wfx_dev *wdev, int cmd_id)
{
	return false;
}

static inline int wfx_sl_decode(struct wfx_dev *wdev, struct hif_sl_msg *m)
{
	return -EIO;
}

static inline int wfx_sl_encode(struct wfx_dev *wdev,
				const struct hif_msg *input,
				struct hif_sl_msg *output)
{
	return -EIO;
}

static inline int wfx_sl_check_pubkey(struct wfx_dev *wdev,
				      const u8 *ncp_pubkey,
				      const u8 *ncp_pubmac)
{
	return -EIO;
}

static inline void wfx_sl_fill_pdata(struct device *dev,
				     struct wfx_platform_data *pdata)
{
	if (of_find_property(dev->of_node, "slk_key", NULL))
		dev_err(dev, "secure link is not supported by this driver, ignoring provided key\n");
}

static inline int wfx_sl_init(struct wfx_dev *wdev)
{
	return -EIO;
}

static inline void wfx_sl_deinit(struct wfx_dev *wdev)
{
}

#endif /* CONFIG_WFX_SECURE_LINK */

#endif
