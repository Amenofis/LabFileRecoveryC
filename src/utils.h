//
// Created by Felipe Muñoz on 02-09-17.
//

#ifndef LABFILERECOVERYC_UTILS_H
#define LABFILERECOVERYC_UTILS_H

void cleanWord(char * word);

void replaceAlNum(char * str, char r);

char * get_line(char * str);

char * get_word(char * line);

int countFileLines(char * pathToFile);

#endif //LABFILERECOVERYC_UTILS_H
