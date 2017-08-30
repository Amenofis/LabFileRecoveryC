#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "constantes.h"
#include "buscador.h"

void chopN(char *str, size_t n)
{
    assert(n != 0 && str != 0);
    size_t len = strlen(str);
    if (n > len)
        return;  // Or: n = len;
    memmove(str, str+n, len - n + 1);
}

char *get_line(char * str) {
    if (*str == '\0') return NULL;
    while(*str == '\n')
        str++;

    char *s = str;
    while(*str != '\n' && *str != '\0')
        str++;

    if (*str == '\n')
        *str = '\0';

    return s;
}

char *get_word(char * line){
    if (*line == '\0') return NULL;
    while(*line == ' '){
        line++;
    }
    char *s = line;
    while(*line != ' ' && *line != '\0'){
        line++;
    }
    if (*line == ' ') {
        *line = '\0';
        //line+=1;
    }
    return s;
}

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
                    createIndex(res_document_path, stopWords, &status);

                } else {
                    printf("No se puede indexar si no se han cargado las StopWords antes");
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

int countDocumentFiles(char * pathToDocument) {
    FILE *fp = fopen(pathToDocument, "r");
    int docs = 0;
    char buff[8];
    if (fp == NULL)
        return 0;
    docs++;
    while (fgets(buff, 8, fp)) {
        int id;
        if (sscanf(buff, ".I %d", &id) == 1) docs++;
    }
    fclose(fp);
    return docs;
}

int countDocumentWords(char * pathToDocument) {
    FILE *fp = fopen(pathToDocument, "r");
    int words = 0;
    char buff[8];
    if (fp == NULL)
        return 0;
    words++;
    while (fgets(buff, 8, fp)) {
        char aux[5];
    }
    return words;
}




Index * createIndex(char * pathDocumentsFile, StopWords * sw, code * statusCode) {
    //int documentsCound = countDocumentFiles(pathDocumentsFile);
    //if (documentsCound > 0) {
    //Index * ptrIndex = (Index*)malloc(sizeof(ptrIndex));
    //if (ptrIndex) {
        FILE *fp = fopen(pathDocumentsFile, "r");
        char *aux, *buff, *line;
        int i;
        Bool isStopWord;
        if (fp == NULL)
            return 0;
        buff = (char*)malloc(sizeof(buff)*255);

        while(fgets(buff, 255, fp)) {
            while ((line = get_line(buff)) != NULL) {
                buff += strlen(line);

                if (sscanf(buff, ".I %d", &i) == 1)
                    printf("\n%s -> It's an ID", buff);
                else if (strstr(buff, ".T\0") != NULL)
                    printf("\n%s -> It's a TITLE", buff);
                else if (strstr(buff, ".A\0") != NULL)
                    printf("\n%s -> It's an AUTHOR", buff);
                else if (strstr(buff, ".B\0") != NULL)
                    printf("\n%s -> It's the BIBLIOGRAPHY", buff);
                else if (strstr(buff, ".W\0") != NULL)
                    printf("\n%s -> It's the body", buff);
                else {
                    printf("\n----> %s", line);
                    while ((aux = get_word(line)) != NULL) {
                        printf("\n~~~~~> -%s-", aux);
                        line += strlen(aux) + 1;
                        //getchar();
                    }
                }


                printf("->buff: %s", buff);
                getchar();
            }
        }
            /*
            while(fscanf(fp, "%s", aux) > 0) {
                cleanWord(aux);
                isStopWord = FALSE;
                for (i = 0; i < sw->count; i++) {
                    if (strcmp(aux, sw->words[i]) == 0) {
                        //printf("\n '%s' - '%s'", aux, sw->words[i]);
                        isStopWord = TRUE;
                    }
                }

                if (isStopWord == FALSE && strcmp(aux, "") != 0) {
                    printf("\n ~> %s is not a StopWord!!", aux);
                }

            }
             */

        //free(ptrIndex);
        // Scan documents, extract values, index
        //ptrIndex->i_term = (Term*)malloc(sizeof(ptrIndex->i_term));
    //}
    //}
    return NULL;
}

static void cleanWord(char * word) {
    unsigned long i = 0; /* Scanning index */
    unsigned long x = 0; /* Write back index */

    char c;
    while ((c = word[i++]) != '\0') {
        if (isalnum(c)) {
            word[x++] = c;
        }
    }
    word[x] = '\0';

}
