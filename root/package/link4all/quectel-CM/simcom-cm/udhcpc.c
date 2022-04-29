#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <net/if.h>
#include "QMIThread.h"

static int sc_system(const char *shell_cmd) {
    int ret = 0;
    dbg_time("%s", shell_cmd);
    ret = system(shell_cmd);
    if (ret) {
        //dbg_time("Fail to system(\"%s\") = %d, errno: %d (%s)", shell_cmd, ret, errno, strerror(errno));
    }
    return ret;
}

static void sc_set_mtu(const char *usbnet_adapter, int ifru_mtu) {
    int inet_sock;
    struct ifreq ifr;

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (inet_sock > 0) {
        strcpy(ifr.ifr_name, usbnet_adapter);

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

#ifdef ANDROID
static void android_property_set(char *ifname, char *type, uint32_t ipaddr) {
    char shell_cmd[128];
    unsigned char *r = (unsigned char *)&ipaddr;

    snprintf(shell_cmd, sizeof(shell_cmd), "/system/bin/setprop net.%s.%s %d.%d.%d.%d", ifname, type, r[3], r[2], r[1], r[0]);
    sc_system(shell_cmd);
}
#endif

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

        while((fgets(buf, sizeof(buf), udhcpc_fp)) != NULL) {
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

void udhcpc_start(PROFILE_T *profile) {
    char *ifname = profile->usbnet_adapter;
    char shell_cmd[128];

    if (profile->rawIP && profile->ipv4.Address && profile->ipv4.Mtu) {
        sc_set_mtu(profile->usbnet_adapter, (profile->ipv4.Mtu));
    }

#ifdef ANDROID
    if(!access("/system/bin/netcfg", F_OK)) {
        snprintf(shell_cmd, sizeof(shell_cmd), "/system/bin/netcfg %s up", ifname);
        sc_system(shell_cmd);
        snprintf(shell_cmd, sizeof(shell_cmd), "/system/bin/netcfg %s dhcp", ifname);
        sc_system(shell_cmd);
    } else {
        snprintf(shell_cmd, sizeof(shell_cmd), "/system/bin/dhcptool %s", ifname);
        sc_system(shell_cmd);
    }

    android_property_set(ifname, "gw", profile->ipv4.Gateway);
    return;
#endif

    snprintf(shell_cmd, sizeof(shell_cmd) - 1, "ifconfig %s up", ifname);
    sc_system(shell_cmd);

#if 1 //for bridge mode, only one public IP, so donot run udhcpc to obtain
    {
        const char *BRIDGE_MODE_FILE = "/sys/module/GobiNet/parameters/bridge_mode";
        const char *BRIDGE_IPV4_FILE = "/sys/module/GobiNet/parameters/bridge_ipv4";

        if (strncmp(qmichannel, "/dev/qcqmi", strlen("/dev/qcqmi"))) {
            BRIDGE_MODE_FILE = "/sys/module/qmi_wwan/parameters/bridge_mode";
            BRIDGE_IPV4_FILE = "/sys/module/qmi_wwan/parameters/bridge_ipv4";
        }

        if (profile->ipv4.Address && !access(BRIDGE_MODE_FILE, R_OK)) {
            int bridge_fd = open(BRIDGE_MODE_FILE, O_RDONLY);
            char bridge_mode[2] = {0, 0};

            if (bridge_fd > 0) {
                read(bridge_fd, &bridge_mode, sizeof(bridge_mode));
                close(bridge_fd);
                if(bridge_mode[0] != '0') {
                    snprintf(shell_cmd, sizeof(shell_cmd), "echo 0x%08x > %s", profile->ipv4.Address, BRIDGE_IPV4_FILE);
                    sc_system(shell_cmd);
                    return;
                }
            }
        }
    }
#endif

//because must use udhcpc to obtain IP when working on ETH mode,
//so it is better also use udhcpc to obtain IP when working on IP mode.
//use the same policy for all modules

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

            pthread_create(&udhcpc_thread_id, &udhcpc_thread_attr, udhcpc_thread_function, (void*)strdup(udhcpc_cmd));
            sleep(1);
        }

        if (profile->ipv6.Address[0] && profile->ipv6.PrefixLengthIPAddr) {
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
            sc_system(shell_cmd);
            snprintf(udhcpc_cmd, sizeof(udhcpc_cmd), "dibbler-client run");
            dibbler_client_alive++;
#endif

            pthread_create(&udhcpc_thread_id, &udhcpc_thread_attr, udhcpc_thread_function, (void*)strdup(udhcpc_cmd));
        }
    }
}

void udhcpc_stop(PROFILE_T *profile) {
    char *ifname = profile->usbnet_adapter;
    char shell_cmd[128];

#ifdef ANDROID
    if(!access("/system/bin/netcfg", F_OK)) {
        snprintf(shell_cmd, sizeof(shell_cmd) - 1, "/system/bin/netcfg %s down", ifname);
    } else {
        snprintf(shell_cmd, sizeof(shell_cmd) - 1, "ifconfig %s down", ifname); //for android 6.0 and above
    }
#else
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
    snprintf(shell_cmd, sizeof(shell_cmd) - 1, "ifconfig %s down", ifname);
#endif
    sc_system(shell_cmd);
}
