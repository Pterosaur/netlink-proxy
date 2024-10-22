#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include <linux/string.h>

#include <netlink_proxy.h>


static struct genl_family netlink_proxy_family;

static int broadcast(struct genl_info *info, const void *payload, int payload_size) {
    struct sk_buff *skb_out = NULL;
    int ret = 0;

    // Allocate new skb
    skb_out = nlmsg_new(payload_size, GFP_KERNEL);
    if (!skb_out) {
        pr_err("Failed to allocate new skb\n");
        return -ENOMEM;
    }

    // Input the payload without header
    ret = nla_put_nohdr(skb_out, payload_size, payload);
    if (ret) {
        pr_err("Failed to add payload to the message\n");
        nlmsg_free(skb_out);
        return ret;
    }

    // Broadcast the message
    ret = genlmsg_multicast(&netlink_proxy_family, skb_out, 0, 0, GFP_KERNEL);

    if (ret == -ESRCH)
    {
        pr_warn("multicast message sent, but nobody was listening...\n");
    }
    else if (ret)
    {
        pr_err("failed to send multicast genl message\n");
    }

    pr_debug("Broadcast message to Pid=%u PortId=%u MsgId = %u\n", info->nlhdr->nlmsg_pid, info->snd_portid, info->snd_seq);

    return ret;
}

static int broadcast_handler(struct sk_buff *skb, struct genl_info *info) {
    struct nlmsghdr *nlh = NULL;
    int ret = 0;

    nlh = nlmsg_hdr(skb);

    ret = broadcast(info, nlmsg_data(nlh), nlmsg_len(nlh));
    if (ret < 0) {
        printk(KERN_ERR "Failed to send message to user-space\n");
        return ret;
    }

    return 0;
}

static char *family_name = "default_proxy_family";
module_param(family_name, charp, 0);

static char *group_name = "default_broadcast_group";
module_param(group_name, charp, 0);

static const struct genl_ops netlink_proxy_ops[] = {
    {
        .cmd = NETLINK_PROXY_CMD_BROADCAST,
        .flags = 0,
        .policy = NULL,
        .doit = broadcast_handler,
    },
};

struct genl_multicast_group netlink_proxy_mcgrps[] = {
    { .name = "" },
};

// Family definition
static struct genl_family netlink_proxy_family = {
    .name = "",
    .version = 1,
    .maxattr = NETLINK_PROXY_ATTR_MAX,
    .netnsok = true,
    .module = THIS_MODULE,
    .ops = netlink_proxy_ops,
    .n_ops = ARRAY_SIZE(netlink_proxy_ops),
    .mcgrps = netlink_proxy_mcgrps,
    .n_mcgrps = ARRAY_SIZE(netlink_proxy_mcgrps),
};

static int __init netlink_proxy_init(void) {
    int ret = 0;

    pr_info("Initializing Netlink Proxy Kernel Module with family=%s groups=%s\n", family_name, group_name);

    strncpy(netlink_proxy_family.name, family_name, GENL_NAMSIZ - 1);
    strncpy(netlink_proxy_mcgrps[0].name, group_name, GENL_NAMSIZ - 1);

    ret = genl_register_family(&netlink_proxy_family);
    if (ret) {
        pr_err("Error registering family.\n");
        return ret;
    }

    return 0;
}

static void __exit netlink_proxy_exit(void) {
    pr_info(KERN_INFO "Exiting Netlink Proxy Kernel Module\n");

    genl_unregister_family(&netlink_proxy_family);

    return;
}

module_init(netlink_proxy_init);
module_exit(netlink_proxy_exit);

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Ze Gan");
MODULE_DESCRIPTION("Netlink Proxy Module");
