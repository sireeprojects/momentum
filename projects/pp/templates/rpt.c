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


#define GTPL_RPT_PATTERNS  17
struct gtpl_patterns {
    char pattern[8];
    uint32_t pattern_size;
};

struct gtpl_patterns patterns [GTPL_RPT_PATTERNS];

int main(){
    // patterns
    strncpy(patterns[ 0].pattern , "ffff"     , 4);
    strncpy(patterns[ 1].pattern , "0000"     , 4);
    strncpy(patterns[ 2].pattern , "aaaa"     , 4);
    strncpy(patterns[ 3].pattern , "5555"     , 4);
    strncpy(patterns[ 4].pattern , "7777"     , 4);
    strncpy(patterns[ 5].pattern , "dddd"     , 4);
    strncpy(patterns[ 6].pattern , "f0f0"     , 4);
    strncpy(patterns[ 7].pattern , "ff00ff00" , 8);
    strncpy(patterns[ 8].pattern , "00ff00ff" , 8);
    strncpy(patterns[ 9].pattern , "ffff0000" , 8);
    strncpy(patterns[10].pattern , "0000ffff" , 8);
    strncpy(patterns[11].pattern , "00010203" , 8);
    strncpy(patterns[12].pattern , "00010002" , 8);
    strncpy(patterns[13].pattern , "fffefdfc" , 8);
    strncpy(patterns[14].pattern , "fffffffe" , 8);
    strncpy(patterns[15].pattern , "7e7e7e7e" , 8);
    strncpy(patterns[16].pattern , "4747476b" , 8);
    patterns[ 0].pattern_size = 4;
    patterns[ 1].pattern_size = 4;
    patterns[ 2].pattern_size = 4;
    patterns[ 3].pattern_size = 4;
    patterns[ 4].pattern_size = 4;
    patterns[ 5].pattern_size = 4;
    patterns[ 6].pattern_size = 4;
    patterns[ 7].pattern_size = 8;
    patterns[ 8].pattern_size = 8;
    patterns[ 9].pattern_size = 8;
    patterns[10].pattern_size = 8;
    patterns[11].pattern_size = 8;
    patterns[12].pattern_size = 8;
    patterns[13].pattern_size = 8;
    patterns[14].pattern_size = 8;
    patterns[15].pattern_size = 8;
    patterns[16].pattern_size = 8;


    uint32_t i = 0;
    uint32_t j = 0;
    uint64_t reversed = 0;;
    uint64_t gen_len = 10;
    int word_size = 4;
    int line_break = 1*word_size;
    unsigned char *data = (unsigned char*) malloc((gen_len*word_size)*sizeof(unsigned char));
    uint32_t mode = 1; // 1:reversed 0:normal
    uint64_t start_value = 0;
    char pat[8];

    for (j=0; j<17; j++) {
        word_size = patterns[j].pattern_size;
        strncpy(pat, patterns[j].pattern, word_size);
        line_break = word_size;

        for (i=0; i<gen_len*word_size; i+=word_size) {
            memcpy(data+i, (unsigned char*)pat, word_size);
        }
        for (i=0; i<gen_len*word_size; i++) { 
            printf("%c", data[i]); 
            if (i%(line_break)==line_break-1) printf("\n");
        }
        printf("\n");
    }
    return 0;
}

