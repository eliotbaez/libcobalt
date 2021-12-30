/* splitstringtest.c
 * 
 * This is a test program that reads the 1st (not 0th) argument from the command
 * line and passes it to the cblt_strsplit() function. The program will print
 * the human readable version of all the important information returned by the
 * function.
 * 
 * This file is intended to be compiled with ../src/sentence.c, NOT linked with
 * libcobalt at compile time.
 * 
 * There's probably a super organized and controlled way to perform things like
 * unit tests, but I'm not that educated yet. 
 */

#include <stdlib.h>
#include <stdio.h>
#include "splitstring.h"

int main(int argc, char **argv) {
	char *group;
	int
		currentStatus,
		nextStatus;
	int i = 0;

	if (argc != 2) {
		fprintf(stderr, "%s requires one argument.\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	printf("%s\n", argv[1]);
	
	group = cblt_splitstr(argv[1], &currentStatus, &nextStatus);
	printf("group %2d at index %2zd is \"%s\"\n", i++, group - argv[0], group);
	printf("  current type is %s\n", cblt_getStatusName(currentStatus));
	printf("  next type is    %s\n", cblt_getStatusName(nextStatus));

	while (currentStatus != EndOfString) {
		group = cblt_splitstr(NULL, &currentStatus, &nextStatus);
		printf("group %2d at index %2zd is \"%s\"\n", i++, group - argv[0], group);
		printf("  current type is %s\n", cblt_getStatusName(currentStatus));
		printf("  next type is    %s\n", cblt_getStatusName(nextStatus));
	}

	return 0;
}