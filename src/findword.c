/*
 * findword.c
 * by Eliot Baez
 *
 * Searches for a word in the word list and returns the number of the word in
 * the list (not the index of the first character of the word with respect to
 * the entire word list)
 */

#include <stdint.h>
#include <stdbool.h>

#include "cobalt.h"

/*
 * TODO:
 * Instead of unfound first-2-character-pairs leading to the end of the
 * word list, have them point to the previous word. maybe?
 */

/* This function is different from strcmp() in that it will ONLY tell us whether
   str1 and str2 point to identical strings. This gives us the power to simply
   return as soon as the first non-matching character is found and save
   processor time.

   It is partially based on the source code for the glibc strcmp(). */
bool cblt_streq(const char *str1, const char *str2) {
	while (1) {
		if (*str1 != *str2)
			return false;

		/* else, it is implied that *str1 == *str2 */
		if (*(str2) == '\0')
			return true;

		++str1, ++str2;
	}
}

/* Will return -2 if passed an empty string. This shouldn't have to happen, but
   it's there just in case. Will return -1 if the string is not found. Returns
   which number word is the first match in the wordlist otherwise. */
int32_t cblt_findWord(const char *str) {
	uint16_t buf;
	uint16_t word;

	/* Empty strings would break this function. */
	if (str[0] == '\0') {
		/* Except they don't. */
		return CBLT_EMPTY_WORD_ARG;
	}
	
	/* word is initialized to the starting point of our search */
	buf = *( (uint16_t *)str );
	word = GUIDETABLE[buf];

	/* Then set buf to the first word with the next set of first 2 characters.
	   This way we don't seach all the way to the end of the word list if we
	   don't have to. */
	if (buf == 0xFFFF) {
		/* We do this so that buf doesn't overflow from being incremented. */
		/* Search until end of wordlist */
		buf = WORDMAP_LEN;
	} else {
		buf = GUIDETABLE[buf + 1];
	}

	for ( ; word < buf; ++word) {
		if (cblt_streq(str, WORDTABLE + WORDMAP[word]))
			return word;
	}

	return CBLT_WORD_NOT_FOUND;
}
