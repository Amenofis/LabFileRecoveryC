/*
 * Created by Felipe Muñoz
 * Modelo SRI
 */

#ifndef _MODELO_H_
#define _MODELO_H_

typedef struct S_Document {
    int d_id;
    int d_SOD;
    int d_EOD;
} Document;

typedef struct S_Term {
    int t_docs_count;
    int * t_docs;
    char * t_word;
    struct S_Term * next;
} Term;

typedef struct S_Index {
    int i_id;
    int i_terms_size;
    int i_docs_size;
    Term * i_terms;
    Document ** i_documents;
} Index;

typedef struct S_StopWords {
    int count;
    char lang[2];
    char ** words;
} StopWords;

typedef struct S_Result {
    int ID, start, end, count;
    struct S_Result * next;
} Result;

typedef struct S_Ranking {
    int count;
    char * query;
    Result ** result;
} Ranking;

#endif