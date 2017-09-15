//
// Created by ameno on 15-09-17.
//

#include "modelo.h"

#ifndef LABFILERECOVERYC_RESULTADOS_H
#define LABFILERECOVERYC_RESULTADOS_H

Result * createResult(int id, Document * document, Result * next);

Result * searchResult(Document * document, Result * head);

int updateResult(Result * result, char * pathToFile);

void disposeResults(Result * head);

#endif //LABFILERECOVERYC_RESULTADOS_H
