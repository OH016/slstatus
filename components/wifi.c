/* See LICENSE file for copyright and license details. */
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <linux/nl80211.h>

#include "../slstatus.h"
#include "../util.h"

#define RSSI_TO_PERC(rssi) \
			rssi >= -50 ? 100 : \
			(rssi <= -100 ? 0 : \
			(2 * (rssi + 100)))

static int nlsock = -1;
static uint32_t seq = 1;
static char resp[4096];

static char *
findattr(int attr, const char *p, const char *e, size_t *len)
{
	while (p < e) {
		struct nlattr nla;
		memcpy(&nla, p, sizeof(nla));
		if (nla.nla_type == attr) {
			*len = nla.nla_len - NLA_HDRLEN;
			return (char *)(p + NLA_HDRLEN);
		}
		p += NLA_ALIGN(nla.nla_len);
	}
	return NULL;
}

static uint16_t
nl80211fam(void)
{
	static const char family[] = "nl80211";
	static uint16_t id;
	ssize_t r;
	size_t len;
	char ctrl[NLMSG_HDRLEN+GENL_HDRLEN+NLA_HDRLEN+NLA_ALIGN(sizeof(family))] = {0}, *p = ctrl;

	if (id)
		return id;

	memcpy(p, &(struct nlmsghdr){
		.nlmsg_len = sizeof(ctrl),
		.nlmsg_type = GENL_ID_CTRL,
		.nlmsg_flags = NLM_F_REQUEST,
		.nlmsg_seq = seq++,
		.nlmsg_pid = 0,
	}, sizeof(struct nlmsghdr));
	p += NLMSG_HDRLEN;
	memcpy(p, &(struct genlmsghdr){
		.cmd = CTRL_CMD_GETFAMILY,
		.version = 1,
	}, sizeof(struct genlmsghdr));
	p += GENL_HDRLEN;
	memcpy(p, &(struct nlattr){
		.nla_len = NLA_HDRLEN+sizeof(family),
		.nla_type = CTRL_ATTR_FAMILY_NAME,
	}, sizeof(struct nlattr));
	p += NLA_HDRLEN;
	memcpy(p, family, sizeof(family));

	if (nlsock < 0)
		nlsock = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
	if (nlsock < 0) {
		warn("socket 'AF_NETLINK':");
		return 0;
	}
	if (send(nlsock, ctrl, sizeof(ctrl), 0) != sizeof(ctrl)) {
		warn("send 'AF_NETLINK':");
		return 0;
	}
	r = recv(nlsock, resp, sizeof(resp), 0);
	if (r < 0) {
		warn("recv 'AF_NETLINK':");
		return 0;
	}
	if ((size_t)r <= sizeof(ctrl))
		return 0;
	p = findattr(CTRL_ATTR_FAMILY_ID, resp + sizeof(ctrl), resp + r, &len);
	if (p && len == 2)
		memcpy(&id, p, 2);

	return id;
}

static int
ifindex(const char *interface)
{
	static struct ifreq ifr;
	static int ifsock = -1;

	if (ifsock < 0)
		ifsock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (ifsock < 0) {
		warn("socket 'AF_UNIX':");
		return -1;
	}
	if (strcmp(ifr.ifr_name, interface) != 0) {
		strcpy(ifr.ifr_name, interface);
		if (ioctl(ifsock, SIOCGIFINDEX, &ifr) != 0) {
			warn("ioctl 'SIOCGIFINDEX':");
			return -1;
		}
	}
	return ifr.ifr_ifindex;
}

const char *
wifi_essid(const char *interface)
{
	uint16_t fam = nl80211fam();
	ssize_t r;
	size_t len;
	char req[NLMSG_HDRLEN+GENL_HDRLEN+NLA_HDRLEN+NLA_ALIGN(4)] = {0}, *p = req;
	int idx = ifindex(interface);
	if (!fam) {
		fprintf(stderr, "nl80211 family not found\n");
		return NULL;
	}
	if (idx < 0) {
		fprintf(stderr, "interface %s not found\n", interface);
		return NULL;
	}

	memcpy(p, &(struct nlmsghdr){
		.nlmsg_len = sizeof(req),
		.nlmsg_type = fam,
		.nlmsg_flags = NLM_F_REQUEST,
		.nlmsg_seq = seq++,
		.nlmsg_pid = 0,
	}, sizeof(struct nlmsghdr));
	p += NLMSG_HDRLEN;
	memcpy(p, &(struct genlmsghdr){
		.cmd = NL80211_CMD_GET_INTERFACE,
		.version = 1,
	}, sizeof(struct genlmsghdr));
	p += GENL_HDRLEN;
	memcpy(p, &(struct nlattr){
		.nla_len = NLA_HDRLEN+4,
		.nla_type = NL80211_ATTR_IFINDEX,
	}, sizeof(struct nlattr));
	p += NLA_HDRLEN;
	memcpy(p, &(uint32_t){idx}, 4);

	if (send(nlsock, req, sizeof(req), 0) != sizeof(req)) {
		warn("send 'AF_NETLINK':");
		return NULL;
	}
	r = recv(nlsock, resp, sizeof(resp), 0);
	if (r < 0) {
		warn("recv 'AF_NETLINK':");
		return NULL;
	}

	if ((size_t)r <= NLMSG_HDRLEN + GENL_HDRLEN)
		return NULL;
	p = findattr(NL80211_ATTR_SSID, resp + NLMSG_HDRLEN + GENL_HDRLEN, resp + r, &len);
	if (p)
		p[len] = 0;

	return p;
}

