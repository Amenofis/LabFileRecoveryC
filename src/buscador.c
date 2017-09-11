#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "buscador.h"
#include "utils.h"
#include "terminos.h"


code status = OK;
StopWords *ptrStopWords = NULL;
Index *ptrIndex = NULL;

int main(int argc, char* argv[]) {

    char* res_stopwords_path = "res/StopWords.txt";
    char* res_document_path = "res/TestCollection_short.txt";

    int op;

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
                ptrStopWords = loadStopWords(res_stopwords_path, &status);
                printf("StopWords cargadas correctamente\n");
                break;
            case 2:
                if (ptrStopWords != NULL) {
                    printf("Crear Índice Invertido\n");
                    ptrIndex = createIndex(res_document_path, ptrStopWords, &status);
                    printf("Index cargado exitosamente\n");
                } else {
                    printf("No se puede indexar si no se han cargado las StopWords antes\n");
                }
                break;
            case 3:
                if (ptrIndex !=NULL) {
                    printTerms(ptrIndex->i_terms);
                }
                break;
            case 0:
                if (ptrStopWords != NULL) {
                    destroyStopWords(&ptrStopWords);
                    if (ptrStopWords == NULL) {
                        printf("StopWords destroyed\n");
                    }
                }
                if (ptrIndex != NULL) {
                    destroyIndex(&ptrIndex);

                    if (ptrIndex == NULL)
                        printf("Index destroyed\n");
                }
                printf("Salir\n");
                break;
            default:
                break;
        }
    } while (op != 0);

    return 0;
}


StopWords * loadStopWords(char * pathStopWordsFile, code * statusCode) {
    int stopWordsCount = countFileLines(pathStopWordsFile);
    printf("StopWords encontradas: %d\n", stopWordsCount);

    if (stopWordsCount > 0) {
        StopWords * newStopWords = (StopWords*) malloc(sizeof(newStopWords));
        if (newStopWords == NULL) {
            printf("Error creating the StopWords\n");
            exit(0);
        }

        newStopWords->count = stopWordsCount;
        newStopWords->words = (char**) malloc(sizeof(char*) * stopWordsCount);

        if (newStopWords->words) {
            printf("Leyendo StopWords...\n");
            FILE *stopWordsFile = fopen(pathStopWordsFile, "r");
            char * buff = (char*) malloc(sizeof(char) * 20);
            int i = 0;
            while(fgets(buff, 20, stopWordsFile)) {
                buff = get_line(buff);
                size_t len = strlen(buff);

                newStopWords->words[i] = (char*) malloc(sizeof(char) * (len + 1));
                strcpy(newStopWords->words[i], buff);
                i++;
            }
            free(buff);
            fclose(stopWordsFile);

            return newStopWords;
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

    Index * newIndex = (Index*) malloc(sizeof(newIndex));
    //newIndex->i_terms = NULL;
    if (newIndex == NULL) {
        printf("Error creating the Index\n");
        exit(0);
    }

    FILE *fp = fopen(pathDocumentsFile, "r");
    char *word, *line, buffer[255], *ptrBuff;
    int nID, line_count = 1;

    if (fp == NULL) return NULL;
    //buff = (char*) malloc(sizeof(char) * 255);

    while( fgets(buffer, 254, fp) ) { // Start of FGETS
        ptrBuff = buffer;
        while ( (line = get_line(ptrBuff)) != NULL ) {
            ptrBuff += strlen(line);

            if (sscanf(line, ".I %d", &nID) == 1) {
                printf("%s -> It's an ID %d\n", line, nID);

            } else if (strstr(line, ".T") != NULL) {
                printf("%s -> It's a TITLE\n", line);

            } else if (strstr(line, ".A") != NULL) {
                printf("%s -> It's an AUTHOR\n", line);

            } else if (strstr(line, ".B") != NULL) {
                printf("%s -> It's the BIBLIOGRAPHY\n", line);

            } else if (strstr(line, ".W") != NULL) {
                printf("%s -> It's the body\n", line);

            } else {
                replaceAlNum(line, ' ');
                //printf("\n LINE: '%s'", line);
                while ((word = get_word(line)) != NULL) {
                    line += (strlen(word) + 1);
                    //printf("\nWORD: '%s'", word);
                    if (isStopWord(sw, word) == FALSE && strlen(word) > 1) {
                        newIndex->i_terms = prependTerm(newIndex->i_terms, word);
                    }
                } // End of word read
            }
            line_count++;
        } // End of line read
    }  // End of FGETS

    if (!fclose(fp)) {
        printf("File closed - OK\n");
    } else {
        printf("File not closed - ERROR\n");
        return NULL;
    }

    return newIndex;
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

void destroyIndex(Index ** mIndex) {
    Index *tmp;
    tmp = *mIndex;
    dispose(tmp->i_terms);
    free(tmp);
    *mIndex = NULL;
}
