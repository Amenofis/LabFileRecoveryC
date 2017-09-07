#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "buscador.h"
#include "utils.h"
#include "terminos.h"
#include "modelo.h"


int main(int argc, char* argv[]) {

    char* res_stopwords_path = "../res/StopWords.txt";
    char* res_document_path = "../res/TestCollection_short.txt";

    int op;
    code status;
    StopWords *stopWords = NULL;
    Index *index = NULL;

    do {
        printf("\n================\n");
        printf("Menu\n");
        printf("1 Cargar StopWords\n");
        printf("2 Indexar Documentos\n");
        printf("3 Mostrar Índice\n");
        printf("0 Salir\n");
        printf("Ingrese opción: ");
        scanf("%d", &op);
        switch(op) {
            case 1:
                printf("\n================\n");
                printf("Cargar Stopwords\n");
                stopWords = loadStopWords(res_stopwords_path, &status);
                if (stopWords != NULL) {
                    printf("StopWords cargadas correctamente\n");
                }
                break;
            case 2:
                if (stopWords != NULL) {
                    printf("Crear Índice Invertido\n");
                    index = createIndex(res_document_path, stopWords, &status);
                    if (index != NULL)
                        printf("Index cargado exitosamente\n");
                } else {
                    printf("No se puede indexar si no se han cargado las StopWords antes");
                }
                break;
            case 3:
                if (index !=NULL) {
                    printTerms(index->i_terms);
                }
                break;
            case 0:
                if (stopWords != NULL) {
                    destroyStopWords(&stopWords);
                    if (stopWords == NULL) {
                        printf("- StopWords destroyed\n");
                    }
                }
                if (index != NULL) {
                    destroyIndex(&index);

                    if (index == NULL)
                        printf("- Index destroyed\n");
                }
                printf("Salir\n");
                break;
            default:
                break;
        }
    } while (op != 0);

    return 0;
}


StopWords * loadStopWords(char* pathStopWordsFile, code *statusCode) {
    int stopWordsCount = countFileLines(pathStopWordsFile);
    printf("StopWords encontradas: %d\n", stopWordsCount);

    if (stopWordsCount > 0) {
        StopWords *ptrStopWords = (StopWords*)malloc(sizeof(ptrStopWords));
        if (ptrStopWords) {
            ptrStopWords->count = stopWordsCount;
            ptrStopWords->words = malloc(sizeof(*ptrStopWords->words)*stopWordsCount);

            if (ptrStopWords->words) {
                printf("Leyendo StopWords...\n");
                FILE *stopWordsFile = fopen(pathStopWordsFile, "r");
                char buff[20];
                int i = 0;
                while (fscanf(stopWordsFile, "%s", buff) > 0) {
                    cleanWord(buff);
                    ptrStopWords->words[i] = (char*)malloc(sizeof(char)*20);
                    strcpy(ptrStopWords->words[i], buff);
                    i++;
                }
                fclose(stopWordsFile);
                printf("Allocated succesfully %d indexes for char*\n", stopWordsCount);
                return ptrStopWords;
            }
        }
    }
    return NULL;
}

int countFileLines(char* pathToFile) {
    FILE *fp = fopen(pathToFile, "r");
    int lines = 0, ch = 0;
    if (fp == NULL)
        return 0;
    lines++;
    while((ch = fgetc(fp)) != EOF)
        if(ch == '\n') lines++;
    fclose(fp);
    return lines;
}

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

Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode) {
    //int documentsCound = countDocumentFiles(pathDocumentsFile);
    //if (documentsCound > 0) {
    Index * ptrIndex = (Index*) malloc(sizeof(Index));
    ptrIndex->i_terms = NULL;

    if (ptrIndex != NULL) {
        FILE *fp = fopen(pathDocumentsFile, "r");
        char *aux, *buff, *line;
        int nID, line_count = 1;

        if (fp == NULL) return NULL;
        buff = (char*) malloc(sizeof(char) * 255);

        while(fgets(buff, 255, fp)) { // Start of FGETS
            while ((line = get_line(buff)) != NULL) {
                buff += strlen(line);
                if (sscanf(line, ".I %d", &nID) == 1) {
                    printf("\n%s -> It's an ID", line);
                } else if (strstr(line, ".T") != NULL)
                    printf("\n%s -> It's a TITLE", line);
                else if (strstr(line, ".A") != NULL)
                    printf("\n%s -> It's an AUTHOR", line);
                else if (strstr(line, ".B") != NULL)
                    printf("\n%s -> It's the BIBLIOGRAPHY", line);
                else if (strstr(line, ".W") != NULL)
                    printf("\n%s -> It's the body", line);
                else {
                    //replaceAlNum(line, ' ');
                    printf("\n LINE: '%s'", line);
                    while ((aux = get_word(line)) != NULL) {
                        line += strlen(aux) + 1;
                        //printf("\nWORD: '%s'", aux);
                        if (isStopWord(sw, aux) == FALSE) {
                            ptrIndex->i_terms = prependTerm(ptrIndex->i_terms, aux);
                        }
                    } // End of word read
                }
                line_count++;
            } // End of line read
        }  // End of FGETS

        if (!fclose(fp)) {
            printf("File closed - OK");
        } else {
            printf("File not closed - ERROR");
            return NULL;
        }

        free(aux);
        free(line);

        return ptrIndex;
    }
    return NULL;
}

Bool isStopWord(StopWords * sw, char * word) {
    int i;
    for (i = 0; i < sw->count; i++) {
        if (strcmp(word, sw->words[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void destroyIndex(Index ** index) {
    Index * tmp;
    tmp = *index;
    dispose(tmp->i_terms);
    free(tmp);
    *index = NULL;
}
