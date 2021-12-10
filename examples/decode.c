/*
 * decode.c
 * by Eliot Baez
 *
 * This file is an example of how to use libcobalt to write a simple text
 * decoding application to undo the encoding of the simple text encoding
 * application.
 *
 * Usage:	./decode COMPRESSEDFILE TXTFILE
 * 	COMPRESSEDFILE  A file containing compressed data, hopefully the output of
 * 	                the corresponding encode command
 * 	TXTFILE         The file where the decoded text will be written
 * 	                Pass /dev/stdout as TXTFILE to conveniently print the
 * 	                decoded contents of COMPRESSEDFILE
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "cobalt.h"

int main(int argc, char **argv) {
	size_t sz;
	char *sentence;		/* input file buffer */
	uint16_t *encoded;	/* output file buffer */
	FILE *infile, *outfile;
	
	if (argc != 3) {
		fprintf(stderr, "Usage:\t%s COMPRESSEDFILE TXTFILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	infile = fopen(argv[1], "r");
	if (infile == NULL) {
		fprintf(stderr, "%s: Error opening file %s\n", argv[0], argv[1]);
		return EXIT_FAILURE;
	}
	outfile = fopen(argv[2], "w");
	if (outfile == NULL) {
		fprintf(stderr, "%s: Error opening file %s\n", argv[0], argv[2]);
		fclose(infile);
		return EXIT_FAILURE;
	}

	/* read stuff */
	fseek(infile, 0, SEEK_END);
	sz = ftell(infile) / sizeof(uint16_t);
	rewind(infile);
	encoded = malloc(sizeof(uint16_t) * (sz + 1));	/* +1 for null terminator */
	if (encoded == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		fclose(infile);
		fclose(outfile);
		return (EXIT_FAILURE);
	}
	fread(encoded, sz, sizeof(uint16_t), infile);
	encoded[sz] = 0;
	fclose(infile);

	/* decode stuff */
	sentence = cblt_decodeSentence(encoded);

	/* write stuff */
	sz = strlen(sentence);
	fwrite(sentence, sizeof(char), sz, outfile);
	fclose(outfile);

	free(sentence);
	free(encoded);
	
	return 0;
}
