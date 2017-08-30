/*
 * Estructuras Propuestas

typedef struct {
....;
....;
} Index;

typedef struct {
....;
....;
} Result;

typedef struct {
....;
....;
} Ranking;

typedef struct {
....;
....;
} StopWords;

 *
 */

#ifndef _MODELO_H_
#define _MODELO_H_

typedef struct {
    int d_id;
    int d_SOL;
    int d_EOL;
} Document;

typedef struct Term {
    char * t_word;
    Document ** document;
    struct Term * next;
} Term;

typedef struct {
    int i_size;
    Term * i_terms;
} Index;

typedef struct {
    int count;
    char lang[2];
    char ** words;
} StopWords;

#endif