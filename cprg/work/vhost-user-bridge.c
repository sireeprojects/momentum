#define _FILE_OFFSET_BITS 64

#include "qemu/osdep.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <sys/eventfd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/vhost.h>

#include "qemu/atomic.h"
#include "standard-headers/linux/virtio_net.h"
#include "standard-headers/linux/virtio_ring.h"

static void
vubr_die(const char *s)
{
    perror(s);
    exit(1);
}

static int
dispatcher_init(Dispatcher *dispr)
{
    FD_ZERO(&dispr->fdset);
    dispr->max_sock = -1;
    return 0;
}

static int
dispatcher_add(Dispatcher *dispr, int sock, void *ctx, CallbackFunc cb)
{
    if (sock >= FD_SETSIZE) {
        fprintf(stderr,
                "Error: Failed to add new event. sock %d should be less than %d\n",
                sock, FD_SETSIZE);
        return -1;
    }

    dispr->events[sock].ctx = ctx;
    dispr->events[sock].callback = cb;

    FD_SET(sock, &dispr->fdset);
    if (sock > dispr->max_sock) {
        dispr->max_sock = sock;
    }
    DPRINT("Added sock %d for watching. max_sock: %d\n",
           sock, dispr->max_sock);
    return 0;
}

/* dispatcher_remove() is not currently in use but may be useful
 * in the future. */
static int
dispatcher_remove(Dispatcher *dispr, int sock)
{
    if (sock >= FD_SETSIZE) {
        fprintf(stderr,
                "Error: Failed to remove event. sock %d should be less than %d\n",
                sock, FD_SETSIZE);
        return -1;
    }

    FD_CLR(sock, &dispr->fdset);
    DPRINT("Sock %d removed from dispatcher watch.\n", sock);
    return 0;
}

/* timeout in us */
static int
dispatcher_wait(Dispatcher *dispr, uint32_t timeout)
{
    struct timeval tv;
    tv.tv_sec = timeout / 1000000;
    tv.tv_usec = timeout % 1000000;

    fd_set fdset = dispr->fdset;

    /* wait until some of sockets become readable. */
    int rc = select(dispr->max_sock + 1, &fdset, 0, 0, &tv);

    if (rc == -1) {
        vubr_die("select");
    }

    /* Timeout */
    if (rc == 0) {
        return 0;
    }

    /* Now call callback for every ready socket. */

    int sock;
    for (sock = 0; sock < dispr->max_sock + 1; sock++) {
        /* The callback on a socket can remove other sockets from the
         * dispatcher, thus we have to check that the socket is
         * still not removed from dispatcher's list
         */
        if (FD_ISSET(sock, &fdset) && FD_ISSET(sock, &dispr->fdset)) {
            Event *e = &dispr->events[sock];
            e->callback(sock, e->ctx);
        }
    }

    return 0;
}

static void
print_buffer(uint8_t *buf, size_t len)
{
    int i;
    printf("Raw buffer:\n");
    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        if (i % 4 == 0) {
            printf("   ");
        }
        printf("%02x ", buf[i]);
    }
    printf("\n............................................................\n");
}

/* Translate guest physical address to our virtual address.  */
static uint64_t
gpa_to_va(VubrDev *dev, uint64_t guest_addr)
{
    int i;

    /* Find matching memory region.  */
    for (i = 0; i < dev->nregions; i++) {
        VubrDevRegion *r = &dev->regions[i];

        if ((guest_addr >= r->gpa) && (guest_addr < (r->gpa + r->size))) {
            return guest_addr - r->gpa + r->mmap_addr + r->mmap_offset;
        }
    }

    assert(!"address not found in regions");
    return 0;
}

