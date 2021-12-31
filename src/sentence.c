/*
 * sentence.c
 * by Eliot Baez
 *
 * This file contains the definitions of functions used for encoding and
 * decoding whole sentences.
 */

#include <stdlib.h>	/* malloc, size_t */
#include <string.h>	/* strtok, strlen, memcpy, strchr */
#include <stdint.h>	/* uint16_t, int32_t */
#include <ctype.h>	/* isalnum, isalpha */
#include <stdio.h>

#include "cobalt.h"
#include "sentence.h"
#include "splitstring.h"

/* this is where the magic happens >:) */

/* copies NC characters from S to DEST, casting from char to uint16_t for each
   character; 1 char in S translates to 1 uint16_t in DEST */
static void cblt_copyCharToUint16(const char *s, uint16_t *dest, size_t nc) {
	size_t i = 0;
	for ( ; i < nc; ++i)
		dest[i] = (uint16_t)s[i];
}

/* 
 * Get the length in elements of the memory block necessary to hold the encoded
 * version of sentence. INCLUDES the null terminating integer.
 * 
 * Returns a nonzero number on success.
 */
size_t cblt_getEncodedLength(const char *sentence) {
	size_t length;			/* length of strings */
	size_t encodedLength;	/* length of encoded integer block */
	char *mSentence;		/* mutable copy of sentence */
	char *group;			/* points to a group of characters */
	int 
		currentStatus,		/* the type of characters stored in group */
		nextStatus;			/* the type of characters following group */

	if (sentence == NULL)
		return 0;

	/* We make a mutable copy of sentence because cblt_splitstr() overwrites
	   modifies in its first argument. */
	length = strlen(sentence);
	mSentence = malloc(sizeof(char) * (length + 1));
	if (mSentence == NULL)
		return 0;
	strcpy(mSentence, sentence);

	/* use an approach similar to the decoding process to calculate how much
	   memory will be needed to store the compressed data */
	encodedLength = 1;

	group = cblt_splitstr(mSentence, &currentStatus, &nextStatus);
	switch (currentStatus) {
	case Word:
		if (cblt_findWord(group) != CBLT_WORD_NOT_FOUND) {
			++encodedLength;
		} else {
			/* string literal injection */
			/* Increment encodedLength by ceil(length / 2). Something
				something, the fun way of doing ceiling division :^) 
				We add 1 at the end because of the special 2-byte signal that
				the following bytes are a string literal. */
			length = strlen(group);
			encodedLength += (length / 2 + (length % 2 != 0)) + 1;
		}
		break;
	case Space:
		encodedLength += strlen(group);
		break;
	case Punctuation:
		encodedLength += strlen(group);
		if (nextStatus == Word)
			/* space omission signal */
			++encodedLength;
		break;
	case EndOfString:
		/* Do nothing; I just included this case to signify that I didn't forget
		   to include it. */
	}

	while (currentStatus != EndOfString) {
		group = cblt_splitstr(NULL, &currentStatus, &nextStatus);
		switch (currentStatus) {
		case Word:
			if (cblt_findWord(group) != CBLT_WORD_NOT_FOUND) {
				++encodedLength;
			} else {
				/* string literal injection */
				length = strlen(group);
				encodedLength += (length / 2 + (length % 2 != 0)) + 1;
			}
			break;
		case Space:
			/* TODO:
			   possibly consider optimizing this case? */
			if (nextStatus == Word) {
				/* 1 space before words (except the first word) are implicit.
				   All extra spaces are encoded by direct ASCII injection. */
				encodedLength += strlen(group) - 1;
			} else {
				/* Punctuation and End of String */
				/* All spaces before punctuation symbols, and all trailing
				   spaces before the end of the string, must be explicit */
				encodedLength += strlen(group);
			}
			break;
		case Punctuation:
			encodedLength += strlen(group);
			if (nextStatus == Word)
				/* space omission signal */
				++encodedLength;
			break;
		case EndOfString:
			/* Once again, do nothing. */
		}
	}

	free(mSentence);
	return encodedLength;
}

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
	size_t length;			/* stores the length of block of memory */

	char *mSentence;		/* points to a mutable copy of sentence */
	char *group;			/* points to a group of characters */
	int 
		currentStatus,		/* the type of characters stored in group */
		nextStatus;			/* the type of characters following group */

	uint16_t *compressed;	/* the compressed sentence */
	size_t i = 0;			/* index for compressed */
	int32_t wordNum;		/* stores result of cblt_findWord() */

	if (sentence == NULL)
		return NULL;
	
	/* We make a mutable copy of sentence because cblt_splitstr() modifies
	   characters in its first argument. */
	length = strlen(sentence);
	mSentence = malloc(sizeof(char) * (length + 1));
	if (mSentence == NULL)
		return NULL;
	strcpy(mSentence, sentence);
	
	length = cblt_getEncodedLength(sentence);
	compressed = malloc(sizeof(uint16_t) * (length));
	if (compressed == NULL) {
		free(mSentence);
		return NULL;
	}

	/* special treatment for first character group */
	group = cblt_splitstr(mSentence, &currentStatus, &nextStatus);
	switch (currentStatus) {
	case Word:
		wordNum = cblt_findWord(group);
		if (wordNum != CBLT_WORD_NOT_FOUND) {
			compressed[i++] = (uint16_t)wordNum;
		} else {
			/* string literal injection */
			compressed[i++] = CBLT_BEGIN_STRING;
			/* In this case, length will include the null byte for practicality
			   reasons */
			length = strlen(group);
			/* We fill this integer with all 1s, to avoid having an element be
			   all zero from the string's null terminator. This would cause a
			   premature termination of the integer block. 
			   length is incremented afterward to allow us to use memset's speed
			   over strcpy, as well as make the next calculation easier. */
			compressed[i + length++ / 2] = 0xffff;
			memcpy(&compressed[i], group, length);
			/* Increment i by ceil(length / 2). Since there's no ceiling
			   division operator for ints, we'll do this the fun way. */
			i += (length / 2 + (length % 2 != 0));
		}
		break;
	case Space:
		length = strlen(group);
		cblt_copyCharToUint16(group, compressed + i, length);
		i += length;
		break;
	case Punctuation:
		length = strlen(group);
		cblt_copyCharToUint16(group, compressed + i, length);
		i += length;
		if (nextStatus == Word)
			/* space omission signal */
			compressed[i++] = CBLT_NO_SPACE;
		break;
	case EndOfString:
		/* Do nothing; I just included this case to signify that I didn't forget
		   to include it. */
	}

	while (currentStatus != EndOfString) {
		group = cblt_splitstr(NULL, &currentStatus, &nextStatus);
		switch (currentStatus) {
		case Word:
			wordNum = cblt_findWord(group);
			if (wordNum != CBLT_WORD_NOT_FOUND) {
				compressed[i++] = (uint16_t)wordNum;
			} else {
				/* string literal injection */
				compressed[i++] = CBLT_BEGIN_STRING;
				length = strlen(group);
				compressed[i + length++ / 2] = 0xffff;
				memcpy(&compressed[i], group, length);
				i += (length / 2 + (length % 2 != 0));
			}
			break;
		case Space:
			if (nextStatus == Word) {
				/* 1 space before words (except the first word) are implicit.
				   All extra spaces are encoded by direct ASCII injection. */
				length = strlen(group) - 1;
				if (length != 0)	/* save a few push and pop instructions */
					cblt_copyCharToUint16(group, compressed + i, length);
				i += length;
			} else {
				/* Punctuation and End of String */
				/* All spaces before punctuation symbols, and all trailing
				   spaces before the end of the string, must be explicit */
				length = strlen(group);
				cblt_copyCharToUint16(group, compressed + i, length);
				i += length;
			}
			break;
		case Punctuation:
			length = strlen(group);
			cblt_copyCharToUint16(group, compressed + i, length);
			i += length;
			if (nextStatus == Word)
				/* space omission signal */
				compressed[i++] = CBLT_NO_SPACE;
			break;
		case EndOfString:
			/* Once again, do nothing. */
		}
	}

	compressed[i] = 0x0000;
	free(mSentence);
	return compressed;
}

