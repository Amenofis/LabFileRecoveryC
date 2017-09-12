//
// Created by Felipe Muñoz on 02-09-17.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "modelo.h"

Term * createTerm(char * word) {
    Term * new_term = malloc(sizeof(Term));
    if (new_term == NULL) {
        printf("Error creating new term.\n");
        exit(0);
    }
    size_t length = strlen(word);
    new_term->t_word = (char*) malloc(sizeof(char) * (length + 1));
    strcpy(new_term->t_word, word);
    return new_term;
}

Term * prependTerm(Term * head, char * word) {
    Term * new_term = createTerm(word);
    new_term->next = head;
    head = new_term;
    return head;
}

void printTerms(Term * head) {
    Term * current = head;
    while(current != NULL) {
        printf("%s\n", current->t_word);
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