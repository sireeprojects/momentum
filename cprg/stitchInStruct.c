#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PACKED  __attribute__((__packed__))

struct PACKED header {
    unsigned int len : 32;
    unsigned int ipg : 32;
};

// packing pragma does not work between struct members and a pointer in struct
struct PACKED burst {
    struct header bhdr;
    char *data;
}; 

int main() {
    unsigned int i;
    unsigned char buf[132];

    struct burst *b = (struct burst*) malloc(sizeof(struct burst)-sizeof(char*));
    b->data = (char*) malloc(sizeof(char)*100);

    b->bhdr.len = 21;
    b->bhdr.ipg = 21;
    for(i=0; i<100; i++) {
        // b->data[i] = 'a';
        b->data[i] = (unsigned short)i;
    }
    for(i=0; i<16; i++) {
        printf("%02d ", (unsigned int) b->data[i]);
    }
    printf("\n");
    memcpy(buf, b, 100);

    for(i=0; i<100; i++) {
        printf("%02d ", buf[i]);
    }
    printf("\n");

    free(b->data);
    free(b);
    return 0;
}
