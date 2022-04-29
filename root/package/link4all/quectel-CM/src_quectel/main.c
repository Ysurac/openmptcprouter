#include "QMIThread.h"
#include <sys/wait.h>
#include <sys/utsname.h>
#include <dirent.h>

#define POLL_DATA_CALL_STATE_SECONDS 15 //poll data call state, for qmi ind maybe not work well

int debug_qmi = 0;
int main_loop = 0;
char * qmichannel;
int qmidevice_control_fd[2];
static int signal_control_fd[2];

#ifdef LINUX_RIL_SHLIB
UINT ifc_get_addr(const char *ifname);
static int check_ipv4_address(PROFILE_T *profile);
#endif

static void usbnet_link_change(int link, PROFILE_T *profile) {
    static int s_link = -1;

    if (s_link == link)
        return;
    
    s_link = link;

    if (link)
        udhcpc_start(profile);
    else
        udhcpc_stop(profile);

#ifdef LINUX_RIL_SHLIB
    if (link) {
        while (ifc_get_addr(profile->usbnet_adapter) == 0) {
            sleep(1);
        }
    }

    if (link && requestGetIPAddress(profile) == 0) {
        unsigned char *r;

        dbg_time("Using interface %s", profile->usbnet_adapter);
        r = (unsigned char *)&profile->ipv4.Address;
        dbg_time("local  IP address %d.%d.%d.%d", r[3], r[2], r[1], r[0]);
        r = (unsigned char *)&profile->ipv4.Gateway;
        dbg_time("remote IP address %d.%d.%d.%d", r[3], r[2], r[1], r[0]);
        r = (unsigned char *)&profile->ipv4.DnsPrimary;
        dbg_time("primary   DNS address %d.%d.%d.%d", r[3], r[2], r[1], r[0]);
        r = (unsigned char *)&profile->ipv4.DnsSecondary;
        dbg_time("secondary DNS address %d.%d.%d.%d", r[3], r[2], r[1], r[0]);
     } 
#endif
}

UINT ifc_get_addr(const char *ifname);
static int check_ipv4_address(PROFILE_T *profile) {
    if (requestGetIPAddress(profile) == 0) {
         UINT localIP = ifc_get_addr(profile->usbnet_adapter);
         UINT remoteIP = ql_swap32(profile->ipv4.Address);
         unsigned char *l = (unsigned char *)&localIP;
         unsigned char *r = (unsigned char *)&remoteIP;
         if (remoteIP != remoteIP || debug_qmi)
             dbg_time("localIP: %d.%d.%d.%d VS remoteIP: %d.%d.%d.%d",
                     l[0], l[1], l[2], l[3], r[0], r[1], r[2], r[3]);
         if (profile->IPType == 0x04)
            return (localIP == remoteIP);
    }
    return 0;
}

static void main_send_event_to_qmidevice(int triger_event) {
     write(qmidevice_control_fd[0], &triger_event, sizeof(triger_event));
}

static void send_signo_to_main(int signo) {
     write(signal_control_fd[0], &signo, sizeof(signo));
}

void qmidevice_send_event_to_main(int triger_event) {
     write(qmidevice_control_fd[1], &triger_event, sizeof(triger_event));
}

#define MAX_PATH 256

static int ls_dir(const char *dir, int (*match)(const char *dir, const char *file, void *argv[]), void *argv[])
{
    DIR *pDir;  
    struct dirent* ent = NULL; 
    int match_times = 0;

    pDir = opendir(dir);
    if (pDir == NULL)  {  
        dbg_time("Cannot open directory: %s, errno: %d (%s)", dir, errno, strerror(errno));  
        return 0;  
    } 

    while ((ent = readdir(pDir)) != NULL)  {
        match_times += match(dir, ent->d_name, argv);
    }    
    closedir(pDir);

    return match_times;
}

