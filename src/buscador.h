//
// Created by Felipe Muñoz on 20-08-17.
//

#ifndef LABFILERECOVERYC_BUSCADOR_H
#define LABFILERECOVERYC_BUSCADOR_H

#include "constantes.h"
#include "modelo.h"

StopWords * loadStopWords(char * pathStopWordsFile, code * statusCode);

Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode);

Bool isStopWord(StopWords * sw, char * word);

void showError(code statusCode);

void destroyStopWords(StopWords ** stopWords);

void destroyIndex(Index ** mIndex);

#endif //LABFILERECOVERYC_BUSCADOR_H
