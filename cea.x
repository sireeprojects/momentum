cpp file

#include "cea.h"

// 1GB frame buffer with read and write capabilities
#define LENGTH (1UL*1024*1024*1024)
#define PROTECTION (PROT_READ | PROT_WRITE)

#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0x40000 /* arch specific */
#endif

// Only ia64 requires this 
#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define FLAGS (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_FIXED)
#else
#define ADDR (void *)(0x0UL)
#define FLAGS (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB)
#endif

namespace cea {

ofstream logfile;

int outbuf::overflow(int_type c) {
    if (c != EOF) {
    c = static_cast<char>(c);
        logfile << static_cast<char>(c);
        if (putchar(c) == EOF) {
           return EOF;
        }
    }
    return c;
}

class cea_init {
public:
    cea_init() {
        logfile.open("run.log", ofstream::out);
        if (!logfile.is_open()) {
            cout << "Error creating logfile. Aborting..." << endl;
            exit(1);
        }
    }
    ~cea_init() { logfile.close(); }
};
cea_init init;

cea_manager::cea_manager() {
    CEA_DBG("%s called", __FUNCTION__);
}

void cea_manager::add_proxy(cea_proxy *pxy) {
    CEA_DBG("%s called", __FUNCTION__);
    proxies.push_back(pxy);
}

void cea_manager::add_proxy(cea_proxy *pxy, uint32_t cnt) {
    CEA_DBG("%s called with %d proxies", __FUNCTION__, cnt);
    for(uint32_t idx=0; idx<cnt; idx++)
        proxies.push_back(&pxy[idx]);
}

void cea_manager::add_stream(cea_stream *stm, cea_proxy *pxy) {
    CEA_DBG("%s called", __FUNCTION__);
    if (pxy != NULL) {
        vector<cea_proxy*>::iterator it;
        for (it = proxies.begin(); it != proxies.end(); it++) {
            if ((*it)->pid == pxy->pid) {
                uint32_t idx = distance(proxies.begin(), it);
                proxies[idx]->add_stream(stm);
            }
        }
    } else {
        for(uint32_t idx=0; idx<proxies.size(); idx++) {
            proxies[idx]->add_stream(stm);
        }
    }
}

void cea_manager::testfn(cea_proxy *p) {
}

void cea_manager::add_cmd(cea_stream *stm, cea_proxy *pxy) {
    CEA_DBG("%s called", __FUNCTION__);
    add_stream(stm, pxy);
}

void cea_manager::exec_cmd(cea_stream *stm, cea_proxy *pxy) {
    CEA_DBG("%s called", __FUNCTION__);
    // TODO
}

cea_proxy::cea_proxy() {
    this->pid = cea::pid;
    cea::pid++;
    CEA_DBG("%s : Value of pid: %d", __FUNCTION__, pid);
}

void cea_proxy::add_stream(cea_stream *stm) {
    streamq.push_back(stm);
}

void cea_proxy::add_cmd(cea_stream *stm) {
    streamq.push_back(stm);
}

void cea_proxy::exec_cmd(cea_stream *stm) {
    // TODO
}

void cea_proxy::testfn(cea_stream *s) {
}

void cea_proxy::start_worker() {
    w = thread (&cea_proxy::worker, this);
    char name[16];
    sprintf(name, "worker_%d", port_num);
    pthread_setname_np(w.native_handle(), name);
    w.detach();
}

void cea_proxy::read() {
}

void cea_proxy::worker() {
    read();
    consolidate();
    set_gen_vars();
    generate();
}

void cea_proxy::consolidate() {
}

void cea_proxy::set_gen_vars() {
}

void cea_proxy::generate() {
    // write to fbuf
    // *(addr + i) = (char)i;
    // read from fbuf
    // if (*(addr + i) != (char)i)
}

void cea_proxy::create_frm_buffer() {
    fbuf = mmap(ADDR, LENGTH, PROTECTION, FLAGS, -1, 0);
    if (fbuf == MAP_FAILED) {
        CEA_MSG("Error: Memory map failed in __FUNCTION__");
        exit(1);
    }
}

void cea_proxy::release_frm_buffer() {
    if (munmap(fbuf, LENGTH)) {
        CEA_MSG("Error: Memory unmap failed in __FUNCTION__");
        exit(1);
    }
}

void cea_stream::do_copy (const cea_stream* rhs) {
    CEA_DBG("Stream CC Called");
}

cea_stream::cea_stream () {
}

cea_stream::cea_stream (const cea_stream& rhs) {
    cea_stream::do_copy(&rhs);
}

cea_stream& cea_stream::operator = (cea_stream& rhs) {
   if (this != &rhs) {
      do_copy(&rhs);
   }
   return *this;
}

ostream& operator << (ostream& os, const cea_stream& f) {
    os << f.describe();
    return os;
}

string cea_stream::describe() const {
    return "";
}

} // namesapce

