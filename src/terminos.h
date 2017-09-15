//
// Created by Felipe Muñoz on 02-09-17.
//

#ifndef LABFILERECOVERYC_TERMINOS_H
#define LABFILERECOVERYC_TERMINOS_H

Term * createTerm(char * word);

Term * prependTerm(Term * head, char * word);

Term * searchTerm(Term * head, char * word);

void saveTerms(FILE * fp, Term * head);

Bool hasDocumentID(int id, Term * term);

void printTerms(Term * head);

void dispose(Term * head);

#endif //LABFILERECOVERYC_TERMINOS_H
