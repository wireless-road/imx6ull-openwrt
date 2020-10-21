/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Extra commands for nl80211 interface.
 *
 * Copyright (c) 2020, Silicon Laboratories, Inc.
 */
#ifndef WFX_NL80211_VENDOR_H
#define WFX_NL80211_VENDOR_H

#include <linux/version.h>
#include <net/netlink.h>
#include <net/cfg80211.h>

#include "hif_api_general.h"

#if (KERNEL_VERSION(4, 20, 0) > LINUX_VERSION_CODE)

#define __NLA_ENSURE(condition) BUILD_BUG_ON_ZERO(!(condition))

#define NLA_ENSURE_INT_TYPE(tp)				\
	(__NLA_ENSURE(tp == NLA_S8 || tp == NLA_U8 ||	\
		      tp == NLA_S16 || tp == NLA_U16 ||	\
		      tp == NLA_S32 || tp == NLA_U32 ||	\
		      tp == NLA_S64 || tp == NLA_U64) + tp)

#define NLA_POLICY_EXACT_LEN(_len)	{ .type = NLA_BINARY }

#define NLA_POLICY_RANGE(tp, _min, _max) {		\
	.type = NLA_ENSURE_INT_TYPE(tp),		\
}

#define NLA_POLICY_MAX(tp, _max) {			\
	.type = NLA_ENSURE_INT_TYPE(tp),		\
}

#endif

#define WFX_NL80211_ID 0x90fd9f

int wfx_nl_burn_antirollback(struct wiphy *wiphy, struct wireless_dev *widev,
			     const void *data, int data_len);
int wfx_nl_pta_params(struct wiphy *wiphy, struct wireless_dev *widev,
		      const void *data, int data_len);

enum {
	WFX_NL80211_SUBCMD_BURN_PREVENT_ROLLBACK        = 0x20,
	WFX_NL80211_SUBCMD_BURN_PREVENT_ROLLBACK_COMPAT = 0x21,
	WFX_NL80211_SUBCMD_PTA_PARMS                    = 0x30,
	WFX_NL80211_SUBCMD_PTA_PARMS_COMPAT             = 0x31,
};

enum {
	WFX_NL80211_ATTR_ROLLBACK_MAGIC = 2,
	WFX_NL80211_ATTR_PTA_ENABLE     = 3,
	WFX_NL80211_ATTR_PTA_SETTINGS   = 4,
	WFX_NL80211_ATTR_PTA_PRIORITY   = 5,
	WFX_NL80211_ATTR_MAX
};

static const struct nla_policy wfx_nl_policy[WFX_NL80211_ATTR_MAX] = {
	[WFX_NL80211_ATTR_ROLLBACK_MAGIC] = { .type = NLA_U32 },
	[WFX_NL80211_ATTR_PTA_ENABLE]     = NLA_POLICY_MAX(NLA_U8, 1),
	[WFX_NL80211_ATTR_PTA_PRIORITY]   = { .type = NLA_U32 },
	[WFX_NL80211_ATTR_PTA_SETTINGS]   =
		NLA_POLICY_EXACT_LEN(sizeof(struct hif_req_pta_settings)),
};

#if (KERNEL_VERSION(4, 20, 0) > LINUX_VERSION_CODE)
static const struct wiphy_vendor_command wfx_nl80211_vendor_commands[] = {
	{
		.info.vendor_id = WFX_NL80211_ID,
		.info.subcmd = WFX_NL80211_SUBCMD_BURN_PREVENT_ROLLBACK_COMPAT,
		.doit = wfx_nl_burn_antirollback,
	}, {
		.info.vendor_id = WFX_NL80211_ID,
		.info.subcmd = WFX_NL80211_SUBCMD_PTA_PARMS_COMPAT,
		.doit = wfx_nl_pta_params,
	},
};
#else
static const struct wiphy_vendor_command wfx_nl80211_vendor_commands[] = {
	{
		.info.vendor_id = WFX_NL80211_ID,
		.info.subcmd = WFX_NL80211_SUBCMD_BURN_PREVENT_ROLLBACK,
		.policy = wfx_nl_policy,
		.doit = wfx_nl_burn_antirollback,
		.maxattr = WFX_NL80211_ATTR_MAX - 1,
	}, {
		// Compat with iw
		.info.vendor_id = WFX_NL80211_ID,
		.info.subcmd = WFX_NL80211_SUBCMD_BURN_PREVENT_ROLLBACK_COMPAT,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = wfx_nl_burn_antirollback,
	}, {
		.info.vendor_id = WFX_NL80211_ID,
		.info.subcmd = WFX_NL80211_SUBCMD_PTA_PARMS,
		.policy = wfx_nl_policy,
		.doit = wfx_nl_pta_params,
		.maxattr = WFX_NL80211_ATTR_MAX - 1,
	}, {
		// Compat with iw
		.info.vendor_id = WFX_NL80211_ID,
		.info.subcmd = WFX_NL80211_SUBCMD_PTA_PARMS_COMPAT,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = wfx_nl_pta_params,
	},
};
#endif

#endif /* WFX_NL80211_VENDOR_H */
