#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


uint64_t revnum(uint64_t in, uint32_t num_bytes_to_reverse) {
   uint64_t reversed = 0;
   uint32_t i=0;
   for (i=0; i<num_bytes_to_reverse; i++) {
       reversed = (reversed<<8) | (in & 0xff);
       in = in >> 8;
   }
   return reversed;
}


int main(){
    uint32_t i = 0;
    uint32_t j = 0;
    uint64_t v = 0;
    uint64_t reversed = 0;;
    uint64_t gen_len = 64;
    int word_size = 4;
    int line_break = 1*word_size;
    unsigned char *data = (unsigned char*) malloc((gen_len*word_size)*sizeof(unsigned char));
    uint32_t mode = 1; // 1:reversed 0:normal
    uint64_t start_value = 0;

    for (i=0, v=0; i<gen_len*word_size; v++,i+=word_size) {
        if (mode) {
            reversed = revnum(v, word_size);
            memcpy(data+i, (unsigned char*)&reversed, word_size);
        } else {
            memcpy(data+i, (unsigned char*)&v, word_size);
        }
    }
    for (i=0; i<gen_len*word_size; i++) { 
        printf("%02x ", data[i]); 
        if (i%(line_break)==line_break-1) printf("\n");
    }
    printf("\n");
    return 0;
}

