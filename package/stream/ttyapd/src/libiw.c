
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <iwinfo.h>

#include "main.h"

typedef struct libiw_t_{
	unsigned char				ifname[512];
	const struct iwinfo_ops		*iwops;
}libiw_t;

static char * format_bssid(unsigned char *mac)
{
	static char buf[18];

	snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return buf;
}
static char * format_essid(char *ssid)
{
	static char buf[IWINFO_ESSID_MAX_SIZE+3];

	if (ssid && ssid[0])
		snprintf(buf, sizeof(buf), "%s", ssid);
	else
		snprintf(buf, sizeof(buf), "unknown");

	return buf;
}
static char * format_int(int num, char check_zero)
{
	static char buf[14];

	if (check_zero && num == 0)
		snprintf(buf, sizeof(buf), "unknown");
	else
		snprintf(buf, sizeof(buf), "%d", num);

	return buf;
}


static char * format_enc_ciphers(int ciphers)
{
	static char str[128] = { 0 };
	char *pos = str;

	if (ciphers & IWINFO_CIPHER_WEP40)
		pos += sprintf(pos, "WEP-40, ");

	if (ciphers & IWINFO_CIPHER_WEP104)
		pos += sprintf(pos, "WEP-104, ");

	if (ciphers & IWINFO_CIPHER_TKIP)
		pos += sprintf(pos, "tkip+");

	if (ciphers & IWINFO_CIPHER_CCMP)
		pos += sprintf(pos, "ccmp+");

	if (ciphers & IWINFO_CIPHER_WRAP)
		pos += sprintf(pos, "wrap+, ");

	if (ciphers & IWINFO_CIPHER_AESOCB)
		pos += sprintf(pos, "aes+, ");

	if (ciphers & IWINFO_CIPHER_CKIP)
		pos += sprintf(pos, "ckip+");

	if (!ciphers || (ciphers & IWINFO_CIPHER_NONE))
		pos += sprintf(pos, "none, ");

	//*(pos - 2) = 0;
	*(pos - 1) = 0;

	return str;
}

static char * format_enc_suites(int suites)
{
	static char str[64] = { 0 };
	char *pos = str;

	if (suites & IWINFO_KMGMT_PSK)
		pos += sprintf(pos, "PSK/");

	if (suites & IWINFO_KMGMT_8021x)
		pos += sprintf(pos, "802.1X/");

	if (!suites || (suites & IWINFO_KMGMT_NONE))
		pos += sprintf(pos, "NONE/");

	*(pos - 1) = 0;

	return str;
}

static char * format_encryption(struct iwinfo_crypto_entry *c)
{
	static char buf[512];

	if (!c)
	{
		snprintf(buf, sizeof(buf), "unknown");
	}
	else if (c->enabled)
	{
		/* WEP */
		if (c->auth_algs && !c->wpa_version)
		{
			if ((c->auth_algs & IWINFO_AUTH_OPEN) &&
				(c->auth_algs & IWINFO_AUTH_SHARED))
			{
				snprintf(buf, sizeof(buf), "wep (%s)",
					format_enc_ciphers(c->pair_ciphers));
			}
			else if (c->auth_algs & IWINFO_AUTH_OPEN)
			{
				snprintf(buf, sizeof(buf), "wep+open (%s)",
					format_enc_ciphers(c->pair_ciphers));
			}
			else if (c->auth_algs & IWINFO_AUTH_SHARED)
			{
				snprintf(buf, sizeof(buf), "wep+shared (%s)",
					format_enc_ciphers(c->pair_ciphers));
			}
		}

		/* WPA */
		else if (c->wpa_version)
		{
			switch (c->wpa_version) {
				case 3:
					
					//snprintf(buf, sizeof(buf), "psk-mixed+%s (%s)",
					//	format_enc_suites(c->auth_suites),
					//	format_enc_ciphers(c->pair_ciphers | c->group_ciphers));
					snprintf(buf, sizeof(buf), "psk-mixed+%s",
						format_enc_ciphers(c->pair_ciphers | c->group_ciphers));
					break;

				case 2:
					//snprintf(buf, sizeof(buf), "psk2 %s (%s)",
					//	format_enc_suites(c->auth_suites),
					//	format_enc_ciphers(c->pair_ciphers | c->group_ciphers));
					snprintf(buf, sizeof(buf), "psk2+%s",
						format_enc_ciphers(c->pair_ciphers | c->group_ciphers));
					break;

				case 1:
					//snprintf(buf, sizeof(buf), "psk %s (%s)",
					//	format_enc_suites(c->auth_suites),
					//	format_enc_ciphers(c->pair_ciphers | c->group_ciphers));
					snprintf(buf, sizeof(buf), "psk+%s",
						format_enc_ciphers(c->pair_ciphers | c->group_ciphers));
					break;
			}
		}
		else
		{
			snprintf(buf, sizeof(buf), "none");
		}
	}
	else
	{
		snprintf(buf, sizeof(buf), "none");
	}

	return buf;
}

void libiw_add_strpair_json(json_object *jobj, unsigned char *key, unsigned char *value){
	json_object *jstr = json_object_new_string(value);
	json_object_object_add(jobj, key, jstr);
}

void libiw_add_result_json(json_object *jobj, unsigned char *str){
	json_object *jstr = json_object_new_string(str);
	json_object_object_add(jobj, "result", jstr);
}

