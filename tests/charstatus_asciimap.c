/*
 * charstatus_asciimap.c
 *
 * This test program will use the rules defined in cblt_getCharStatus() to 
 * determine the type of every possible value held by an unsigned char. The
 * output will be written to stdout, and no command line arguments will be
 * taken.
 */

#include <stdio.h>
#include <stdlib.h>
#include "splitstring.h"

int main(int argc, char **argv) {
    unsigned short i;
    int status;

    if (argc != 1) {
        fprintf(stderr, "%s does not take any arguments.\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (i = 0; i < 0x100; ++i) {
        status = cblt_getCharStatus(i);
        printf("0x%02hhX: %-11s ('%1c')\n", i, cblt_getStatusName(status), i);
    }

    return 0;
}