#pragma once

enum {
    NETLINK_PROXY_ATTR_UNSPEC,
    __NETLINK_PROXY_ATTR_MAX,
};
#define NETLINK_PROXY_ATTR_MAX (__NETLINK_PROXY_ATTR_MAX)

enum {
    NETLINK_PROXY_CMD_UNSPEC,
    NETLINK_PROXY_CMD_BROADCAST,
    __NETLINK_PROXY_CMD_MAX,
};
#define NETLINK_PROXY_CMD_MAX (__NETLINK_PROXY_CMD_MAX)
