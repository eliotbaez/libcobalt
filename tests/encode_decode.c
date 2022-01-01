/*
 * encode_decode.c
 *
 * This test program takes 0 or 1 command line arguments and passes it through
 * cblt_encodeSentence() and cblt_decodeSentence(). The output of the latter
 * will be compared to the original input. If the strings match, the program
 * exits successfully. 
 * 
 * This program is to be linked with libcobalt at compile time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cobalt.h"

int main(int argc, char **argv) {
    const char empty = '\0';
    const char *sentence;
    uint16_t *encoded;
    char *decoded;
    bool identical;

    if (argc == 1) {
        sentence = &empty;
    } else if (argc == 2) {
        sentence = argv[1];
    } else {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Input:\n\"%s\"\n", sentence);

    encoded = cblt_encodeSentence(sentence);
    if (encoded == NULL) {
        fprintf(stderr, "Error during encoding.\n");
        return EXIT_FAILURE;
    }
    decoded = cblt_decodeSentence(encoded);
    if (decoded == NULL) {
        fprintf(stderr, "Error during decoding.\n");
        return EXIT_FAILURE;
    }

    printf("Output:\n\"%s\"\n", decoded);
    identical = cblt_streq(sentence, decoded);
    free(encoded);
    free(decoded);

    if (identical) {
        printf("Strings are identical\n");
        return EXIT_SUCCESS;
    } else {
        printf("Strings are not identical!\n");
        return EXIT_FAILURE;
    }
}