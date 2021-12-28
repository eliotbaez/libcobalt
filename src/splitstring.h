/*
 * This enum defines the possible return values of cblt_strsplit().
 * 
 * Word:		Current substring is a word
 * Punctuation:	Current substring is a group of punctuation characters
 * Space:		Current substring is a group of spaces, separated by spaces on
 * 				either side
 * EndOfString:	Current substring is the null byte at the end of the string
 */

#ifndef SPLITSTRING_H
#define SPLITSTRING_H

enum substringStatus{
	Unassigned = 0,
	Word,
    Space,
	Punctuation,
	EndOfString
};

int cblt_splitstr(const char *s, char const **ppcurrent, char const **ppnext);

#endif /* SPLITSTRING_H */