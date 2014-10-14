#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#define SLEDSIZE 512

const unsigned char alphabet[] = {
    0x40, 0x42, 0x45, 0x47, 0x48, 0x4A, 0x4D, 0x4F, 0x90
};

unsigned char buffer[SLEDSIZE];

// Compile:
//  g++ -z execstack t_control.cpp -o t_control

int main(int argc, char** argv) {
    srand((int)time(NULL));

    double surprise = 0.0;

    if (argc>1)
        sscanf(argv[1], "%lf", &surprise);
    else {
        printf("Please supply a entropy [0.0 - 0.5] as an arg\n");
        exit(0);
    }

    int asize = sizeof(alphabet);

    int freq[asize];
    for (int i=0;i<asize;i++) freq[i]=0;

    int pfxcnt = 0;
    int forcecnt = 0;
    for (int i=0; i < SLEDSIZE; i++) {
        int max = std::distance(freq, std::max_element(freq, freq+asize));
        int j=max;
        if (pfxcnt == 14) {
            pfxcnt = 0;
            forcecnt++;
            j = asize-1; //0x90
        }
        else if (!forcecnt && ((double)rand()/RAND_MAX <= surprise-(1.0/15))) {
            j = rand()%asize;
        }
        else {
            j=max;
            if (forcecnt) forcecnt--;
        }

        // copy the data over to the working buffer
        buffer[i] = alphabet[j];

        freq[j]++;

        if (j < asize-1) {
            pfxcnt++;
        }
    }

    printf("\n");
    for(int i=0; i<SLEDSIZE; i++)
        printf("\\x%02x", buffer[i]);
    printf("\nNow to run and test it.\n");

    buffer[SLEDSIZE] = 0xc3;

    // call the created sled,  compile with -z execstack to work.
    int (*func)() = (int(*)())buffer;
    func();
    printf("made it!\n");

    return 0;
}
