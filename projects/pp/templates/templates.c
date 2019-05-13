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
} gtpl_data_pattern_type;


typedef enum {
    GTPL_NORMAL,
    GTPL_REVERSED
} gtpl_data_pattern_byte_order;


static void gtpl_gen_template(
    char *arr, 
    gtpl_data_pattern_type type, 
    gtpl_data_pattern_byte_order order,
    uint32_t patten_id);


static void gtpl_print_template(
    char *arr, 
    gtpl_data_pattern_type type);


static uint64_t gtpl_reverse_bytes(
    uint64_t in, 
    uint32_t num_bytes_to_reverse);


static uint64_t gtpl_pow(
    uint32_t base, 
    uint32_t exponent);


static char *gtpl_inc_byte;
static char *gtpl_inc_word;
static char *gtpl_dec_byte;
static char *gtpl_dec_word;
static char *gtpl_random;
static char *gtpl_repeating_pattern[GTPL_RPT_PATTERNS];
static char *gtpl_repeating_fixed[GTPL_RPT_PATTERNS];
static char *gtpl_inc_byte_rev;
static char *gtpl_inc_word_rev;
static char *gtpl_dec_byte_rev;
static char *gtpl_dec_word_rev;


struct gtpl_patterns {
    char pattern[8];
    uint32_t pattern_size;
};


struct gtpl_patterns patterns [GTPL_RPT_PATTERNS];


// free template memory
void gtpl_free() {
    free(gtpl_inc_byte);
    free(gtpl_inc_word);
    free(gtpl_dec_byte);
    free(gtpl_dec_word);
    free(gtpl_random);
    free(gtpl_inc_byte_rev);
    free(gtpl_inc_word_rev);
    free(gtpl_dec_byte_rev);
    free(gtpl_dec_word_rev);
    int iFree;
    for (iFree=0; iFree<GTPL_RPT_PATTERNS; iFree++) {
        free(gtpl_repeating_pattern[iFree]);
        free(gtpl_repeating_fixed[iFree]);
    }
}


void gtpl_gen() {
    // allocate template memory
    gtpl_inc_byte     = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_inc_word     = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_dec_byte     = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_dec_word     = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_random       = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_inc_byte_rev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_inc_word_rev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_dec_byte_rev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    gtpl_dec_word_rev = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));

    int iRpt;
    for (iRpt=0; iRpt<GTPL_RPT_PATTERNS; iRpt++) {
        gtpl_repeating_pattern[iRpt] = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
        gtpl_repeating_fixed[iRpt] = (char*)malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    }
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

    // generate templates
    gtpl_gen_template(gtpl_inc_byte,     GTPL_INCBYTE, GTPL_NORMAL  , 0);
    gtpl_gen_template(gtpl_inc_word,     GTPL_INCWORD, GTPL_NORMAL  , 0);
    gtpl_gen_template(gtpl_dec_byte,     GTPL_DECBYTE, GTPL_NORMAL  , 0);
    gtpl_gen_template(gtpl_dec_word,     GTPL_DECWORD, GTPL_NORMAL  , 0);
    gtpl_gen_template(gtpl_inc_byte_rev, GTPL_INCBYTE, GTPL_REVERSED, 0);
    gtpl_gen_template(gtpl_inc_word_rev, GTPL_INCWORD, GTPL_REVERSED, 0);
    gtpl_gen_template(gtpl_dec_byte_rev, GTPL_DECBYTE, GTPL_REVERSED, 0);
    gtpl_gen_template(gtpl_dec_word_rev, GTPL_DECWORD, GTPL_REVERSED, 0);
    gtpl_gen_template(gtpl_random,       GTPL_RANDOM , GTPL_NORMAL  , 0);

    int i_rpt_pat;
    for (i_rpt_pat=0; i_rpt_pat<GTPL_RPT_PATTERNS; i_rpt_pat++) {
        gtpl_gen_template(gtpl_repeating_pattern[i_rpt_pat], GTPL_REPEAT,  GTPL_NORMAL, i_rpt_pat);
    }
    int i_rpt_fixed;
    for (i_rpt_fixed=0; i_rpt_fixed<GTPL_RPT_PATTERNS; i_rpt_fixed++) {
        gtpl_gen_template(gtpl_repeating_fixed[i_rpt_fixed], GTPL_FIXED,  GTPL_NORMAL, i_rpt_fixed);
    }
}


