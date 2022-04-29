/******************************************************************************
  @file    udhcpc.c
  @brief   call DHCP tools to obtain IP address.

  DESCRIPTION
  Connectivity Management Tool for USB network adapter of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <endian.h>

#include "util.h"
#include "QMIThread.h"

static int ql_system(const char *shell_cmd) {
    dbg_time("%s", shell_cmd);
    return system(shell_cmd);
}

static void ifc_init_ifr(const char *name, struct ifreq *ifr)
{
    memset(ifr, 0, sizeof(struct ifreq));
    strncpy(ifr->ifr_name, name, IFNAMSIZ);
    ifr->ifr_name[IFNAMSIZ - 1] = 0;
}

static void ql_set_mtu(const char *ifname, int ifru_mtu) {
    int inet_sock;
    struct ifreq ifr;

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (inet_sock > 0) {
        ifc_init_ifr(ifname, &ifr);

        if (!ioctl(inet_sock, SIOCGIFMTU, &ifr)) {
            if (ifr.ifr_ifru.ifru_mtu != ifru_mtu) {
                dbg_time("change mtu %d -> %d", ifr.ifr_ifru.ifru_mtu , ifru_mtu);
                ifr.ifr_ifru.ifru_mtu = ifru_mtu;
                ioctl(inet_sock, SIOCSIFMTU, &ifr);
            }
        }

        close(inet_sock);
    }
}

static int ifc_get_addr(const char *name, in_addr_t *addr)
{
    int inet_sock;
    struct ifreq ifr;
    int ret = 0;

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    ifc_init_ifr(name, &ifr);
    if (addr != NULL) {
        ret = ioctl(inet_sock, SIOCGIFADDR, &ifr);
        if (ret < 0) {
            *addr = 0;
        } else {
            *addr = ((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr;
        }
    }
    close(inet_sock);
    return ret;
}

static short ifc_get_flags(const char *ifname)
{
    int inet_sock;
    struct ifreq ifr;
    int ret = 0;

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (inet_sock > 0) {
        ifc_init_ifr(ifname, &ifr);

        if (!ioctl(inet_sock, SIOCGIFFLAGS, &ifr)) {
            ret = ifr.ifr_ifru.ifru_flags;
        }

        close(inet_sock);
    }

    return ret;
}

static int ql_raw_ip_mode_check(const char *ifname) {
    int fd;
    in_addr_t addr;
    char raw_ip[128];
    char shell_cmd[128];
    char mode[2] = "X";
    int mode_change = 0;

    ifc_get_addr(ifname, &addr);
    if (addr)
        return 0;
    
    snprintf(raw_ip, sizeof(raw_ip), "/sys/class/net/%s/qmi/raw_ip", ifname);
    if (access(raw_ip, F_OK))
        return 0;

    fd = open(raw_ip, O_RDWR | O_NONBLOCK | O_NOCTTY);
    if (fd < 0) {
        dbg_time("%s %d fail to open(%s), errno:%d (%s)", __FILE__, __LINE__, raw_ip, errno, strerror(errno));
        return 0;
    }

    read(fd, mode, 2);
    if (mode[0] == '0' || mode[0] == 'N') {
        dbg_time("File:%s Line:%d udhcpc fail to get ip address, try next:", __func__, __LINE__);
        snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s down", ifname);
        ql_system(shell_cmd);
        dbg_time("echo Y > /sys/class/net/%s/qmi/raw_ip", ifname);
        mode[0] = 'Y';
        write(fd, mode, 2);
        mode_change = 1;
        snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s up", ifname);
        ql_system(shell_cmd);
    }

    close(fd);
    return mode_change;
}

static void* udhcpc_thread_function(void* arg) {
    FILE * udhcpc_fp;
    char *udhcpc_cmd = (char *)arg;

    if (udhcpc_cmd == NULL)
        return NULL;

    dbg_time("%s", udhcpc_cmd);
    udhcpc_fp = popen(udhcpc_cmd, "r");
    free(udhcpc_cmd);
    if (udhcpc_fp) {
        char buf[0xff];

        buf[sizeof(buf)-1] = '\0';
        while((fgets(buf, sizeof(buf)-1, udhcpc_fp)) != NULL) {
            if ((strlen(buf) > 1) && (buf[strlen(buf) - 1] == '\n'))
                buf[strlen(buf) - 1] = '\0';
            dbg_time("%s", buf);
        }

        pclose(udhcpc_fp);
    }

    return NULL;
}

//#define USE_DHCLIENT
#ifdef USE_DHCLIENT
static int dhclient_alive = 0;
#endif
static int dibbler_client_alive = 0;

void ql_get_driver_rmnet_info(PROFILE_T *profile, RMNET_INFO *rmnet_info) {
    int ifc_ctl_sock;
    struct ifreq ifr;
    int rc;
    int request = 0x89F3;
    unsigned char data[512];

    memset(rmnet_info, 0x00, sizeof(*rmnet_info));

    ifc_ctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ifc_ctl_sock <= 0) {
        dbg_time("socket() failed: %s\n", strerror(errno));
        return;
    }
    
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, profile->usbnet_adapter, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;    
    ifr.ifr_ifru.ifru_data = (void *)data;
        
    rc = ioctl(ifc_ctl_sock, request, &ifr);
    if (rc < 0) {
        dbg_time("ioctl(0x%x, qmap_settings) failed: %s, rc=%d", request, strerror(errno), rc);
    }
    else {
        memcpy(rmnet_info, data, sizeof(*rmnet_info));
    }

    close(ifc_ctl_sock);
}

void ql_set_driver_qmap_setting(PROFILE_T *profile, QMAP_SETTING *qmap_settings) {
    int ifc_ctl_sock;
    struct ifreq ifr;
    int rc;
    int request = 0x89F2;

    ifc_ctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ifc_ctl_sock <= 0) {
        dbg_time("socket() failed: %s\n", strerror(errno));
        return;
    }
    
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, profile->usbnet_adapter, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;    
    ifr.ifr_ifru.ifru_data = (void *)qmap_settings;
        
    rc = ioctl(ifc_ctl_sock, request, &ifr);
    if (rc < 0) {
        dbg_time("ioctl(0x%x, qmap_settings) failed: %s, rc=%d", request, strerror(errno), rc);
    }

    close(ifc_ctl_sock);	
}

void ql_set_driver_link_state(PROFILE_T *profile, int link_state) {
    char link_file[128];
    int fd;
    int new_state = 0;

    snprintf(link_file, sizeof(link_file), "/sys/class/net/%s/link_state", profile->usbnet_adapter);
    fd = open(link_file, O_RDWR | O_NONBLOCK | O_NOCTTY);
    if (fd == -1) {
        if (errno != ENOENT)
            dbg_time("Fail to access %s, errno: %d (%s)", link_file, errno, strerror(errno));
        return;
    }

    if (profile->qmap_mode <= 1)
        new_state = !!link_state;
    else {
        //0x80 means link off this pdp
        new_state = (link_state ? 0x00 : 0x80) + profile->pdp;
    }

    snprintf(link_file, sizeof(link_file), "%d\n", new_state);
    write(fd, link_file, sizeof(link_file));

    if (link_state == 0 && profile->qmapnet_adapter && strcmp(profile->qmapnet_adapter, profile->usbnet_adapter)) {
        size_t rc;

        lseek(fd, 0, SEEK_SET);
        rc = read(fd, link_file, sizeof(link_file));
        if (rc > 1 && (!strncasecmp(link_file, "0\n", 2) || !strncasecmp(link_file, "0x0\n", 4))) {
            snprintf(link_file, sizeof(link_file), "ifconfig %s down", profile->usbnet_adapter);
            ql_system(link_file);
        }
    }

    close(fd);
}

static const char *ipv4Str(const uint32_t Address) {
    static char str[] = {"255.225.255.255"};
    uint8_t *ip = (uint8_t *)&Address;

    snprintf(str, sizeof(str), "%d.%d.%d.%d", ip[3], ip[2], ip[1], ip[0]);
    return str;
}

static const char *ipv6Str(const UCHAR Address[16]) {
    static char str[64];
    uint16_t ip[8];
    int i;
    for (i = 0; i < 8; i++) {
        ip[i] = (Address[i*2]<<8) + Address[i*2+1];
    }

    snprintf(str, sizeof(str), "%x:%x:%x:%x:%x:%x:%x:%x",
        ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7]);

    return str;
}

//#define QL_OPENWER_NETWORK_SETUP
#ifdef QL_OPENWER_NETWORK_SETUP
static const char *openwrt_lan = "br-lan";
static const char *openwrt_wan = "wwan0";

static int ql_openwrt_system(const char *cmd) {
    int i;
    int ret = 1;
    char shell_cmd[128];

    snprintf(shell_cmd, sizeof(shell_cmd), "%s 2>1 > /dev/null", cmd);
    
    for (i = 0; i < 15; i++) {
        dbg_time("%s", cmd);
        ret = system(shell_cmd);
        if (!ret)
            break;
        sleep(1);
    }

    return ret;
}

static int ql_openwrt_is_wan(const char *ifname) {
    if (openwrt_lan == NULL) {
        system("uci show network.wan.ifname");
    }

    if (strcmp(ifname, openwrt_wan))
        return 0;

    return 1;
}

static void ql_openwrt_setup_wan(const char *ifname, const IPV4_T *ipv4) {
    FILE *fp = NULL;
    char config[64];

    snprintf(config, sizeof(config), "/tmp/rmnet_%s_ipv4config", ifname);
     
    if (ipv4 == NULL) {
        if (ql_openwrt_is_wan(ifname))
            ql_openwrt_system("ifdown wan");
        return;
    }

    fp = fopen(config, "w");
    if (fp == NULL)
        return;

    fprintf(fp, "IFNAME=\"%s\"\n", ifname);
    fprintf(fp, "PUBLIC_IP=\"%s\"\n", ipv4Str(ipv4->Address));
    fprintf(fp, "NETMASK=\"%s\"\n", ipv4Str(ipv4->SubnetMask));
    fprintf(fp, "GATEWAY=\"%s\"\n", ipv4Str(ipv4->Gateway));
    fprintf(fp, "DNSSERVERS=\"%s", ipv4Str(ipv4->DnsPrimary));
    if (ipv4->DnsSecondary != 0)
        fprintf(fp, " %s", ipv4Str(ipv4->DnsSecondary));
    fprintf(fp, "\"\n");

    fclose(fp);

    if (!ql_openwrt_is_wan(ifname))
        return;
    
    ql_openwrt_system("ifup wan");
}

static void ql_openwrt_setup_wan6(const char *ifname, const IPV6_T *ipv6) {
    FILE *fp = NULL;
    char config[64];
    int first_ifup;
     
    snprintf(config, sizeof(config), "/tmp/rmnet_%s_ipv6config", ifname);
     
    if (ipv6 == NULL) {
        if (ql_openwrt_is_wan(ifname))
            ql_openwrt_system("ifdown wan6");
        return;
    }

    first_ifup = (access(config, F_OK) != 0);

    fp = fopen(config, "w");
    if (fp == NULL)
        return;

    fprintf(fp, "IFNAME=\"%s\"\n", ifname);
    fprintf(fp, "PUBLIC_IP=\"%s\"\n", ipv6Str(ipv6->Address));
    fprintf(fp, "NETMASK=\"%s\"\n", ipv6Str(ipv6->SubnetMask));
    fprintf(fp, "GATEWAY=\"%s\"\n", ipv6Str(ipv6->Gateway));
    fprintf(fp, "PrefixLength=\"%d\"\n", ipv6->PrefixLengthIPAddr);
    fprintf(fp, "DNSSERVERS=\"%s", ipv6Str(ipv6->DnsPrimary));
    if (ipv6->DnsSecondary[0])
        fprintf(fp, " %s", ipv6Str(ipv6->DnsSecondary));
    fprintf(fp, "\"\n");

    fclose(fp);

    if (!ql_openwrt_is_wan(ifname))
        return;

    if (first_ifup)
        ql_openwrt_system("ifup wan6");
    else
        ql_openwrt_system("/etc/init.d/network restart"); //make PC to release old IPV6 address, and RS new IPV6 address

#if 1 //TODO? why need this?
    if (openwrt_lan) {
        int i;
        char shell_cmd[128];
        UCHAR Address[16] = {0};

        ql_openwrt_system(("ifstatus lan"));

        for (i = 0; i < (ipv6->PrefixLengthIPAddr/8); i++)
            Address[i] = ipv6->Address[i];

        snprintf(shell_cmd, sizeof(shell_cmd), "ip route del %s/%u dev %s", ipv6Str(Address), ipv6->PrefixLengthIPAddr, ifname);
        ql_openwrt_system(shell_cmd);

        snprintf(shell_cmd, sizeof(shell_cmd), "ip route add %s/%u dev %s", ipv6Str(Address), ipv6->PrefixLengthIPAddr, openwrt_lan);
        ql_system(shell_cmd);        
    }
#endif
}
#endif

void udhcpc_start(PROFILE_T *profile) {
    char *ifname = profile->usbnet_adapter;
    char shell_cmd[128];

    ql_set_driver_link_state(profile, 1);

    if (profile->qmapnet_adapter) {
        ifname = profile->qmapnet_adapter;
    }

    if (profile->rawIP && profile->ipv4.Address && profile->ipv4.Mtu) {
        ql_set_mtu(ifname, (profile->ipv4.Mtu));
    }

    if (strcmp(ifname, profile->usbnet_adapter)) {
        snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s up", profile->usbnet_adapter);
        ql_system(shell_cmd);
        if (ifc_get_flags(ifname)&IFF_UP) {
            snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s down", ifname);
            ql_system(shell_cmd);
        }
    }

    snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s up", ifname);
    ql_system(shell_cmd);
	
    if (profile->ipv4.Address) {
        if (profile->PCSCFIpv4Addr1)
            dbg_time("pcscf1: %s", ipv4Str(profile->PCSCFIpv4Addr1));
        if (profile->PCSCFIpv4Addr2)
            dbg_time("pcscf2: %s", ipv4Str(profile->PCSCFIpv4Addr2));
    }

    if (profile->ipv6.Address[0] && profile->ipv6.PrefixLengthIPAddr) {
        if (profile->PCSCFIpv6Addr1[0])
            dbg_time("pcscf1: %s", ipv6Str(profile->PCSCFIpv6Addr1));
        if (profile->PCSCFIpv6Addr2[0])
            dbg_time("pcscf2: %s", ipv6Str(profile->PCSCFIpv6Addr2));
    }

#if 1 //for bridge mode, only one public IP, so do udhcpc manually
    if (ql_bridge_mode_detect(profile)) {
        return;
    }
#endif

//because must use udhcpc to obtain IP when working on ETH mode,
//so it is better also use udhcpc to obtain IP when working on IP mode.
//use the same policy for all modules
#if 0
    if (profile->rawIP != 0) //mdm9x07/ec25,ec20 R2.0
    {
        if (profile->ipv4.Address) {
            unsigned char *ip = (unsigned char *)&profile->ipv4.Address;
            unsigned char *gw = (unsigned char *)&profile->ipv4.Gateway;
            unsigned char *netmask = (unsigned char *)&profile->ipv4.SubnetMask;
            unsigned char *dns1 = (unsigned char *)&profile->ipv4.DnsPrimary;
            unsigned char *dns2 = (unsigned char *)&profile->ipv4.DnsSecondary;

            snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s %d.%d.%d.%d netmask %d.%d.%d.%d",ifname,
                ip[3], ip[2], ip[1], ip[0], netmask[3], netmask[2], netmask[1], netmask[0]);
            ql_system(shell_cmd);

            //Resetting default routes
            snprintf(shell_cmd, sizeof(shell_cmd), "route del default gw 0.0.0.0 dev %s", ifname);
            while(!system(shell_cmd));

            snprintf(shell_cmd, sizeof(shell_cmd), "route add default gw %d.%d.%d.%d dev %s metric 0", gw[3], gw[2], gw[1], gw[0], ifname);
            ql_system(shell_cmd);

            //Adding DNS
            if (profile->ipv4.DnsSecondary == 0)
                profile->ipv4.DnsSecondary = profile->ipv4.DnsPrimary;

            if (dns1[0]) {
                dbg_time("Adding DNS %d.%d.%d.%d %d.%d.%d.%d", dns1[3], dns1[2], dns1[1], dns1[0], dns2[3], dns2[2], dns2[1], dns2[0]);
                snprintf(shell_cmd, sizeof(shell_cmd), "echo -n \"nameserver %d.%d.%d.%d\nnameserver %d.%d.%d.%d\n\" > /etc/resolv.conf",
                    dns1[3], dns1[2], dns1[1], dns1[0], dns2[3], dns2[2], dns2[1], dns2[0]);
                system(shell_cmd);
            }
        }


        if (profile->ipv6.Address[0] && profile->ipv6.PrefixLengthIPAddr) {
            unsigned char *ip = (unsigned char *)profile->ipv6.Address;
#if 1
            snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s inet6 add %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x/%d",
                ifname, ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7], ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15], profile->ipv6.PrefixLengthIPAddr);
#else
            snprintf(shell_cmd, sizeof(shell_cmd), "ip -6 addr add %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x/%d dev %s",
                ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7], ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15], profile->ipv6.PrefixLengthIPAddr, ifname);
#endif
            ql_system(shell_cmd);
            snprintf(shell_cmd, sizeof(shell_cmd), "route -A inet6 add default dev %s", ifname);
            ql_system(shell_cmd);
        }
        return;
    }
#endif

/* Do DHCP using busybox tools */
    {
        char udhcpc_cmd[128];
        pthread_attr_t udhcpc_thread_attr;
        pthread_t udhcpc_thread_id;

        pthread_attr_init(&udhcpc_thread_attr);
        pthread_attr_setdetachstate(&udhcpc_thread_attr, PTHREAD_CREATE_DETACHED);

        if (profile->ipv4.Address) {
#ifdef USE_DHCLIENT
            snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "dhclient -4 -d --no-pid %s", ifname);
            dhclient_alive++;
#else
            if (access("/usr/share/udhcpc/default.script", X_OK)) {
                dbg_time("Fail to access /usr/share/udhcpc/default.script, errno: %d (%s)", errno, strerror(errno));
            }

            //-f,--foreground    Run in foreground
            //-b,--background    Background if lease is not obtained
            //-n,--now        Exit if lease is not obtained
            //-q,--quit        Exit after obtaining lease
            //-t,--retries N        Send up to N discover packets (default 3)
            snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "busybox udhcpc -f -n -q -t 5 -i %s", ifname);
