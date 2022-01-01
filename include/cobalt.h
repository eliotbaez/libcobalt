/*
 * cobalt.h
 * by Eliot Baez
 *
 * Header file for libcobalt; CObaLT; Compression, Obviously by a Lookup Table.
 * CObaLT is a library for compressing plaintext in the English language (and
 * possibly others!) on a word-by-word basis.
 *
 * This header file contains declarations of the functions and global variables
 * that form the CObaLT API.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef COBALT_H
#define COBALT_H

/*
 * TODO:
 * Consider creating a typedef for uint16_t specifying that it is a raw data
 * type for libcobalt.
 * Alternatively, you could also go so far as to define an entire struct that
 * holds metadata and a pointer to data for a compressed data block. This would
 * also entail wrapper functions, like constructors and destructors, that would
 * encapsulate the entire functionality of libcobalt.
 */

/* 
 * TODO:
 * Use a naming convention to distinguish between internal symbols and symbols
 * intended for user
 */

/* 
 * WORDTABLE is a contiguous array of characters containing every word in a
 * predefined list, defined at compile time. The words are null-separated, and
 * the entire list is null-terminated.
 *
 * WORDTABLE_LEN is the length of the entire WORDTABLE array, including the
 * final null byte. WORDTABLE_STRLEN is the length of WORDTABLE, excluding the
 * final null byte. I.e., WORDTABLE_STRLEN is the maximum valid index that can
 * be used to access WORDTABLE.
 *
 * NUMBER_OF_WORDS is the number of words in the table.
 */
extern const unsigned char WORDTABLE[];
extern const size_t WORDTABLE_LEN;
extern const size_t WORDTABLE_STRLEN;
extern const uint16_t NUMBER_OF_WORDS;

/* 
 * WORDMAP is an array of 32-bit unsigned integers that store indexes within
 * WORDTABLE. For example, WORDMAP[807] stores the index of word 807 within
 * WORDTABLE, such that &WORDTABLE[ WORDMAP[807] ] is the address in memory of
 * the first character of the 807th word in the table.
 *
 * Since each of the words are null-terminated within the table, the address
 * obtained from &WORDTABLE[ WORDMAP[n] ] can be treated directly as a null-
 * terminated string containing the nth word in the table.
 *
 * WORDMAP_LEN is the total number of elements in WORDMAP, including the first
 * 256 reserved elements.
 */
extern const uint32_t WORDMAP[];
extern const size_t WORDMAP_LEN;

/*
 * GUIDETABLE is an array of 16-bit unsigned integers that store indexes within
 * WORDMAP.
 *
 * The underlying code of CObaLT uses a uint16_t to store the first 2 characters
 * in a string when searching for a string in the table. The order that these
 * characters occur in the integer depend on the endianness of the machine where
 * CObaLT is compiled, but it does not affect the function of the library.
 *
 * You should not have to ever use GUIDETABLE when using CObaLT through the
 * intended API functions. However, if you were to choose to, this is how you
 * would:
 *
 * 		const char *str = "Something";
 * 		uint16_t buf = *( (uint16_t *)str );
 * 		uint32_t word =	GUIDETABLE[buf];
 *
 * In this example, `word' stores the the ordinal number of the first word in
 * the table that shares the first 2 characters with `str'. If no word in the
 * table shares its first 2 characters with `str', then `word' is the ordinal
 * number of the last word in the list.
 */
extern const uint16_t GUIDETABLE[];
extern const size_t GUIDETABLE_LEN;

/* 
 * cblt_streq takes two null-terminated strings as arguments and returns true
 * only if the two strings are identical. The two pointers do not need to be
 * unique.
 *
 * This function is different from strcmp() in that it will ONLY tell us whether
 * str1 and str2 point to identical strings. This gives it the power to simply
 * return as soon as the first non-matching character is found.
 * 
 * It is intended only for internal use, but it may be useful, so it is exposed
 * in the API.
 */
bool cblt_streq(const char *str1, const char *str2);

