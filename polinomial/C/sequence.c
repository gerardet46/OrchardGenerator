#include "sequence.h"
#include <stdio.h>

char *seq_print(sequence *S, int out) {
    char *result = NULL;
    if (!out)
        result = (char *)malloc(BUFFER_SZ * sizeof(char));

    item *aux = S->fst;

    // empty sequence
    if (aux == NULL) {
        return "()";
    }

    // non-empty sequence
    while (aux != NULL) {
        if (out)
            printf("(%d,%d)", aux->val, aux->next->val);
        else
            sprintf(result, "%s(%d,%d)", result, aux->val, aux->next->val);
        aux = aux->next->next;
    }
    if (out)
        printf("\n");

    return result;
}

sequence *seq_new(int x, int y) {
    item *a     = (item *)malloc(sizeof(item));
    item *b     = (item *)malloc(sizeof(item));
    sequence *S = (sequence *)malloc(sizeof(sequence));

    a->val  = x;
    b->val  = y;
    a->next = b;

    S->fst   = a;
    S->pairs = 1;

    return S;
}

void seq_add_pair(sequence *S, int x, int y) {
    item *a = (item *)malloc(sizeof(item));
    item *b = (item *)malloc(sizeof(item));

    a->val  = x;
    b->val  = y;
    a->next = b;
    b->next = S->fst;

    S->fst = a;
    S->pairs++;
}

void seq_del_pair(sequence *S) {
    item *a = S->fst;
    item *b = a->next;

    S->fst = b->next;
    free(a);
    free(b);

    S->pairs--;
}
