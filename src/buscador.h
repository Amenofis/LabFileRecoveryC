//
// Created by ameno on 20-08-17.
//

#ifndef LABFILERECOVERYC_BUSCADOR_H
#define LABFILERECOVERYC_BUSCADOR_H

#include "constantes.h"
#include "modelo.h"

int countFileLines(char * pathToFile);

StopWords * loadStopWords(char * pathStopWordsFile, code * statusCode);

void destroyStopWords(StopWords ** stopWords);

Bool isStopWord(StopWords * sw, char * word);

Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode);

void destroyIndex(Index ** mIndex);

#endif //LABFILERECOVERYC_BUSCADOR_H