static int is_same_linkfile(const char *dir, const char *file,  void *argv[]) 
{
    const char *qmichannel = (const char *)argv[1];
    char linkname[MAX_PATH];
    char filename[MAX_PATH];
    int linksize;

    snprintf(linkname, MAX_PATH, "%s/%s", dir, file);  
    linksize = readlink(linkname, filename, MAX_PATH);
    if (linksize <= 0)
        return 0;

    filename[linksize] = 0;
    if (strcmp(filename, qmichannel))
        return 0;
    
    dbg_time("%s -> %s", linkname, filename);
    return 1;
}

static int is_brother_process(const char *dir, const char *file, void *argv[]) 
{
    //const char *myself = (const char *)argv[0];
    char linkname[MAX_PATH];
    char filename[MAX_PATH];
    int linksize;
    int i = 0, kill_timeout = 15;
    pid_t pid;
        
    //dbg_time("%s", file);
    while (file[i]) {
        if (!isdigit(file[i]))
            break;
        i++;
    }
    
	if (file[i]) {
        //dbg_time("%s not digit", file);           
        return 0;
    }

    snprintf(linkname, MAX_PATH, "%s/%s/exe", dir, file);  
    linksize = readlink(linkname, filename, MAX_PATH);
    if (linksize <= 0)
        return 0;
    
    filename[linksize] = 0;
#if 0 //check all process
    if (strcmp(filename, myself))
        return 0;
#endif
   
    pid = atoi(file);
    if (pid == getpid())
        return 0;
    
    snprintf(linkname, MAX_PATH, "%s/%s/fd", dir, file);  
    if (!ls_dir(linkname, is_same_linkfile, argv))
        return 0;
    
    dbg_time("%s/%s/exe -> %s", dir, file, filename);
	while (kill_timeout-- && !kill(pid, 0))
	{
        kill(pid, SIGTERM);
        sleep(1);
	}
    if (!kill(pid, 0))
    {
        dbg_time("force kill %s/%s/exe -> %s", dir, file, filename);
        kill(pid, SIGKILL);
        sleep(1);
    }

    return 1;    
}

static int kill_brothers(const char *qmichannel)
{
    char myself[MAX_PATH];
    int filenamesize;
    void *argv[2] = {myself, (void *)qmichannel};

    filenamesize = readlink("/proc/self/exe", myself, MAX_PATH);
    if (filenamesize <= 0)
        return 0;
    myself[filenamesize] = 0;   

    if (ls_dir("/proc", is_brother_process, argv))
		sleep(1);

	return 0;
}

static void ql_sigaction(int signo) {
     if (SIGCHLD == signo)
         waitpid(-1, NULL, WNOHANG);
     else if (SIGALRM == signo)
         send_signo_to_main(SIGUSR1);
     else
     {
        if (SIGTERM == signo || SIGHUP == signo || SIGINT == signo)
            main_loop = 0;
         send_signo_to_main(signo);
        main_send_event_to_qmidevice(signo); //main may be wating qmi response
    }
}

pthread_t gQmiThreadID;

static int usage(const char *progname) {
     dbg_time("Usage: %s [-s [apn [user password auth]]] [-p pincode] [-f logfilename] ", progname);
     dbg_time("-s [apn [user password auth]] Set apn/user/password/auth get from your network provider");
     dbg_time("-p pincode                    Verify sim card pin if sim card is locked");
     dbg_time("-f logfilename                Save log message of this program to file");
     dbg_time("Example 1: %s ", progname);
     dbg_time("Example 2: %s -s 3gnet ", progname);
     dbg_time("Example 3: %s -s 3gnet carl 1234 0 -p 1234 -f gobinet_log.txt", progname);
     return 0;
}

