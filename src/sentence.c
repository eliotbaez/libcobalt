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
 * This function takes a null-terminated string as input, interpreted as a
 * sentence. the sentence is split into words that are separated by spaces. Each
 * word is then looked up with cblt_findword(). If the word is found in the
 * word table, then its ordinal number is written to the output array of
 * uint16_t's.
 * 
 * If the word is not found, then the string that contains the word is copied
 * directly into the array, including the null terminating character.
 *
 * It is the job of the programmer to free() the returned pointer after doing
 * something meaningful with the output of this function.
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
	
	/* The compressed sentence will never have a length greater than twice the
	   input string, so we will allocate enough memory for the worse case
	   scenario. */
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
			/* Increment i by ceil(length / 2). Since there's no ceiling
			   division operator for ints, we'll do this the fun way. */
			i += (length / 2 + (length % 2 != 0));
		} else {
			compressed[i++] = (uint16_t)wordNum;
		}
	}
	compressed[i] = 0;
	free(mSentence);

	return compressed;
}


char *cblt_decodeSentence(const uint16_t *compressed) {
	size_t i;		/* index for compressed */
	size_t j;		/* index for sentence */
	size_t size;	/* size of memory block needed to decode compressed */
	size_t length;	/* length of a string */
	char *sentence;

	if (compressed == NULL)
		return NULL;
	
	/* find the amount of memory needed to decode */
	size = 1;
	for (i = 0; ; ) {
		if (compressed[i] == 0) {
			/* null terminator */
			break;
		} else if (compressed[i] == CBLT_BEGIN_STRING) {
			/* incrementing i to skip past the CBLT_BEGIN_STRING signal 
			   The +1 is because there is a space after every string.*/
			length = strlen( (char *)&compressed[++i] ) + 1;
			size += length;
			/* increment i by ceil((length + 1) / 2) */
			i += (length / 2 + (length % 2 != 0));
		} else if (compressed[i] < WORDMAP_LEN) {
			/* in range! */
			/* +1 because all words are separated by a space too */
			size += strlen(WORDTABLE + WORDMAP[ compressed[i] ] ) + 1;
			++i;
		} else {
			/* otherwise do nothing and skip to the next int if
			   compressed[i] is out of range */
			++i;
		}
	}

	sentence = malloc(sizeof(char) * size);
	if (sentence == NULL)
		return NULL;

	for (i = 0, j = 0 ; ; ) {
		if (compressed[i] == 0) {
			/* use the character before sentence[j] and terminate the sentence
			   there, because the function tacks on a space after every word,
			   even the last one. */
			sentence[j - 1] = '\0';
			break;
		} else if (compressed[i] == CBLT_BEGIN_STRING) {
			length = strlen( (char *)&compressed[++i] );
			memcpy(sentence + j, compressed + i, length);
			j += length;
			/* increment length beforehand to make this calculation
			   similar to the ones above */
			++length;
			i += (length / 2 + (length % 2 != 0));
			/* and append a space like we said */
			sentence[j++] = ' ';
		} else if (compressed[i] < WORDMAP_LEN) {
			length = strlen(WORDTABLE + WORDMAP[ compressed[i] ]);
			memcpy(sentence + j, WORDTABLE + WORDMAP[ compressed[i] ], length);
			j += length;
			++i;
			/* append a space */
			sentence[j++] = ' ';
		} else {
			/* compressed[i] is out of range, do nothing */
			++i;
		}
	}

	return sentence;
}

#if 0
int main(int argc, char **argv) {
	uint16_t *compressed;
	char *restored;
	if (argc != 2) {
		printf("bruh\n");
		return 1;
	}

	compressed = cblt_encodeSentence(argv[1]);
	restored = cblt_decodeSentence(compressed);

	printf("%s\n%s\n", argv[1], restored);
	free(compressed);
	free(restored);
	return 0;
}
#endif
