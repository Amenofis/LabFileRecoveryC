#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "buscador.h"
#include "utils.h"
#include "terminos.h"

/**
 *  Created by Felipe Muñoz
 *  SRI - Indexer and Search
 */

code status = OK;
StopWords * ptrStopWords = NULL;
Index * ptrIndex = NULL;
Ranking * ptrRanking = NULL;

char* res_stopwords_path = "../res/StopWords.txt";
char* res_document_path = "../res/TestCollection_short.txt";

int main(int argc, char* argv[]) {
    int op;

    srand(time(NULL));

    printf("================\n");
    printf("Indexador y Bucador C (SRI)\n");
    printf("================\n");

    do {
        printf("\n--- Menu ---\n");

        printf("1. Cargar StopWords\n");
        printf("2. Indexar documentos\n");
        printf("3. Cargar índice\n");

        if (ptrIndex != NULL) {
            printf("4. Realizar búsqueda\n");
            printf("5. Guardar índice\n");

            if (ptrRanking != NULL) {
                printf("6. Mostrar última búsqueda\n");
            }
        }

        printf("0 Salir\n");
        printf("Ingrese opción: ");
        scanf("%d", &op);
        switch(op) {
            case 1:
                printf("\nCargando StopWords...\n");
                ptrStopWords = loadStopWords(res_stopwords_path, &status);
                if (status == OK) {
                    printf("-> StopWords cargadas OK (%d)\n\n", ptrStopWords->count);
                } else {
                    showError(status);
                }

                break;
            case 2:
                if (ptrStopWords != NULL) {
                    printf("Creando Índice...\n");
                    ptrIndex = createIndex(res_document_path, ptrStopWords, &status);
                    if (status == OK) {
                        printf("-> Índice cargado OK (%d)\n\n", ptrIndex->i_terms_size);
                    } else {
                        showError(status);
                    }
                } else {
                    showError(status);
                }
                break;
            case 3:
                printf("Cargar índice\n");
                if (ptrIndex != NULL) destroyIndex(&ptrIndex);
                int id = 0;
                printf("Ingrese el ID del Índice a cargar: ");
                scanf("%d", &id);
                ptrIndex = loadIndex(id, &status);
                if (status == OK)
                    printf("-> Índice cargado OK (%d)\n", ptrIndex->i_id);
                else
                    showError(status);
                break;
            case 4:
                if (ptrIndex != NULL) {
                    printf("Búsqueda\n");
                }
                break;
            case 5:
                if (ptrIndex != NULL) {
                    saveIndex(ptrIndex, &ptrIndex->i_id, &status);

                    if (status == OK)
                        printf("-> Índice guardado OK (%d)\n", ptrIndex->i_id);
                    else
                        showError(status);
                }
                break;
            case 6:
                if (ptrIndex != NULL) {
                    printf("Última búsqueda\n");
                }
                break;
            case 9:
                if (ptrIndex != NULL) {
                    printTerms(ptrIndex->i_terms);
                    printf("Documents \n");
                    int i;
                    for (i = 0; i < ptrIndex->i_docs_size; i++) {
                        printf("%d (%d, %d) \n", ptrIndex->i_documents[i]->d_id,
                               ptrIndex->i_documents[i]->d_SOD,
                               ptrIndex->i_documents[i]->d_EOD);
                    }
                }
                break;
            case 0:
                printf("Exit\n");
                break;
            default:
                break;
        }
    } while (op != 0);

    if (ptrStopWords != NULL) {
        destroyStopWords(&ptrStopWords);
        if (ptrStopWords == NULL)
            printf("StopWords destroyed - OK\n");
    }

    if (ptrIndex != NULL) {
        destroyIndex(&ptrIndex);
        if (ptrIndex == NULL)
            printf("Index destroyed - OK\n");
    }
    return 0;
}

/**
 * loadStopWords
 *
 * @param pathStopWordsFile Path to the file
 * @param statusCode Function result status
 * @return Pointer to the StopWords
 */
StopWords * loadStopWords(char * pathStopWordsFile, code * statusCode) {
    int stopwords_count = countFileLines(pathStopWordsFile);
    StopWords * new_stopwords = malloc(sizeof(StopWords));
    if (new_stopwords == NULL) {
        *statusCode = NOT_ENOUGH_MEMORY;
        return NULL;
    }

    new_stopwords->count = stopwords_count;
    if (stopwords_count == 0) {
        *statusCode = ERR_EMPTY_STOPWORDS;
        return new_stopwords;
    }

    new_stopwords->words = malloc(stopwords_count * sizeof(char*));

    FILE * fp = fopen(pathStopWordsFile, "r");
    if (fp == NULL) {
        *statusCode = ERR_FILE_NOT_FOUND;
        return NULL;
    }

    char * buffer = (char*) malloc(sizeof(char) * 20);
    int i = 0;
    while(fgets(buffer, 20, fp)) {
        buffer = get_line(buffer);
        size_t len = strlen(buffer);

        new_stopwords->words[i] = (char*) malloc((len + 1) * sizeof(char));
        strcpy(new_stopwords->words[i], buffer);
        i++;
    }

    free(buffer);
    if (!fclose(fp)) {
        printf("File closed - OK\n");
    } else {
        *statusCode = ERR_FILE_NOT_PERM;
        destroyStopWords(&new_stopwords);
    }
    return new_stopwords;
}

