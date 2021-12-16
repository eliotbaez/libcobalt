/*
 * construct_guidetable.c
 * by Eliot Baez
 *
 * This program will construct a table of guide addresses, similar to guide
 * words in a paper dictionary.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cobalt.h"

#define GUIDETABLE_NAME	"guidetable.bin"

int main(int argc, char **argv) {
	/* Index for guidetable; needs to be greater than 16 bits to prevent integer
	 * overflow from causing an infinite loop. Ask me how I know. */
	uint32_t first2chars;
	uint16_t buf;	/* stores the actual first 2 chars of the string */
	uint16_t word;	/* index for wordmap */
	uint16_t guidetable[0x10000];
	size_t i;	/* counting variable for unit test stuff at the end */
	FILE *out;

	out = fopen("guidetable.bin", "w");
	if (out == NULL) {
		fprintf(stderr, "%s: Error opening file %s\n", argv[0], GUIDETABLE_NAME);
		return EXIT_FAILURE;
	}

	/* build the guide table */
	/* in construct_map.c, the first actual word is placed at index 256, since
	   the first 255 indices are reserved, so we will initialize our word index
	   accordingly. */
	word = 0x100;
	for (first2chars = 0; first2chars < 0x10000; ) {
		/* This line is pretty difficult to understand if you haven't
		   familiarized yourself with the structures that this library uses.
		   Make sure to read /include/cobalt.h first. */
		buf = *( (uint16_t *) &(WORDTABLE[WORDMAP[word]]) );
		
		if (buf > first2chars) {
			/* if this pair of characters doesn't exist in the word table, then
			   set the value of the guide table at this index to the last word
			   in the word table. */
			guidetable[first2chars++] = NUMBER_OF_WORDS - 1;
		} else if (buf == first2chars) {
			guidetable[first2chars++] = word;
			/* increment if there are more words to come */
			if (word < NUMBER_OF_WORDS - 1) ++word;
		} else {
			/* buf is too small, we have to scroll farther if there are
			   more words */
			if (word < NUMBER_OF_WORDS - 1) {
				++word;
			} else {
				/* if we are at the end of the list, just give up */
				guidetable[first2chars++] = NUMBER_OF_WORDS - 1;
			}
		}
	}
	
	fprintf(stderr, "%s: Writing guide table to %s\n",
			argv[0], GUIDETABLE_NAME);
	fwrite(guidetable, 2, 0x10000, out);
	fclose(out);
	
	/* test lookups, basically unit test stuff */
	for (i = 1; i < argc; ++i) {
		printf("%s\n", argv[i]);
		buf = *( (uint16_t *)argv[i]);
		printf("\tfirst 2 bytes are 0x%04hx\n", buf);
		printf("\tfirst match is    0x%04hx\n", guidetable[buf]);
		printf("\tword is \"%s\"\n", WORDTABLE + WORDMAP[guidetable[buf]]);
	}
	
	fprintf(stderr, "%s: Done.\n", argv[0]);
	return 0;
}
