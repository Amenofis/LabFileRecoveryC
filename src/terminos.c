//
// Created by Felipe Muñoz on 02-09-17.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "modelo.h"
#include "constantes.h"

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

Term * searchTerm(Term * head, char * word) {
    Term * current = head;
    while (current != NULL) {
        if (strcmp(current->t_word, word) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

void saveTerms(FILE * fp, Term * head) {
    int i;
    Term * current = head;
    while (current != NULL) {
        fprintf(fp, "%s %d ", current->t_word, current->t_docs_count);
        char sep = ',';
        for (i = 0; i < current->t_docs_count; i++) {
            fprintf(fp, "%d", current->t_docs[i]);

            if ((i+1) != current->t_docs_count)
                fprintf(fp, "%c", sep);
        }
        fprintf(fp, "\n");
        current = current->next;
    }
}

Bool hasDocumentID(int id, Term * term) {
    int i;
    for (i = 0; i < term->t_docs_count; i++)
        if (term->t_docs[i] == id) return TRUE;
    return FALSE;
}

void printTerms(Term * head) {
    Term * current = head;
    while(current != NULL) {
        printf("%s (%d)\n", current->t_word, current->t_docs_count);
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