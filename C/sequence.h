#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

/*
  the sequence is implemented as an stack. For example,
  the list 1,2,2,3 corresponds to the seq (1,2)(2,3).

  1,2,2,3 are the items and the sequence contains also the
  length divided by 2 to know the number of pairs
*/
typedef struct item {
    int val;
    struct item *next;
} item;

typedef struct sequence {
	item *fst;
    int pairs;
} sequence;

// functions
char *seq_print(sequence *S, int out);        // print seq. in (a,b) form, out=1 for stdout, 0 for char*
sequence *seq_new(int x, int y);              // creates new sequence with one pair (x,y)
void seq_add_pair(sequence *S, int x, int y); // add (x,y) to S
void seq_del_pair(sequence *S);               // delete last added pair

#endif /* SEQUENCE_H */
