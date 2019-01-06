#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sched.h>
#include <math.h>
#include "colors.h"

#define MAX_PORTS     256
#define RX_BUFMAX     16384
#define DATA_PLANE    "/tmp/mpktgen_dp.sock"
#define CONTROL_PLANE "/tmp/mpktgen_cp.sock"
#define PACKED        __attribute__((__packed__))
#define ERROR(...)    MSG(0, "ERR", ##__VA_ARGS__);
#define INFO(...)     MSG(1, "INF", ##__VA_ARGS__);
#define DEBUG(...)    MSG(2, "DBG", ##__VA_ARGS__);

#define MSG(level,prefix,format,...) { \
    if (verbosity >= level) { \
        fprintf(stderr, prefix ": " format, ##__VA_ARGS__); \
        fprintf (stderr, "\n"); \
        fflush (stderr); \
        fprintf(logfile, prefix ": " format, ##__VA_ARGS__); \
        fprintf (logfile, "\n"); \
        fflush (logfile); \
    } \
}

FILE *logfile;

enum msg_verbosity {error=0, info=1, debug=2};

// uint32_t verbosity = info; // default
uint32_t verbosity = debug; // default

int en_traffic = 0;

// frame descriptor
struct PACKED mpktgen_frame_t {
    // metadata 32 bytes
    uint32_t len : 32;
    uint32_t ipg : 32;
    char pad [24];
    // frame
    uint64_t src : 48;
    uint64_t dst : 48;
    uint32_t lt : 16;
    char payload [512];
};

// commandline arguments
struct mpktgen_opts_t {
    int txbuffer_size;
    int rxbuffer_size;
    int frm_size;  // default 64
    int frm_cnt;   // default 1 million
    int verbosity; // default info
    int pin_start; // default -1
    int cfg_sock_buf;
};

// port descriptor
struct port_t {
    int fd;
    int id;
    uint32_t frm_size;
    uint32_t frm_cnt;
    pthread_t tx;
    pthread_t rx;
    char *rxbuf;
    uint32_t txpin;
    uint32_t rxpin;
    uint32_t do_pin;
};

// data plane server descriptor
struct mpktgen_dp_t {
    struct sockaddr_un addr;
    fd_set events;
    int max_events;
    pthread_t worker;
    int fd;
    uint32_t do_pin;
    struct mpktgen_opts_t *opts;
    int nof_ports_added;
    struct port_t port [MAX_PORTS];
};

// control plane server descriptor
struct mpktgen_cp_t {
    struct sockaddr_un addr;
    fd_set events;
    int max_events;
    pthread_t worker;
    int fd;
    uint32_t do_pin;
    struct mpktgen_opts_t *opts;
};

void open_logfile() {
    logfile = fopen ("run.log", "w+");
    assert (logfile);
}

void close_logfile() {
    close (logfile);
}

struct mpktgen_opts_t *parse_cmdline_opts (int argv, char *argc[]) {
    int opt = 0;
    struct mpktgen_opts_t *op = 
        (struct mpktgen_opts_t*) 
        malloc (sizeof (struct mpktgen_opts_t));
    enum {
        opt_txbuffer_size,
        opt_rxbuffer_size,
        opt_frm_size,
        opt_frm_cnt,
        opt_pin_start,
        opt_cfg_sock_buf,
        opt_verbosity         
    };
    static struct option long_options[] = {
        { "txbuffer_size", required_argument, 0, opt_txbuffer_size },
        { "rxbuffer_size", required_argument, 0, opt_rxbuffer_size },
        { "frm_size"     , required_argument, 0, opt_frm_size      },
        { "frm_cnt"      , required_argument, 0, opt_frm_cnt       },
        { "pin_start"    , required_argument, 0, opt_pin_start     },
        { "verbosity"    , required_argument, 0, opt_verbosity     },
        { "cfg_sock_buf" , required_argument, 0, opt_cfg_sock_buf  },
        { 0, 0, 0, 0 }
    };
    // defaults
    op->txbuffer_size = 10240;
    op->rxbuffer_size = 10240;
    op->frm_size = 64;
    op->frm_cnt = 100;
    op->cfg_sock_buf = -1;
    op->pin_start = -1; // no pinning
    op->verbosity = info;

    while ((opt = getopt_long_only (argv, argc, "", long_options, NULL)) != -1) {
        switch (opt) {
            case opt_txbuffer_size:{
                op->txbuffer_size = atoi(optarg);
                INFO ("Option TX Buffer: %d", op->txbuffer_size);
                break;
            }
            case opt_rxbuffer_size:{
                op->rxbuffer_size = atoi(optarg);
                INFO ("Option RX Buffer: %d", op->rxbuffer_size);
                break;
            }
            case opt_frm_size:{
                op->frm_size = atoi(optarg);
                INFO ("Frame Size: %d", op->frm_size);
                break;
            }
            case opt_frm_cnt:{
                op->frm_cnt = atoi(optarg);
                INFO ("Frame Count: %d", op->frm_cnt);
                break;
            }
            case opt_pin_start:{
                op->pin_start = atoi(optarg);
                INFO ("Start CPU pinning from: %d", op->pin_start);
                break;
            }
            case opt_verbosity:{
                INFO ("Verbosity: %d", verbosity);
                verbosity = atoi(optarg);
                break;
            }
            case opt_cfg_sock_buf:{
                INFO ("Configure Socket Buffers Enabled");
                op->cfg_sock_buf = 1;
                break;
            }
            default:{
                ERROR ("%s", "Unsupported Option");
                exit(0);
                break;
            }
        }
    }
    return op;
}

int init_dp_controller (struct mpktgen_dp_t *dp) {
    INFO ("Creating data plane socket server");
    int idx = 0;
    socklen_t len;
    // initialize descriptor
    FD_ZERO (&dp->events);
    dp->max_events = -1;
    dp->fd = -1;
    dp->nof_ports_added = 0;
    for (idx=0; idx<MAX_PORTS; idx++) {
        dp->port[idx].fd = -1;
        dp->port[idx].id = 0;
        // TODO allocate port rxbuf on port request
    }
    if ((dp->fd = socket (AF_UNIX, SOCK_STREAM, 0)) == -1) {
        ERROR ("Server Creation: %s", strerror(errno));
        exit (0);
    }
    len = sizeof (struct sockaddr_un);
    dp->addr.sun_family = AF_UNIX;
    strncpy (dp->addr.sun_path, DATA_PLANE, (len-sizeof (short)));
    unlink (dp->addr.sun_path); 

    if ((bind (dp->fd, (struct sockaddr*) &dp->addr, len)) == -1) {
        ERROR ("Server Bind: %s", strerror(errno));
        exit (0);
    }
    DEBUG ("Socket server fd: %d", dp->fd);
    listen (dp->fd, MAX_PORTS); 
    FD_SET (dp->fd, &dp->events);

    if (dp->fd > dp->max_events) {
        dp->max_events = dp->fd;
    }
}

int init_cp_controller (struct mpktgen_cp_t *cp) {
    INFO ("Creating control plane socket server");
    int idx = 0;
    socklen_t len;
    // initialize descriptor
    FD_ZERO (&cp->events);
    cp->max_events = -1;
    cp->fd = -1;
    if ((cp->fd = socket (AF_UNIX, SOCK_SEQPACKET, 0)) == -1) {
        ERROR ("Server Creation: %s", strerror(errno));
        exit (0);
    }
    len = sizeof (struct sockaddr_un);
    cp->addr.sun_family = AF_UNIX;
    strncpy (cp->addr.sun_path, CONTROL_PLANE, (len-sizeof (short)));
    unlink (cp->addr.sun_path); 

    if ((bind (cp->fd, (struct sockaddr*) &cp->addr, len)) == -1) {
        ERROR ("Server Bind: %s", strerror(errno));
        exit (0);
    }
    DEBUG ("Socket server fd: %d", cp->fd);
    listen (cp->fd, 8); // an arbitrary choice
    FD_SET (cp->fd, &cp->events);

    if (cp->fd > cp->max_events) {
        cp->max_events = cp->fd;
    }
}

int cleanup (struct mpktgen_dp_t *dp, struct mpktgen_cp_t *cp) {
    close(dp->fd);
    close(cp->fd);
}

void print_cdata (unsigned char* tmp, int len) {
    uint32_t idx = 0;
    int plen = 16;
    printf ("Packet Len: %d\n", len);

    int x;
    for (x=0; x<len/plen; x++) {
        int y;
        for (y=0; y<plen; y++) {
            printf("%02x ", (uint16_t)tmp[idx]);
            if (idx%8==7) printf("  ");
            idx++;
        }
        printf("\n");
        if (x%4==3) printf("\n");
    }
    for (x=idx; x<len; x++) {
       printf ("%02x ", (uint16_t)tmp[idx]);
       if (idx%8==7) printf("  ");
       idx++;
    }
    printf("\n");
    fflush (stdout);
}

void *tx_worker (void *arg) {
    uint32_t idx;
    uint32_t cnt = 0;
    struct port_t *p = (struct port_t*) arg;
    INFO ("TX Worker started for port (%d)", p->id);

    // cpu pin
    if (p->do_pin) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(p->txpin, &cpuset);
        pthread_setaffinity_np (pthread_self(), sizeof (cpu_set_t), &cpuset);
    }
    // generate a frame with metadata
    struct mpktgen_frame_t *frm = 
        (struct mpktgen_frame_t*) 
        malloc (sizeof (struct mpktgen_frame_t));
    memset((char*)frm, 0, sizeof(struct mpktgen_frame_t));

    frm->ipg = 12;
    frm->len = p->frm_size;
    frm->lt = 0x8888;
    frm->src = 0xaabbccddeeffull;
    frm->dst = 0x112233445566ull;

    // effective payload size
    // 18=6+6+2+4 (enet hdr fields)
    int pl_size = p->frm_size-18; 

    for (idx=0; idx<pl_size; idx++) {
        frm->payload[idx] = (uint16_t)idx;
    }
    // nof 64 byte elements to be sent
    int len64 = (ceil)((float)(p->frm_size+32)/64);
    DEBUG("port[%d]:FrmSize:(%d):Nof 64 Byte Elems:%d", p->id, p->frm_size, len64);

    DEBUG("frm.ipg: %x",frm->ipg);
    DEBUG("frm.len: %x",frm->len);
    DEBUG("frm.src: %llx",frm->src);
    DEBUG("frm.dst: %llx",frm->dst);
    DEBUG("frm.lt : %x",frm->lt);

    if (verbosity==2) { // debug verbosity
        print_cdata((char*)frm, (len64*64));
    }
    while (1) {
        if (en_traffic) { // controlled from GUI app
            if (cnt < p->frm_cnt) {
                INFO ("port[%d]:Sending Frame:len(%d):Cnt(%d)", p->id, frm->len, cnt);
                send (p->fd, frm, (len64*64), 0);
                cnt++;
            }
        } else {
            sleep(1);
        }
    }
}

