/*
 * blocksize.c
 * by Eliot Baez
 *
 * This file contains the definitions of the function used for finding
 * the size of a null-terminated array of uint16_t's, including the null
 * terminator.
 *
 * I might make one of these for chars as well, but I'm going to wait
 * until I can fully understand the glibc implementation of strlen()
 * first.
 */

#include <stdlib.h>
#include <stdint.h>

/* I don't know whether I should be proud of disappointed of this
 * function */
size_t cblt_getUint16BlockSize(const uint16_t *block) {
	const uint16_t *intptr = block + 1;

	while (1)
		if (*block++ == 0)
			return block - intptr;
}
