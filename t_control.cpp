#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#define ASIZE 9

const char alphabet[] = {
    0x40, 0x42, 0x45, 0x47, 0x48, 0x4A, 0x4D, 0x4F, 0x90
};

int main(int argc, char** argv) {
    srand((int)time(NULL));

    double surprise = 0.0;
    if (argc>1) sscanf(argv[1], "%lf", &surprise);

    int freq[ASIZE];
    for (int i=0;i<ASIZE;i++) freq[i]=0;

    int pfxcnt = 0;
    int forcecnt = 0;
    for (int i=0;i<512;i++) {
        int max = std::distance(freq, std::max_element(freq, freq+ASIZE));
        int j=max;
        if (pfxcnt == 14) {
            pfxcnt = 0;
            forcecnt++;
            j = ASIZE-1; //0x90
        }
        else if (!forcecnt && ((double)rand()/RAND_MAX <= surprise-(1.0/15))) {
            j = rand()%ASIZE;
        }
        else {
            j=max;
            if (forcecnt) forcecnt--;
        }
        printf("%c", alphabet[j]);
        freq[j]++;

        if (j < ASIZE-1) {
            pfxcnt++;
        }
    }

    return 0;
}
