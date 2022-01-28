// gcc -S file.c 
//  or
// create a.out and then use objdump, example
//  bjdump -S --disassemble a.out > a.asm

#include <stdio.h>
#include <asm/system.h>

int main() {
    int temp;
    temp=temp+1;
    printf("value of temp is: %d", temp);
}
