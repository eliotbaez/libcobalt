/*
 * construct_map.c
 * by Eliot Baez
 *
 * This is a helper program to index all the different words in the word table
 * by the index of the word and its offset in the WORDTABLE array.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cobalt.h"

#define MAP_NAME "wordmap.bin"
#define WORDMAP_LEN NUMBER_OF_WORDS + 256

int main (int argc, char **argv) {
	size_t i; /* offset within the WORDTABLE char array */
	unsigned int word;
	uint32_t *wordMap;
	FILE *out;

	out = fopen(MAP_NAME, "wb");
	if (out == NULL) {
		fprintf(stderr, "%s: Error opening file %s.\n", argv[0], MAP_NAME);
		return EXIT_FAILURE;
	}

	/* WORDMAP_LEN is defined as 256 + NUMBER_OF_WORDS.
	   There will be NUMBER_OF_WORDS + 256 elements in wordMap, because the
	   first 256 addresses are reserved for special characters. This means that
	   words 0-255 will not point to an actual word in the table. I have made
	   the executive decision to fill all these elements with the address of the
	   last character in the wordtable, the null terminator. This way, the first
	   256 addresses all point to an empty string. */
	wordMap = malloc(sizeof(uint32_t) * (WORDMAP_LEN));
	if (wordMap == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* now we have enough memory, we may proceed */
	fprintf(stderr, "%s: Writing word map to %s\n", argv[0], MAP_NAME);
	/* start by filling the first 256 addresses */
	for (word = 0; word < 0x100; ++word)
		wordMap[word] = WORDTABLE_STRLEN;

	for (i = 0; i < WORDTABLE_STRLEN; ) { /* each byte in WORDTABLE */
		wordMap[word++] = i;
		/* advance to the next word */
		while (WORDTABLE[i++] != '\0') ;
			/* skip until after the next null character */
	}

	/* now the address block is filled, let's write it to a file */
	fwrite(wordMap, sizeof(uint32_t), WORDMAP_LEN, out);
	fclose(out);
	free(wordMap);

	fprintf(stderr, "%s: Done.\n", argv[0]);
	return 0;
}
