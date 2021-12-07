/*
 * uint32_array_to_c.py
 * by Eliot Baez
 *
 * The filename is pretty self explanatory, but I'll explain anyways.
 * This script converts an array of uint32_t's in a binary file to C
 * header code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define WORDMAP_NAME	"wordmap.bin"
#define WORDMAP_HEADER_NAME	"../src/wordmap.h"

int main(int argc, char **argv) {
	size_t i;		/* index for buf */
	size_t size;	/* size of buf in uint16_t's */
	size_t blocked;	/* size rounded down to nearest 16 bytes */
	uint32_t *buf;
	FILE *fp;
	
	/* open the input file */
	fp = fopen(WORDMAP_NAME, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: Error opening file %s\n", argv[0], WORDMAP_NAME);
		return EXIT_FAILURE;
	}

	/* find size of the file */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp) / 4;
	rewind(fp);
	
	/* allocate some memory */
	buf = malloc(4 * size);
	if (buf == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		fclose(fp);
		return EXIT_FAILURE;
	}
	fread(buf, 4, size, fp);
	fclose(fp);
	
	/* open output file */
	fp = fopen(WORDMAP_HEADER_NAME, "wt");
	if (fp == NULL) {
		fprintf(stderr, "%s: Error opening file %s\n", argv[0], WORDMAP_HEADER_NAME);
		free(buf);
		return EXIT_FAILURE;
	}

	/* now we can do all our printing shenanigans */
	fprintf(fp, "#include <stdint.h>\n\n"
			"#ifndef WORDMAP_H\n"
			"#define WORDMAP_H\n\n"
			"const uint32_t wordmap[] = {\n");
	
	/* this ensures that the last set of 4 or less integers always get
	   special treatment */
	blocked = 4 * (size / 4 - ((size % 4) == 0));
	/* like a hex dump but better */
	for (i = 0; i < blocked; i += 4) {
		fprintf(fp, "\t0x%08x, 0x%08x, 0x%08x, 0x%08x,\n",
				buf[i], buf[i + 1], buf[i + 2], buf[i + 3]);
	}
	
	/* last line and end of header file */
	fputc('\t', fp);
	for ( ; i < size - 1; i++) {
		fprintf(fp, "0x%08x, ", buf[i]);
	}
	fprintf(fp, "0x%08x\n"
			"}\n\n"
			"#endif\t/* WORDMAP_H */\n"
			, buf[i]);

	fprintf(stderr, "%s: Done.\n", argv[0]);

	fclose(fp);
	free(buf);

	return 0;
}
