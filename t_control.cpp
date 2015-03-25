#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

const unsigned char alphabet[] = {
    0x40, 0x42, 0x45, 0x47, 0x48, 0x4A, 0x4D, 0x4F, 0x90
};


// Compile:
//  g++ t_control.cpp -o t_control -Wall -ggdb -O2

int main(int argc, char** argv) {
    srand((int)time(NULL) ^ (getpid() * 4));

    double surprise = 0.0;
    int sledSize = 512;

    // TODO: Use getopt to change sledSize. Validate sledSize.
    if (argc>1)
        sscanf(argv[1], "%lf", &surprise);
    else {
        fprintf(stderr, "Please supply a entropy [0.0 - 0.5] as an arg\n");
        exit(1);
    }
    if (surprise > 0.5) {
        surprise = 0.5;
    }
    if (surprise < 0.0) {
        surprise = 0.0;
    }

    int asize = sizeof(alphabet);

    int freq[asize];
    for (int i=0;i<asize;i++) freq[i]=0;

    // Need to allocate more than sledsize because buffer[sledSize] = 0xc3;
    // Read, write, no, all we do is execute. -- Dual Core 'Kick Back'
    // Note that this is insecure by choice. We could change this to 
    // READ/WRITE, then switch to READ/EXECUTE aftewards, but when handing over 
    // control to an unknown set of assembly, you're not getting much 
    // protection from this.
    unsigned char *buffer = (unsigned char *)mmap(0, sledSize + 24, PROT_READ | 
        PROT_WRITE | PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    if (buffer == 0) {
        perror("mmap");
        exit(1);
    }
    
    int pfxcnt = 0;
    int forcecnt = 0;
    for (int i=0; i < sledSize; i++) {
        // FIXME: Too many nops, we want other things to be the max.
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
    for(int i=0; i<sledSize; i++)
        printf("\\x%02x", buffer[i]);
    printf("\n");

#ifdef I_AM_CONFIDENT_THAT_I_KNOW_WHAT_I_AM_DOING_NO_REALLY
    printf("Now to run and test it.\n");

    buffer[sledSize] = 0xc3;

    // call the created sled,  compile with -z execstack to work.
    int (*func)() = (int(*)())buffer;
    func();
    printf("made it!\n");
#endif // I_AM_CONFIDENT_THAT_I_KNOW_WHAT_I_AM_DOING_NO_REALLY
    
    munmap(buffer, sledSize + 24);
    
    return 0;
}
