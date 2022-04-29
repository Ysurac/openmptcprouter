#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/netlink.h>
#include <linux/route.h>
#include <linux/rtnetlink.h>
#include <linux/sockios.h>
#include "QMIThread.h"

static int ifc_ctl_sock = -1;

static int ifc_init(void)
{
    int ret;
    if (ifc_ctl_sock == -1) {
        ifc_ctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (ifc_ctl_sock < 0) {
            dbg_time("socket() failed: %s\n", strerror(errno));
        }
    }

    ret = ifc_ctl_sock < 0 ? -1 : 0;
    return ret;
}

static void ifc_close(void)
{
    if (ifc_ctl_sock != -1) {
        (void)close(ifc_ctl_sock);
        ifc_ctl_sock = -1;
    }
}

static void ifc_init_ifr(const char *name, struct ifreq *ifr)
{
    memset(ifr, 0, sizeof(struct ifreq));
    strncpy(ifr->ifr_name, name, IFNAMSIZ);
    ifr->ifr_name[IFNAMSIZ - 1] = 0;
}

static int ifc_set_flags(const char *name, unsigned set, unsigned clr)
{
    struct ifreq ifr;
    ifc_init_ifr(name, &ifr);

    if(ioctl(ifc_ctl_sock, SIOCGIFFLAGS, &ifr) < 0) return -1;
    ifr.ifr_flags = (ifr.ifr_flags & (~clr)) | set;
    return ioctl(ifc_ctl_sock, SIOCSIFFLAGS, &ifr);
}

static int ifc_up(const char *name, int rawIP)
{
    int ret = ifc_set_flags(name, IFF_UP | (rawIP ? IFF_NOARP : 0), 0);
    return ret;
}

static int ifc_down(const char *name)
{
    int ret = ifc_set_flags(name, 0, IFF_UP);
    return ret;
}

static void init_sockaddr_in(struct sockaddr *sa, in_addr_t addr)
{
    struct sockaddr_in *sin = (struct sockaddr_in *) sa;
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    sin->sin_addr.s_addr = addr;
}

static int ifc_set_addr(const char *name, in_addr_t addr)
{
    struct ifreq ifr;
    int ret;

    ifc_init_ifr(name, &ifr);
    init_sockaddr_in(&ifr.ifr_addr, addr);

    ret = ioctl(ifc_ctl_sock, SIOCSIFADDR, &ifr);
    return ret;
}

static pthread_attr_t udhcpc_thread_attr;
static pthread_t udhcpc_thread_id;

#ifdef ANDROID
void do_dhcp_request(PROFILE_T *profile);
static void* udhcpc_thread_function(void*  arg) {
    do_dhcp_request((PROFILE_T *)arg);
    return NULL;
}
#else
//#define USE_DHCLIENT
static pid_t udhcpc_pid = 0;
static FILE * ql_popen(const char *program, const char *type)
{
    FILE *iop;
    int pdes[2];
    pid_t pid;
    char *argv[20];
    int argc = 0;
    char *dup_program = strdup(program);
    char *pos = dup_program;

    while (*pos != '\0')
    {
        while (isblank(*pos)) *pos++ = '\0';
        if (*pos != '\0')
        {
            argv[argc++] = pos;
            while (*pos != '\0' && !isblank(*pos)) pos++;
            //dbg_time("argv[%d] = %s", argc-1, argv[argc-1]);
        }
    }
    argv[argc++] = NULL;

    if (pipe(pdes) < 0) {
        return (NULL);
    }

    switch (pid = fork()) {
    case -1:            /* Error. */
        (void)close(pdes[0]);
        (void)close(pdes[1]);
        return (NULL);
        /* NOTREACHED */
    case 0:                /* Child. */
        {
        if (*type == 'r') {
            (void) close(pdes[0]);
            if (pdes[1] != STDOUT_FILENO) {
                (void)dup2(pdes[1], STDOUT_FILENO);
                (void)close(pdes[1]);
            }
        } else {
            (void)close(pdes[1]);
            if (pdes[0] != STDIN_FILENO) {
                (void)dup2(pdes[0], STDIN_FILENO);
                (void)close(pdes[0]);
            }
        }
#if 1 //child do not use these fds
        if (cdc_wdm_fd > 0) {
            close(cdc_wdm_fd);
        } else {
            unsigned int i;
            for (i = 0; i < sizeof(qmiclientId)/sizeof(qmiclientId[0]); i++) {
                if (qmiclientId[i] != 0)
                    close(qmiclientId[i]);
            }
        }
#endif
        execvp(argv[0], argv);
        _exit(127);
        /* NOTREACHED */
        }
           break;
       default:
            udhcpc_pid = pid;
            free(dup_program);
       break;
    }

    /* Parent; assume fdopen can't fail. */
    if (*type == 'r') {
        iop = fdopen(pdes[0], type);
        (void)close(pdes[1]);
    } else {
        iop = fdopen(pdes[1], type);
        (void)close(pdes[0]);
    }

    return (iop);
}

