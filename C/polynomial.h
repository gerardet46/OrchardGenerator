#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <stdlib.h>

#include "globals.h"

// structs: the polynomial is a linked list (ll) whose items are monomials
// the first item is the exponent of lambda and then x1, x2, ...
typedef struct monomial {
    int exp[N + 1];
    struct monomial *next;
} monomial;

typedef struct polynomial {
    monomial *fst, *lst, *prevlst; // first, last, and the previous of last
} polynomial;

// operations with monomials
monomial *mon_from_cherry(int x, int y);     // create x_i x_j
monomial *mon_from_ret_cherry(int x, int y); // create r x_i x_j
monomial *mon_copy(monomial *m);             // create a copy of m
int mon_equal(monomial *m1, monomial *m2);   // check if m1 and m2 are the same
int mon_degree(monomial *m);                 // returns the degree of m

// basic polynomial operations
polynomial *pol_new(monomial *m);          // new polynomial (ll of monomials)
void pol_push(polynomial *p, monomial *m); // push term
void pol_pop(polynomial *p);               // pop term

// cherry related monomial operations
void pol_add(polynomial *pol, int x, int y);        // add cherry (x,y)
void pol_add_ret(polynomial *pol, int x, int y);    // add ret. cherry (x,y)
void pol_reduce(polynomial *pol, int x);            // reduce cherry (x,y)
void pol_reduce_ret(polynomial *pol, int x, int y); // reduce ret. cherry (x,y)
int *pol_min_cherry(polynomial *pol);               // get smallest cherry

#endif /* POLYNOMIAL_H */
