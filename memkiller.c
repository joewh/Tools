/*
 * memkiller.c
 *
 * Allocates memory as much as possible.
 * Can be used to reproduce OOM events in the system.
 *
 *  Created on: Oct 1, 2018
 *      Author: Joerg Weinhardt
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern int usleep (__useconds_t __useconds);
void printUsage(void);


int main (int argc, char **argv)
{
    int fastFlag = 0;
    int stopFlag = 0;
    unsigned int memAmount = 0; // 0 = infinite, until OOM

    optind = 1;		// Reset parsing
    while (1)
    {
        char *endPtr = NULL;

        int opt = getopt(argc, argv, "hfm:");  // accept -h and -f without arguments, -m has one parameter

        if (opt == -1)	// getopt() has finished?
            break;

        switch (opt)
        {
        case 'h':
            printUsage();
            return EXIT_SUCCESS;
        case 'f':
            fastFlag = 1;
            break;
        case 'm':
            memAmount = (unsigned int) strtoul(optarg, &endPtr, 10);
            if (endPtr != NULL && endPtr[0] != '\0')
            {
                fprintf(stderr, "Illegal memory amount: %s\n", optarg);
                printUsage();
                return EXIT_FAILURE;
            }
            stopFlag = 1;
            break;
        default:
            fprintf(stderr, "Cancelled\n");
            printUsage();
            return EXIT_FAILURE;
        }
    } // while(1)

    // malloc-Loop
    unsigned int n = 0;
    while (1) {
    	void *memPtr;

        if (memAmount > 0 && (n >= memAmount))
            break;

        memPtr = malloc(1024 * 1024);

        if (memPtr == NULL)
        {
            printf("malloc failure after %u MiB\n", n);

            if (stopFlag)	// break if stop required
            	return 0;
        }
        else
        {
        	memset(memPtr, 0xaa, 1024 * 1024);
        	printf ("got %u MiB\n", ++n);
        }

        if (!fastFlag)
            usleep(1000 * 10);
    }
}


void printUsage(void)
{
	printf("memkiller [-h] [-f] [-m <sizeMB>]\n");
	printf("Allocates all the memory\n");
	printf("  -h           Prints this help\n");
	printf("  -f           Fast (no usleep in the loop)\n");
	printf("  -m <sizeMB>  Stop after <size> MBytes has been allocated\n");
	printf("\n");
}
