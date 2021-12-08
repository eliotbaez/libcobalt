/*
 * cobalt.h
 * by Eliot Baez
 *
 * Header file for libcobalt; CObaLT; Compression, Obviously by a Lookup
 * Table. CObaLT is a library for compressing plaintext in the English
 * language (and possibly others!) on a word-by-word basis.
 *
 * This header file contains declarations of the functions and global
 * variables that form the CObaLT API.
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef COBALT_H
#define COBALT_H

/* 
 * WORDTABLE is a contiguous array of characters containing every word
 * in a predefined list, defined at compile time. The words are null-
 * separated, and the entire list is null-terminated. 
 * 
 * WORDTABLE_LEN is the length of the entire WORDTABLE array, excluding
 * the final null byte. 
 */
extern const char *WORDTABLE;
extern const size_t WORDTABLE_LEN;

/* 
 * WORDMAP is an array of 32-bit unsigned integers that store indexes
 * within WORDTABLE. For example, WORDMAP[3] stores the index of word 3
 * within WORDTABLE, such that WORDTABLE[ WORDMAP[3] ] is the address in
 * memory of the first character of the 4th word in the table. 
 * 
 * Since each of the words are null-terminated within the table, the
 * address obtained from WORDTABLE[ WORDMAP[n] ] can be treated directly
 * as a null-terminated string containing the nth word in the table.
 *
 * The number of elements in WORDMAP is defined in the macro NUMBER_OF_WORDS.
 */
extern const uint32_t WORDMAP[];

/*
 * GUIDETABLE is an array of 16-bit unsigned integers that store indexes
 * within WORDMAP.
 *
 * The underlying code of CObaLT uses a uint16_t to store the first 2
 * characters in a string when searching for a string in the table. The
 * order that these characters occur in the integer depend on the
 * endianness of the machine where CObaLT is compiled, but it does not
 * affect the function of the library.
 *
 * You should not have to ever use GUIDETABLE when using CObaLT through
 * the intended API functions. However, if you were to choose to, this
 * is how you would:
 *
 * 		const char *str = "Something";
 * 		uint16_t buf = *( (uint16_t *)str );
 * 		uint32_t word =	GUIDETABLE[buf];
 *
 * In this example, `word' stores the the ordinal number of the first
 * word in the table that shares the first 2 characters with `str'. If
 * no word in the table shares its first 2 characters with `str', then
 * `word' is the ordinal number of the last word in the list.
 */
extern const uint16_t GUIDETABLE[];

/* 
 * streq takes two null-terminated strings as arguments and returns true
 * only if the two strings are identical. The two pointers do not need
 * to be unique.
 *
 * This function is different from strcmp() in that it will ONLY tell us
 * whether str1 and str2 point to identical strings. This gives us the
 * power to simply return as soon as the first non-matching character is
 * found and save processor time. 
 *
 * It is intended only for internal use, but it may be useful, so it is
 * exposed in the API.
 */
bool streq(const char *str1, const char *str2);

/*
 * findword searches for str in the word table and returns the ordinal
 * number of the word that matches str. Returns -1 if the string is not
 * found and -2 if passed an empty string.
 *
 * The value returned by findword can be safely cast to uint16_t without
 * any loss of information, as long as neither of the 2 error codes are
 * returned.
 */
int32_t findword(const char *str);

#endif /* COBALT_H */
