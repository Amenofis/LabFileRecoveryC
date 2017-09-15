#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "buscador.h"
#include "utils.h"
#include "terminos.h"
#include "resultados.h"

/**
 *  Created by Felipe Muñoz
 *  SRI - Indexer and Search
 */

code status = OK;
StopWords * ptrStopWords = NULL;
Index * ptrIndex = NULL;
Ranking * ptrRanking = NULL;

char* res_stopwords_path = "res/StopWords.txt";
char* res_document_path = "res/TestCollection.txt";

int main(int argc, char* argv[]) {
    int op;
    char queryStr[100];

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
                printf("6. Mostrar última búsqueda (%d)\n", ptrRanking->r_count);
            }
        }

        printf("0 Salir\n");
        printf("Ingrese opción: ");
        scanf("%d", &op);

        int c;
        while ((c = getchar()) != EOF && c != '\n');

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
                    printf("Ingrese búsqueda: ");
                    //scanf("%[^\n]%*c", queryStr);
                    fgets( queryStr, sizeof queryStr, stdin);
                    //replaceAlNum(queryStr, ' ');

                    if (strlen(queryStr) > 0)
                        ptrRanking = query(ptrIndex, ptrStopWords, queryStr, &status);
                    else
                        printf("\nDebe ingresar una búsqueda con al menos una palabra");
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
                    int amount = 0;
                    printf("\nIngrese cantidad de resultados a mostrar: ");
                    scanf("%d", &amount);
                    displayResults(ptrRanking, amount, &status);
                }
                break;
            case 9:
                if (ptrIndex != NULL) {
                    printTerms(ptrIndex->i_terms);
                    printf("Documents \n");
                    int i;
                    for (i = 0; i < ptrIndex->i_docs_size; i++) {
                        printf("%d (%ld, %d) \n", ptrIndex->i_documents[i]->d_id,
                               ptrIndex->i_documents[i]->d_SOD,
                               ptrIndex->i_documents[i]->d_EOD);
                    }
                }
                break;
            case 0:
                printf("Exit\n");
                break;
            default:
                printf("Opción inválida\n");
                exit(0);
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

    if (ptrRanking != NULL) {
        destroyRanking(&ptrRanking);
        if (ptrRanking == NULL)
            printf("Ranking destroyed - OK\n");
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
                    new_document->d_SOD = ftell(fp);
                    new_document->d_EOD = 0;
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
            new_index->i_documents[docs_count-1]->d_EOD++;
            line_count++;
        } // End of line read
    }  // End of FGETS

    // Update the document's count and add the last line to the last document
    new_index->i_docs_size = docs_count;
    //new_index->i_documents[docs_count-1]->d_EOD = line_count-1;

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
        fprintf(fp, "%d %ld %d\n", ndx->i_documents[i]->d_id,
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

/**
 *
 * @param id
 * @param statusCode
 * @return
 */
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
                sscanf(buffer, "%d %ld %d", &loadedIndex->i_documents[count]->d_id,
                       &loadedIndex->i_documents[count]->d_SOD,
                       &loadedIndex->i_documents[count]->d_EOD);
                count++;
            } else {
                int d = 0, i = 0;
                char * dArr = malloc(sizeof(char) * 255);
                sscanf(buffer, "%s %d %s", word, &d, dArr);

                Term * term = prependTerm(loadedIndex->i_terms, word);
                term->t_docs_count = d;
                term->t_docs = calloc(sizeof(int), (size_t) d);

                char *n = strtok(dArr, ",");
                do {
                    term->t_docs[i++] = atoi(n);
                } while ((n = strtok(NULL, ",")));

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
 *
 * @param i
 * @param sw
 * @param text
 * @param statusCode
 * @return
 */
Ranking * query(Index * i, StopWords * sw, char * text, code * statusCode) {
    Ranking * new_ranking = (Ranking*) malloc(sizeof(Ranking));
    if (new_ranking == NULL) {
        *statusCode = NOT_ENOUGH_MEMORY;
        destroyRanking(&new_ranking);
        return NULL;
    }

    new_ranking->r_results = NULL;
    new_ranking->r_query = (char*) malloc(sizeof(char) * strlen(text));
    if (new_ranking->r_query == NULL) {
        *statusCode = NOT_ENOUGH_MEMORY;
        destroyRanking(&new_ranking);
        return NULL;
    }
    strcpy(new_ranking->r_query, text);

    char * word;
    int j, count = 0;

    while ((word = get_word(text)) != NULL) {
        text += strlen(word)+1;
        if (isStopWord(sw, word) == FALSE) { // Word from query is not a StopWord
            Term *term = searchTerm(i->i_terms, word);
            if (term != NULL) {
                for (j = 0; j < term->t_docs_count; j++) {
                    Result *result = searchResult(findDocument(term->t_docs[j], i), new_ranking->r_results);
                    if (result != NULL) {
                        result->r_count++;
                    } else {
                        new_ranking->r_results = createResult(count, findDocument(term->t_docs[j], i),
                                                              new_ranking->r_results);
                    }
                    count++;
                }
                //printf("WORD '(%s)' found in '%d' Document(s) ", word, term->t_docs_count);
            }
        }
    }

    printf("Resultados de la búsqueda: (%d) coincidencia(s) encontrada(s)\n", count);

    new_ranking->r_count = count;
    return new_ranking;
}

void displayResults(Ranking * r, int TopK, code * statusCode) {


    if (r->r_count > 0) {
        FILE * fp = fopen(res_document_path, "r");
        if (fp == NULL) {
            *statusCode = ERR_FILE_NOT_FOUND;
            return;
        }

        printf("\nMostrando resultados de la consulta: '%s'\n", get_word(r->r_query));

        int count = 0, line_count;
        char buffer[255];
        Result * current = r->r_results;
        while (current != NULL) {
            fseek(fp, current->r_document->d_SOD, SEEK_SET);
            line_count = 0;
            printf("\n(%d/%d)------------------\n", count + 1, r->r_count);
            while(line_count < current->r_document->d_EOD - 1) {
                fgets(buffer, 254, fp);
                printf("%s\n", get_line(buffer));
                line_count++;
            }

            rewind(fp);
            count++;
            if (count == TopK) break;
            current = current->next;
        }

        printf("\n-------------------------\n");
        printf("Fin de los resultados\n");

        if (!fclose(fp)) {
            printf("File closed - OK\n");
        } else {
            *statusCode = ERR_FILE_NOT_PERM;
            return;
        }
    } else {
        printf("No hay resultados para mostrar\n");
    }


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
 *
 * @param id
 * @param documents
 * @return
 */
Document * findDocument(int id, Index * index) {
    int i;
    for (i = 0; i < index->i_docs_size; i++)
        if (id == index->i_documents[i]->d_id) return index->i_documents[i];

    return NULL;
}

/**
 * destroyStopWords
 * Free the given stopword's allocated memory
 * @param mStopWords StopWord's memory pointer
 */
void destroyStopWords(StopWords **mStopWords) {
    StopWords *temp;
    temp = *mStopWords;
    int i;
    for (i = 0; i < temp->count; i++)
        free(temp->words[i]);
    free(temp->words);
    free(temp);

    *mStopWords = NULL;
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
        disposeTerms(tmp->i_terms);
    free(tmp);
    *mIndex = NULL;
}


/**
 *
 * @param mRanking
 */
void destroyRanking(Ranking ** mRanking) {
    Ranking *tmp;
    tmp = *mRanking;
    if (tmp->r_results != NULL)
        disposeResults(tmp->r_results);
    free(tmp->r_query);
    free(tmp);
    *mRanking = NULL;
}