#endif

#if 1 //for OpenWrt
            if (!access("/lib/netifd/dhcp.script", X_OK) && !access("/sbin/ifup", X_OK) && !access("/sbin/ifstatus", X_OK)) {
                dbg_time("you are use OpenWrt?");
                dbg_time("should not calling udhcpc manually?");
                dbg_time("should modify /etc/config/network as below?");
                dbg_time("config interface wan");
                dbg_time("\toption ifname	%s", ifname);
                dbg_time("\toption proto	dhcp");
                dbg_time("should use \"/sbin/ifstaus wan\" to check %s 's status?", ifname);
            }
#endif

#ifdef USE_DHCLIENT            
            pthread_create(&udhcpc_thread_id, &udhcpc_thread_attr, udhcpc_thread_function, (void*)strdup(udhcpc_cmd));
            sleep(1);
#else
            pthread_create(&udhcpc_thread_id, NULL, udhcpc_thread_function, (void*)strdup(udhcpc_cmd));
            pthread_join(udhcpc_thread_id, NULL);
            if (ql_raw_ip_mode_check(ifname)) {
                pthread_create(&udhcpc_thread_id, NULL, udhcpc_thread_function, (void*)strdup(udhcpc_cmd));
                pthread_join(udhcpc_thread_id, NULL);
            }
