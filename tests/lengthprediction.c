/* 
 * lengthprediction.c
 *
 * This is a test program that will take a single string as a command line
 * argument. The encoded length of the string is calculated and printed. The
 * string is then compressed, and the actual length is printed. Finally, the
 * decoded length is calculated and printed.
 * 
 * This program should be linked with libcobalt at compile time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cobalt.h"

int main(int argc, char **argv) {
    size_t length;
    uint16_t *block;

    if (argc != 2) {
		fprintf(stderr, "%s requires one argument.\n", argv[0]);
		return EXIT_FAILURE;
	}

    length = cblt_getEncodedLength(argv[1]);
    printf("\"%s\"\nPredicted encoded length is %zd\n", argv[1], length);

    block = cblt_encodeSentence(argv[1]);
    if (block == NULL) {
        fprintf(stderr, "cblt_encodeSentence() returned NULL\n");
        return EXIT_FAILURE;
    }
    length = cblt_getUint16BlockSize(block);
    printf("Actual encoded length is %zd\n", length);

    length = cblt_getDecodedLength(block);
    printf("Predicted decoded length is %zd\n"
        "Actual string length is %zd\n",
        length, strlen(argv[1]) + 1);

    /* The updated decode function has not been implemented at this time. */
    free(block);
    return 0;
}