void gtpl_gen_template(char *arr, gtpl_data_pattern_type type, gtpl_data_pattern_byte_order order, uint32_t patten_id) {
    uint32_t idx;
    uint32_t value;
    memset(arr, '0', GTPL_TEMPLATE_SIZE );

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
                    reversed = gtpl_reverse_bytes(value, GTPL_WORDSIZE);
                    memcpy(arr+idx, (unsigned char*)&reversed, GTPL_WORDSIZE);
                } else {
                    memcpy(arr+idx, (unsigned char*)&value, GTPL_WORDSIZE);
                }
            }
            break;
        }
        case GTPL_DECBYTE:{
            uint32_t idx = 0;
            uint64_t value = 0;
            uint64_t start_value = gtpl_pow(2, (GTPL_WORDSIZE*8));
            for (idx=0, value=(start_value-1); idx<GTPL_GENLEN*GTPL_WORDSIZE; value--,idx+=GTPL_WORDSIZE) {
                memcpy(arr+idx, (unsigned char*)&value, GTPL_WORDSIZE);
            }
            break;
        }
        case GTPL_DECWORD:{
            uint32_t idx = 0;
            uint64_t value = 0;
            uint64_t reversed = 0;
            uint64_t start_value = gtpl_pow(2, (GTPL_WORDSIZE*8));
            for (idx=0, value=(start_value-1); idx<GTPL_GENLEN*GTPL_WORDSIZE; value--,idx+=GTPL_WORDSIZE) {
                if (order) {
                    reversed = gtpl_reverse_bytes(value, GTPL_WORDSIZE);
                    memcpy(arr+idx, (unsigned char*)&reversed, GTPL_WORDSIZE);
                } else {
                    memcpy(arr+idx, (unsigned char*)&value, GTPL_WORDSIZE);
                }
            }
            break;
        }
        case GTPL_RANDOM:{
            for (idx=0; idx<GTPL_TEMPLATE_SIZE; idx++) {
                arr[idx] = rand();
            }
            break;
        }
        case GTPL_REPEAT:{
            uint32_t i = 0;
            char pat[8];
            int word_size = patterns[patten_id].pattern_size;
            strncpy(pat, patterns[patten_id].pattern, word_size);
            for (i=0; i<GTPL_GENLEN*word_size; i+=word_size) {
                memcpy(arr+i, (unsigned char*)pat, word_size);
            }
            break;
        }
        case GTPL_FIXED:{
            char pat[8];
            int word_size = patterns[patten_id].pattern_size;
            strncpy(pat, patterns[patten_id].pattern, word_size);
            memcpy(arr, (unsigned char*)pat, word_size);
            break;
        }
    }
}


uint64_t gtpl_reverse_bytes(uint64_t in, uint32_t num_bytes_to_reverse) {
   uint64_t reversed = 0;
   uint32_t idx=0;
   for (idx=0; idx<num_bytes_to_reverse; idx++) {
       reversed = (reversed<<8) | (in & 0xff);
       in = in >> 8;
   }
   return reversed;
}


uint64_t gtpl_pow(uint32_t base, uint32_t exponent) {
    int cnt=0;
    uint64_t result = 1;
    for (cnt=0; cnt<exponent; cnt++) {
        result = result * base;
    }
    return result;
}


void gtpl_print_template(char *arr, gtpl_data_pattern_type type){
    uint32_t idx;
    uint32_t value;
    switch (type) {
        case GTPL_INCBYTE:{
            break;
        }
        case GTPL_INCWORD:{
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
    }
}


int main() {
    // time is the seed
    // can be used again to generate same random valued
    srand((unsigned int) time (NULL));
    gtpl_gen();
    gtpl_free();
}
