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
 * strtok(). This function WILL modify its first argument. However, after
 * iterating completely through all substrings in S, the string stored in S will
 * be identical to its initial value.
 * 
 * Parameters:
 * s:
 * 		the string to be divided into substrings
 * pcurrentStatus:
 * 		a pointer to an int holding the status of the current substring
 * ppnextStatus:
 * 		a pointer to an int holding the status of the next substring
 * 
 * Return value:
 * This function returns a pointer to the current substring, which is described
 * by the value of *pcurrentStatus.
 */
char *cblt_splitstr(char *s, int *pcurrentStatus, int *pnextStatus) {
	static char
		*pcurrent = NULL,
		*pnext = NULL,
		swap = '\0';

	if (s == NULL) {
		/* swap the real value of *pnext back */
		*pnext = swap;

		if (*pnext == '\0') {
			*pcurrentStatus = EndOfString;
			*pnextStatus = EndOfString;
			return pnext;
		}

		pcurrent = pnext;
		*pcurrentStatus = *pnextStatus;
	} else {
		/* only if this is the first time operating on s */
		if (*s == '\0') {
			*pcurrentStatus = EndOfString;
			*pnextStatus = EndOfString;
			return s;
		}

		/* find current status */
		pcurrent = s;
		pnext = pcurrent + 1;
		*pcurrentStatus = cblt_getCharStatus(*pcurrent);
	}

	/* move pnext to beginning of next substring */
	while (cblt_getCharStatus(*pnext) == *pcurrentStatus)
		++pnext;
	
	*pcurrentStatus = cblt_getCharStatus(*pcurrent);
	*pnextStatus = cblt_getCharStatus(*pnext);

	/* and place a temporary null terminator there */
	swap = *pnext;
	*pnext = '\0';
	return pcurrent;
}
