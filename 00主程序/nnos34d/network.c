/**
 * ===================NNOS-NetWork============================
 *【网络管理系统】（移植自GNU/Linux）
 * -----------------------------------------------------------
 * ·日期：2019-5-1      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/	
#include "./lib/nnos.h"	//系统函数声明
#define IFNAMSIZ 32
struct net_device{
   //用于存放网络设备的设备名称；
   char name[IFNAMSIZ];
   //网络设备的别名；
   char *ifalias;
   //网络设备的接口索引值，独一无二的网络设备标识符；
   int ifindex;
	//这个字段用于构建网络设备名的哈希散列表，而struct net中的
  //name_hlist就指向每个哈希散列表的链表头；
  struct hlist_node name_hlist;
	//用于构建网络设备的接口索引值哈希散列表，在struct net中的
  //index_hlist用于指向接口索引值哈希散列表的链表头；
  struct hlist_node index_hlist;
  //用于将每一个网络设备加入到一个网络命名空间中的网络设备双链表中
  struct list_head dev_list;
  //网络设备接口的标识符,其状态类型被定义在<linux/if.h>之中；
  unsigned int flags;
  //网络设备接口的标识符，但对用户空间不可见；
  unsigned short  priv_flags;
  //接口硬件类型，在<if_arp.h>中定义了每一个接口硬件类型；
  unsigned short type；
  //网络设备接口的最大传输单元；
  unsigned mtu;
  //硬件接口头长度；
  unsigned short hard_header_len;
  //网络设备接口的MAC地址；
  unsigned char *dev_addr;
  //网络设备接口的单播模式
  int uc_promisc;
  //网络设备接口的混杂模式；
  unsigned int promiscuity;
  //网络设备接口的全组播模式；
  unsigend int allmulti;
  //secondary unicast mac address
  struct netdev_hw_addr_list uc;
  //list of device hw address;
  struct netdev_hw_addr_list dev_addrs;
  //hw broadcast address;
  unsigned char broadcast[MAX_ADDR_LEN];
  //multicast mac address;
  struct dev_addr_list *mac_list;
	//网络设备接口的数据包接收队列；
  struct netdev_queue rx_queue;
  //网络设备接口的数据包发送队列；
  struct netdev_queue *tx;
  //Number of TX queues allocated at alloc_netdev_mq() time
  unsigned int num_tx_queues;
  //Number of TX queues currently active in device;
  unsigned int real_num_tx_queues;
  //Max frame per queue allowned;
  unsigned long tx_queue_len;
	//网络设备接口的状态；
  unsigned long state;
  //网络设备接口的统计情况；
  struct net_device_state states;
  //用于执行网络设备所在的命名空间；
  struct net *nd_net;     
}
   
static struct net_device *v_ndev;    //定义网卡设备

static int virtual_net_init(){
 
    /* 1.分配一个net_device结构 */
    v_ndev = alloc_etherdev(0);
    if(!v_ndev) {
        //printk("alloc_etherdev fail\n");
		//sysprintl("alloc_etherdev fail\n")
        //return -ENOMEM;
		return -1;
    }
 
    /* 2.设置v_ndev,设置硬件等 */
 
    /* 3. 注册网卡设备 */
    register_netdev(v_ndev);
 
    return 0;
}

static void virtual_net_exit(void)
{
    unregister_netdev(v_ndev);
    free_netdev(v_ndev);        /* free device structure */
}

module_init(virtual_net_init);
module_exit(virtual_net_exit);

int register_netdev(struct net_device *dev) 
{ 
    int err;

    ////rtnl_lock();

    /* 
     * If the name is a format string the caller wants us to do a 
     * name allocation. 
     */ 
    if (strchr(dev->name, '%')) { 
        err = dev_alloc_name(dev, dev->name); 
        if (err < 0) 
            ////goto out;
		return -1; 		
    }

    err = register_netdevice(dev); 
//out: 
    ////rtnl_unlock(); 
    return err; 
} 
EXPORT_SYMBOL(register_netdev);



/* int register_netdev(struct net_device *dev)
{
    int err; 
 
    rtnl_lock();
    err = register_netdevice(dev);                                                                                       
    rtnl_unlock();
    return err; 
}
   
 void rtnl_unlock(void){
    netdev_run_todo();                                                          
}
EXPORT_SYMBOL(rtnl_unlock); */

int dev_alloc_name(struct net_device *dev, const char *name) 
{ 
    char buf[IFNAMSIZ]; 
    struct net *net; 
    int ret;

    BUG_ON(!dev_net(dev)); 
    net = dev_net(dev); 
    ret = __dev_alloc_name(net, name, buf); 
    if (ret >= 0) 
        strlcpy(dev->name, buf, IFNAMSIZ); //将返回的设备名称复制到net_device的name字段 
    return ret; 
}