/* Translate qemu virtual address to our virtual address.  */
static uint64_t
qva_to_va(VubrDev *dev, uint64_t qemu_addr)
{
    int i;

    /* Find matching memory region.  */
    for (i = 0; i < dev->nregions; i++) {
        VubrDevRegion *r = &dev->regions[i];

        if ((qemu_addr >= r->qva) && (qemu_addr < (r->qva + r->size))) {
            return qemu_addr - r->qva + r->mmap_addr + r->mmap_offset;
        }
    }

    assert(!"address not found in regions");
    return 0;
}

static void
vubr_message_read(int conn_fd, VhostUserMsg *vmsg)
{
    char control[CMSG_SPACE(VHOST_MEMORY_MAX_NREGIONS * sizeof(int))] = { };
    struct iovec iov = {
        .iov_base = (char *)vmsg,
        .iov_len = VHOST_USER_HDR_SIZE,
    };
    struct msghdr msg = {
        .msg_iov = &iov,
        .msg_iovlen = 1,
        .msg_control = control,
        .msg_controllen = sizeof(control),
    };
    size_t fd_size;
    struct cmsghdr *cmsg;
    int rc;

    rc = recvmsg(conn_fd, &msg, 0);

    if (rc == 0) {
        vubr_die("recvmsg");
        fprintf(stderr, "Peer disconnected.\n");
        exit(1);
    }
    if (rc < 0) {
        vubr_die("recvmsg");
    }

    vmsg->fd_num = 0;
    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg))
    {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) 
        {
            fd_size = cmsg->cmsg_len - CMSG_LEN(0);
            vmsg->fd_num = fd_size / sizeof(int);
            memcpy(vmsg->fds, CMSG_DATA(cmsg), fd_size);
            break;
        }
    }

    if (vmsg->size > sizeof(vmsg->payload)) {
        fprintf(stderr,
                "Error: too big message request: %d, size: vmsg->size: %u, "
                "while sizeof(vmsg->payload) = %zu\n",
                vmsg->request, vmsg->size, sizeof(vmsg->payload));
        exit(1);
    }

    if (vmsg->size) {
        rc = read(conn_fd, &vmsg->payload, vmsg->size);
        if (rc == 0) {
            vubr_die("recvmsg");
            fprintf(stderr, "Peer disconnected.\n");
            exit(1);
        }
        if (rc < 0) {
            vubr_die("recvmsg");
        }

        assert(rc == vmsg->size);
    }
}

static void
vubr_message_write(int conn_fd, VhostUserMsg *vmsg)
{
    int rc;

    do {
        rc = write(conn_fd, vmsg, VHOST_USER_HDR_SIZE + vmsg->size);
    } while (rc < 0 && errno == EINTR);

    if (rc < 0) {
        vubr_die("write");
    }
}

static void
vubr_backend_udp_sendbuf(VubrDev *dev, uint8_t *buf, size_t len)
{
    int slen = sizeof(struct sockaddr_in);

    if (sendto(dev->backend_udp_sock, buf, len, 0,
               (struct sockaddr *) &dev->backend_udp_dest, slen) == -1) {
        vubr_die("sendto()");
    }
}

static int
vubr_backend_udp_recvbuf(VubrDev *dev, uint8_t *buf, size_t buflen)
{
    int slen = sizeof(struct sockaddr_in);
    int rc;

    rc = recvfrom(dev->backend_udp_sock, buf, buflen, 0,
                  (struct sockaddr *) &dev->backend_udp_dest,
                  (socklen_t *)&slen);
    if (rc == -1) {
        vubr_die("recvfrom()");
    }

    return rc;
}

static void
vubr_consume_raw_packet(VubrDev *dev, uint8_t *buf, uint32_t len)
{
    int hdrlen = dev->hdrlen;
    DPRINT("    hdrlen = %d\n", dev->hdrlen);

    if (VHOST_USER_BRIDGE_DEBUG) {
        print_buffer(buf, len);
    }
    vubr_backend_udp_sendbuf(dev, buf + hdrlen, len - hdrlen);
}

