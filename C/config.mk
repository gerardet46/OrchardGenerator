# The number of leaves
N           = 3

# The max length of the sequence (|S|=n+r-1)
MAXLENGTH   = 3

# The condition to check
# This are the implemented conditions (functions.h):
# - condNone -> does not check anything
# - condSF   -> check if it's stack-free
# - condTC   -> check if it's tree child
CONDITION = condNone

# The output function. Check functions.h for more details
# This are the implemented functions:
# - printNothing -> does not output anything
# - printSeq     -> print the generated sequence
# - printPol     -> print the polynomial (raw)
# - printTexPol  -> print the polynomial (LaTeX format)
OUTPUT_FUNC = printNothing

# 1 to output the total count after finish (only in generate.c)
OUTPUT_TOTAL = 1

# 1 for output all intermediate sequences on X
# 0 for output only final sequence
OUTPUT_ALL = 0

# 1 for counting total seconds
COUNT_SECONDS = 0

# 1 for parallel execution (multithread)
PARALLEL = 0

# 1 to print when a child process finished
# (only with PARALLEL = 1)
PARALLEL_PRINT_FINISHED = 1
