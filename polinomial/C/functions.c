#include <limits.h>
#include "sequence.h"
#include "polynomial.h"

/************** CONDITIONS **************/
/*
  FORMAT OF CONDITIONS:
  condName(S, p, x, y, R) and returns 1/0 (True/False) when trying
      to add (x,y) to S (and p). R=1 means (x,y) is reticulated, 0 otherwise
*/

// "no condition"
int condNone(sequence *S, polynomial *p, int x, int y, int R) {
    return 1;
}

// the stack-free condition
int condSF(sequence *S, polynomial *p, int x, int y, int R) {
    // trivial case
    if (R == 0 || S->pairs < 2)
        return 1;

    // check if parent of x is already a reticulation in S
    int nextX = 0; // check if next (a,b) containing x is a=x

    item *aux = S->fst;
    while (aux != NULL) {
        int a = aux->val, b = aux->next->val;

        // check if x is really a reticulation
        if (nextX) {
            if (a == x || b == x)
                return 0;
        } else {
            if (a == x) // p. of x may be a reticulation
                nextX = 1;
            else if (b == x) // p. of x is tree node, then is stack-free
                return 1;
        }

        aux = aux->next->next;
    }
	return 1;
}

// the tree-child condition
int condTC(sequence *S, polynomial *p, int x, int y, int R) {
    if (R == 0 || S->pairs < 2)
        return 1;

    // search min multiple of x in p
    int deg = INT_MAX;
    monomial *aux = p->fst, *argmin = NULL;
    while (aux != NULL) {
        if (aux->exp[x] > 0) { // check it has the term x
            int d = mon_degree(aux);
            if (d < deg) {
                deg = d;
                argmin = aux;
            }
        }
        aux = aux->next;
    }

    // check if degree without x is 1
    if (deg - argmin->exp[x] == 1)
        return 1;

    // check if the monomial without x is in p
    monomial *copy = mon_copy(argmin);
    copy->exp[x] = 0;

    aux = p->fst;
	while (aux != NULL) {
        if (mon_equal(aux, copy)) {
            free(copy);
            return 1;
        }

        aux = aux->next;
    }
    free(copy);
    return 0;
}

/************** OUTPUT FUNCTIONS **************/
/*
  FORMAT OF OUTPUT FUNCTIONS:
  printName(S, p, c) -> process sequence and polynomial, c is the total count
*/

// don't print anything
void printNothing(sequence *S, polynomial *p, int c) {
    // uncomment this to print count every 1000000
    //if ((c+1) % 10000000 == 0)
    //    printf("%d\n", c+1);
}

// print the sequence
void printSeq(sequence *S, polynomial *p, int c) {
    seq_print(S, 1);
}

// print the polynomial
void printPol(sequence *S, polynomial *p, int c) {
    monomial *aux = p->fst;
    while (aux != NULL) {
        if (aux->exp[0] == 1)
            printf("r");
        else if (aux->exp[0] > 1)
            printf("r**%d", aux->exp[0]);

        for (int i = 1; i <= N; i++) {
            if (aux->exp[i] == 1)
                printf(" x_%d", i);
            else if (aux->exp[i] > 1)
                printf(" x_%d**%d", i, aux->exp[i]);
        }

		aux = aux->next;
        if (aux != NULL)
            printf(" + ");
    }
    printf("\n");
}

// print latex polynomial
void printTexPol(sequence *S, polynomial *p, int c) {
    monomial *aux = p->fst;
    while (aux != NULL) {
        if (aux->exp[0] == 1)
            printf("\\lambda");
        else if (aux->exp[0] > 1)
            printf("\\lambda^{%d}", aux->exp[0]);

        for (int i = 1; i <= N; i++) {
            if (aux->exp[i] == 1)
                printf(" x_{%d}", i);
            else if (aux->exp[i] > 1)
                printf(" x_{%d}^{%d}", i, aux->exp[i]);
        }

		aux = aux->next;
        if (aux != NULL)
            printf("+");
    }
    printf("\n");
}
