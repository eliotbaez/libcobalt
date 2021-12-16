/*
 * c_hexdump.c
 * by Eliot Baez
 *
 * This xxd style hexdump program takes a binary file as input and writes its
 * output in C header format to some other file, both named by the user. The
 * first argument after the program is an integer specifying the width in bytes
 * of the integers to be used to store the data in the header file. For example,
 * 4 bytes +. uint32_t, 2 bytes => uint16_t, etc.
 *
 * The width in bytes is NOT simply how the digits are grouped in the header
 * file. The width affects the order in which bytes are read from the file, as
 * this file obeys the endianness of the system that compiles it. The user must
 * be weary of this fact and use widths other than 1 ONLY IF THEY ARE CERTAIN
 * that they know what they are doing. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

/* These are specialized printing functions for different size unsigned
   integers. The generic function pointer in main() will point to one of these
   functions based on the integer width specified by the user. */
int fprintu8(FILE *fp, void *uint8ptr, size_t index) {
	return fprintf(fp, "0x%02hhx", *( (uint8_t *)uint8ptr + index ));
}

int fprintu16(FILE *fp, void *uint16ptr, size_t index) {
	return fprintf(fp, "0x%04hx", *( (uint16_t *)uint16ptr + index ));
}

int fprintu32(FILE *fp, void *uint32ptr, size_t index) {
	return fprintf(fp, "0x%08x", *( (uint32_t *)uint32ptr + index ));
}

int fprintu64(FILE *fp, void *uint64ptr, size_t index) {
	return fprintf(fp, "0x%016lx", *( (uint64_t *)uint64ptr + index ));
}

/* filename magic - end the string at the first dot unless the file name begins
   with a dot, and capitalize everything */
void filenameMagic(char *name) {
	size_t i;
	
	if (!isalpha(name[0]))
		name[0] = '_';
	else
		name[0] = toupper(name[0]);

	for (i = 1; ; ++i) {
		if (name[i] == '\0' || name[i] == '.') {
			name[i] = '\0';
			break;
		}
		if (!isalnum(name[i]))
			name[i] = '_';
		else
			name[i] = toupper(name[i]);
	}
}

int main(int argc, char **argv) {
	size_t i;		/* index for buf */
	int c;			/* for keeping track of columns */
	size_t size;	/* size of buf in elements of size n bytes */
	size_t blocked;	/* size, but rounded down to the second to last line */
	uint8_t *buf;	/* pointer to raw data */

	int width;		/* width of integer in bytes */
	int columns;
	const char *typename;
	int (*printFunc)(FILE*, void*, size_t);
	FILE *fp;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s int_width in_file out_file\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	width = strtol(argv[1], NULL, 10);
	switch (width) {
	case 1:
		typename = "unsigned char";
		printFunc = fprintu8;
		break;
	case 2:
		typename = "uint16_t";
		printFunc = fprintu16;
		break;
	case 4:
		typename = "uint32_t";
		printFunc = fprintu32;
		break;
	case 8:
		typename = "uint64_t";
		printFunc = fprintu64;
		break;
	default:
		fprintf(stderr, "%s: %d is not a valid integer width.\n",
			argv[1], width);
		return EXIT_FAILURE;
	}
	
	columns = 16 / width;

	/* open the input file */
	fp = fopen(argv[2], "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: Error opening file %s\n", argv[0], argv[2]);
		return EXIT_FAILURE;
	}

	

	/* find size of the file */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp) / width;
	rewind(fp);
	
	/* allocate some memory */
	buf = malloc(width * size);
	if (buf == NULL) {
		fprintf(stderr, "%s: Error allocating memory.\n", argv[0]);
		fclose(fp);
		return EXIT_FAILURE;
	}
	fread(buf, width, size, fp);
	fclose(fp);
	
	/* open output file */
	fp = fopen(argv[3], "wt");
	if (fp == NULL) {
		fprintf(stderr, "%s: Error opening file %s\n", argv[0], argv[3]);
		free(buf);
		return EXIT_FAILURE;
	}

	filenameMagic(argv[2]);

	/* now we can do all our printing shenanigans */
	fprintf(fp, "#include <stdint.h>\n\n"
			"const %s %s[] = {\n",
			typename, argv[2]);
	
	/* this ensures that the last set of n or less integers always get
	   special treatment */
	blocked = columns * (size / columns - ((size % columns) == 0));
	/* like a hex dump but better */
	for (i = 0; i < blocked; i += columns) {
		fputc('\t', fp);
		for (c = 0; c < columns; ++c) {
			printFunc(fp, buf, i + c);
			fputs(", ", fp);
		}
		fputc('\n', fp);
	}
	
	/* last line and end of header file */
	fputc('\t', fp);
	for ( ; i < size - 1; i++) {
		printFunc(fp, buf, i);
		fputs(", ", fp);
	}
	printFunc(fp, buf, i);
	fprintf(fp, "\n};");

	fprintf(stderr, "%s: Done.\n", argv[0]);

	fclose(fp);
	free(buf);

	return 0;
}