#endif
#ifdef QL_OPENWER_NETWORK_SETUP
            ql_openwrt_setup_wan(ifname, &profile->ipv4);
#endif
        }

        if (profile->ipv6.Address[0] && profile->ipv6.PrefixLengthIPAddr) {
#if 1
            //module do not support DHCPv6, only support 'Router Solicit'
            //and it seem if enable /proc/sys/net/ipv6/conf/all/forwarding, Kernel do not send RS
            const char *forward_file = "/proc/sys/net/ipv6/conf/all/forwarding";
            int forward_fd = open(forward_file, O_RDONLY);
            if (forward_fd > 0) {
                char forward_state[2];
                read(forward_fd, forward_state, 2);
                if (forward_state[0] == '1') {
                    //dbg_time("%s enabled, kernel maybe donot send 'Router Solicit'", forward_file);
                }
                close(forward_fd);
            }

            snprintf(shell_cmd, sizeof(shell_cmd), "ip -%d address add %s/%u dev %s",
                6, ipv6Str(profile->ipv6.Address), profile->ipv6.PrefixLengthIPAddr, ifname);
            ql_system(shell_cmd);

            //ping6 www.qq.com
            snprintf(shell_cmd, sizeof(shell_cmd), "ip -%d route add default via %s dev %s",
                6, ipv6Str(profile->ipv6.Gateway), ifname);
            ql_system(shell_cmd);

            if (profile->ipv6.DnsPrimary[0] || profile->ipv6.DnsSecondary[0]) {
                char dns1str[64], dns2str[64];

                if (profile->ipv6.DnsPrimary[0]) {
                    strcpy(dns1str, ipv6Str(profile->ipv6.DnsPrimary));
                }

                if (profile->ipv6.DnsSecondary[0]) {
                    strcpy(dns2str, ipv6Str(profile->ipv6.DnsSecondary));
                }
                
                update_resolv_conf(6, ifname, profile->ipv6.DnsPrimary[0] ? dns1str : NULL, profile->ipv6.DnsSecondary ? dns2str : NULL);
#ifdef QL_OPENWER_NETWORK_SETUP
                ql_openwrt_setup_wan6(ifname, &profile->ipv6);
#endif
            }
#else
#ifdef USE_DHCLIENT
            snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "dhclient -6 -d --no-pid %s",  ifname);
            dhclient_alive++;
