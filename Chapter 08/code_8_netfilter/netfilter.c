#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netdevice.h>

static unsigned int hook_func_tunnel_in(unsigned int hooknum,
                                        struct sk_buff *skb,
                                        const struct net_device *in,
                                        const struct net_device *out,
                                        int (*okfn)(struct sk_buff *)) {
    struct iphdr *ip_header;
    struct net_device *dev;
    struct Qdisc *q;
    int len;

    // Check if the packet has IP header
    if (!skb || !skb_network_header(skb))
        return NF_ACCEPT;

    ip_header = ip_hdr(skb);

    // Check if the packet is IPv4
    if (ip_header->version != 4)
        return NF_ACCEPT;

    // Check if the packet is received from eth0 and destination IP is ip1
    if (in && strcmp(in->name, "eth0") == 0 && ip_header->daddr == htonl(ip1)) {
        len = skb->len;

        // Perform tunneling
        skb_push(skb, ETH_HLEN);
        skb_reset_mac_header(skb);
        memcpy(skb->data, hd_mac2, ETH_ALEN * 2);

        // Enqueue the packet for transmission on eth1
        dev = dev_get_by_name("eth1");
        if (!dev)
            return NF_ACCEPT;

        q = dev->qdisc;
        spin_lock_bh(&dev->queue_lock);
        q->enqueue(skb, q);
        qdisc_run(dev);
        spin_unlock_bh(&dev->queue_lock);

        return NF_STOLEN; // Indicate that we have taken ownership of the packet
    }

    return NF_ACCEPT; // Allow the packet to proceed normally
}

// Netfilter hook structure
static struct nf_hook_ops nfho_tunnel_in = {
    .hook = hook_func_tunnel_in,
    .hooknum = NF_INET_PRE_ROUTING, // Use NF_INET_PRE_ROUTING for IPv4
    .pf = NFPROTO_IPV4, // Use NFPROTO_IPV4 for IPv4
    .priority = NF_IP_PRI_FIRST,
};

// Module initialization function
static int __init nf_hook_init(void) {
    int ret;

    // Register the Netfilter hook
    ret = nf_register_hook(&nfho_tunnel_in);
    if (ret < 0) {
        printk(KERN_ERR "Failed to register Netfilter hook\n");
        return ret;
    }

    printk(KERN_INFO "NF_HOOK: Module installed\n");
    return 0;
}

// Module cleanup function
static void __exit nf_hook_exit(void) {
    // Unregister the Netfilter hook
    nf_unregister_hook(&nfho_tunnel_in);

    printk(KERN_INFO "NF_HOOK: Module removed\n");
}

module_init(nf_hook_init);
module_exit(nf_hook_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BPB");
MODULE_DESCRIPTION("Netfilter hook example");