/* static int __dev_alloc_name(struct net *net, const char *name, char *buf) 
{ 
    int i = 0; 
    const char *p; 
    const int max_netdevices = 8*PAGE_SIZE; 
    unsigned long *inuse; 
    struct net_device *d;


   p = strnchr(name, IFNAMSIZ-1, '%'); 
    if (p) { 
          if (p[1] != 'd' || strchr(p + 2, '%')) 
            return -EINVAL;
        inuse = (unsigned long *) get_zeroed_page(GFP_ATOMIC); 
        if (!inuse) 
            return -ENOMEM;

        for_each_netdev(net, d) { 
            if (!sscanf(d->name, name, &i)) //获取同类型网络设备的其序列号，这里极为巧妙 
                continue; 
            if (i < 0 || i >= max_netdevices) //判断序列号的范围 
                continue;

           snprintf(buf, IFNAMSIZ, name, i); 
            if (!strncmp(buf, d->name, IFNAMSIZ)) //验证解析的序列号是否正确
                set_bit(i, inuse); //在位图中将该位标记 
        }

        i = find_first_zero_bit(inuse, max_netdevices); //找到第一个为0的序列号 
        free_page((unsigned long) inuse); 
    }

    if (buf != name) 
        snprintf(buf, IFNAMSIZ, name, i); //根据找到的序列号，输出完整的设备名 
    if (!__dev_get_by_name(net, buf)) //在name_list链表中查找是否有同名的设备 
        return i;
    return -ENFILE; 
} */

/* int register_netdevice(struct net_device *dev) 
{ 
    int ret; 
    struct net *net = dev_net(dev);  //设备的网络空间

    BUG_ON(dev_boot_phase); 
    ASSERT_RTNL();

    might_sleep();

    BUG_ON(dev->reg_state != NETREG_UNINITIALIZED); //alloc_netdev时不需要设置这个成员，因为其为0 
    BUG_ON(!net);

    spin_lock_init(&dev->addr_list_lock); 
    netdev_set_addr_lockdep_class(dev);

    dev->iflink = -1;

    if (dev->netdev_ops->ndo_init) { //调用设备驱动程序操作中实现的初始化函数 
        ret = dev->netdev_ops->ndo_init(dev); 
        if (ret) { 
            if (ret > 0) 
                ret = -EIO; 
            goto out; 
        } 
    }

    ret = dev_get_valid_name(dev, dev->name, 0); //检查设备名称的有效性 
    if (ret) 
        goto err_uninit;

    dev->ifindex = dev_new_index(net); //为设备分配一个唯一的索引号 
    if (dev->iflink == -1) 
        dev->iflink = dev->ifindex;

    dev->hw_features |= NETIF_F_SOFT_FEATURES; 
    dev->features |= NETIF_F_SOFT_FEATURES; 
    dev->wanted_features = dev->features & dev->hw_features;

    dev->vlan_features |= (NETIF_F_GRO | NETIF_F_HIGHDMA);

    ret = call_netdevice_notifiers(NETDEV_POST_INIT, dev); //调用通知链，发出事件通知 
    ret = notifier_to_errno(ret); 
    if (ret) 
        goto err_uninit;

    ret = netdev_register_kobject(dev); //设备注册的核心函数，主要是调用device_add函数，将设备添加到内核的设备管理器中 
    if (ret) 
        goto err_uninit; 
    dev->reg_state = NETREG_REGISTERED;  //设置net_device的状态

    netdev_update_features(dev);

    set_bit(__LINK_STATE_PRESENT, &dev->state);

    dev_init_scheduler(dev); //在这里会设置设备的看门狗定时器 
    dev_hold(dev); //增加设备的引用计数 
    list_netdevice(dev);  //将设备加入系统的indexlist、namelist和devlist中

    ret = call_netdevice_notifiers(NETDEV_REGISTER, dev); //通过通知链发出设备注册通知 
    ret = notifier_to_errno(ret); 
    if (ret) { 
        rollback_registered(dev); 
        dev->reg_state = NETREG_UNREGISTERED; 
    } 

    if (!dev->rtnl_link_ops || 
        dev->rtnl_link_state == RTNL_LINK_INITIALIZED) 
        rtmsg_ifinfo(RTM_NEWLINK, dev, ~0U);

out: 
    return ret;

err_uninit: 
    if (dev->netdev_ops->ndo_uninit) 
        dev->netdev_ops->ndo_uninit(dev); 
    goto out; 
}  */


   