void *rx_worker (void *arg) {
    struct port_t *p = (struct port_t*) arg;
    INFO ("RX Worker started for port (%d)", p->id);
    
    if (p->do_pin) { // cmdline param
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(p->rxpin, &cpuset);
        pthread_setaffinity_np (pthread_self(), sizeof (cpu_set_t), &cpuset);
    }
    while (1) {
        if (en_traffic) {
            read (p->fd, p->rxbuf, RX_BUFMAX, 0);
        } else {
            sleep(1);
        }
    }
}

void start_port_worker (struct port_t *p) {
    pthread_create (&p->tx, NULL, *tx_worker, p);
    pthread_create (&p->rx, NULL, *rx_worker, p);
    // name the threads
    char thread_name[80];
    sprintf(thread_name, "txw%d", p->id);
    pthread_setname_np(p->tx, thread_name);
    sprintf(thread_name, "rxw%d", p->id);
    pthread_setname_np(p->rx, thread_name);
}

char *readable_fs (double size, char *buf) {
    int i = 0;
    memset (buf, 0, 100);
    const char* units[] = {"B","kB","MB","GB","TB","PB","EB","ZB","YB"};
    while (size > 1023) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.0f %s", i, size, units[i]);
    return buf;
}

void config_dp_buffer (struct port_t *p, uint32_t txsize, uint32_t rxsize) {
    char buf[100];
    int bufsize; 
    socklen_t bufsize_len;
    bufsize_len = sizeof(bufsize);

    // current size
    getsockopt(p->fd, SOL_SOCKET, SO_SNDBUF, &bufsize, &bufsize_len);
    DEBUG("Port(%d): Current TX Socket size: %s", p->id, readable_fs(bufsize, buf));
    getsockopt(p->fd, SOL_SOCKET, SO_RCVBUF, &bufsize, &bufsize_len);
    DEBUG("Port(%d): Current RX Socket size: %s", p->id, readable_fs(bufsize, buf));

    // setting new size
    bufsize = txsize*1024*1024;
    setsockopt(p->fd, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize_len));
    bufsize = rxsize*1024*1024;
    setsockopt(p->fd, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize_len));

    // check new socket size
    getsockopt(p->fd, SOL_SOCKET, SO_SNDBUF, &bufsize, &bufsize_len);
    INFO("Port(%d): Update TX Socket size: %s", p->id, readable_fs(bufsize, buf));
    getsockopt(p->fd, SOL_SOCKET, SO_RCVBUF, &bufsize, &bufsize_len);
    INFO("Port(%d): Updated RX Socket size: %s", p->id, readable_fs(bufsize, buf));
}

