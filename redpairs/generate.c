#include <stdio.h>

/*** MACROS ***/
#define N          3
#define R          1
#define L          (N + R - 1)
#define COND       condTC // condNone, condTC, condSF
#define PRINTSEQ          // uncomment to print each sequence
#define PRINTCOUNT        // uncomment to print total count
//#define ALL        // uncomment to include the nets with <= R reticulations

/*** VARIABLES ***/
// 0: not a leaf; 2: parent is ret; 3: sibling is ret; 1 otherwise
int Xstate[N];

#ifdef PRINTSEQ
int S[2 * L]; // sequence, in reversed order
#endif
int R1[L][N * 2 / 3];     // first coordinate of reducible pair matrix
int R2[L][N * 2 / 3];     // second coordinate of reducible pair matrix
int R3[L][N * 2 / 3];     // third coordinate of reducible pair matrix
int C[L];                 // count of red. pairs for each generation
int Xcount;               // number of leaves in current net
unsigned long long count; // total count

/*** FUNCTION DECLARATIONS ***/
int condNone(int a, int b, int isret); // generate all orchard networks
int condSF(int a, int b, int isret);   // only stack-free
int condTC(int a, int b, int isret);   // only tree-child
int lt(int a, int b, int c, int d);    // checks (a,b) < (c,d)
int addpair(int a, int b, int isret, int k, int *chleaf, int *chstate);
int addcherry(int a, int b, int k);
int addretcherry(int a, int b, int k, int *chleaf, int *chstate);
int addsafe(int ai, int bi, int ci, int flip, int k);
void generate(int k);
void printSeq(int k);

/*** IMPLEMENTATIONS ***/
int condNone(int a, int b, int isret) { return 1; }
int condSF(int a, int b, int isret) { return !isret || Xstate[a - 1] != 2; }
int condTC(int a, int b, int isret) { return !isret || Xstate[a - 1] < 2; }
int lt(int a, int b, int c, int d) { return a < c || (a == c && b < d); }

int addpair(int a, int b, int isret, int k, int *chleaf, int *chstate) {
    int r = isret ? addretcherry(a, b, k, chleaf, chstate) : addcherry(a, b, k);
	r = r || C[k] == 1 || lt(a, b, R1[k][1], R2[k][1]);
    if (r) {
        // set first pair (a, b) (isret)
        R1[k][0] = a;
        R2[k][0] = b;
        R3[k][0] = isret;
    }
    return r;
}

int addcherry(int a, int b, int k) {
    int smallest = 0; // if (a,b) is the smallest
    C[k]         = 1; // set first pair as (a, b)

    // transform all pairs
	for (int i = 0; i < C[k - 1]; i++) {
        // compare with first survival
        if (!smallest && C[k] > 1) {
            if (lt(a, b, R1[k][1], R2[k][1]))
                smallest = 1; // it's the smallest. Transform remaining pairs
            else
                return 0; // it's not the smallest. Avort.
        }
        // process pair (ai, bi) (ci)
        int ai = R1[k - 1][i];
        int bi = R2[k - 1][i];
        int ci = R3[k - 1][i];

        // Case 1: remains invariant
		if (ai != b && bi != b) {
            R1[k][C[k]] = ai;
            R2[k][C[k]] = bi;
            R3[k][C[k]] = ci;
            C[k]++;
        }
        // Case 2: do nothing
    }
    return smallest;
}

