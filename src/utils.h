//
// Created by Felipe Muñoz on 02-09-17.
//

#ifndef LABFILERECOVERYC_UTILS_H
#define LABFILERECOVERYC_UTILS_H

#include "constantes.h"

void cleanWord(char * word);

void replaceAlNum(char * str, char r);

char * get_line(char * str);

char * get_word(char * line);

int countFileLines(char * pathToFile);

int countFileDocuments(char * pathToFile);

#endif //LABFILERECOVERYC_UTILS_H
