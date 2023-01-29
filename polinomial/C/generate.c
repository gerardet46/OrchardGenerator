#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <stdio.h>
#include <time.h>

#if PARALLEL == 1
#include <unistd.h>
#endif

#include "globals.h"
#include "polynomial.h"
#include "sequence.h"

unsigned long count = 0;

void generate();
void findSequences(sequence *S, polynomial *p, int *L, int Lcount);

void generate() {
#if N == 1
    count++;
    OUTPUT_FUNC(NULL, NULL, count);
    return;
#endif

    // leaves: list of 1 if it's in S or 0 if isn't
    int *L     = (int *)malloc(sizeof(int) * N);
    int Lcount = 1; // how many in L are set to 1

    // Set only leaf N to 1
    for (int i = 1; i < N; i++)
        L[i - 1] = 0;

    L[N - 1] = 1;

    // init pointers
    sequence *S   = seq_new(0, N);
    polynomial *p = pol_new(mon_from_cherry(N, N));

#if PARALLEL == 1
    int child = 0;
    int chanel[2];
    pipe(chanel);
#endif

    // add pair (i,N)
    for (int i = 1; i < N; i++) {
#if PARALLEL == 1
        if (fork() == 0) {
            close(chanel[0]);
            child = 1;
#endif

            S->fst->val = i;

            // now we have leaves i, N
            L[i - 1] = 1;
            Lcount++;

            // in case we end
#if MAXLENGTH == 1
            count++;
            OUTPUT_FUNC(S, NULL, count);
#if PARALLEL == 1
            free(p);
            free(S);
            exit(0)
#endif
                continue;
#endif
#if N == 2 && OUTPUT_ALL != 0
            count++;
            OUTPUT_FUNC(S, NULL, count);
#endif

            // search sequences
            p->lst->exp[i] = 1;
            findSequences(S, p, L, Lcount);

#if PARALLEL == 1
#if PARALLEL_PRINT_FINISHED == 1
            printf("PID %d finished. Count for (%d,%d): %d\n", getpid(), i, N, count);
#endif
            write(chanel[1], &count, sizeof(count));
            free(p);
            free(S);
            exit(0);
        }
#else
        p->lst->exp[i] = 0;
        L[i - 1]       = 0;
        Lcount--;
#endif
    }

    // if parallel, sum all the results
#if PARALLEL == 1
    if (!child) {
        close(chanel[1]);
        int c;
        while (read(chanel[0], &c, sizeof(c)))
            count += c;
        int pid;
        do {
            pid = wait(NULL);
        } while (pid != -1);
        close(chanel[0]);
        close(chanel[1]);
    }
#endif

    // free pointers
    free(p);
    free(S);
}

void findSequences(sequence *S, polynomial *p, int *L, int Lcount) {
    for (int i = 1; i <= N; i++) { // for each in X
        // if it's already in L, it's a cherry, otherwise reticulated
        int is_ret_cherry = L[i - 1];
        int j             = is_ret_cherry ? 1 : i + 1;

        // check if we are forced to add a cherry
        if (is_ret_cherry && MAXLENGTH - N == S->pairs - Lcount)
            continue;

        for (; j <= N; j++) { // for each in L
            if (i == j || L[j - 1] == 0)
                continue;

            if (!CONDITION(S, p, i, j, is_ret_cherry))
                continue;

            // add (i,j)
            seq_add_pair(S, i, j);
            if (is_ret_cherry)
                pol_add_ret(p, i, j);
            else {
                pol_add(p, i, j);
                L[i - 1] = 1;
                Lcount++;
            }

            int *smallest = pol_min_cherry(p);
            if (smallest[0] == i && smallest[1] == j) {
                // it's the smallest
                free(smallest);
                if (S->pairs < MAXLENGTH) { // continue
#if OUTPUT_ALL != 0
                    if (Lcount == N) {
                        count++;
                        OUTPUT_FUNC(S, p, count);
                    }
#endif
                    findSequences(S, p, L, Lcount);
                }

                else { // generated!
                    OUTPUT_FUNC(S, p, count);
                    count++;
                }
            } else
                free(smallest);

            seq_del_pair(S);
            if (is_ret_cherry)
                pol_reduce_ret(p, i, j);
            else {
                pol_reduce(p, i);
                L[i - 1] = 0;
                Lcount--;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // check necessary condition
#if MAXLENGTH < N - 1
    printf("Error: maxlength < n - 1\n");
    exit(1);
#endif

    generate();

    // print total count
#if OUTPUT_TOTAL != 0
    printf("%ld\n", count);
#endif

    return 0;
}
