/*
 * sentence.h
 *
 * contains the declarations of functions defined in sentence.c
 */

#include <stdint.h>
#include <stdlib.h>

#ifndef SENTENCE_H
#define SENTENCE_H

size_t cblt_getEncodedLength(const char *sentence);
uint16_t *cblt_encodeSentence(const char *sentence);

size_t cblt_getDecodedLength(const uint16_t *compressed);
char *cblt_decodeSentence(const uint16_t *compressed);

#endif  /* SENTENCE_H */