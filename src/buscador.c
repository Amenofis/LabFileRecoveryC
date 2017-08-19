#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modelo.c"
#include "constantes.h"

int countFileLines(char* pathToFile);

StopWords* loadStopWords(char* pathStopWordsFile, code *statusCode);

void destroyStopWords(StopWords **stopWords);

int main(int argc, char* argv[]) {

    char* res_stopwords_path = "/home/ameno/Projects/LabFileRecoveryC/res/StopWords.txt";
    char* res_document_path = "res/TestCollection.txt";

    int op;
    code status;
    StopWords *stopWords = NULL;

    do {
        printf("\n================\n");
        printf("Menu\n");
        printf("1 Cargar StopWords\n");
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
            case 0:
                if (stopWords != NULL) {
                    destroyStopWords(&stopWords);
                    if (stopWords == NULL) {
                        printf("\nStopWords destroyed\n");
                    }
                }
                printf("Salir\n");
                break;
            default:
                break;
        }
    } while (op != 0);

    return 0;
}


StopWords* loadStopWords(char* pathStopWordsFile, code *statusCode) {
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
                while (fgets(buff, 20, stopWordsFile)) {
                    //printf("%s\n", buff);
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