#include <stdio.h>


int uniform_distribution_secure(int rangeLow, int rangeHigh) {
    int range = rangeHigh - rangeLow + 1;
    int secureMax = RAND_MAX - RAND_MAX % range;
    int x;
    do x = secure_rand(); while (x >= secureMax);
    return rangeLow + x % (secureMax / range);
}


int main ()
{
    
    return 0;
}
