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

#define WORDLIST_NAME	"50k-newline-separated.txt"
#define WORDLIST_SORTED_NAME	"50k-newline-separated-sorted.txt"

/* this method of sorting strings is taken directly from the example
   code in the man page for qsort() */
static int cmpstringp(const void *p1, const void *p2) {
	return strcmp(*(const char **) p1, *(const char **) p2);
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
	qsort(substrings, words, sizeof(char *), cmpstringp);
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
