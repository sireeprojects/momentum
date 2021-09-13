#include <iostream>
#include <thread>
#include <fstream>
#include <cstdint>
#include <sys/socket.h>
#include <sys/un.h>
using namespace std;

#define SOCKPATH "/tmp/genesis.sock"

// log file handle and initialization
ofstream logFile;
class preRun {
public:
    preRun() {
        logFile.open("run.log", ofstream::out);
        if (!logFile.is_open()) {
            cout << "Error creating logFile. Aborting..." << endl;
            exit(1);
        }
    }
    ~preRun() { logFile.close(); }
};
preRun init;


// Custom stream operator
class outbuf : public streambuf {
protected:
    virtual int overflow(int c) {
        if (c != EOF) {
            c = static_cast<char>(c);
            logFile << static_cast<char>(c);
            if (putchar(c) == EOF) {
                return EOF;
            }
        }
        return c;
    }
};
outbuf ob;
ostream logger(&ob);


class genesis {
public:
    genesis();
    void startConsolidate();
    void startGenerate();
    void stopThreads();
    int consolidate();
    void generate();
    void initSocket();
    void joinThreads();
    void start();
private:
    thread c;
    thread g;
    bool stop;
    int sockServerFd;
    struct sockaddr_un sockServerAddr;
    fd_set fds;
    int maxFds;
};


// CTOR
genesis::genesis() {
    stop = false;
    FD_ZERO (&fds);
    initSocket();
}


void genesis::start() {
    startConsolidate();
    startGenerate();
    joinThreads();
}


void genesis::initSocket() {
    if ((sockServerFd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) == -1) {
        logger << "Socket Creation Failed" << endl;
        exit(1);
    }
    socklen_t len;
    len = sizeof(struct sockaddr_un);
    sockServerAddr.sun_family = AF_UNIX;
    strncpy (sockServerAddr.sun_path, SOCKPATH, (len - sizeof(short)));

    if ((bind(sockServerFd, (struct sockaddr*) &sockServerAddr, len)) == -1) {
        logger << "Socket Binding Failed" << endl;
        exit (1);
    }
    listen (sockServerFd, 256);
    FD_SET(sockServerFd, &fds);
    logger << "Socket server created successfully" << endl;    
    logger << "Waiting for GUI to connect..." << endl;
}


void genesis::startConsolidate() {
   c = thread(&genesis::consolidate, this);
   pthread_setname_np(c.native_handle(), "Consolidate");
}


void genesis::startGenerate() {
   g = thread(&genesis::generate, this);
   pthread_setname_np(g.native_handle(), "Generate");
}


int genesis::consolidate() {
    int clientFd;
    while (!stop) {
        uint32_t timeout = 200000;
        struct timeval tv;
        tv.tv_sec = timeout / 1000000;
        tv.tv_usec = timeout % 1000000;
        fd_set fdset_rd = fds;
        int rc = select(maxFds+1, &fdset_rd, 0, 0, &tv);
        if (rc == -1) perror ("select");
        if (rc == 0) return 0;
        int fd;
        for (fd=0; fd<maxFds+1; fd++) {
            if (FD_ISSET(fd, &fdset_rd) && FD_ISSET(fd, &fds)) {
                if (fd == sockServerFd) {
                    // new connection request
                    struct sockaddr_un un;
                    socklen_t len = sizeof(un);
                    clientFd = accept(sockServerFd, (struct sockaddr *) &un, &len);
                    FD_SET(clientFd, &fds);
                    if (clientFd > maxFds) {
                        maxFds = clientFd;
                    }
                } else if (fd == clientFd) {
                    // data available in socket
                }
            }
        }
    }
}


void genesis::generate() {
    while (!stop) {
    }
}


void genesis::stopThreads() {
    stop = true;
}


void genesis::joinThreads() {
    c.join();
    g.join();
}


int main() {
    genesis g;
    // g.start();
    return 0;
}

