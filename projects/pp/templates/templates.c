#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define GTPL_TEMPLATE_SIZE              1024*16
#define GTPL_RPT_PATTERNS               10
#define GTPL_DBG_PRINT_TEMPLATE_SIZE    128

typedef enum {
    GTPL_INCBYTE,
    GTPL_INCWORD,
    GTPL_DECBYTE,
    GTPL_DECWORD,
    GTPL_RANDOM
} dataType;

void genTemplate(char *arr, dataType type);
void printTemplate(char *arr, dataType type);

int main() {
    char *incByte;
    char *incWord;
    char *decByte;
    char *decWord;
    char *random;
    char *repeating[GTPL_RPT_PATTERNS];

    // allocate template memory
    incByte = (char*) malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    incWord = (char*) malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    decByte = (char*) malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    decWord = (char*) malloc(GTPL_TEMPLATE_SIZE*sizeof(char));
    random  = (char*) malloc(GTPL_TEMPLATE_SIZE*sizeof(char));

    // generate templates
    genTemplate(incByte, GTPL_INCBYTE);
    // genTemplate(incWord, GTPL_INCWORD);
    // genTemplate(decByte, GTPL_DECBYTE);
    // genTemplate(decWord, GTPL_DECWORD);
    // genTemplate(random,  GTPL_RANDOM);

    // clean templates
    free(incByte);
    free(incWord);
    free(decByte);
    free(decWord);
    free(random);

    return 0;
}

void genTemplate(char *arr, dataType type) {
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
            for (idx=4, value=0; idx<GTPL_TEMPLATE_SIZE; value++, idx+=4) {
                arr[idx] = idx;
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
        default:{
            break;
        }
    }
}

void printTemplate(char *arr, dataType type){
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
            for (idx=4, value=0; idx<GTPL_TEMPLATE_SIZE; value++, idx+=4) {
                arr[idx] = idx;
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
        default:{
            break;
        }
    }
}
