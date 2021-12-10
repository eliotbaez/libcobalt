/*
 * encode.c
 * by Eliot Baez
 *
 * This file is an example of how to use libcobalt to write a simple text
 * encoding application.
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
		fprintf(stderr, "Usage:\t%s TXTFILE COMPRESSEDFILE\n", argv[0]);
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
	sz = ftell(infile);
	rewind(infile);
	sentence = malloc(sizeof(char) * (sz + 1));	/* +1 for null terminator */
	if (sentence == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		fclose(infile);
		fclose(outfile);
		return (EXIT_FAILURE);
	}
	fread(sentence, sz, sizeof(char), infile);
	sentence[sz] = '\0';
	fclose(infile);

	/* encode stuff */
	encoded = cblt_encodeSentence(sentence);

	/* write stuff */
	sz = cblt_getUint16BlockSize(encoded);
	fwrite(encoded, sizeof(uint16_t), sz, outfile);
	fclose(outfile);

	free(sentence);
	free(encoded);
	
	return 0;
}
