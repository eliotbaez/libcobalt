/*
 * splitstring.h
 *
 * Contains declarations of functions defined in and definitions of 
 * enumerations used in splitstring.c
 */
#ifndef SPLITSTRING_H
#define SPLITSTRING_H

/*
 * This enum defines the possible return values of cblt_strsplit().
 * 
 * Word:		Current substring is a word
 * Punctuation:	Current substring is a group of punctuation characters
 * Space:		Current substring is a group of spaces, separated by spaces on
 * 				either side
 * EndOfString:	Current substring is the null byte at the end of the string
 */
enum substringStatus{
	Unassigned = 0,
	Word,
    Space,
	Punctuation,
	EndOfString
};

int cblt_splitstr(const char *s, char const **ppcurrent, char const **ppnext);

int cblt_getCharStatus(unsigned char c);

const char *cblt_getStatusName(int status);

#endif /* SPLITSTRING_H */