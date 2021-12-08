/*
 * construct_map.c
 * by Eliot Baez
 *
 * This is a helper program to index all the different words in the word
 * table by the index of the word and its offset in the data block.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "wordtable.h"

#ifndef MAP_NAME
#define MAP_NAME "wordmap.bin"
#endif

int main (int argc, char **argv) {
	size_t i; /* offset within the WORDTABLE char array */
	unsigned int word = 0;
	uint32_t *addrBlock;
	FILE *out;

	out = fopen(MAP_NAME, "w");
	if (out == NULL) {
		fprintf(stderr, "%s: Error opening file %s.\n", argv[0], MAP_NAME);
		return EXIT_FAILURE;
	}

	/* otherwise if &out is a valid file... */
	addrBlock = malloc(4 * NUMBER_OF_WORDS);
	if (addrBlock == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* now we have enough memory, we may proceed */
	fprintf(stderr, "%s: Writing word map to %s\n", argv[0], MAP_NAME);
	for (i = 0; i < WORDTABLE_LEN; ) { /* each byte in WORDTABLE */
		addrBlock[word++] = i;
		/* advance to the next word */
		while (WORDTABLE[i++] != '\0') ;
			/* skip until after the next null character */
	}

	/* now the address block is filled, let's write it to a file */
	fwrite(addrBlock, 4, NUMBER_OF_WORDS, out);
	fclose(out);
	free(addrBlock);

	fprintf(stderr, "%s: Done.\n", argv[0]);
	return 0;
}
