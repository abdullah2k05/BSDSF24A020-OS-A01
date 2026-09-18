#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mystrfunctions.h"
#include "../include/myfilefunctions.h"

int main() {
    printf("--- Testing String Functions ---\n");

    const char* testStr = "Hello, World!";
    printf("mystrlen(\"%s\") = %d\n", testStr, mystrlen(testStr));

    char dest[50];
    mystrcpy(dest, testStr);
    printf("mystrcpy dest = \"%s\"\n", dest);

    char dest2[10];
    mystrncpy(dest2, testStr, 5);
    dest2[5] = '\0';
    printf("mystrncpy(dest2, \"%s\", 5) = \"%s\"\n", testStr, dest2);

    char catDest[50] = "Hello";
    mystrcat(catDest, " World!");
    printf("mystrcat(\"Hello\", \" World!\") = \"%s\"\n", catDest);

    printf("\n--- Testing File Functions ---\n");

    FILE* fp = fopen("testfile.txt", "w+");
    if (fp == NULL) {
        printf("Error: Could not create test file\n");
        return 1;
    }

    fprintf(fp, "Hello World\n");
    fprintf(fp, "This is a test file\n");
    fprintf(fp, "Operating Systems course\n");
    fprintf(fp, "Hello again from the test file\n");

    int lines, words, chars;
    if (wordCount(fp, &lines, &words, &chars) == 0) {
        printf("wordCount: lines=%d, words=%d, chars=%d\n", lines, words, chars);
    } else {
        printf("wordCount failed\n");
    }

    char** matches;
    int matchCount = mygrep(fp, "Hello", &matches);
    if (matchCount >= 0) {
        printf("mygrep(\"Hello\"): found %d matches\n", matchCount);
        for (int i = 0; i < matchCount; i++) {
            printf("  Match %d: %s", i + 1, matches[i]);
            free(matches[i]);
        }
        free(matches);
    } else {
        printf("mygrep failed\n");
    }

    fclose(fp);
    remove("testfile.txt");

    return 0;
}