/*
 * Returns the length of the decoded sentence contained by COMPRESSED, including
 * the null terminator at the end of the string.
 * 
 * This function expects the input array to be perfectly formatted, with no
 * invalid sequences of numbers.
 */
size_t cblt_getDecodedLength(const uint16_t *compressed) {
	size_t length;			/* length of a word */
	size_t decodedLength;	/* length of the output string */
	unsigned words = 0;		/* number of actual words counted */
	size_t i;				/* index for compressed */
	
	/* initialize to 1 to account for null terminator */
	decodedLength = 1;
	for (i = 0; compressed[i] != 0; ) {
		if (compressed[i] < 0x100) {
			/* direct byte injection */
			++decodedLength;
			++i;
		} else if (compressed[i] < WORDMAP_LEN) {
			/* valid words */
			decodedLength += strlen( WORDTABLE + WORDMAP[compressed[i]] );
			++i;
			++words;
		} else if (compressed[i] == CBLT_BEGIN_STRING) {
			/* string literal */
			/* increment i to skip past the CBLT_BEGIN_STRING signal */
			++i;
			length = strlen( (char *)(compressed + i) );
			decodedLength += length;
			/* increment length because the next operation is significantly
			   easier when the null terminator is included in the length */
			++length;
			/* increment i by ceil((length + 1) / 2) */
			i += (length / 2 + (length % 2 != 0));
			++words;
		} else if (compressed[i] == CBLT_NO_SPACE) {
			--decodedLength;
			++i;
		} else {
			/* any other codes are invalid, so move on */
			++i;
		}
	}

	/* If there is a punctuation character followed by a word character at the
	   beginning of the string, then there is an implicit space before the word
	   character. Skip past all leading spaces and check if the first non-space
	   symbol is a punctuation character. If it is, increment WORDS since a
	   leading punctuation marker counts as a pseudo-word. */
	i = 0;
	while (compressed[i] == ' ')
		/* this syntax is easier to understand than an empty for loop. */
		++i;
	/* Using cblt_getCharStatus() only makes sense for values up to 255. */
	if ( (compressed[i] < 0x100)
		&& (cblt_getCharStatus(compressed[i]) == Punctuation) )
		++words;

	/* There is an implicit space between every word, except for where
	   CBLT_NO_SPACE compensated by decrementing the encodedLength. If there are
	   less than 2 words, there are no implicit spaces. */
	if (words > 1)
		decodedLength += words - 1;

	return decodedLength;
}

/*
 * This function takes a single null-terminated array of 16-bit unsigned
 * integers as an argument and returns a pointer to a null-terminated string of
 * characters containing the original sentence. The block of memory pointed to
 * by the return value is dynamically allocated, so it is the job of the
 * programmer to free() the pointer after doing something meaningful with the
 * data.
 */
char *cblt_decodeSentence(const uint16_t *compressed) {
	size_t i;		/* index for compressed */
	size_t j;		/* index for sentence */
	size_t size;	/* size of memory block needed to decode compressed */
	size_t length;	/* length of a string */
	char *sentence;

	if (compressed == NULL)
		return NULL;
	
	/* Dynamically reallocating memory as we decode the sentence would be
	   pretty inefficient, so we read through the compressed data once to
	   calculate the amount of memory needed to decompress it first. size is
	   initialized to 1 so that there is space for a null terminator even for
	   en empty string. */
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
