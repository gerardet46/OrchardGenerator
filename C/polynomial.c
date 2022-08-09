#include "polynomial.h"

monomial *mon_from_cherry(int x, int y) {
    monomial *m = (monomial *)malloc(sizeof(monomial));

    for (int i = 0; i <= N; i++)
        m->exp[i] = 0;

    m->exp[x] = 1;
    m->exp[y] = 1;

    return m;
}

monomial *mon_from_ret_cherry(int x, int y) {
    monomial *m = mon_from_cherry(x, y);

    // set lambda exponent to one
    m->exp[0] = 1;

    return m;
}

monomial *mon_copy(monomial *m) {
    monomial *new = (monomial *)malloc(sizeof(monomial));
    for (int i = 0; i <= N; i++)
        new->exp[i] = m->exp[i];

    return new;
}

int mon_equal(monomial *m1, monomial *m2) {
    for (int i = 0; i <= N; i++)
        if (m1->exp[i] != m2->exp[i])
            return 0;

    return 1;
}

int mon_degree(monomial *m) {
    int sum = 0;
    for (int i = 0; i <= N; i++)
        sum += m->exp[i];

    return sum;
}

polynomial *pol_new(monomial *m) {
    polynomial *p = (polynomial *)malloc(sizeof(polynomial));
    p->fst = p->lst = p->prevlst = m;

    return p;
}

void pol_push(polynomial *p, monomial *m) {
    if (p->fst == p->lst) {
        p->prevlst = m;
    }

    m->next = p->fst;
    p->fst  = m;
}

void pol_pop(polynomial *p) {
    monomial *deleted = p->fst;
    p->fst            = p->fst->next;
    free(deleted);
}

void pol_add(polynomial *pol, int x, int y) {
    // set exponent of x as the same of y
    monomial *aux = pol->fst;
    while (aux != NULL) {
        aux->exp[x] = aux->exp[y];
        aux         = aux->next;
    }

    // add (x,y) monomial
    pol_push(pol, mon_from_cherry(x, y));
}

void pol_add_ret(polynomial *pol, int x, int y) {
    // set exponent of x as the same of y
    monomial *aux = pol->fst;
    int r_exp, x_exp, y_exp;
    while (aux != NULL) {
        r_exp = aux->exp[0];
        x_exp = aux->exp[x];
        y_exp = aux->exp[y];

        aux->exp[0] = r_exp + x_exp + y_exp;
        aux->exp[x] = x_exp + y_exp;

        aux = aux->next;
    }

    // add (x,y) monomial
    pol_push(pol, mon_from_ret_cherry(x, y));
}

/* suposing m is a cherry */
void pol_reduce(polynomial *pol, int x) {
    // remove (x,y) monomial
    pol_pop(pol);

    // set exponent of x as 0
    monomial *aux = pol->fst;
    while (aux != NULL) {
        aux->exp[x] = 0;
        aux         = aux->next;
    }
}

/* suposing m is a reticulated cherry */
void pol_reduce_ret(polynomial *pol, int x, int y) {
    // remove (x,y) monomial
    pol_pop(pol);

    // correct exponents
    monomial *aux = pol->fst;
    int r_exp, x_exp, y_exp;
    while (aux != NULL) {
        r_exp = aux->exp[0];
        x_exp = aux->exp[x];
        y_exp = aux->exp[y];

        aux->exp[0] = r_exp - x_exp;
        aux->exp[x] = x_exp - y_exp;

        aux = aux->next;
    }
}

int *pol_min_cherry(polynomial *pol) {
    // returns pair = {a, b, r} where r=1 means it's reticulated
    monomial *aux = pol->fst;
    int *pair     = (int *)malloc(sizeof(int) * 3);
    pair[0]       = N;
    pair[1]       = N;
    pair[2]       = -1;

    // search for smallest pair in monomial m, whose previous is prev
    while (aux != NULL) {
        // check if aux is a cherry
        int r;
        if (aux->exp[0] == 0)
            r = 0; // maybe cherry
        else if (aux->exp[0] == 1)
            r = 1; // maybe ret. cherry
        else
            goto skip; // not a cherry, go to next monomial

        // check if it's a cherry
        int a = 0, b = 0;
        int is_cherry = 1; // continue?
        for (int i = 1; i <= N; i++) {
            if (aux->exp[i] > 1)
                break; // not a (ret) cherry
            if (aux->exp[i] == 1) {
                if (a == 0)
                    a = i; // 1st coordinate
                else if (b == 0)
                    b = i; // 2nd coordinate
                else
                    goto skip; // 3rd coordinate NO!!
            }
        }
        if (b == 0)
            is_cherry = 0;

        if (!is_cherry)
            goto skip;

        // check if we have to swap (a,b) to (b,a). We look at the last monomial
        if (pol->lst->exp[a] < pol->lst->exp[b]) {
            int temp = a;
            a        = b;
            b        = temp;
        }
        // check if it's smaller
        if (a < pair[0] || (a == pair[0] && b < pair[1])) {
            // it's smaller
            pair[0] = a;
            pair[1] = b;
            pair[2] = r;
        }
    skip:
        aux = aux->next;
    }

    return pair;
}
