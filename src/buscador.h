//
// Created by ameno on 20-08-17.
//

#ifndef LABFILERECOVERYC_BUSCADOR_H
#define LABFILERECOVERYC_BUSCADOR_H

#include "constantes.h"
#include "modelo.c"

int countFileLines(char * pathToFile);

StopWords * loadStopWords(char * pathStopWordsFile, code * statusCode);

void destroyStopWords(StopWords ** stopWords);

int countDocumentFiles(char * pathToDocument);

int countDocumentWords(char * pathToDocument);

Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode);

void destroyIndex(Index ** index);

static void cleanWord(char * word);

#endif //LABFILERECOVERYC_BUSCADOR_H