static int ql_pclose(FILE *iop)
{
    (void)fclose(iop);
    udhcpc_pid = 0;
    return 0;
}

static void* udhcpc_thread_function(void*  arg) {
    FILE * udhcpc_fp;
    char udhcpc_cmd[128];
    PROFILE_T *profile = (PROFILE_T *)arg;
    char *ifname = profile->usbnet_adapter;
    int need_add_default_route = 0;

#ifdef USE_DHCLIENT
    snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "dhclient -%d -d --no-pid %s", profile->IPType, (char *)ifname);
#else
    if (profile->IPType == 0x04)
    {
        if (access("/usr/share/udhcpc/default.script", X_OK)) {
            dbg_time("Fail to access /usr/share/udhcpc/default.script, errno: %d (%s)", errno, strerror(errno));
        }

        //-f,--foreground    Run in foreground
        //-b,--background    Background if lease is not obtained
        //-n,--now        Exit if lease is not obtained
        //-q,--quit        Exit after obtaining lease
        //-t,--retries N        Send up to N discover packets (default 3)
        snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "busybox udhcpc -f -n -q -t 5 -i %s", (char *)ifname);
    }
    else
    {
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
        need_add_default_route = 1;
        snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "dibbler-client run");
    }
#endif

    udhcpc_fp = ql_popen(udhcpc_cmd, "r");
    if (udhcpc_fp) {
        char buf[0xff];

        if (need_add_default_route)
        {
            snprintf(buf, sizeof(buf), "route %s add default %s", (profile->IPType == 0x04) ? "" : "-A inet6", ifname);
            system(buf);
        }
        
        while((fgets(buf, sizeof(buf), udhcpc_fp)) != NULL) {
            if ((strlen(buf) > 1) && (buf[strlen(buf) - 1] == '\n'))
                buf[strlen(buf) - 1] = '\0';
            dbg_time("%s", buf);
        }
        ql_pclose(udhcpc_fp);
    }

    return NULL;
}
#endif

