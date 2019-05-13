#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


// fixed/ repeat
// ff ff
// 00 00
// aa aa
// 55 55
// 77 77
// dd dd
// f0 f0
// ff 00 ff 00
// 00 ff 00 ff
// ff ff 00 00
// 00 00 ff ff
// 00 01 02 03
// 00 01 00 02
// ff fe fd fc
// ff ff ff fe
// 7e 7e 7e 7e
// 47 47 47 6b

uint64_t revnum(uint64_t in, uint32_t numBytesToReverse) {
   uint64_t reversed = 0;
   uint32_t i=0;
   for (i=0; i<numBytesToReverse; i++) {
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
    uint64_t genLen = 64;
    int wordSize = 4;
    int lineBreak = 1*wordSize;
    unsigned char *data = (unsigned char*) malloc((genLen*wordSize)*sizeof(unsigned char));
    uint32_t mode = 1; // 1:reversed 0:normal

    for (i=0, v=0; i<genLen*wordSize; v++,i+=wordSize) {
        if (mode) {
            reversed = revnum(v, wordSize);
            memcpy(data+i, (unsigned char*)&reversed, wordSize);
        } else {
            memcpy(data+i, (unsigned char*)&v, wordSize);
        }
    }
    for (i=0; i<genLen*wordSize; i++) { 
        printf("%02x ", data[i]); 
        if (i%(lineBreak)==lineBreak-1) printf("\n");
    }
    printf("\n");
    return 0;
}
