#ifndef GLOBALS_H
#define GLOBALS_H

#define BUFFER_SZ   1024     // num of chars

// check if the following are not defined in config.mk
#ifndef N
#define N 3
#endif

#ifndef MAXLENGTH
#define MAXLENGTH 2
#endif

#ifndef CONDITION
#define CONDITION condNone
#endif

#ifndef OUTPUT_FUNC
#define OUTPUT_FUNC printSeq
#endif

#ifndef OUTPUT_TOTAL
#define OUTPUT_TOTAL 1
#endif

#ifndef OUTPUT_ALL
#define OUTPUT_ALL 0
#endif

#ifndef COUNT_SECONDS
#define COUNT_SECONDS 0
#endif

#ifndef PARALLEL
#define PARALLEL 1
#endif

#ifndef PARALLEL_PRINT_FINISHED
#define PARALLEL_PRINT_FINISHED 1
#endif

#endif /* GLOBALS_H */