void udhcpc_start(PROFILE_T *profile) {
    const char *ifname = profile->usbnet_adapter;

//because must use udhcpc to obtain IP when working on ETH mode, 
//so it is better also use udhcpc to obtain IP when working on IP mode.
//use the same policy for all modules
#if 0 
#ifndef ANDROID
    if (profile->rawIP && ((profile->IPType==0x04 && profile->ipv4.Address)))
    {
        char shell_cmd[128];
        unsigned char *ip = (unsigned char *)&profile->ipv4.Address;
        unsigned char *gw = (unsigned char *)&profile->ipv4.Gateway;
        unsigned char *netmask = (unsigned char *)&profile->ipv4.SubnetMask;
        unsigned char *dns1 = (unsigned char *)&profile->ipv4.DnsPrimary;
        unsigned char *dns2 = (unsigned char *)&profile->ipv4.DnsSecondary;

        snprintf(shell_cmd, sizeof(shell_cmd), "ifconfig %s %d.%d.%d.%d netmask %d.%d.%d.%d",ifname,
            ip[3], ip[2], ip[1], ip[0], netmask[3], netmask[2], netmask[1], netmask[0]);
        dbg_time("%s", shell_cmd);
        system(shell_cmd);

        //Resetting default routes
        snprintf(shell_cmd, sizeof(shell_cmd), "route del default gw 0.0.0.0 dev %s", ifname);
        while(!system(shell_cmd));

        snprintf(shell_cmd, sizeof(shell_cmd), "route add default gw %d.%d.%d.%d dev %s metric 0",
            gw[3], gw[2], gw[1], gw[0], ifname);
        dbg_time("%s", shell_cmd);
        system(shell_cmd);

        #ifdef ANDROID
        do_dhcp_request(profile);
        return;
        #endif        

        //Adding DNS
        if (profile->ipv4.DnsSecondary == 0)
            profile->ipv4.DnsSecondary = profile->ipv4.DnsPrimary;
        if (dns1[0])
        {
            dbg_time("Adding DNS %d.%d.%d.%d %d.%d.%d.%d", dns1[3], dns1[2], dns1[1], dns1[0], dns2[3], dns2[2], dns2[1], dns2[0]);
            snprintf(shell_cmd, sizeof(shell_cmd), "echo -n \"nameserver %d.%d.%d.%d\nnameserver %d.%d.%d.%d\n\" > /etc/resolv.conf",
                dns1[3], dns1[2], dns1[1], dns1[0], dns2[3], dns2[2], dns2[1], dns2[0]);
             system(shell_cmd); 
        }           
        
        return;
    }
#endif        
#endif

    ifc_init();
    if (ifc_set_addr(profile->usbnet_adapter, 0)) {
        dbg_time("failed to set ip addr for %s to 0.0.0.0: %s\n", ifname, strerror(errno));
        return;
    }

    if (ifc_up(profile->usbnet_adapter, profile->rawIP)) {
        dbg_time("failed to bring up interface %s: %s\n", ifname, strerror(errno));
        return;
    }
    ifc_close();

    pthread_attr_init(&udhcpc_thread_attr);
    pthread_attr_setdetachstate(&udhcpc_thread_attr, PTHREAD_CREATE_DETACHED);
    if(pthread_create(&udhcpc_thread_id, &udhcpc_thread_attr, udhcpc_thread_function, (void*)profile) !=0 ) {
        dbg_time("failed to create udhcpc_thread for %s: %s\n", ifname, strerror(errno));
    }
    pthread_attr_destroy(&udhcpc_thread_attr);
}

void udhcpc_stop(PROFILE_T *profile) {
    const char *ifname = profile->usbnet_adapter;

#ifdef ANDROID
#else
    pid_t pid = udhcpc_pid;
    //dbg_time("%s kill %d/%d", __func__, pid, getpid());
    if (pid > 0 && !kill(pid, 0))
    {
        int kill_time = 50;
        do {
            kill(pid, SIGTERM);
            usleep(100*1000);
        } while(kill_time-- && !kill(pid, 0)); //wait udhcpc quit
        if (!kill(pid, 0))
            kill(pid, SIGKILL);
        //dbg_time("%s kill udhcpc_pid time=%d", __func__, (50 - kill_time) * 100);
    }
#endif    
    ifc_init();
    ifc_set_addr(ifname, 0);
    ifc_down(ifname);
    ifc_close();
}

UINT ifc_get_addr(const char *ifname) {
    int inet_sock;
    struct ifreq ifr;
    UINT addr = 0;
    
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, ifname);

    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0) {
        goto error;
    }
    addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
error:
    close(inet_sock);

    return addr;
}