void *dp_worker(void *arg) {
    struct mpktgen_dp_t *s = (struct mpktgen_dp_t*) arg;
    int fd;
    int ret;
    // cpu pin
    if (s->opts->pin_start != -1) {
        s->do_pin = 1;
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(s->opts->pin_start, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof (cpu_set_t), &cpuset);
        s->opts->pin_start += 1;
    }
    INFO ("DP: Waiting for connection requests...");
    while (1) {
        uint32_t timeout = 200000;
        struct timeval tv;
        tv.tv_sec = timeout/1000000;
        tv.tv_usec = timeout%1000000;
        fd_set events = s->events;

        ret = select(s->max_events+1, &events, 0, 0, &tv);
        if (ret == -1) {
            ERROR("Select: %s", strerror(errno));
            exit(0);
        }
        if (ret == 0) {
            continue;
        }
        for (fd=0; fd<s->max_events+1; fd++) {
            if (FD_ISSET(fd, &events) && FD_ISSET(fd, &s->events)) {
                if (fd == s->fd) {
                    struct sockaddr_un un;
                    socklen_t len = sizeof(un);
                    int client_fd = accept(s->fd, (struct sockaddr*) &un, &len);

                    INFO ("Dataplane connection request accepted, adding port: %d", s->nof_ports_added);
                    s->port[s->nof_ports_added].fd = client_fd;
                    s->port[s->nof_ports_added].id = s->nof_ports_added;
                    s->port[s->nof_ports_added].frm_size = s->opts->frm_size;
                    s->port[s->nof_ports_added].frm_cnt = s->opts->frm_cnt;
                    s->port[s->nof_ports_added].rxbuf = (char*) malloc (RX_BUFMAX);

                    if (s->opts->cfg_sock_buf != -1) {
                        config_dp_buffer(
                            &s->port[s->nof_ports_added], 
                            s->opts->txbuffer_size, 
                            s->opts->rxbuffer_size);
                    }
                    // pin logic
                    if (s->opts->pin_start != -1) {
                        s->port[s->nof_ports_added].do_pin = 1;
                        s->port[s->nof_ports_added].txpin = s->opts->pin_start;
                        s->opts->pin_start += 1;
                        s->port[s->nof_ports_added].rxpin = s->opts->pin_start;
                        s->opts->pin_start += 1;
                    }
                    start_port_worker(&s->port[s->nof_ports_added]);
                    s->nof_ports_added += 1;
                }
            }
        }
    }
}

