//
// Created by ameno on 15-09-17.
//

#include <stdlib.h>
#include <string.h>
#include "resultados.h"

Result * createResult(int id, Document * document, Result * next) {
    Result * result = (Result*) malloc(sizeof(Result));
    result->r_id = id;
    result->r_count = 1;
    result->r_document = (Document*) malloc(sizeof(Document));
    memcpy(result->r_document, document, sizeof(Document));
    result->next = next;
}

Result * searchResult(Document * document, Result * head) {
    Result * current = head;
    while (current != NULL) {
        if (current->r_document->d_id == document->d_id)
            return current;
        current = current->next;
    }
}

int updateResults(Result * result, char * pathToFile) {

}


void disposeResults(Result * head) {
    Result *result, *tmp;
    if (head != NULL) {
        result = head->next;
        head->next = NULL;
        while (result != NULL) {
            tmp = result->next;
            free(result->r_document);
            free(result);
            result = tmp;
        }
    }
}