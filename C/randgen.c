#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "globals.h"
#include "polynomial.h"
#include "sequence.h"

// here we store all possible choices for smallest red. pairs
// we store in negative numbers if the pair is reticulated
int candidates[2 * N * (N - 1)]; // with N leaves, we cannot have N red. pairs
int candcount;         // the total candidates

// more variables and pointers (same as in generate.c)
sequence *S;
polynomial *p;
int *L;
int Lcount;

void clean();        // clean sequence and polynomial (restart)
int genrandom();     // base case
int findSequences(); // the recursive function (retuns 0 if it fails),

void clean() {
    // S and p have the same number of pairs
    while (S->pairs > 1) {
        seq_del_pair(S);
        pol_pop(p);
    }
}

int genrandom() {
    // trivial case
#if N == 1
    OUTPUT_FUNC(NULL, NULL, 0)
    return 1;
#endif
    // Set only leaf N to 1
    for (int i = 1; i < N; i++)
        L[i - 1] = 0;

    L[N - 1] = 1;

    // select a random leaf in 1..n-1
    int i = rand() % (N - 1) + 1;
    //int i = 2;

    S->fst->val = i; // S = (i,N)
    L[i - 1] = 1;

    // in case we end
#if MAXLENGTH == 1
    OUTPUT_FUNC(S, NULL, 0);
    return 1;
#endif

    // search sequences
    Lcount = 2;
    p->lst->exp[i] = 1;

    int r = findSequences();

    // clean
    clean();
    p->lst->exp[i] = 0;
    L[i - 1]       = 0;

    // return if it has find one
    return r;
}

int findSequences() {
    /* now we need to compute all the possible candidates of
       smallest reducible pairs and then select one randomly */

    // reset candidates
    candcount = 0;

    for (int i = 1; i <= N; i++) { // for each in X
        // if it's already in L, it's a cherry, otherwise reticulated
        int is_ret_cherry = L[i - 1];
        int j             = is_ret_cherry ? 1 : i + 1;

        // check if we are forced to add a cherry
        if (is_ret_cherry && MAXLENGTH - N == S->pairs - Lcount) {
            continue;
        }

        for (; j <= N; j++) { // for each in L
            if (i == j || L[j - 1] == 0)
                continue;

            if (!CONDITION(S, p, i, j, is_ret_cherry))
                continue;


            // check (i,j) is the smallest
            if (is_ret_cherry)
                pol_add_ret(p, i, j);
            else
                pol_add(p, i, j);

            int *smallest = pol_min_cherry(p);
            if (smallest[0] == i && smallest[1] == j) {
                if (is_ret_cherry) {
                    candidates[2 * (candcount)]     = -i;
                    candidates[2 * (candcount) + 1] = -j;
                } else {
                    candidates[2 * (candcount)]     = i;
                    candidates[2 * (candcount) + 1] = j;
                }
                candcount++;
            }
            free(smallest);

            // return to original polynomial
            if (is_ret_cherry)
                pol_reduce_ret(p, i, j);
            else
                pol_reduce(p, i);
        }
    }

    // check if there are no candidates
	if (!candcount)
		return 0;

    // now choose a random candidate (a,b)
    int c = rand() % candcount;
    int i = candidates[2 * c], j = candidates[2 * c + 1];

    // apply this candidate
    if (i < 0) { // reticulated
        i = -i;
        j = -j;
        pol_add_ret(p, i, j);
    } else { // not reticulated
        pol_add(p, i, j);
        L[i - 1] = 1;
        Lcount++;
    }
    seq_add_pair(S, i, j);

    if (S->pairs < MAXLENGTH) // continue
        return findSequences();

    // else, we finish so output the sequence
    OUTPUT_FUNC(S, p, 0);
    return 1;
}

int main(int argc, char *argv[]) {
    // check necessary condition
#if MAXLENGTH < N - 1
    printf("Error: maxlength < n - 1\n");
    exit(1);
#endif

    // random seed
    srand(time(NULL));

    // init pointers
    L = (int *)malloc(sizeof(int) * N);
    S = seq_new(0, N);
    p = pol_new(mon_from_cherry(N, N));

    // generate some random CPS
    int times = argc > 1 ? strtol(argv[1], NULL, 10) : 1;
    while (--times >= 0) {
        // generate a random CPS
        int r = genrandom();

		if (!r)
            times++; // repeat if it has failed
	}

	return 0;
}
