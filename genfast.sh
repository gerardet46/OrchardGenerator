#!/usr/bin/env sh

# Shell: POSIX compliant
# Dependencies: A C compiler

SC="$0"                           # this script
DIR="."                           # directory of the C program
HELPFILE="$DIR/help.txt"          # help.txt file
GENFILE="$DIR/genfast.c"          # the C code
TMPFILE="$DIR/tmpconsts.h"        # the constants temporary C header
CTSFILE="$DIR/consts.h"           # the constants C header
EXEFILE="$DIR/genfast"            # the generator executable
GCCOPT="-O3 -pipe -march=native"  # flags for GCC
GCCCMD="cc $GCCOPT -o '$EXEFILE' '$GENFILE'" # the compiler command

help() {
    sed "s|%s|$SC|" "$HELPFILE"
}

usage() {
    echo "Usage: $SC [-n leaves] [-r reticulations] [-c none|tc|sf] [-hCSpm]"
}

# Transform long options to short ones
for arg in "$@"; do
    shift
    case "$arg" in
        '--help')           help; exit 0      ;;
        '--condition')      set -- "$@" '-c'   ;;
        '--print-sequence') set -- "$@" '-S'   ;;
        '--print-count')    set -- "$@" '-C'   ;;
        '--more-counts')    set -- "$@" '-m'   ;;
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
while getopts "hn:r:c:SCpm" opt
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
        'm') echo "#define MORECOUNTS" >> "$TMPFILE"   ;;
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
