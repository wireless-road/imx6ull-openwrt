// SPDX-License-Identifier: GPL-2.0-only
/*
 * Extra commands for nl80211 interface.
 *
 * Copyright (c) 2020, Silicon Laboratories, Inc.
 */
#include "nl80211_vendor.h"
#include "wfx.h"
#include "sta.h"
#include "hif_tx.h"

int wfx_nl_burn_antirollback(struct wiphy *wiphy, struct wireless_dev *widev,
			     const void *data, int data_len)
{
	struct ieee80211_hw *hw = wiphy_to_ieee80211_hw(wiphy);
	struct wfx_dev *wdev = (struct wfx_dev *)hw->priv;
	struct nlattr *tb[WFX_NL80211_ATTR_MAX];
	u32 magic;
	int rc;

#if (KERNEL_VERSION(4, 12, 0) > LINUX_VERSION_CODE)
	rc = nla_parse(tb, WFX_NL80211_ATTR_MAX - 1, data, data_len,
		       wfx_nl_policy);
#else
	rc = nla_parse(tb, WFX_NL80211_ATTR_MAX - 1, data, data_len,
		       wfx_nl_policy, NULL);
#endif
	if (rc)
		return rc;
	if (!tb[WFX_NL80211_ATTR_ROLLBACK_MAGIC])
		return -EINVAL;
	magic = nla_get_u32(tb[WFX_NL80211_ATTR_ROLLBACK_MAGIC]);
	rc = hif_burn_prevent_rollback(wdev, magic);
	if (rc)
		return -EINVAL;
	return 0;
}

int wfx_nl_pta_params(struct wiphy *wiphy, struct wireless_dev *widev,
		      const void *data, int data_len)
{
	struct ieee80211_hw *hw = wiphy_to_ieee80211_hw(wiphy);
	struct wfx_dev *wdev = (struct wfx_dev *)hw->priv;
	int reply_size = nla_total_size(sizeof(wdev->pta_settings)) +
			 nla_total_size(sizeof(u8)) +
			 nla_total_size(sizeof(u32));
	struct nlattr *tb[WFX_NL80211_ATTR_MAX];
	bool do_enable = false;
	struct sk_buff *msg;
	struct nlattr *nla;
	int rc;

#if (KERNEL_VERSION(4, 12, 0) > LINUX_VERSION_CODE)
	rc = nla_parse(tb, WFX_NL80211_ATTR_MAX - 1, data, data_len,
		       wfx_nl_policy);
#else
	rc = nla_parse(tb, WFX_NL80211_ATTR_MAX - 1, data, data_len,
		       wfx_nl_policy, NULL);
#endif
	if (rc)
		return rc;
	nla = tb[WFX_NL80211_ATTR_PTA_ENABLE];
	if (nla) {
		do_enable = true;
		wdev->pta_enable = nla_get_u8(tb[WFX_NL80211_ATTR_PTA_ENABLE]);
	}
	if (do_enable && !wdev->pta_enable)
		rc = hif_pta_enable(wdev, wdev->pta_enable);
	if (rc)
		return rc;
	nla = tb[WFX_NL80211_ATTR_PTA_SETTINGS];
#if (KERNEL_VERSION(4, 20, 0) > LINUX_VERSION_CODE)
	if (nla && nla_len(nla) != sizeof(wdev->pta_settings))
		return -EINVAL;
#endif
	if (nla) {
		// User has to care about endianness of data it send.
		memcpy(&wdev->pta_settings, nla_data(nla),
		       sizeof(wdev->pta_settings));
		rc = hif_pta_settings(wdev, &wdev->pta_settings);
	}
	if (rc)
		return rc;
	nla = tb[WFX_NL80211_ATTR_PTA_PRIORITY];
	if (nla) {
		wdev->pta_priority =
			nla_get_u32(tb[WFX_NL80211_ATTR_PTA_PRIORITY]);
		rc = hif_pta_priority(wdev, wdev->pta_priority);
	}
	if (rc)
		return rc;
	if (do_enable && wdev->pta_enable)
		rc = hif_pta_enable(wdev, wdev->pta_enable);
	if (rc)
		return rc;

	msg = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, reply_size);
	if (!msg)
		return -ENOMEM;
	rc = nla_put(msg, WFX_NL80211_ATTR_PTA_SETTINGS,
		     sizeof(wdev->pta_settings), &wdev->pta_settings);
	if (rc)
		goto error;
	rc = nla_put_u32(msg, WFX_NL80211_ATTR_PTA_PRIORITY,
			 wdev->pta_priority);
	if (rc)
		goto error;
	rc = nla_put_u8(msg, WFX_NL80211_ATTR_PTA_ENABLE,
			wdev->pta_enable ? 1 : 0);
	if (rc)
		goto error;
	return cfg80211_vendor_cmd_reply(msg);

error:
	kfree_skb(msg);
	return rc;
}

