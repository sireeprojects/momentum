#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define GTPL_TEMPLATE_SIZE 1024*16
#define GTPL_RPT_PATTERNS  17
#define GTPL_GENLEN        64
#define GTPL_WORDSIZE      4
#define GTPL_LINEBREAK     GTPL_WORDSIZE


typedef enum {
    GTPL_INCBYTE,
    GTPL_INCWORD,
    GTPL_DECBYTE,
    GTPL_DECWORD,
    GTPL_RANDOM,
    GTPL_REPEAT,
    GTPL_FIXED
} gtpl_dataPatternType;


typedef enum {
    GTPL_NORMAL,
    GTPL_REVERSED
} gtpl_dataPatternByteOrder;


static void gtpl_genTemplate(
    char *arr, 
    gtpl_dataPatternType type, 
    gtpl_dataPatternByteOrder order);


// static void gtpl_printTemplate(
//     char *arr, 
//     gtpl_dataPatternType type);


static uint64_t gtpl_revnum(
    uint64_t in, 
    uint32_t numBytesToReverse);


static char *gtpl_incByte;
static char *gtpl_incWord;
static char *gtpl_decByte;
static char *gtpl_decWord;
static char *gtpl_random;
static char *gtpl_repeating[GTPL_RPT_PATTERNS];
static char *gtpl_incByteRev;
static char *gtpl_incWordRev;
static char *gtpl_decByteRev;
static char *gtpl_decWordRev;
static char *gtpl_randomRev;
static char *gtpl_patterns [GTPL_RPT_PATTERNS];


// free template memory
void gtpl_free() {
    free(gtpl_incByte);
    free(gtpl_incWord);
    free(gtpl_decByte);
    free(gtpl_decWord);
    free(gtpl_random);
    free(gtpl_incByteRev);
    free(gtpl_incWordRev);
    free(gtpl_decByteRev);
    free(gtpl_decWordRev);
    free(gtpl_randomRev);
    int iFree;
    for (iFree=0; iFree<GTPL_RPT_PATTERNS; iFree++) {
        free(gtpl_repeating[iFree]);
    }
}


void gtpl_gen() {
    // allocate template memory
    gtpl_incByte    = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_incWord    = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_decByte    = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_decWord    = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_random     = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_incByteRev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_incWordRev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_decByteRev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_decWordRev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_randomRev  = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));

    int iRpt;
    for (iRpt=0; iRpt<GTPL_RPT_PATTERNS; iRpt++) {
        gtpl_repeating[iRpt] = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    }
    // patterns
    gtpl_patterns[0]  = "ffff";
    gtpl_patterns[1]  = "0000";
    gtpl_patterns[2]  = "aaaa";
    gtpl_patterns[3]  = "5555";
    gtpl_patterns[4]  = "7777";
    gtpl_patterns[5]  = "dddd";
    gtpl_patterns[6]  = "f0f0";
    gtpl_patterns[7]  = "ff00ff00";
    gtpl_patterns[8]  = "00ff00ff";
    gtpl_patterns[9]  = "ffff0000";
    gtpl_patterns[10] = "0000ffff";
    gtpl_patterns[11] = "00010203";
    gtpl_patterns[12] = "00010002";
    gtpl_patterns[13] = "fffefdfc";
    gtpl_patterns[14] = "fffffffe";
    gtpl_patterns[15] = "7e7e7e7e";
    gtpl_patterns[16] = "4747476b";

    // generate templates
    gtpl_genTemplate(gtpl_incByte,    GTPL_INCBYTE, GTPL_NORMAL);
    gtpl_genTemplate(gtpl_incWord,    GTPL_INCWORD, GTPL_NORMAL);
    gtpl_genTemplate(gtpl_decByte,    GTPL_DECBYTE, GTPL_NORMAL);
    gtpl_genTemplate(gtpl_decWord,    GTPL_DECWORD, GTPL_NORMAL);
    gtpl_genTemplate(gtpl_random,     GTPL_RANDOM , GTPL_NORMAL);
    gtpl_genTemplate(gtpl_incByteRev, GTPL_INCBYTE, GTPL_REVERSED);
    gtpl_genTemplate(gtpl_incWordRev, GTPL_INCWORD, GTPL_REVERSED);
    gtpl_genTemplate(gtpl_decByteRev, GTPL_DECBYTE, GTPL_REVERSED);
    gtpl_genTemplate(gtpl_decWordRev, GTPL_DECWORD, GTPL_REVERSED);
    gtpl_genTemplate(gtpl_randomRev,  GTPL_RANDOM,  GTPL_REVERSED);
}


void gtpl_genTemplate(char *arr, gtpl_dataPatternType type, gtpl_dataPatternByteOrder order) {
    uint32_t idx;
    uint32_t value;
    switch (type) {
        case GTPL_INCBYTE:{
            for (idx=0; idx<GTPL_TEMPLATE_SIZE; idx++) {
                arr[idx] = idx;
            }
            break;
        }
        case GTPL_INCWORD:{
            uint32_t idx = 0;
            uint64_t value = 0;
            uint64_t reversed = 0;
            for (idx=0, value=0; idx<GTPL_GENLEN*GTPL_WORDSIZE; value++,idx+=GTPL_WORDSIZE) {
                if (order) {
                    reversed = gtpl_revnum(value, GTPL_WORDSIZE);
                    memcpy(arr+idx, (unsigned char*)&reversed, GTPL_WORDSIZE);
                } else {
                    memcpy(arr+idx, (unsigned char*)&value, GTPL_WORDSIZE);
                }
            }
            break;
        }
        case GTPL_DECBYTE:{
            break;
        }
        case GTPL_DECWORD:{
            break;
        }
        case GTPL_RANDOM:{
            break;
        }
        case GTPL_REPEAT:{
            break;
        }
        case GTPL_FIXED:{
            break;
        }
        default:{
            break;
        }
    }
}


uint64_t gtpl_revnum(uint64_t in, uint32_t numBytesToReverse) {
   uint64_t reversed = 0;
   uint32_t idx=0;
   for (idx=0; idx<numBytesToReverse; idx++) {
       reversed = (reversed<<8) | (in & 0xff);
       in = in >> 8;
   }
   return reversed;
}


int main() {
    // time is the seed
    // can be used again to generate same random valued
    srand((unsigned int) time (NULL));

    gtpl_gen();
    gtpl_free();
}

// void gtpl_printTemplate(char *arr, gtpl_dataPatternType type){
//     uint32_t idx;
//     uint32_t value;
//     switch (type) {
//         case GTPL_INCBYTE:{
//             for (idx=0; idx<GTPL_TEMPLATE_SIZE; idx++) {
//                 arr[idx] = idx;
//             }
//             break;
//         }
//         case GTPL_INCWORD:{
//             for (idx=4, value=0; idx<GTPL_TEMPLATE_SIZE; value++, idx+=4) {
//                 arr[idx] = idx;
//             }
//             break;
//         }
//         case GTPL_DECBYTE:{
//             break;
//         }
//         case GTPL_DECWORD:{
//             break;
//         }
//         case GTPL_RANDOM:{
//             for (idx=0; idx<GTPL_TEMPLATE_SIZE; idx++) {
//                 arr[idx] = rand();
//             }
//             break;
//         }
//         case GTPL_REPEAT:{
//             break;
//         }
//         case GTPL_FIXED:{
//             break;
//         }
//         default:{
//             break;
//         }
//     }
// }