/* Kick the log_call_fd if required. */
static void
vubr_log_kick(VubrDev *dev)
{
    if (dev->log_call_fd != -1) {
        DPRINT("Kicking the QEMU's log...\n");
        eventfd_write(dev->log_call_fd, 1);
    }
}

/* Kick the guest if necessary. */
static void
vubr_virtqueue_kick(VubrVirtq *vq)
{
    if (!(vq->avail->flags & VRING_AVAIL_F_NO_INTERRUPT)) {
        DPRINT("Kicking the guest...\n");
        eventfd_write(vq->call_fd, 1);
    }
}

static void
vubr_log_page(uint8_t *log_table, uint64_t page)
{
    DPRINT("Logged dirty guest page: %"PRId64"\n", page);
    atomic_or(&log_table[page / 8], 1 << (page % 8));
}

static void
vubr_log_write(VubrDev *dev, uint64_t address, uint64_t length)
{
    uint64_t page;

    if (!(dev->features & (1ULL << VHOST_F_LOG_ALL)) ||
        !dev->log_table || !length) {
        return;
    }

    assert(dev->log_size > ((address + length - 1) / VHOST_LOG_PAGE / 8));

    page = address / VHOST_LOG_PAGE;
    while (page * VHOST_LOG_PAGE < address + length) {
        vubr_log_page(dev->log_table, page);
        page += VHOST_LOG_PAGE;
    }
    vubr_log_kick(dev);
}

static void
vubr_post_buffer(VubrDev *dev, VubrVirtq *vq, uint8_t *buf, int32_t len)
{
    struct vring_desc *desc = vq->desc;
    struct vring_avail *avail = vq->avail;
    struct vring_used *used = vq->used;
    uint64_t log_guest_addr = vq->log_guest_addr;
    int32_t remaining_len = len;

    unsigned int size = vq->size;

    uint16_t avail_index = atomic_mb_read(&avail->idx);

    /* We check the available descriptors before posting the
     * buffer, so here we assume that enough available
     * descriptors. */
    assert(vq->last_avail_index != avail_index);
    uint16_t a_index = vq->last_avail_index % size;
    uint16_t u_index = vq->last_used_index % size;
    uint16_t d_index = avail->ring[a_index];

    int i = d_index;
    uint32_t written_len = 0;

    do {
        DPRINT("Post packet to guest on vq:\n");
        DPRINT("    size             = %d\n", vq->size);
        DPRINT("    last_avail_index = %d\n", vq->last_avail_index);
        DPRINT("    last_used_index  = %d\n", vq->last_used_index);
        DPRINT("    a_index = %d\n", a_index);
        DPRINT("    u_index = %d\n", u_index);
        DPRINT("    d_index = %d\n", d_index);
        DPRINT("    desc[%d].addr    = 0x%016"PRIx64"\n", i, desc[i].addr);
        DPRINT("    desc[%d].len     = %d\n", i, desc[i].len);
        DPRINT("    desc[%d].flags   = %d\n", i, desc[i].flags);
        DPRINT("    avail->idx = %d\n", avail_index);
        DPRINT("    used->idx  = %d\n", used->idx);

        if (!(desc[i].flags & VRING_DESC_F_WRITE)) {
            /* FIXME: we should find writable descriptor. */
            fprintf(stderr, "Error: descriptor is not writable. Exiting.\n");
            exit(1);
        }

        void *chunk_start = (void *)(uintptr_t)gpa_to_va(dev, desc[i].addr);
        uint32_t chunk_len = desc[i].len;
        uint32_t chunk_write_len = MIN(remaining_len, chunk_len);

        memcpy(chunk_start, buf + written_len, chunk_write_len);
        vubr_log_write(dev, desc[i].addr, chunk_write_len);
        remaining_len -= chunk_write_len;
        written_len += chunk_write_len;

        if ((remaining_len == 0) || !(desc[i].flags & VRING_DESC_F_NEXT)) {
            break;
        }

        i = desc[i].next;
    } while (1);

    if (remaining_len > 0) {
            fprintf(stderr,
                    "Too long packet for RX, remaining_len = %d, Dropping...\n",
                    remaining_len);
            return;
    }

    /* Add descriptor to the used ring. */
    used->ring[u_index].id = d_index;
    used->ring[u_index].len = len;
    vubr_log_write(dev,
                   log_guest_addr + offsetof(struct vring_used, ring[u_index]),
                   sizeof(used->ring[u_index]));

    vq->last_avail_index++;
    vq->last_used_index++;

    atomic_mb_set(&used->idx, vq->last_used_index);
    vubr_log_write(dev,
                   log_guest_addr + offsetof(struct vring_used, idx),
                   sizeof(used->idx));

    /* Kick the guest if necessary. */
    vubr_virtqueue_kick(vq);
}