#else
            /*
                DHCPv6: Dibbler - a portable DHCPv6
                1. download from http://klub.com.pl/dhcpv6/
                2. cross-compile
                    2.1 ./configure --host=arm-linux-gnueabihf
                    2.2 copy dibbler-client to your board
                3. mkdir -p /var/log/dibbler/ /var/lib/ on your board
                4. create /etc/dibbler/client.conf on your board, the content is
                    log-mode short
                    log-level 7
                    iface wwan0 {
                        ia
                        option dns-server
                    }
                 5. run "dibbler-client start" to get ipV6 address
                 6. run "route -A inet6 add default dev wwan0" to add default route
            */
            snprintf(shell_cmd, sizeof(shell_cmd), "route -A inet6 add default %s", ifname);
            ql_system(shell_cmd);
            snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "dibbler-client run");
            dibbler_client_alive++;
#endif

            pthread_create(&udhcpc_thread_id, &udhcpc_thread_attr, udhcpc_thread_function, (void*)strdup(udhcpc_cmd));
#endif
        }
    }
}

void udhcpc_stop(PROFILE_T *profile) {
    char *ifname = profile->usbnet_adapter;
    char shell_cmd[128];

    ql_set_driver_link_state(profile, 0);

    if (profile->qmapnet_adapter) {
        ifname = profile->qmapnet_adapter;
    }

#ifdef USE_DHCLIENT
    if (dhclient_alive) {
        system("killall dhclient");
        dhclient_alive = 0;
    }
#endif
    if (dibbler_client_alive) {
        system("killall dibbler-client");
        dibbler_client_alive = 0;
    }

//it seems when call netif_carrier_on(), and netcard 's IP is "0.0.0.0", will cause netif_queue_stopped()
    snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s 0.0.0.0", ifname);
    ql_system(shell_cmd);
    snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s down", ifname);
    ql_system(shell_cmd);

#ifdef QL_OPENWER_NETWORK_SETUP
    ql_openwrt_setup_wan(ifname, NULL);
    ql_openwrt_setup_wan6(ifname, NULL);
#endif
}