void libiw_status(void *iwptr, unsigned char **retbuf){
	int len, ch;
	char buf[IWINFO_BUFSIZE];
	char essid[IWINFO_ESSID_MAX_SIZE+1] = { 0 };
	struct iwinfo_assoclist_entry *e;
	const char *str;
	libiw_t *iw = (libiw_t *)iwptr;
	json_object *jobj = json_object_new_object();
	
	if (iw->iwops->ssid(iw->ifname, essid))
		memset(essid, 0, sizeof(essid));
	
	if (iw->iwops->channel(iw->ifname, &ch))
		ch = 0;
	
	if (iw->iwops->assoclist(iw->ifname, buf, &len)) {
		libiw_add_result_json(jobj, "no information available");
		goto exit;
	} else if (len <= 0) {
		libiw_add_result_json(jobj, "not connected");
		goto exit;
	}
	
	libiw_add_result_json(jobj, "connected");
	
	/* On MT7601u we have only station mode so only one associated remote host. */
	e = (struct iwinfo_assoclist_entry *) &buf[0];

	libiw_add_strpair_json(jobj, "bssid", format_bssid(e->mac));
	libiw_add_strpair_json(jobj, "essid", format_essid(essid));
	libiw_add_strpair_json(jobj, "channel", format_int(ch, 1));
	libiw_add_strpair_json(jobj, "signal", format_int(e->signal, 1));
	libiw_add_strpair_json(jobj, "noise", format_int(e->noise, 1));
	libiw_add_strpair_json(jobj, "snr", format_int(e->signal - e->noise, 0));
	libiw_add_strpair_json(jobj, "last_active_ms", format_int(e->inactive, 0));
	libiw_add_strpair_json(jobj, "rx_rate_kbit", format_int(e->rx_rate.rate, 1));
	libiw_add_strpair_json(jobj, "tx_rate_kbit", format_int(e->tx_rate.rate, 1));
	libiw_add_strpair_json(jobj, "rx_packets", format_int(e->rx_packets, 0));
	libiw_add_strpair_json(jobj, "tx_packets", format_int(e->tx_packets, 0));

exit:
	str = json_object_to_json_string(jobj);
	*retbuf = calloc(strlen(str) + 1, sizeof(unsigned char));
	strcpy(*retbuf, str);
	json_object_put(jobj);
}

void libiw_scan(void *iwptr, unsigned char **retbuf){
	int i, x, len;
	char buf[IWINFO_BUFSIZE] = { 0 };
	struct iwinfo_scanlist_entry *e;
	const char *str;
	libiw_t *iw = (libiw_t *)iwptr;
	json_object *jobj = json_object_new_object();
	json_object *jstr;
	json_object *jarr, *jarr_element;
	
	if (iw->iwops->scanlist(iw->ifname, buf, &len))
	{
		libiw_add_result_json(jobj, "scan not possible");
		goto exit;
	}
	else if (len <= 0)
	{
		libiw_add_result_json(jobj, "no scan results");
		goto exit;
	}
	libiw_add_result_json(jobj, "ok");
	
	jarr = json_object_new_array();
	for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_scanlist_entry), x++)
	{
		e = (struct iwinfo_scanlist_entry *) &buf[i];
		
		/* We skip adhoc, p2p and other network types, we can connect only to AccessPoints in STA mode.*/
		if(e->mode != IWINFO_OPMODE_MASTER)
			continue;
		
		jarr_element = json_object_new_object();
		
		libiw_add_strpair_json(jarr_element, "idx", format_int(x, 0));
		libiw_add_strpair_json(jarr_element, "bssid", format_bssid(e->mac));
		libiw_add_strpair_json(jarr_element, "essid", format_essid(e->ssid));
		libiw_add_strpair_json(jarr_element, "mode", "AccessPoint");
		libiw_add_strpair_json(jarr_element, "channel", format_int(e->channel, 1));
		libiw_add_strpair_json(jarr_element, "signal", format_int(e->signal - 0x100, 0));
		libiw_add_strpair_json(jarr_element, "quality", format_int(e->quality, 1));
		libiw_add_strpair_json(jarr_element, "quality_max", format_int(e->quality_max, 1));
		libiw_add_strpair_json(jarr_element, "encryption", format_encryption(&e->crypto));
		
		json_object_array_add(jarr, jarr_element);

	}
	jstr = json_object_new_string(format_int(x - 1, 0));
	json_object_object_add(jobj, "num_elems", jstr);
	
	json_object_object_add(jobj, "scan_data", jarr);
	
exit:
	str = json_object_to_json_string(jobj);
	*retbuf = calloc(strlen(str) + 1, sizeof(unsigned char));
	strcpy(*retbuf, str);
	json_object_put(jobj);
}

int libiw_init(unsigned char *wlandev, void **iwptr){
	const struct iwinfo_ops *iwops;
	libiw_t	*iw;
	
	iwops = iwinfo_backend(wlandev);
	
	if(!iwops)
		return -1;
	
	iw = calloc(1, sizeof(libiw_t));
	strcpy(iw->ifname, wlandev);
	iw->iwops = iwops;
	
	*iwptr = (void *)iw;
	
	return 0;
}

void libiw_end(void **iwptr){
	m_free(*iwptr);
	iwinfo_finish();
	
}