static int
vubr_process_desc(VubrDev *dev, VubrVirtq *vq)
{
    struct vring_desc *desc = vq->desc;
    struct vring_avail *avail = vq->avail;
    struct vring_used *used = vq->used;
    uint64_t log_guest_addr = vq->log_guest_addr;

    unsigned int size = vq->size;

    uint16_t a_index = vq->last_avail_index % size;
    uint16_t u_index = vq->last_used_index % size;
    uint16_t d_index = avail->ring[a_index];

    uint32_t i, len = 0;
    size_t buf_size = 4096;
    uint8_t buf[4096];

    DPRINT("Chunks: ");
    i = d_index;
    do {
        void *chunk_start = (void *)(uintptr_t)gpa_to_va(dev, desc[i].addr);
        uint32_t chunk_len = desc[i].len;

        assert(!(desc[i].flags & VRING_DESC_F_WRITE));

        if (len + chunk_len < buf_size) {
            memcpy(buf + len, chunk_start, chunk_len);
            DPRINT("%d ", chunk_len);
        } else {
            fprintf(stderr, "Error: too long packet. Dropping...\n");
            break;
        }

        len += chunk_len;

        if (!(desc[i].flags & VRING_DESC_F_NEXT)) {
            break;
        }

        i = desc[i].next;
    } while (1);
    DPRINT("\n");

    if (!len) {
        return -1;
    }

    /* Add descriptor to the used ring. */
    used->ring[u_index].id = d_index;
    used->ring[u_index].len = len;
    vubr_log_write(dev,
                   log_guest_addr + offsetof(struct vring_used, ring[u_index]),
                   sizeof(used->ring[u_index]));

    vubr_consume_raw_packet(dev, buf, len);

    return 0;
}

static void
vubr_process_avail(VubrDev *dev, VubrVirtq *vq)
{
    struct vring_avail *avail = vq->avail;
    struct vring_used *used = vq->used;
    uint64_t log_guest_addr = vq->log_guest_addr;

    while (vq->last_avail_index != atomic_mb_read(&avail->idx)) {
        vubr_process_desc(dev, vq);
        vq->last_avail_index++;
        vq->last_used_index++;
    }

    atomic_mb_set(&used->idx, vq->last_used_index);
    vubr_log_write(dev,
                   log_guest_addr + offsetof(struct vring_used, idx),
                   sizeof(used->idx));
}