int addretcherry(int a, int b, int k, int *chleaf, int *chstate) {
    int smallest = 0;  // if (a,b) is the smallest
    int flip     = -1; // index of the flip (-1: not detected, -2: fixed)
    C[k]         = 1;  // set first pair as (a, b)

    // transform all pairs
    for (int i = 0; i < C[k - 1]; i++) {
        // compare with first survival
        if (!smallest && C[k] > 1) {
            if (lt(a, b, R1[k][1], R2[k][1]))
                smallest = 1; // it's the smallest. Transform remaining pairs
            else
                return 0; // it's not the smallest. Avort.
        }

        // process pair (ai, bi) (ci)
        int ai = R1[k - 1][i];
        int bi = R2[k - 1][i];
        int ci = R3[k - 1][i];

        // Case 1: remains invariant
        if (ai != a && ai != b && bi != a && bi != b)
            flip = addsafe(ai, bi, ci, flip, k);

        // Case 2: from cherry to reticulated cherry (happens only once)
        else if (!ci && flip == -1) {
            if (a == ai && b != bi) {
                flip           = addsafe(ai, bi, 1, -2, k);
                *chleaf        = bi;
                *chstate       = Xstate[bi - 1];
                Xstate[bi - 1] = 3;
            } else if (a == bi && b != ai) {
                flip           = i;
                *chleaf        = ai;
                *chstate       = Xstate[ai - 1];
                Xstate[ai - 1] = 3;
            }
        }
		// Case 3: do nothing
    }
    // check if flip persists
    if (flip >= 0)
        addsafe(R2[k - 1][flip], R1[k - 1][flip], 1, -2, k);

    return smallest;
}

int addsafe(int ai, int bi, int ci, int flip, int k) {
    // check if we have to change the flip
    if (flip >= 0 && lt(R2[k - 1][flip], R1[k - 1][flip], ai, bi)) {
        R1[k][C[k]] = R2[k - 1][flip];
        R2[k][C[k]] = R1[k - 1][flip];
        R3[k][C[k]] = 1;
        C[k]++;
        flip = -2;
    }
    // add (ai, bi) (ci)
    R1[k][C[k]] = ai;
    R2[k][C[k]] = bi;
    R3[k][C[k]] = ci;
    C[k]++;

    // update flip
    return flip;
}

void generate(int k) {
    for (int a = 1; a <= N; a++) {
        int isret = Xstate[a - 1] > 0; // 1: reticulated-cherry, 0: cherry

        // check if we are forced to add a cherry
		if (isret && L - N == k - Xcount)
            continue;

        for (int b = 1 + a * (!isret); b <= N; b++) {
            // try to add (a, b)
            if (a == b || !Xstate[b - 1] || !COND(a, b, isret))
                continue;

            int chleaf = 1, chstate = Xstate[0];
            if (addpair(a, b, isret, k, &chleaf, &chstate)) {
                // it's the smallest, prepare for expanding
#ifdef PRINTSEQ
                S[2 * k]     = a;
                S[2 * k + 1] = b;
#endif

				int stA = Xstate[a - 1], stB = Xstate[b - 1];
                Xstate[a - 1] = 1 + isret;
                Xstate[b - 1] = 1 + 2 * isret;
                Xcount += !isret;

				if (k + 1 == L) { // end
                    count++;
                    printSeq(k + 1);
                } else { // continue
#ifdef ALL
                    if (Xcount == N) {
                        count++;
                        printSeq(k + 1);
                    }
#endif // ALL
                    generate(k + 1); // keep going
                }
				// undo changes
                Xstate[a - 1] = stA;
                Xstate[b - 1] = stB;
                Xcount -= !isret;
            }
            // undo changes
            Xstate[chleaf - 1] = chstate;
        }
	}
}

void printSeq(int k) {
#ifdef PRINTSEQ
    for (int i = 2 * k; i > 0; i -= 2)
        printf("(%d,%d)", S[i - 2], S[i - 1]);
    printf("\n");
#endif
}

int main(int argc, char *argv[]) {
    count = 0;
    for (int i = 1; i < N; i++) {
        // prepare network (i,N)
        C[0]     = 1;
        R1[0][0] = i;
        R2[0][0] = N;
        R3[0][0] = 0;

        Xcount        = 2;
        Xstate[i - 1] = 1;
        Xstate[N - 1] = 1;

#ifdef PRINTSEQ
        S[0] = i;
        S[1] = N;
#endif
#ifdef ALL
#if N == 2
		printSeq(1);
        count++;
#endif
#endif
        generate(1);

        // prepare next iteration
        Xstate[i - 1] = 0;
    }
#ifdef PRINTCOUNT
    printf("%llu\n", count);
#endif
    return 0;
}
