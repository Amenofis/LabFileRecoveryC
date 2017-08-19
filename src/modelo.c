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
    // Word ('hola'); Documents (1,2,3);
} Index;


typedef struct {
    int count;
    char ** words;
} StopWords;

#endif