/**
 * createIndex
 *
 * @param pathDocumentsFile Path to the file
 * @param sw Pointer to the StopWords
 * @param statusCode Function result status
 * @return Pointer to the Index
 */
Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode) {

    int total_docs = countFileDocuments(pathDocumentsFile);

    Index * new_index = (Index*) malloc(sizeof(Index));
    if (new_index == NULL) {
        *statusCode = NOT_ENOUGH_MEMORY;
        return NULL;
    }
    // Index is not NULL so we proceed to initialize the struct
    new_index->i_terms = NULL;
    new_index->i_terms_size = 0;
    new_index->i_documents = (Document**) malloc(sizeof(Document*) * total_docs);
    new_index->i_docs_size = 0;

    FILE *fp = fopen(pathDocumentsFile, "r");
    if (fp == NULL) {
        *statusCode = ERR_FILE_NOT_FOUND;
        destroyIndex(&new_index);
        return NULL;
    }
    // If the file exists, declare all the variables we'll be using to hold onto and count the data
    char *word, *line, buffer[255], *ptrBuff;
    int nID, line_count = 1, docs_count = 0;

    while( fgets(buffer, 254, fp) ) { // Start of FGETS
        ptrBuff = buffer;
        while ( (line = get_line(ptrBuff)) != NULL ) { // Start of line read
            ptrBuff += strlen(line);

            if (isFileHeader(line) == FALSE) { // Check if is any kind of header
                if (sscanf(line, ".I %d", &nID) == 1) { // Scan the string for the ID format
                    Document * new_document = (Document*) malloc(sizeof(Document));
                    new_document->d_id = nID;
                    new_document->d_SOD = line_count;

                    if (docs_count > 0) // If this is the +1 document, then save in the last one the ending line
                        new_index->i_documents[docs_count-1]->d_EOD = line_count-1;

                    new_index->i_documents[docs_count] = new_document;
                    docs_count++;
                } else {
                    replaceAlNum(line, ' ');
                    //printf("\n LINE: '%s'", line);
                    while ((word = get_word(line)) != NULL) {
                        line += (strlen(word) + 1);
                        if (isStopWord(sw, word) == FALSE && strlen(word) > 1) {
                            //printf("WORD: '%s'\n", word);
                            Term * term = searchTerm(new_index->i_terms, word);
                            if (term != NULL) {
                                // Checks if the word was already found in the current Document
                                if (hasDocumentID(nID, term) == FALSE) {
                                    // Add the document id to the term documents array
                                    term->t_docs[term->t_docs_count] = nID;
                                    term->t_docs_count++;
                                }
                            } else {
                                // Add a new term to the beginning of the list
                                term = prependTerm(new_index->i_terms, word);
                                term->t_docs = (int*) calloc(sizeof(int), (size_t) total_docs);
                                term->t_docs_count = 1;
                                term->t_docs[0] = nID;

                                // Replace the head pointer in the index struct
                                new_index->i_terms = term;
                                new_index->i_terms_size++;
                            }
                        }
                    } // End of word read
                }
            }
            line_count++;
        } // End of line read
    }  // End of FGETS

    // Update the document's count and add the last line to the last document
    new_index->i_docs_size = docs_count;
    new_index->i_documents[docs_count-1]->d_EOD = line_count-1;

    if (!fclose(fp)) {
        printf("File closed - OK\n");
    } else {
        *statusCode = ERR_FILE_NOT_PERM;
        destroyIndex(&new_index);
        return NULL;
    }
    return new_index;
}


/**
 *
 * @param ndx
 * @param id
 * @param statusCode
 */
void saveIndex(Index * ndx, int * id, code * statusCode) {
    if (ndx == NULL || id == NULL){
        *statusCode = INVALID_ARGUMENTS;
        return;
    }

    int tempID = rand() % 100;
    char fileName[20];
    snprintf(fileName, sizeof fileName, "%d%s", tempID, "_index.txt");
    FILE * fp = fopen(fileName, "wb");
    if (fp == NULL) {
        *statusCode = ERR_FILE_NOT_FOUND;
        return;
    }

    int i;
    // First line is id docs_size terms_size
    fprintf(fp, ".I\n%d %d %d\n", tempID, ndx->i_docs_size, ndx->i_terms_size);
    // Store the documents
    fprintf(fp, ".D\n");
    for(i = 0; i < ndx->i_docs_size; i++) {
        fprintf(fp, "%d %d %d\n", ndx->i_documents[i]->d_id,
                ndx->i_documents[i]->d_SOD,
                ndx->i_documents[i]->d_EOD);
    }
    // Store the terms
    fprintf(fp, ".T\n");
    saveTerms(fp, ndx->i_terms);
    fclose(fp);

    *statusCode = OK;
    *id = tempID;
}