/*
 * Somewhat like strtok(), this function will split a string into substrings
 * based on a predefined set of delimiter characters. The value returned is a
 * pointer to the substring found, terminated by a null byte, just like
 * strtok(). This function WILL modify its first argument. However, after
 * iterating completely through all substrings in S, the string stored in S will
 * be identical to its initial value. This function is intended for internal use
 * only.
 * 
 * Parameters:
 * s:
 * 		the string to be divided into substrings
 * pcurrentStatus:
 * 		a pointer to an int holding the status of the current substring
 * pnextStatus:
 * 		a pointer to an int holding the status of the next substring
 * 
 * Return value:
 * This function returns a pointer to the current substring, which is described
 * by the value of *pcurrentStatus.
 */
char *cblt_splitstr(char *s, int *pcurrentStatus, int *pnextStatus);

/*
 * cblt_findWord searches for str in the word table and returns the ordinal
 * number of the first word that matches str. Returns -1 if the string is not
 * found and -2 if passed an empty string.
 *
 * The value returned by cblt_findWord can be safely caset to uint16_t without
 * any loss of information, as long as neither of the 2 error codes are
 * returned.
 */
int32_t cblt_findWord(const char *str);

#define CBLT_WORD_NOT_FOUND	-1
#define CBLT_EMPTY_WORD_ARG	-2

/*
 * In a uint16_t array, an integer with the value CBLT_BEGIN_STRING indicates
 * that the address of the next integer is the beginning of a null-terminated
 * character array.
 * 
 * Since spaces are very common in sentences, they are interpreted as the
 * delimiter character when encoding and decoding. An integer with the value
 * CBLT_NO_SPACE indicates that the space that would normally follow the last
 * character of the previous character group is absent.
 */
#define CBLT_BEGIN_STRING	0xFFFF
#define CBLT_NO_SPACE       0xFFFE

/*
 * cblt_encodeSentence takes a single null-terminated sentence as an argument
 * and returns a pointer to a null-terminated array of 16-bit unsigned integers
 * containing the compressed sentence data. The memory block pointed to by the
 * return value is dynamically allocated, so it is the job of the programmer to
 * free() the pointer after doing something meaningful with the data.
 *
 * This function will return a NULL pointer if either of the following errors
 * occur:
 * 	- sentence is a NULL Pointer 
 * 	- a memory allocation fails within the function
 *
 * If passed an empty string, the function will return a pointer to a 16-bit
 * unsigned integer with the value 0.
 * 
 * cblt_getEncodedLength takes a sentence to be encoded as its argument, and
 * returns the length in elements of the block of 16-bit unsigned integers
 * necessary to store the string when encoded. This function will return 0 upon
 * failure.
 */
uint16_t *cblt_encodeSentence(const char *sentence);
size_t cblt_getEncodedLength(const char *sentence);

/*
 * cblt_decodeSentence takes a single null-terminated array of 16-bit unsigned
 * integers as an argument and returns a pointer to a null-terminated string of
 * characters containing the original sentence. The block of memory pointed to
 * by the return value is dynamically allocated, so it is the job of the
 * programmer to free() the pointer after doing something meaningful with the
 * data.
 *
 * This function will return a NULL pointer if either of the following errors
 * occur:
 * 	- compressed is a NULL pointer
 * 	- a memory allocation fails within the function
 *
 * If passed a pointer to a 16-bit unsigned integer with the value 0, the
 * function will return a pointer to an empty string.
 * 
 * cblt_getDecodedLength is used internally to calculate the memory needed to
 * store the string held by a block of compressed data. This function will
 * return 0 on failure.
 */
char *cblt_decodeSentence(const uint16_t *compressed);
size_t cblt_getDecodedLength(const uint16_t *compressed);

/*
 * cblt_getUint16BlockSize takes a pointer to a null-terminated array of 16-bit
 * unsigned integers as an argument and returns the size, in elements, of that
 * block, including the null terminator.
 *
 * This function is intended for finding the size of an array of ints to know
 * how many bytes to write to a file, or a similar scenario.
 */
size_t cblt_getUint16BlockSize(const uint16_t *block);

#endif /* COBALT_H */
