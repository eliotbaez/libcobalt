/*
 * sentence.c
 * by Eliot Baez
 *
 * This file contains the definitions of functions used for encoding and
 * decoding whole sentences.
 */

#include <stdlib.h>	/* malloc */
#include <string.h>	/* strtok, strlen, memcpy */
#include <stdint.h>	/* uint16_t, int32_t */
#include <stdio.h>

#include "cobalt.h"

/* this is where the magic happens >:) */

/*
 * This function takes a null-terminated string as input, interpreted
 * as a sentence. The sentence is split into words that are separated
 * by spaces. Each word is then looked up with cblt_findWord(). If the
 * word is found in the word list, then its ordinal number is written
 * to the output array of uint16_t's.
 * 
 * If the word is not found, then the string that contains the word is
 * copied directly into the array, including the null terminating
 * character.
 *
 * It is the job of the programmer to free() the returned pointer after
 * doing something meaningful with the output of this function.
 */
uint16_t *cblt_encodeSentence(const char *sentence) {
	char *mSentence;		/* points to a mutable copy of sentence */
	uint16_t *compressed;	/* the compressed sentence */
	size_t i;				/* index for compressed */
	char *word;				/* points to a string that is a word */
	int32_t wordNum;		/* stores result of cblt_findWord() */
	size_t length;			/* stores the length of a string */

	if (sentence == NULL)
		return NULL;
	
	/* We make a mutable copy of sentence because strtok() overwrites
	 * characters in its first argument. */
	length = strlen(sentence);
	mSentence = malloc(sizeof(char) * (length + 1));
	if (mSentence == NULL)
		return NULL;
	strcpy(mSentence, sentence);
	
	/* The compressed sentence will never have a length greater than
	   twice the input string, so we will allocate enough memory for the
	   worst case scenario. */
	compressed = malloc(sizeof(uint16_t) * (2 * length + 1));
	if (compressed == NULL) {
		free(mSentence);
		return NULL;
	}

	for (i = 0, word = mSentence; ; word = NULL) {
		word = strtok(word, " ");
		if (word == NULL)
			break;
		
		wordNum = cblt_findWord(word);
		if (wordNum == CBLT_WORD_NOT_FOUND) {
			compressed[i++] = CBLT_BEGIN_STRING;
			/* In this case, length will include the null byte for
			   practicality reasons */
			length = strlen(word) + 1;
			memcpy(&compressed[i], word, length);
			printf("%s: word not found\n", (char *)&compressed[i]);
			/* Increment i by ceil(length / 2). Since there's no ceiling
			   division operator for ints, we'll do this the fun way. */
			i += (length / 2 + (length % 2 != 0));
		} else {
			printf("%s is word %d\n", word, wordNum);
			compressed[i++] = (uint16_t)wordNum;
		}
	}
	compressed[i] = 0;
	free(mSentence);

	return compressed;
}

int main(int argc, char **argv) {
	uint16_t *compressed;
	if (argc != 2) {
		printf("bruh\n");
		return 1;
	}

	compressed = cblt_encodeSentence(argv[1]);
	free(compressed);
	return 0;
}
