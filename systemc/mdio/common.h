using namespace std;


#define wdump(signal) sc_trace (waves, signal, #signal);


ofstream myfile;
stringstream runlog;


#define MSG(...) {          \
    runlog.str("");         \
    runlog << "["           \
        << sc_time_stamp()  \
        <<"] "              \
        << id()             \
        << ": "             \
        << __VA_ARGS__      \
        << endl;            \
    cout << runlog.str();   \
    myfile << runlog.str(); \
}


string id() { return "top"; }


void open_logfile() { myfile.open ("run.log");; }


void close_logfile() { myfile.close(); }
