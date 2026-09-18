#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/myfilefunctions.h"

int wordCount(FILE* file, int* lines, int* words, int* chars) {
    if (file == NULL) {
        return -1;
    }

    rewind(file);

    *lines = 0;
    *words = 0;
    *chars = 0;

    int c;
    int inWord = 0;
    int lastChar = 0;

    while ((c = fgetc(file)) != EOF) {
        (*chars)++;
        lastChar = c;

        if (c == '\n') {
            (*lines)++;
        }

        if (c == ' ' || c == '\n' || c == '\t') {
            inWord = 0;
        } else if (!inWord) {
            inWord = 1;
            (*words)++;
        }
    }

    if (*chars > 0 && lastChar != '\n') {
        (*lines)++;
    }

    rewind(file);
    return 0;
}

int mygrep(FILE* fp, const char* search_str, char*** matches) {
    if (fp == NULL || search_str == NULL || matches == NULL) {
        return -1;
    }

    int capacity = 10;
    int count = 0;
    *matches = (char**)malloc(capacity * sizeof(char*));

    if (*matches == NULL) {
        return -1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    rewind(fp);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strstr(line, search_str) != NULL) {
            if (count >= capacity) {
                capacity *= 2;
                char** temp = (char**)realloc(*matches, capacity * sizeof(char*));
                if (temp == NULL) {
                    free(line);
                    return -1;
                }
                *matches = temp;
            }
            (*matches)[count] = (char*)malloc((read + 1) * sizeof(char));
            if ((*matches)[count] == NULL) {
                free(line);
                return -1;
            }
            strcpy((*matches)[count], line);
            count++;
        }
    }

    free(line);
    rewind(fp);
    return count;
}
