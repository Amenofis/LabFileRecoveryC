//
// Created by Felipe Muñoz on 20-08-17.
//

#ifndef LABFILERECOVERYC_BUSCADOR_H
#define LABFILERECOVERYC_BUSCADOR_H

#include "constantes.h"
#include "modelo.h"

StopWords * loadStopWords(char * pathStopWordsFile, code * statusCode);

Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode);

void saveIndex(Index * i, int * id, code * statusCode);

Index * loadIndex(int id, code * statusCode);

Ranking * query(Index * i, StopWords * sw, char * text, code * statusCode);

void displayResults(Ranking * r, int TopK, code * statusCode);

Bool isFileHeader(char * str);

Bool isStopWord(StopWords * sw, char * word);

Document * findDocument(int id, Index * index);

void destroyStopWords(StopWords ** mStopWords);

void destroyIndex(Index ** mIndex);

void destroyRanking(Ranking ** mRanking);

#endif //LABFILERECOVERYC_BUSCADOR_H
