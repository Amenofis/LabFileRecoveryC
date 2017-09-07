//
// Created by ameno on 02-09-17.
//

#ifndef LABFILERECOVERYC_TERMINOS_H
#define LABFILERECOVERYC_TERMINOS_H

Term * createTerm(char * word, Term * next);

Term * prependTerm(Term * head, char * word);

void printTerms(Term * head);

void dispose(Term * head);

#endif //LABFILERECOVERYC_TERMINOS_H
