/* 
 * getencodedlength.c
 *
 * This is a test program that will take a single string as a command line
 * argument and pass it to cblt_getEncodedLength(), to print the result.
 * It is the responsibility of the programmer to make sure the value printed
 * actually makes sense.
 * 
 * This program should be linked with libcobalt at compile time.
 */

#include <stdio.h>
#include <stdlib.h>
#include "cobalt.h"

int main(int argc, char **argv) {
    size_t length;

    if (argc != 2) {
		fprintf(stderr, "%s requires one argument.\n", argv[0]);
		return EXIT_FAILURE;
	}

    length = cblt_getEncodedLength(argv[1]);
    printf("\"%s\"\nLength is %zd\n", argv[1], length);

    return 0;
}
