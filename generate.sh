#!/usr/bin/sh

# Shell: POSIX compliant
# Dependencies: gcc

SC="$0"                                # this script
DIR="."                                # directory of the C program
GENFILE="$DIR/generator.c"             # the C code
TMPFILE="$DIR/tmpconsts.h"             # the constants temporary C header
CTSFILE="$DIR/consts.h"                # the constants C header
EXEFILE="$DIR/generator"               # the generator executable
GCCOPT="-O3 -pipe -march=native"       # flags for GCC
GCCCMD="gcc $GCCOPT -o '$EXEFILE' '$GENFILE'" # the compiler command

help() {
    cat <<EOF
Usage: $SC [-n n] [-r r] [-c condition] [-hCSp]
Generates all orchard networks (in fact MCRSs) over {1,...,n} and r reticulations.

The following options can be set to modify the behaviour of the generator.

Mandatory:
-n n                    number of leaves
-r r                    number of reticulations

Restriction (optional):
-c, --condition none    generate orchard networks (default)
-c, --condition tc      restrict to tree-child networks
-c, --condition sf      restrict to stack-free networks

Misc (optional):
-p, --partials          generate all nets with <=r reticulations
-S, --print-sequence    print the generated MCRSs to stdout
-C, --print-count       print the total count of generated MCRS to stdout
-h, --help              Display this help message

Examples of usage:
- Show all orchard networks with 3 leaves and exactly 1 reticulation
      $SC -n3 -r1 -S

- Count all tree-child networks with 6 leaves
      $SC -n6 -r5 -c tc -Cp
EOF
}

usage() {
    echo "Usage: $SC [-n leaves] [-r reticulations] [-c none|tc|sf] [-hCSp]"
}

# Transform long options to short ones
for arg in "$@"; do
    shift
    case "$arg" in
        '--help')           help; exit 0      ;;
        '--condition')      set -- "$@" '-c'   ;;
        '--print-sequence') set -- "$@" '-S'   ;;
        '--print-count')    set -- "$@" '-C'   ;;
        '--partials')       set -- "$@" '-p'   ;;
        "--"*)              usage; exit 2      ;;
        *)                  set -- "$@" "$arg" ;;
    esac
done

# start temp header file
echo "#ifndef CONSTS_H"      >  "$TMPFILE"
echo "#define CONSTS_H"      >> "$TMPFILE"

# Parse short options
COND="none"
OPTIND=1
while getopts "hn:r:c:SCp" opt
do
    case "$opt" in
        'h')
            help
            exit 0
            ;;
        'n')
            N=$OPTARG
            echo "#define N $N" >> "$TMPFILE"
            ;;
        'r')
            R=$OPTARG
            echo "#define R $R" >> "$TMPFILE"
            ;;
        'c') COND=$(echo $OPTARG | tr '[:upper:]' '[:lower:]') ;;
        'S') echo "#define PRINTSEQ"   >> "$TMPFILE"   ;;
        'C') echo "#define PRINTCOUNT" >> "$TMPFILE"   ;;
        'p') echo "#define PARTIALS"   >> "$TMPFILE"   ;;
        '?')
            usage;
            exit 2
            ;;
    esac
done
#shift $(expr $OPTIND - 1) # remove options from positional parameters

# check errors
[ -z "$N" ] && echo "ERROR: No value for n" && usage && exit 2
[ -z "$R" ] && echo "ERROR: No value for r" && usage && exit 2
if ! [ "$COND" = "none" ] && ! [ "$COND" = "sf" ] && ! [ "$COND" = "tc" ]; then
    echo "ERROR: Condition must be none, tc, sf"
    usage
    exit 2
fi

# end temp header file
echo "#define COND cond$COND" >> "$TMPFILE"
echo "#endif /* CONSTS_H */"  >> "$TMPFILE"

# move to consts.h file
mv "$TMPFILE" "$CTSFILE"

# compile
eval "$GCCCMD"

# execute if compilation was correct
if [ "$?" = "0" ]; then
    eval "$EXEFILE"
fi
