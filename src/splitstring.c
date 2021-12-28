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

/* 
 * This helper function gets the name corresponding to the result of 
 * cblt_getCharStatus(). It is not compiled into libcobalt, and only exists
 * for testing and debugging purposes.
 */
const char *cblt_getStatusName(int status) {
    switch(status) {
    case Word:
        return "Word";
    case Space:
        return "Space";
    case Punctuation:
        return "Punctuation";
    case EndOfString:
        return "EndOfString";
    default:
        return "Unassigned";
    }
}

/* This function is gonna get called a lot. For nearly every character in the
   string, in fact. 
   TODO: 
   Find a way to define this function as inline to allow the compiler to
   probably optimize the function. */
int cblt_getCharStatus(unsigned char c) {
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

		/* skip ahead 1 space */
		if (currentStatus == Space && 0) {
			++pcurrent;
			++pnext;
			currentStatus = cblt_getCharStatus(*pcurrent);
			
			/* explicitly interpret a space character if there is a single
			   trailing space in the string */
			if (currentStatus == EndOfString) {
				*ppcurrent = pcurrent - 1;
				*ppnext = pcurrent;
				return Space;
			}
		}
	} else {
		/* only if this is the first time operating on s */
		pcurrent = s;
		pnext = s;

		if (*s == '\0') {
			*ppcurrent = pcurrent;
			*ppnext = pnext;
			return EndOfString;
		}
		/* find current status */
		currentStatus = cblt_getCharStatus(*pcurrent);
	}

	/* move *next to beginning of next substring */
	while (cblt_getCharStatus(*pnext) == currentStatus) {
		++pnext;	/* next char... */
	}

	nextStatus = cblt_getCharStatus(*pnext);
	*ppcurrent = pcurrent;
	*ppnext = pnext;
	return currentStatus;
}