void *cp_worker(void *arg) {
    INFO ("CP: Waiting for Control Commands...");
    struct mpktgen_cp_t *cp = (struct mpktgen_cp_t*) arg;
    int fd;
    int ret;

    // pin dp and cp on same CPU
    if (cp->opts->pin_start != -1) {
        cp->do_pin = 1;
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cp->opts->pin_start, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof (cpu_set_t), &cpuset);
    }
    while (1) {
        uint32_t timeout = 200000;
        struct timeval tv;
        tv.tv_sec = timeout/1000000;
        tv.tv_usec = timeout%1000000;
        fd_set events = cp->events;

        ret = select (cp->max_events+1, &events, 0, 0, &tv);
        if (ret == -1) {
            ERROR("Select: %s", strerror(errno));
            exit(0);
        }
        if (ret == 0) {
            continue;
        }
        for (fd=0; fd<cp->max_events+1; fd++) {
            if (FD_ISSET(fd, &events) && FD_ISSET(fd, &cp->events)) {
                if (fd == cp->fd) {
                    struct sockaddr_un un;
                    socklen_t len = sizeof (un);
                    int client_fd = accept (cp->fd, (struct sockaddr*) &un, &len);
                    INFO("Controlplane connection request accepted (%d)", client_fd);
                    FD_SET(client_fd, &cp->events);
                    if (client_fd>cp->max_events)
                        cp->max_events = client_fd;
                } else { 
                    // process commands from gui and avip
                    char cmd[32];
                    int nbytes = recv(fd, (char*)&cmd, 32, 0);

                    if (nbytes!=0) {
                        if (strcmp((char*)&cmd, "start")==0) {
                            INFO(FGRN("Command Received:Start Traffic")RST);
                            fflush(stdout);
                            en_traffic = 1;
                        } else if (strcmp((char*)&cmd, "stop")==0) {
                            INFO(FGRN("Command Received:Stop Traffic")RST);
                            fflush(stdout);
                            en_traffic = 0;
                        } else {
                            INFO(FRED("Invalid Command Received")RST);
                            fflush(stdout);
                        }
                    } else {
                        INFO("Controlplane connection closed (%d)", fd);
                        FD_CLR(fd, &cp->events);
                        close(fd);
                        en_traffic = 0;
                    }
                }
            }
        }
    }
}

