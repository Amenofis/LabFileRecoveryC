//
// Created by ameno on 02-09-17.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "modelo.h"
#include "terminos.h"

Term * createTerm(char * word, Term * next) {
    Term * new_term = malloc(sizeof(Term));
    if (new_term == NULL) {
        printf("Error creating new term.\n");
        exit(0);
    }
    new_term->t_word = (char*) malloc(sizeof(char) * strlen(word));
    strcpy(new_term->t_word, word);
    new_term->next = next;
    return new_term;
}

Term * prependTerm(Term * head, char * word) {
    Term * new_term = createTerm(word, head);
    head = new_term;
    return head;
}

void printTerms(Term * head) {
    Term * current = head;
    while(current != NULL) {
        printf("\n%s", current->t_word);
        current = current->next;
    }
}

void dispose(Term * head) {
    Term *term, *tmp;
    if (head != NULL) {
        term = head->next;
        head->next = NULL;
        while (term != NULL) {
            tmp = term->next;
            free(term->t_word);
            free(term);
            term = tmp;
        }
    }
}