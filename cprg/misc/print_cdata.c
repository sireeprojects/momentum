#include <stdio.h>
#include <stdlib.h> // malloc
#include <stdint.h> // uint*_t
#define PACKED __attribute__((__packed__))

// frame descriptor
struct PACKED mpktgen_frame_t {
    // metadata 32 bytes
    uint32_t ipg : 32;
    uint32_t len : 32;
    char pad [24];
    // frame
    uint64_t src : 48;
    uint64_t dst : 48;
    uint32_t lt : 16;
    char payload [512];
};

void print_cdata (unsigned char* tmp, int len) {
    uint32_t idx = 0;
    printf ("Packet Len: %d\n", len);

    int plen = 16;

    int x;
    for (x=0; x<len/plen; x++) {
        int y;
        for (y=0; y<plen; y++) {
            printf("%02x ", (uint16_t)tmp[idx]);
            if (idx%8==7) printf("  ");
            idx++;
        }
        printf("\n");
    }
    for (x=idx; x<len; x++) {
       printf ("%02x ", (uint16_t)tmp[idx]);
       if (idx%8==7) printf("  ");
       idx++;
    }
    printf("\n");
    fflush (stdout);
}

int main () {
    uint32_t idx;
    uint32_t frm_size = 60;
    // generate a frame with metadata
    struct mpktgen_frame_t *frm =
        (struct mpktgen_frame_t*)
        malloc (sizeof (struct mpktgen_frame_t));
    frm->ipg = 12;
    frm->len = 64;
    frm->src = 0xaabbccddeeffull;
    frm->dst = 0x112233445566ull;
    for (idx=0; idx<frm_size; idx++) {
        frm->payload[idx] = idx;
    }
    print_cdata ((char*)frm, 64);
}
