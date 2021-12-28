/*
 * splitstring.c
 * 
 * Contains the definitions for functions and constants to be used when
 * splitting a sentence into substrings or tokens.
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "cobalt.h"
#include "splitstring.h"

/* This function is gonna get called a lot. For nearly every character in the
   string, in fact. It is defined as inline in case the compiler lets us
   sacrifice some program space for time saved by not using function overhead. */
static inline int cblt_getCharStatus(char c) {
	if (c == ' ')
		return Space;
	if (c == '\0')
		return EndOfString;
	if ( isalnum(c)
			|| (c > 0x7f)
			|| (c == '\'')
			|| (c == '-') )
		return Word;
	return Punctuation;
}

/*
 * Somewhat like strtok(), this function will split a string into substrings
 * based on a predefined set of delimiter characters. The value returned is a
 * pointer to the substring found, terminated by a null byte, just like
 * strtok().
 * 
 * Parameters:
 * s:			the string to be divided into substrings
 * ppcurrent:	a pointer to a pointer to the beginning of the current substring
 * ppnext:		a pointer to a pointer to the beginning of the next substring or
 * 				the null byte at the end of the string, if the last substring has
 * 				been reached
 * 
 * Return value:
 * This function returns an integer describing the substring pointed to by
 * PPCURRENT. See the above enum definition for more details.
 */
int cblt_splitstr(const char *s, char const **ppcurrent, char const **ppnext) {
	/* Prepare to immerse yourself in a world of char pointer pointers. There
	   will be some light dereferencing involved here. */
	static int
		currentStatus = Unassigned,
		nextStatus = Unassigned;
	/* dereferencing the pointer pointers once and storing in a local variable,
	   to avoid dereferencing more times than we have to */
	const char *pcurrent = *ppcurrent;
	const char *pnext = *ppnext;

	if (s == NULL) {
		if (*pnext == '\0') {
			*ppcurrent = pnext;
			*ppnext = pnext;
			return EndOfString;
		}

		pcurrent = pnext;
		currentStatus = nextStatus;

		if (currentStatus == Space) {
			++pcurrent;
			++pnext;
			currentStatus = cblt_getCharStatus(*pcurrent);
		}
		
		switch(currentStatus) {
		case Space:
			while (cblt_getCharStatus(*pnext) == currentStatus) {
				++pnext;	/* next char... */
			}

			nextStatus = cblt_getCharStatus(*pnext);
			*ppcurrent = pcurrent;
			*ppnext = pnext;
			return currentStatus;
		case Word:
			while (cblt_getCharStatus(*pnext) == currentStatus) {
				++pnext;	/* next char... */
			}

			nextStatus = cblt_getCharStatus(*pnext);
			*ppcurrent = pcurrent;
			*ppnext = pnext;
			return currentStatus;
		case Punctuation:
			while (cblt_getCharStatus(*pnext) == currentStatus) {
				++pnext;	/* next char... */
			}

			nextStatus = cblt_getCharStatus(*pnext);
			*ppcurrent = pcurrent;
			*ppnext = pnext;
			return currentStatus;
		}
	} else {
		/* only if this is the first time operating on s */
		pcurrent = s;
		pnext = s;

		if (*s == '\0')
			return EndOfString;

		/* find current status */
		currentStatus = cblt_getCharStatus(*pcurrent);
		/* move *next to beginning of next substring */
		while (cblt_getCharStatus(*pnext) == currentStatus) {
			++pnext;	/* next char... */
		}

		nextStatus = cblt_getCharStatus(*pnext);
		*ppcurrent = pcurrent;
		*ppnext = pnext;
		return currentStatus;
	}

	return 0;
}
