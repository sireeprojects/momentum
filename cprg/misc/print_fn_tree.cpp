#include <iostream>
#include <cstdint>
using namespace std;

uint32_t nof_spaces = 0;

void add_space() {
    nof_spaces += 3;
}

void delete_space() {
    nof_spaces -= 3;
}

void fn3() {
    add_space();
    cout << string(nof_spaces, ' ') << "|-- " << "Hello3" << endl;
    delete_space();
}
void fn2() {
    add_space();
    cout << string(nof_spaces, ' ') << "|-- " << "Hello3" << endl;
    delete_space();
}
void fn1() {
    add_space();
    cout << string(nof_spaces, ' ') << "|-- " << "Hello3" << endl;
    fn2();
    fn2();
    delete_space();
}

int main() {
    fn1();
    fn3();
    return(0);
}