static void
vubr_backend_recv_cb(int sock, void *ctx)
{
    VubrDev *dev = (VubrDev *) ctx;
    VubrVirtq *rx_vq = &dev->vq[0];
    uint8_t buf[4096];
    struct virtio_net_hdr_v1 *hdr = (struct virtio_net_hdr_v1 *)buf;
    int hdrlen = dev->hdrlen;
    int buflen = sizeof(buf);
    int len;

    if (!dev->ready) {
        return;
    }

    DPRINT("\n\n   ***   IN UDP RECEIVE CALLBACK    ***\n\n");
    DPRINT("    hdrlen = %d\n", hdrlen);

    uint16_t avail_index = atomic_mb_read(&rx_vq->avail->idx);

    /* If there is no available descriptors, just do nothing.
     * The buffer will be handled by next arrived UDP packet,
     * or next kick on receive virtq. */
    if (rx_vq->last_avail_index == avail_index) {
        DPRINT("Got UDP packet, but no available descriptors on RX virtq.\n");
        return;
    }

    memset(buf, 0, hdrlen);
    /* TODO: support mergeable buffers. */
    if (hdrlen == 12)
        hdr->num_buffers = 1;
    len = vubr_backend_udp_recvbuf(dev, buf + hdrlen, buflen - hdrlen);

    vubr_post_buffer(dev, rx_vq, buf, len + hdrlen);
}

static void
vubr_kick_cb(int sock, void *ctx)
{
    VubrDev *dev = (VubrDev *) ctx;
    eventfd_t kick_data;
    ssize_t rc;

    rc = eventfd_read(sock, &kick_data);
    if (rc == -1) {
        vubr_die("eventfd_read()");
    } else {
        DPRINT("Got kick_data: %016"PRIx64"\n", kick_data);
        vubr_process_avail(dev, &dev->vq[1]);
    }
}

static void
vubr_close_log(VubrDev *dev)
{
    if (dev->log_table) {
        if (munmap(dev->log_table, dev->log_size) != 0) {
            vubr_die("munmap()");
        }

        dev->log_table = 0;
    }
    if (dev->log_call_fd != -1) {
        close(dev->log_call_fd);
        dev->log_call_fd = -1;
    }
}

static int
vubr_execute_request(VubrDev *dev, VhostUserMsg *vmsg)
{
    return 0;
}

static void
vubr_receive_cb(int sock, void *ctx)
{
    VubrDev *dev = (VubrDev *) ctx;
    VhostUserMsg vmsg;
    int reply_requested;

    vubr_message_read(sock, &vmsg);
    reply_requested = vubr_execute_request(dev, &vmsg);
    if (reply_requested) {
        /* Set the version in the flags when sending the reply */
        vmsg.flags &= ~VHOST_USER_VERSION_MASK;
        vmsg.flags |= VHOST_USER_VERSION;
        vmsg.flags |= VHOST_USER_REPLY_MASK;
        vubr_message_write(sock, &vmsg);
    }
}

static void
vubr_accept_cb(int sock, void *ctx)
{
    VubrDev *dev = (VubrDev *)ctx;
    int conn_fd;
    struct sockaddr_un un;
    socklen_t len = sizeof(un);

    conn_fd = accept(sock, (struct sockaddr *) &un, &len);
    if (conn_fd == -1) {
        vubr_die("accept()");
    }
    DPRINT("Got connection from remote peer on sock %d\n", conn_fd);
    dispatcher_add(&dev->dispatcher, conn_fd, ctx, vubr_receive_cb);
}

static VubrDev *
vubr_new(const char *path, bool client)
{
    VubrDev *dev = (VubrDev *) calloc(1, sizeof(VubrDev));
    dev->nregions = 0;
    int i;
    struct sockaddr_un un;
    CallbackFunc cb;
    size_t len;

    for (i = 0; i < MAX_NR_VIRTQUEUE; i++) {
        dev->vq[i] = (VubrVirtq) {
            .call_fd = -1, .kick_fd = -1,
            .size = 0,
            .last_avail_index = 0, .last_used_index = 0,
            .desc = 0, .avail = 0, .used = 0,
            .enable = 0,
        };
    }

    /* Init log */
    dev->log_call_fd = -1;
    dev->log_size = 0;
    dev->log_table = 0;
    dev->ready = 0;
    dev->features = 0;

    /* Get a UNIX socket. */
    dev->sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (dev->sock == -1) {
        vubr_die("socket");
    }

    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, path);
    len = sizeof(un.sun_family) + strlen(path);

    if (!client) {
        unlink(path);

        if (bind(dev->sock, (struct sockaddr *) &un, len) == -1) {
            vubr_die("bind");
        }

        if (listen(dev->sock, 1) == -1) {
            vubr_die("listen");
        }
        cb = vubr_accept_cb;

        DPRINT("Waiting for connections on UNIX socket %s ...\n", path);
    } else {
        if (connect(dev->sock, (struct sockaddr *)&un, len) == -1) {
            vubr_die("connect");
        }
        cb = vubr_receive_cb;
    }

    dispatcher_init(&dev->dispatcher);
    dispatcher_add(&dev->dispatcher, dev->sock, (void *)dev, cb);

    return dev;
}