---------------------------------------

h file

#include <iostream>
#include <vector>
#include <thread>
#include <cstdint>
#include <sys/mman.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#define PACKED __attribute__((packed))

using namespace std;

namespace cea {

class outbuf : public streambuf {
protected:
    virtual int overflow(int c) ;
};
outbuf ob;
ostream cealog(&ob);

typedef enum {
    NONE,
    LOW,
    FULL,
} msg_verbosity;

typedef enum {
    INFO,
    ERROR,
    WARNING,
    SCBD_ERROR,
    PKT
} msg_type;

typedef enum {
    V2,
    RAW
} frame_type;

typedef enum {
    mac_preamble,
    mac_dest_addr,
    mac_src_addr,
    mac_len,
    mac_ether_type,
    mac_fcs,
    ipv4_version,
    ipv4_ihl,
    ipv4_tos,
    ipv4_total_len,
    ipv4_id,
    ipv4_flags,
    ipv4_frag_offset,
    ipv4_ttl,
    ipv4_protocol,
    ipv4_hdr_csum,
    ipv4_src_addr,
    ipv4_dest_addr,
    ipv4_opts,
    ipv4_pad,
    udp_src_port,
    udp_dest_port,
    udp_len,
    udp_csum,
    stream_type,
    stream_pkts_per_burst,
    stream_burst_per_stream,
    stream_inter_burst_gap,
    stream_inter_stream_gap,
    stream_start_delay,
    stream_rate_type,
    stream_ipg,
    stream_percentage,
    stream_pkts_per_aec,
    stream_bit_rate,
    payload_type,
    fields_size
} field_id;

typedef enum {
    // value modifiers
    fixed,                  // init value
    random,                 // from 0 to max int
    rrandom,                // random in range
    increment,              // from start to stop with steps
    decrement,              // from start to stop with steps
    cincrement,             // starts from 0 and loops at repeat after
    cdecrement,             // starts from ff and loops at repeat after
    // data modifiers
    incr_byte,              // start from zero
    incr_word,              // start from zero
    decr_byte,              // start from FF
    decr_word,              // start from FF
    repeat_pattern,        // repeat a data pattern
    fixed_pattern,          // use data pattern only once, rest 0s
    // stream modifiers
    continuous_pkts,
    continuous_burst,
    stop_after_stream,
    goto_next_stream,
    // stream rate modifiers
    ipg,
    percentage,
    pkts_per_sec,
    bit_rate
} field_modifier;

struct PACKED cea_field {
    bool touched : 1;
    bool merge : 1;
    uint64_t mask : 64;
    uint32_t id : 32;
    uint32_t len: 32;
    uint32_t ofset : 32;
    field_modifier modifier : 32;
    uint64_t value: 64;
    uint32_t start: 32;
    uint32_t stop: 32;
    uint32_t step: 32;
    uint32_t repeat: 32;
    char name[32];
    char pad[47];
};

// forward declaration
class cea_stream;
class cea_proxy;
class cea_manager;

//---------
// Manager
//---------
class cea_manager {
public:
    cea_manager();
    void add_proxy(cea_proxy *pxy);
    void add_proxy(cea_proxy *pxy, uint32_t cnt);
    void add_stream(cea_stream *stm, cea_proxy *pxy=NULL);
    void add_cmd(cea_stream *stm, cea_proxy *pxy=NULL);
    void exec_cmd(cea_stream *stm, cea_proxy *pxy=NULL);
    void testfn(cea_proxy *s);
private:
    vector<cea_proxy*> proxies;
};

//-------
// Proxy
//-------
class cea_proxy {
public:
    cea_proxy();
    void add_stream(cea_stream *stm);
    void add_cmd(cea_stream *stm);
    void exec_cmd(cea_stream *stm);
    void testfn(cea_stream *s);
    uint32_t pid;
    void start();

private:
    uint32_t port_num;
    vector<cea_stream*> streamq;
    thread w;

