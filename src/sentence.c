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

/* 
 * A note about integer ceiling division:
 *
 * This code makes frequent use of integer ceiling division, specifically when
 * calculating the space occupied by a string literal, rounded up to the nearest
 * 2 bytes. Instead of the output of strlen(), which does not include the null
 * terminator, this calculation uses strlen() + 1. To not confuse myself and
 * anyone else reading this code, I'll put a reminder of this every time this
 * calculation is made.
 * 
 * In particular, some value will be incremented by ceil(length/2), where length
 * is the length of a string plus 1. Since no ceiling division operator exists
 * for integers, we implement this manually:
 * 		( ceil(length/2) )
 * 	is expressed as
 * 		( length/2 + (length%2 != 0) )
 * 
 * In this implementation, the ceiling devision calculation is easier to express
 * and more understandable when length is the length of a string PLUS 1, as
 * opposed to just the length as returned from strlen().
 */


/* copies NC characters from S to DEST, casting from char to uint16_t for each
   character; 1 char in S translates to 1 uint16_t in DEST */
static void cblt_copyCharToUint16(const char *s, uint16_t *dest, size_t nc) {
	size_t i;
	for (i = 0; i < nc; ++i)
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
			/* integer ceiling division */
			length = strlen(group) + 1;
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
	/* EndOfString is handled in the while loop below */
	}

	while (currentStatus != EndOfString) {
		group = cblt_splitstr(NULL, &currentStatus, &nextStatus);
		switch (currentStatus) {
		case Word:
			if (cblt_findWord(group) != CBLT_WORD_NOT_FOUND) {
				++encodedLength;
			} else {
				/* string literal injection */
				/* integer ceiling division */
				length = strlen(group) + 1;
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
			length = strlen(group);
			/* We fill this integer with all 1s, to avoid having an element be
			   all zero from the string's null terminator. This would cause a
			   premature termination of the integer block. */
			compressed[i + length / 2] = 0xffff;
			/* memcpy and integer ceiling division require ++length */
			++length;
			memcpy(&compressed[i], group, length);
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
	/* EndOfString is handled by the while loop below */
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
				compressed[i + length / 2] = 0xffff;
				++length;
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
	
	if (compressed == NULL)
		return 0;

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
			++i;	/* skip past the CBLT_BEGIN_STRING symbol */
			length = strlen( (char *)(compressed + i) );
			decodedLength += length;
			/* then integer ceiling division */
			++length;
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
	char *sentence;	/* decoded data */
	size_t j;		/* index for sentence */
	size_t length;	/* length of a word */

	if (compressed == NULL)
		return NULL;

	/* We use calloc in this case because we will make a special assumption
	   during decoding: that untouched bytes in SENTENCE are zero. A non-zero
	   byte indicates that a CBLT_NO_SPACE symbol was present in the last
	   element in COMPRESSED. */
	length = cblt_getDecodedLength(compressed);
	sentence = calloc(length, sizeof(char));
	if (sentence == NULL)
		return NULL;

	/* By my specification, the first word will not have a leading space unless
	   explicitly specified by a literal ASCII space. */
	sentence[0] = 1;

	for (i = 0; compressed[i] != 0; ) {
		if (compressed[i] < 0x100) {
			/* direct byte injection */
			sentence[j++] = (char)compressed[i++];
		} else if (compressed[i] < WORDMAP_LEN) {
			/* valid words */
			/* insert leading space if applicable */
			if (sentence[j] == 0)
				sentence[j++] = ' ';
			
			length = strlen( WORDTABLE + WORDMAP[compressed[i]] );
			memcpy(sentence + j, WORDTABLE + WORDMAP[compressed[i]], length);
			j += length;
			++i;
		} else if (compressed[i] == CBLT_BEGIN_STRING) {
			/* string literal */
			++i;	/* skip past the CBLT_BEGIN_STRING symbol */
			/* insert leading space if applicable */
			if (sentence[j] == 0)
				sentence[j++] = ' ';
			
			length = strlen( (char *)(compressed + i) );
			memcpy(sentence + j, compressed + i, length);
			j += length;
			/* then integer ceiling division */
			++length;
			i += (length / 2 + (length % 2 != 0));
		} else if (compressed[i] == CBLT_NO_SPACE) {
			/* The next word will not have a leading space */
			sentence[j] = 1;
			++i;
		} else {
			/* any other codes are invalid, so move on */
			++i;
		}
	}
	sentence[j] = '\0';

	return sentence;
}