Index * loadIndex(int id, code * statusCode) {
    Index * loadedIndex = (Index*) malloc(sizeof(Index));

    char fileName[20];
    snprintf(fileName, sizeof fileName, "%d%s", id, "_index.txt");
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        *statusCode = ERR_FILE_NOT_FOUND;
        destroyIndex(&loadedIndex);
        return NULL;
    }

    int w = 0, count = 0;
    char buffer[100], word[25];
    while(fgets(buffer, sizeof buffer, fp)) {
        if (strncmp(buffer, ".I",2) == 0) {
            w = 0;
        } else if (strncmp(buffer, ".D", 2) == 0) {
            w = 1;
            count = 0;
        } else if (strncmp(buffer, ".T", 2) == 0){
            w = 2;
            count = 0;
        } else {

            if (w == 0) {
                sscanf(buffer, "%d %d %d", &loadedIndex->i_id,
                       &loadedIndex->i_docs_size,
                       &loadedIndex->i_terms_size);
                loadedIndex->i_documents = (Document**) malloc(sizeof(Document*) * loadedIndex->i_docs_size);
                loadedIndex->i_terms = NULL;
            } else if (w == 1) {
                loadedIndex->i_documents[count] = (Document*) malloc(sizeof(Document));
                sscanf(buffer, "%d %d %d", &loadedIndex->i_documents[count]->d_id,
                       &loadedIndex->i_documents[count]->d_SOD,
                       &loadedIndex->i_documents[count]->d_EOD);
                count++;
            } else {
                int d = 0, i = 0;
                char * dArr = malloc(sizeof(char) * 100);
                sscanf(buffer, "%s %d %s", word, &d, dArr);

                Term * term = prependTerm(loadedIndex->i_terms, word);
                term->t_docs_count = d;
                term->t_docs = calloc(sizeof(int), (size_t) d);

                char *n = strtok(dArr, ",");
                do {
                    term->t_docs[i++] = atoi(n);
                } while (n = strtok(NULL, ","));

                loadedIndex->i_terms = term;
                free(dArr);
            }
        }
    }

    if (!fclose(fp)) {
        printf("File closed - OK\n");
    } else {
        *statusCode = ERR_FILE_NOT_PERM;
        destroyIndex(&loadedIndex);
        return NULL;
    }

    return loadedIndex;
}


/**
 * ifFileHeader
 * Compare the given string to the Index's document headers
 *
 * @param str String to compare
 * @return TRUE or FALSE depending if the given string equals any of the headers
 */
Bool isFileHeader(char * str) {
    char * headers[4] = {".T", ".A", ".B", ".W"};
    int i;
    for (i = 0; i < 3; i++)
        if (strncmp(str, headers[i], 2) == 0) return TRUE;

    return FALSE;
}

/**
 * isStopWord
 * Checks if the given word is contained in the StopWord's structure
 * @param sw StopWord's pointer
 * @param word String to compare to
 * @return TRUE or FALSE depending if the word was found in the StopWord's structure
 */
Bool isStopWord(StopWords * sw, char * word) {
    int i;
    for (i = 0; i < sw->count; i++) {
        if (strcmp(word, sw->words[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * destroyStopWords
 * Free the given stopword's allocated memory
 * @param stopWords StopWord's memory pointer
 */
void destroyStopWords(StopWords **stopWords) {
    StopWords *temp;
    temp = *stopWords;
    int i;
    for (i = 0; i < temp->count; i++)
        free(temp->words[i]);
    free(temp->words);
    free(temp);

    *stopWords = NULL;
}

/**
 * destroyIndex
 * Free the given index's allocated memory
 * @param mIndex Index's memory pointer
 */
void destroyIndex(Index ** mIndex) {
    Index *tmp;
    tmp = *mIndex;
    if (tmp->i_terms != NULL)
        dispose(tmp->i_terms);
    free(tmp);
    *mIndex = NULL;
}

/**
 * showError
 * Prints the error depending on the given statusCode value
 * @param statusCode error enum
 */
void showError(code statusCode) {
    switch (statusCode) {
        case ERR_FILE_NOT_FOUND:
            printf("Error archivo no encontrado\n");
            break;
        case ERR_FILE_NOT_PERM:
            printf("Error abriendo/cerrando el archivo\n");
            break;
        case ERR_EMPTY_STOPWORDS:
            printf("No se han encontrado StopWords\n");
            break;
        default:
            printf("Error no definido\n");
            break;
    }
}