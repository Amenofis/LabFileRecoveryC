//
// Created by Felipe Muñoz on 19-08-17.
//

#include <ctype.h>
#include <stdio.h>
#include "utils.h"

void cleanWord(char * word) {
    unsigned int i = 0; /* Scanning index */
    unsigned int x = 0; /* Write back index */

    char c;
    while ((c = word[i++]) != '\0')
        if (isalnum(c))
            word[x++] = c;
}

void replaceAlNum(char * str, char r) {
    unsigned int i = 0; /* Scanning index */
    unsigned int x = 0; /* Write back index */

    char c;
    while ((c = str[i++]) != '\0') {
        if (isalnum(c))
            str[x++] = c;
        else
            str[x++] = r;
    }
}

char * get_line(char * str) {
    if (*str == '\0' || *str == '\r') return NULL;
    while(*str == '\n')
        str++;

    char *s = str;
    while(*str != '\n' && *str != '\0' && *str != '\r')
        str++;

    if (*str == '\n' || *str == '\r') {
        *str = '\0';
        str++;
    }
    return s;
}

char * get_word(char * line) {
    if (*line == '\0' || *line == '\n') return NULL;
    while(*line == ' ')
        line++;

    char *s = line;
    while(*line != ' ' && *line != '\0' && *line != '\n')
        line++;

    if (*line == ' ' || *line == '\n') {
        *line = '\0';
        line++;
    }
    return s;
}

int countFileLines(char * pathToFile) {
    FILE *fp = fopen(pathToFile, "r");
    int lines = 0, ch = 0;
    if (fp == NULL)
        return 0;
    lines++;
    while((ch = fgetc(fp)) != EOF)
        if(ch == '\n') lines++;
    fclose(fp);
    return lines;
}

int countFileDocuments(char * pathToFile) {
    FILE *fp = fopen(pathToFile, "r");
    int docs = 0, docID;
    char buffer[100];
    if (fp == NULL)
        return 0;
    while (fgets(buffer, 99, fp))
        if (sscanf(buffer, ".I %d", &docID) == 1) docs++;
    fclose(fp);
    return docs;
}