static int qmidevice_detect(char **pp_qmichannel, char **pp_usbnet_adapter) {
    struct dirent* ent = NULL;
    DIR *pDir;
#if 0 //ndef ANDROID
    int osmaj, osmin, ospatch;
    static struct utsname utsname;    /* for the kernel version */
    static int kernel_version;
#define KVERSION(j,n,p)    ((j)*1000000 + (n)*1000 + (p))

    /* get the kernel version now, since we are called before sys_init */
    uname(&utsname);
    osmaj = osmin = ospatch = 0;
    sscanf(utsname.release, "%d.%d.%d", &osmaj, &osmin, &ospatch);
    kernel_version = KVERSION(osmaj, osmin, ospatch);
#endif

    if ((pDir = opendir("/dev")) == NULL)  {
        dbg_time("Cannot open directory: %s, errno:%d (%s)", "/dev", errno, strerror(errno));
        return -ENODEV;
    }

    while ((ent = readdir(pDir)) != NULL) {
        if ((strncmp(ent->d_name, "cdc-wdm", strlen("cdc-wdm")) == 0) || (strncmp(ent->d_name, "qcqmi", strlen("qcqmi")) == 0)) {
            char net_path[64];

            *pp_qmichannel = (char *)malloc(32);
            sprintf(*pp_qmichannel, "/dev/%s", ent->d_name);
            dbg_time("Find qmichannel = %s", *pp_qmichannel);

            if (strncmp(ent->d_name, "cdc-wdm", strlen("cdc-wdm")) == 0)
                sprintf(net_path, "/sys/class/net/wwan%s", &ent->d_name[strlen("cdc-wdm")]);
            else
            {
                sprintf(net_path, "/sys/class/net/usb%s", &ent->d_name[strlen("qcqmi")]);
                #if 0//ndef ANDROID
                if (kernel_version >= KVERSION( 2,6,39 ))
                    sprintf(net_path, "/sys/class/net/eth%s", &ent->d_name[strlen("qcqmi")]);
                #else
                if (access(net_path, R_OK) && errno == ENOENT)
                    sprintf(net_path, "/sys/class/net/eth%s", &ent->d_name[strlen("qcqmi")]);
                #endif
            }

            if (access(net_path, R_OK) == 0)
            {
                if (*pp_usbnet_adapter && strcmp(*pp_usbnet_adapter, (net_path + strlen("/sys/class/net/"))))
                {
                    free(*pp_qmichannel); *pp_qmichannel = NULL;
                    continue;
                }
                *pp_usbnet_adapter = strdup(net_path + strlen("/sys/class/net/"));
                dbg_time("Find usbnet_adapter = %s", *pp_usbnet_adapter);
                break;
            }
            else
            {
                dbg_time("Failed to access %s, errno:%d (%s)", net_path, errno, strerror(errno));
                free(*pp_qmichannel); *pp_qmichannel = NULL;
            }
        }
    }
    closedir(pDir);

    return (*pp_qmichannel && *pp_usbnet_adapter);
}