    void worker(); // main thread
    void start_worker(); // on stream start
    void read();
    void consolidate();
    void set_gen_vars();
    void generate();
    void *fbuf;
    void create_frm_buffer();
    void release_frm_buffer();
};

uint32_t pid = 0;

//--------
// Stream
//--------
// notes:
// stream should only be copied into queues
class cea_stream {
public:    
    cea_stream();
    char* pack();
    void unpack(char *data);
    void do_copy (const cea_stream* rhs);
    friend ostream& operator << (ostream& os, const cea_stream& cmd);
    string describe() const;
    cea_stream (const cea_stream& rhs);
    cea_stream& operator = (cea_stream& rhs);

    cea_field fields[64];
    void set(uint32_t id, uint64_t value);
    void set(uint32_t id, field_modifier spec);

};

template<typename ... Args>
string string_format(const string& format, Args ... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    if(size <= 0){ throw runtime_error("Error during formatting."); }
    unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return string(buf.get(), buf.get() + size - 1);
}

#define CEA_MSG(...) \
    cealog << string_format(__VA_ARGS__) << endl; 

#ifdef CEA_DEBUG
#define CEA_DBG(...) { CEA_MSG(__VA_ARGS__) }
#else
#define CEA_DBG(...) {}
#endif

} // namespace

------------------------------

DBG ?= "off"
ifeq "$(DBG)" "on"
LIBPARAMS = -DCEA_DEBUG
else
LIBPARAMS =
endif

default:sim

sim:makelib
	@g++ test.cpp -o sim.x -lcea -L${PWD} ${LIBPARAMS}
	@./sim.x

makelib:clean
	@g++ cea.cpp -s -fPIC -shared -o libcea.so -Wall -Wno-unused -lpthread ${LIBPARAMS}

clean:
	@rm -rf *.x *.log *.so *.o

------------------------------

test

#include "cea.h"
using namespace cea;

int main() {
    cea_manager *mgr = new cea_manager;

    cea_proxy *s0 = new cea_proxy;
    cea_proxy *s1 = new cea_proxy;
    cea_proxy *s2 = new cea_proxy;
    cea_proxy *s3 = new cea_proxy;
    cea_proxy *s4 = new cea_proxy;

    mgr->add_proxy(s0);
    mgr->add_proxy(s1);
    mgr->add_proxy(s2);
    mgr->add_proxy(s3);
    mgr->add_proxy(s4);

    mgr->testfn(s0);
    mgr->testfn(s1);
    mgr->testfn(s2);
    mgr->testfn(s3);
    mgr->testfn(s4);

    return 0;
}

// int main() {
//     cea_manager *mgr = new cea_manager;
//     cea_proxy *pxy = new cea_proxy;
//     cea_stream *stm = new cea_stream;
// 
//     uint32_t a = 10;
// 
//     CEA_MSG("Just a testbench message %d", a);
// 
//     mgr->add_proxy(pxy);
//     pxy->add_stream(stm);
// 
//     cea_proxy p[10];
//     mgr->add_proxy(p, 10);
// 
//     cea_proxy *pp = new cea_proxy[10];
//     mgr->add_proxy(pp, 10);
// 
//     return 0;
// }





