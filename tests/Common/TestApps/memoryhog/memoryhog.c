#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    
    void * p;
    int  megabytes = 512;
    int  offset;

    printf("MEMORYHOG: MYPID: %d, gonna eat %d MB of mem.\n", getpid(), megabytes);
	
    for (; megabytes > 0; --megabytes) {
	
	p = malloc(1024 * 1024);
	
	/* write a byte in every 4k page in the malloc'd
	   block so that every page gets *really* allocated */
	
	if (!p) {
	    printf("could not allocate new block\n");
	    printf("%d megabytes left unallocated.\n", megabytes);
	    exit(1);
	}

	offset = 0;
	while (offset < 1024 * 1024) {
	    memset(p + offset, 1, 1);
	    offset += 1024;
	}

    }

    printf("MEMORYHOG: I got it all.\n");
    
    printf("Press ENTER.\n");
    fflush(stdout);
    read(0, p, 1);
    return 0;
}