static void
vubr_set_host(struct sockaddr_in *saddr, const char *host)
{
    if (isdigit(host[0])) {
        if (!inet_aton(host, &saddr->sin_addr)) {
            fprintf(stderr, "inet_aton() failed.\n");
            exit(1);
        }
    } else {
        struct hostent *he = gethostbyname(host);

        if (!he) {
            fprintf(stderr, "gethostbyname() failed.\n");
            exit(1);
        }
        saddr->sin_addr = *(struct in_addr *)he->h_addr;
    }
}

static void
vubr_backend_udp_setup(VubrDev *dev,
                       const char *local_host,
                       const char *local_port,
                       const char *remote_host,
                       const char *remote_port)
{
    int sock;
    const char *r;

    int lport, rport;

    lport = strtol(local_port, (char **)&r, 0);
    if (r == local_port) {
        fprintf(stderr, "lport parsing failed.\n");
        exit(1);
    }

    rport = strtol(remote_port, (char **)&r, 0);
    if (r == remote_port) {
        fprintf(stderr, "rport parsing failed.\n");
        exit(1);
    }

    struct sockaddr_in si_local = {
        .sin_family = AF_INET,
        .sin_port = htons(lport),
    };

    vubr_set_host(&si_local, local_host);

    /* setup destination for sends */
    dev->backend_udp_dest = (struct sockaddr_in) {
        .sin_family = AF_INET,
        .sin_port = htons(rport),
    };
    vubr_set_host(&dev->backend_udp_dest, remote_host);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        vubr_die("socket");
    }

    if (bind(sock, (struct sockaddr *)&si_local, sizeof(si_local)) == -1) {
        vubr_die("bind");
    }

    dev->backend_udp_sock = sock;
    dispatcher_add(&dev->dispatcher, sock, dev, vubr_backend_recv_cb);
    DPRINT("Waiting for data from udp backend on %s:%d...\n",
           local_host, lport);
}

static void
vubr_run(VubrDev *dev)
{
    while (1) {
        /* timeout 200ms */
        dispatcher_wait(&dev->dispatcher, 200000);
        /* Here one can try polling strategy. */
    }
}

static int
vubr_parse_host_port(const char **host, const char **port, const char *buf)
{
    char *p = strchr(buf, ':');

    if (!p) {
        return -1;
    }
    *p = '\0';
    *host = strdup(buf);
    *port = strdup(p + 1);
    return 0;
}

int main(int argc, char *argv[])
{
    VubrDev *dev;
    int opt;
    bool client = false;

    while ((opt = getopt(argc, argv, "l:r:u:c")) != -1) {

        switch (opt) {
        case 'l':
            if (vubr_parse_host_port(&lhost, &lport, optarg) < 0) {
            }
            break;
        case 'r':
            if (vubr_parse_host_port(&rhost, &rport, optarg) < 0) {
            }
            break;
        case 'u':
            ud_socket_path = strdup(optarg);
            break;
        case 'c':
            client = true;
            break;
        default:
        }
    }

    dev = vubr_new(ud_socket_path, client);

    if (!dev)
        return 1;

    vubr_backend_udp_setup(dev, lhost, lport, rhost, rport);
    vubr_run(dev);
    return 0;
}
