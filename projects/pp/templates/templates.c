#include <stdio.h>
#include <stdlib.h> // malloc
#include <stdint.h>

#define TEMPLATE_SIZE   1024*16
#define RPT_PATTERNS    10

typedef enum {
    INCBYTE,
    INCWORD,
    DECBYTE,
    DECWORD,
    RANDOM
} dataType;

void genTemplate(char *arr, dataType type);

int main() {

    char *incByte;
    char *incWord;
    char *decByte;
    char *decWord;
    char *random;
    char *repeating[RPT_PATTERNS];

    // allocate template memory
    incByte = (char*) malloc(TEMPLATE_SIZE*sizeof(char));
    incWord = (char*) malloc(TEMPLATE_SIZE*sizeof(char));
    decByte = (char*) malloc(TEMPLATE_SIZE*sizeof(char));
    decWord = (char*) malloc(TEMPLATE_SIZE*sizeof(char));
    random  = (char*) malloc(TEMPLATE_SIZE*sizeof(char));

    // generate templates
    genTemplate(incByte, INCBYTE);
    genTemplate(incWord, INCWORD);
    genTemplate(decByte, DECBYTE);
    genTemplate(decWord, DECWORD);
    genTemplate(random,  RANDOM);

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
    switch (type) {
        case INCBYTE:{
            for (idx=0; idx<TEMPLATE_SIZE; idx++) {
                arr[idx] = idx;
            }
            break;
        }
        case INCWORD:{
            for (idx=0; idx<TEMPLATE_SIZE; idx+=4) {
                arr[idx] = idx;
            }
            break;
        }
        case DECBYTE:{
            break;
        }
        case DECWORD:{
            break;
        }
        case RANDOM:{
            break;
        }
        default:{
            break;
        }
    }
}