void run_dp_controller(struct mpktgen_dp_t *dp) {
    pthread_create(&dp->worker, NULL, *dp_worker, dp);
    pthread_setname_np(dp->worker, "dp_worker");
}

void run_cp_controller(struct mpktgen_cp_t *cp) {
    pthread_create(&cp->worker, NULL, *cp_worker, cp);
    pthread_setname_np(cp->worker, "cp_worker");
}

int main (int argc, char * argv[]) {
    open_logfile();

    // create data plane conroller
    struct mpktgen_dp_t *dp = 
        (struct mpktgen_dp_t*) 
        malloc(sizeof(struct mpktgen_dp_t));

    // create control plane conroller
    struct mpktgen_cp_t *cp = 
        (struct mpktgen_cp_t*)
        malloc(sizeof(struct mpktgen_cp_t));

    // parse and register commandline aguments
    struct mpktgen_opts_t *opts = parse_cmdline_opts(argc, argv);
    dp->opts = opts;
    cp->opts = opts;

    // initialize data and control plane with
    // default paremeters
    init_dp_controller(dp);
    init_cp_controller(cp);

    // start controllers
    run_dp_controller(dp);
    run_cp_controller(cp);

    pthread_join(dp->worker, NULL);
    pthread_join(cp->worker, NULL);

    // cleanup
    cleanup(dp, cp); 

    close_logfile();
    return 0;
}
