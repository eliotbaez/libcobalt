/* 
 * sort_wordlist.c
 * by Eliot Baez
 *
 * This program opens the already uncommented wordlist file and sorts
 * its contents alphabetically into a new file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define WORDLIST_NAME	"50k-newline-separated.txt"
#define WORDLIST_SORTED_NAME	"50k-newline-separated-sorted.txt"

/*
 * This is a conceptually dense function, so let me explain.
 *
 * qsort() will call this function, passing it two string pointers.
 * Emphasis on *string pointers*; we are dealing with pointers TO char
 * pointers. 
 *
 * uint16_t is twice the width of unsigned char, so we will cast the
 * pointers to uint16_t** first to let the compiler know of our
 * intentions. From there, we dereference the pointers twice, and assign
 * their values to two uint16_t's. These integers will effectively store
 * the first 2 chars as a combined value that can be directly compared
 * to the first 2 chars of another string.
 *
 * We can now effectively sort strings by only the first 2 characters.
 * Do not pass pointers to empty strings to this function. Length must
 * be at least 1, excluding the null byte.
 */
static int cmpstringp_first2(const void *p1, const void *p2) {
	uint16_t i1 = **(uint16_t **) p1;
	uint16_t i2 = **(uint16_t **) p2;
	return i1 - i2;
}

int main (int argc, char **argv) {
	size_t i;		/* index for buf */
	size_t word;	/* index for substrings*/
	unsigned int words;	/* number of words in wordlist */
	size_t size;	/* size of file */
	char *buf;		/* buffer for storing file */
	char *sortedBuf;	/* output buffer */
	char **substrings;	/* array of null terminated words */
	FILE *fp;		/* take a guess */

	fp = fopen(WORDLIST_NAME, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: Error opening file %s.\n", argv[0], WORDLIST_NAME);
		return EXIT_FAILURE;
	}

	/* otherwise if &fp is a valid file... */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	buf = malloc(size + 1);	/* +1 for null terminator */
	sortedBuf = malloc(size + 1);
	if (buf == NULL || buf == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* read entire file into memory and close file */
	fread(buf, 1, size, fp);
	fclose(fp);
	buf[size] = '\0';

	/* count words and convert newlines to null terminators */
	words = 0;
	for (i = 0; i < size; i++) {
		if (buf[i] == '\n'){
			buf[i] = '\0';
			words++;
		}
	}
	
	/* more memory allocation */
	substrings = malloc(words * sizeof(char *));
	if (substrings == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		free(buf);
		return EXIT_FAILURE;
	}

	/* now build the index of all the words to pass to qsort */
	word = 0;
	for (i = 0 ; i < size; ) {
		substrings[word++] = buf + i;
		/* advance to the next word */
		while (buf[i++] != '\0') ;
			/* skip until after the next null character */
	}
	
	/* now we can finally sort! */
	fprintf(stderr, "%s: Sorting %u words...\n", argv[0], words);
	qsort(substrings, words, sizeof(char *), cmpstringp_first2);
	fprintf(stderr, "%s: Done sorting.\n", argv[0]);

	/* Sorting is done, now concatenate the strings in order into
	   ANOTHER BUFFER!!! I emphasize this because all the pointers in
	   the substrings array point to addresses that are part of buf.
	   Thus, we cannot overwrite buf right now. */
	word = 0;
	for (i = 0; i < size; ) {
		strcpy(sortedBuf + i, substrings[word]);
		i += strlen(substrings[word++]);
		sortedBuf[i++] = '\n';	/* make the null byte a newline */
	}
	
	fp = fopen(WORDLIST_SORTED_NAME, "w");
	if (fp == NULL) {
		fprintf(stderr, "%s: Error opening file %s.\n", argv[0], WORDLIST_NAME);
		free(substrings);
		free(buf);
		free(sortedBuf);
		return (EXIT_FAILURE);
	}
	
	/* finally write results to file */
	fprintf(stderr, "%s: Writing sorted word list to %s\n", argv[0], WORDLIST_SORTED_NAME);
	fwrite(sortedBuf, 1, size, fp);
	fclose(fp);

	free(sortedBuf);
	free(substrings);

	fprintf(stderr, "%s: Done.\n", argv[0]);
	return 0;
}
