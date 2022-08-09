#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "sequence.h"
#include "polynomial.h"

int condNone(sequence *S, polynomial *p, int x, int y, int R); // don't impose any condition
int condSF(sequence *S, polynomial *p, int x, int y, int R);   // stack-free condition
int condTC(sequence *S, polynomial *p, int x, int y, int R);   // tree child condition

void printNothing(sequence *S, polynomial *p, int c); // do not print anything
void printSeq(sequence *S, polynomial *p, int c);     // print the generated sequence
void printPol(sequence *S, polynomial *p, int c);     // print the generated polynomial
void printTexPol(sequence *S, polynomial *p, int c);  // print the generated polynomial (LaTeX)

#endif /* FUNCTIONS_H */