#if defined(ANDROID) || defined(LINUX_RIL_SHLIB)
int quectel_CM(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    int triger_event = 0;
    int opt = 1;
    int signo;
#ifdef CONFIG_SIM
    SIM_Status SIMStatus;
#endif
    UCHAR PSAttachedState;
    UCHAR  ConnectionStatus = 0xff; //unknow state
    char * save_usbnet_adapter = NULL;
    PROFILE_T profile;
    int slient_seconds = 0;

    memset(&profile, 0x00, sizeof(profile));
#if CONFIG_DEFAULT_PDP 
    profile.pdp = CONFIG_DEFAULT_PDP;
#else
    profile.pdp = 1;
#endif
    profile.IPType = 0x04; //ipv4 first
    
    if (!strcmp(argv[argc-1], "&"))
        argc--;

    opt = 1;
    while  (opt < argc)
    {
        if (argv[opt][0] != '-')
            return usage(argv[0]);

        switch (argv[opt++][1])
        {
#define has_more_argv() ((opt < argc) && (argv[opt][0] != '-'))
            case 's':
                profile.apn = profile.user = profile.password = "";
                if (has_more_argv())
                    profile.apn = argv[opt++];
                if (has_more_argv())
                    profile.user = argv[opt++];
                if (has_more_argv())
                {
                    profile.password = argv[opt++];
                    if (profile.password && profile.password[0])
                        profile.auth = 2; //default chap, customers may miss auth
                }
                if (has_more_argv())
                    profile.auth = argv[opt++][0] - '0';
            break;
            
            case 'p':
                if (has_more_argv())
                    profile.pincode = argv[opt++];
            break;
            
            case 'n':
                if (has_more_argv())
                    profile.pdp = argv[opt++][0] - '0';
            break;

            case 'f':
                if (has_more_argv())
                {
                    const char * filename = argv[opt++];
                    logfilefp = fopen(filename, "a+");
                    if (!logfilefp) {
                        dbg_time("Fail to open %s, errno: %d(%s)", filename, errno, strerror(errno));
                     }
                }
            break;

            case 'i':
                if (has_more_argv())
                    profile.usbnet_adapter = save_usbnet_adapter = argv[opt++];
            break;

            case 'v':
                debug_qmi = 1;
            break;

            case 'l':
                main_loop = 1;
            break;

            default:               
                return usage(argv[0]);
            break;
        }
    }

    dbg_time("Quectel_Linux_ConnectManager_SR01A01V21");
    dbg_time("%s profile[%d] = %s/%s/%s/%d, pincode = %s", argv[0], profile.pdp, profile.apn, profile.user, profile.password, profile.auth, profile.pincode);

    signal(SIGUSR1, ql_sigaction);
    signal(SIGUSR2, ql_sigaction);
    signal(SIGINT, ql_sigaction);
    signal(SIGTERM, ql_sigaction);
    signal(SIGHUP, ql_sigaction);
    signal(SIGCHLD, ql_sigaction);
    signal(SIGALRM, ql_sigaction);

    if (socketpair( AF_LOCAL, SOCK_STREAM, 0, signal_control_fd) < 0 ) {
        dbg_time("%s Faild to create main_control_fd: %d (%s)", __func__, errno, strerror(errno));
        return -1;
    }

    if ( socketpair( AF_LOCAL, SOCK_STREAM, 0, qmidevice_control_fd ) < 0 ) {
        dbg_time("%s Failed to create thread control socket pair: %d (%s)", __func__, errno, strerror(errno));
        return 0;
    }

//sudo apt-get install udhcpc
//sudo apt-get remove ModemManager
__main_loop:
    while (!profile.qmichannel)
    {
        if (qmidevice_detect(&profile.qmichannel, &profile.usbnet_adapter))
            break;
        if (main_loop)
        {
            int wait_for_device = 3000;
            dbg_time("Wait for Quectel modules connect");
            while (wait_for_device && main_loop) {
                wait_for_device -= 100;
                usleep(100*1000);
            }
            continue;
        }
        dbg_time("Cannot find qmichannel(%s) usbnet_adapter(%s) for Quectel modules", profile.qmichannel, profile.usbnet_adapter);
        return -ENODEV;
    }

    if (access(profile.qmichannel, R_OK | W_OK)) {
        dbg_time("Fail to access %s, errno: %d (%s)", profile.qmichannel, errno, strerror(errno));
        return errno;
    }

#if 0 //for test only, make fd > 255
{
    int max_dup = 255;
    while (max_dup--)
        dup(0);
}
#endif

    kill_brothers(profile.qmichannel);

    qmichannel = profile.qmichannel;
    if (!strncmp(profile.qmichannel, "/dev/qcqmi", strlen("/dev/qcqmi")))
    {
        if (pthread_create( &gQmiThreadID, 0, GobiNetThread, (void *)profile.qmichannel) != 0)
        {
            dbg_time("%s Failed to create GobiNetThread: %d (%s)", __func__, errno, strerror(errno));
            return 0;
        }
    }
    else
    {
        if (pthread_create( &gQmiThreadID, 0, QmiWwanThread, (void *)profile.qmichannel) != 0)
        {
            dbg_time("%s Failed to create QmiWwanThread: %d (%s)", __func__, errno, strerror(errno));
            return 0;
        }
    }

    if ((read(qmidevice_control_fd[0], &triger_event, sizeof(triger_event)) != sizeof(triger_event))
        || (triger_event != RIL_INDICATE_DEVICE_CONNECTED)) {
        dbg_time("%s Failed to init QMIThread: %d (%s)", __func__, errno, strerror(errno));
        return 0;
    }

    if (!strncmp(profile.qmichannel, "/dev/cdc-wdm", strlen("/dev/cdc-wdm"))) {
        if (QmiWwanInit()) {
            dbg_time("%s Failed to QmiWwanInit: %d (%s)", __func__, errno, strerror(errno));
            return 0;
        }
    }

#ifdef CONFIG_VERSION
    requestBaseBandVersion(NULL);
#endif
    requestSetEthMode(&profile);
    if (profile.rawIP && !strncmp(profile.qmichannel, "/dev/cdc-wdm", strlen("/dev/cdc-wdm"))) {
        char raw_ip_switch[128] = {0};
        sprintf(raw_ip_switch, "/sys/class/net/%s/qmi/raw_ip", profile.usbnet_adapter);
        if (!access(raw_ip_switch, R_OK)) {
            int raw_ip_fd = -1;
            raw_ip_fd = open(raw_ip_switch, O_RDWR);
            if (raw_ip_fd >= 0) {
                write(raw_ip_fd, "1", strlen("1"));
                close(raw_ip_fd);
                raw_ip_fd = -1;
            } else {
                dbg_time("open %s failed, errno = %d(%s)\n", raw_ip_switch, errno, strerror(errno));
            }
        }
    }
#ifdef CONFIG_SIM
    requestGetSIMStatus(&SIMStatus);
    if ((SIMStatus == SIM_PIN) && profile.pincode) {
        requestEnterSimPin(profile.pincode);
    }
#endif
#ifdef CONFIG_APN
    if (profile.apn || profile.user || profile.password) {
        requestSetProfile(&profile);
    }
    requestGetProfile(&profile);
#endif
    requestRegistrationState(&PSAttachedState);

    if (!requestQueryDataCall(&ConnectionStatus) && (QWDS_PKT_DATA_CONNECTED == ConnectionStatus))
        usbnet_link_change(1, &profile);
     else
        usbnet_link_change(0, &profile);

    send_signo_to_main(SIGUSR1);                                   

    while (1)
    {
        struct pollfd pollfds[] = {{signal_control_fd[1], POLLIN, 0}, {qmidevice_control_fd[0], POLLIN, 0}};
        int ne, ret, nevents = sizeof(pollfds)/sizeof(pollfds[0]);
                   
        do {
            ret = poll(pollfds, nevents, (ConnectionStatus == QWDS_PKT_DATA_CONNECTED) ? 1000 : -1);
        } while ((ret < 0) && (errno == EINTR));

        if (ret == 0)
        {
            #if POLL_DATA_CALL_STATE_SECONDS
            if (++slient_seconds >= POLL_DATA_CALL_STATE_SECONDS)
                send_signo_to_main(SIGUSR2);
            #endif
            continue;
        }
        slient_seconds = 0;

        if (ret <= 0) {
            dbg_time("%s poll=%d, errno: %d (%s)", __func__, ret, errno, strerror(errno));
            goto __main_quit;
        }

        for (ne = 0; ne < nevents; ne++) {
            int fd = pollfds[ne].fd;
            short revents = pollfds[ne].revents;

            if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
                dbg_time("%s poll err/hup", __func__);
                dbg_time("epoll fd = %d, events = 0x%04x", fd, revents);
                main_send_event_to_qmidevice(RIL_REQUEST_QUIT);
                if (revents & POLLHUP)
                    goto __main_quit;
            }

            if ((revents & POLLIN) == 0)
                continue;

            if (fd == signal_control_fd[1])
            {
                if (read(fd, &signo, sizeof(signo)) == sizeof(signo))
                {
                    alarm(0);
                    switch (signo)
                    {
                        case SIGUSR1:
                            requestQueryDataCall(&ConnectionStatus);
                            if (QWDS_PKT_DATA_CONNECTED != ConnectionStatus)
                            {
                                usbnet_link_change(0, &profile);
                                requestRegistrationState(&PSAttachedState);
                                if (PSAttachedState == 1 && requestSetupDataCall(&profile) == 0)
                                {
                                    //succssful setup data call
                                }
                                else
                                {
                                    alarm(5); //try to setup data call 5 seconds later
                                }                                    
                            } 
                        break;
                        
                        case SIGUSR2:
                            if (QWDS_PKT_DATA_CONNECTED == ConnectionStatus)
                            {
                                 requestQueryDataCall(&ConnectionStatus);
                            }
                            if (QWDS_PKT_DATA_CONNECTED != ConnectionStatus || check_ipv4_address(&profile) == 0) //local ip is different with remote ip
                            {
                                requestDeactivateDefaultPDP();
                                #if defined(ANDROID) || defined(LINUX_RIL_SHLIB)
                                kill(getpid(), SIGTERM); //android will setup data call again
                                #else
                                send_signo_to_main(SIGUSR1);                                   
                                #endif
                            }
                        break;
                        
                        case SIGTERM:
                        case SIGHUP:
                        case SIGINT:
                            if (QWDS_PKT_DATA_CONNECTED == ConnectionStatus)
                                requestDeactivateDefaultPDP();
                            usbnet_link_change(0, &profile);
                            if (!strncmp(profile.qmichannel, "/dev/cdc-wdm", strlen("/dev/cdc-wdm")))
                                QmiWwanDeInit();
                            main_send_event_to_qmidevice(RIL_REQUEST_QUIT);
                            goto __main_quit;
                        break;
                        
                        default:
                        break;
                    }
                }
            }

            if (fd == qmidevice_control_fd[0]) {
                if (read(fd, &triger_event, sizeof(triger_event)) == sizeof(triger_event)) {
                    switch (triger_event) {
                        case RIL_INDICATE_DEVICE_DISCONNECTED:
                            usbnet_link_change(0, &profile);
                            if (main_loop)
                            {
                                if (pthread_join(gQmiThreadID, NULL)) {
                                    dbg_time("%s Error joining to listener thread (%s)", __func__, strerror(errno));
                                }
                                profile.qmichannel = NULL;
                                profile.usbnet_adapter = save_usbnet_adapter;
                                goto __main_loop;
                            }
                            goto __main_quit;
                        break;
                        
                        case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED:
                            requestRegistrationState(&PSAttachedState);
                            if (PSAttachedState == 1 && QWDS_PKT_DATA_DISCONNECTED == ConnectionStatus)
                                send_signo_to_main(SIGUSR1);
                        break;
                        
                        case RIL_UNSOL_DATA_CALL_LIST_CHANGED:
                        {
                            #if defined(ANDROID) || defined(LINUX_RIL_SHLIB)
                            UCHAR oldConnectionStatus = ConnectionStatus;
                            #endif
                            requestQueryDataCall(&ConnectionStatus);
                            if (QWDS_PKT_DATA_CONNECTED != ConnectionStatus)
                            {
                                usbnet_link_change(0, &profile);
                                #if defined(ANDROID) || defined(LINUX_RIL_SHLIB)
                                if (oldConnectionStatus == QWDS_PKT_DATA_CONNECTED) //connected change to disconnect
                                    kill(getpid(), SIGTERM); //android will setup data call again
                                #else
                                send_signo_to_main(SIGUSR1);                                   
                                #endif
                            } else if (QWDS_PKT_DATA_CONNECTED == ConnectionStatus) {
                                requestGetIPAddress(&profile);
                                usbnet_link_change(1, &profile);
                            }
                        }
                        break;
                        
                        default:
                        break;
                    }
                }
            }
        }
    }

__main_quit:
    usbnet_link_change(0, &profile);
    if (pthread_join(gQmiThreadID, NULL)) {
        dbg_time("%s Error joining to listener thread (%s)", __func__, strerror(errno));
    }
    close(signal_control_fd[0]);
    close(signal_control_fd[1]);
    close(qmidevice_control_fd[0]);
    close(qmidevice_control_fd[1]);
    dbg_time("%s exit", __func__);
    if (logfilefp)
        fclose(logfilefp);

    return 0;
}
