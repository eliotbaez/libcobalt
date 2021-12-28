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
    if (argc != 2) {
        fprintf(stderr, "%s requires one argument.\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    //char buf;
    char *current, *next;
    int status;
    int i = 0;

    printf("%s\n", argv[1]);
    
    status = cblt_splitstr(argv[1], (const char **)&current, (const char **)&next);
    printf("group %d is \"", i++);
    fwrite(current, 1, next - current, stdout);
    printf("\"\n  type is ");
    puts(cblt_getStatusName(status));
    printf("  current at index %2zd: 0x%02hhx '%c'\n", current - argv[1], *current, *current);
    printf("  next at index    %2zd: 0x%02hhx '%c'\n", next - argv[1], *next, *next);

    while (status != EndOfString) {
        status = cblt_splitstr(NULL, (const char **)&current, (const char **)&next);
        printf("group %d is \"", i++);
        fwrite(current, 1, next - current, stdout);
        printf("\"\n  type is ");
        puts(cblt_getStatusName(status));
        printf("  current at index %2zd: 0x%02hhx '%c'\n", current - argv[1], *current, *current);
        printf("  next at index    %2zd: 0x%02hhx '%c'\n", next - argv[1], *next, *next);
    }

    return 0;
}