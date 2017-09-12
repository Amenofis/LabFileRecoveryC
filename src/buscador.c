#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "buscador.h"
#include "utils.h"
#include "terminos.h"

/**
 *  Created by Felipe Muñoz
 *  SRI Indexer and Search
 */

code status = OK;
StopWords * ptrStopWords = NULL;
Index * ptrIndex = NULL;
Ranking * ptrRanking = NULL;

char* res_stopwords_path = "../res/StopWords.txt";
char* res_document_path = "../res/TestCollection_short.txt";

int main(int argc, char* argv[]) {
    int op;

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
                    printf("-> StopWords cargadas correctamente (%d)\n\n", ptrStopWords->count);
                } else {
                    showError(status);
                }

                break;
            case 2:
                if (ptrStopWords != NULL) {
                    printf("Creando Índice...\n");
                    ptrIndex = createIndex(res_document_path, ptrStopWords, &status);
                    if (status == OK) {
                        printf("-> Índice cargado exitosamente (%d)\n\n", ptrIndex->i_size);
                    } else {
                        showError(status);
                    }
                } else {
                    showError(status);
                }
                break;
            case 3:
                printf("Cargar índice\n");
                break;
            case 4:
                if (ptrIndex != NULL) {
                    printf("Búsqueda\n");
                }
                break;
            case 5:
                if (ptrIndex != NULL) {
                    printf("Guardar índice\n");
                }
                break;
            case 6:
                if (ptrIndex != NULL) {
                    printf("Última búsqueda\n");
                }
                break;
            case 0:
                exit(0);
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
 *
 * @param pathStopWordsFile
 * @param statusCode
 * @return
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
 *
 * @param pathDocumentsFile
 * @param sw
 * @param statusCode
 * @return
 */
Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode) {

    Index * new_index = (Index*) malloc(sizeof(Index));
    if (new_index == NULL) {
        *statusCode = NOT_ENOUGH_MEMORY;
        return NULL;
    }
    new_index->i_terms = NULL;
    new_index->i_size = 0;

    FILE *fp = fopen(pathDocumentsFile, "r");
    char *word, *line, buffer[255], *ptrBuff;
    int nID, line_count = 1;

    if (fp == NULL) {
        *statusCode = ERR_FILE_NOT_FOUND;
        destroyIndex(&new_index);
        return NULL;
    }

    while( fgets(buffer, 254, fp) ) { // Start of FGETS
        ptrBuff = buffer;
        while ( (line = get_line(ptrBuff)) != NULL ) {
            ptrBuff += strlen(line);

            if (sscanf(line, ".I %d", &nID) == 1) {
                //printf("%s -> It's an ID %d\n", line, nID);

            } else if (strstr(line, ".T") != NULL) {
                //printf("%s -> It's a TITLE\n", line);

            } else if (strstr(line, ".A") != NULL) {
                //printf("%s -> It's an AUTHOR\n", line);

            } else if (strstr(line, ".B") != NULL) {
                //printf("%s -> It's the BIBLIOGRAPHY\n", line);

            } else if (strstr(line, ".W") != NULL) {
                //printf("%s -> It's the body\n", line);

            } else {
                replaceAlNum(line, ' ');
                //printf("\n LINE: '%s'", line);
                while ((word = get_word(line)) != NULL) {
                    line += (strlen(word) + 1);
                    if (isStopWord(sw, word) == FALSE && strlen(word) > 1) {
                        //printf("WORD: '%s'\n", word);
                        new_index->i_terms = prependTerm(new_index->i_terms, word);
                        new_index->i_size++;
                    }
                } // End of word read
            }
            line_count++;
        } // End of line read
    }  // End of FGETS

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
 * @param sw
 * @param word
 * @return
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
 *
 * @param stopWords
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
 *
 * @param mIndex
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
 *
 * @param statusCode
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