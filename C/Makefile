include config.mk

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

# definitions in config.mk
CFLAGS = -DN=$(N) \
		-DMAXLENGTH=$(MAXLENGTH) \
		-DOUTPUT_FUNC=$(OUTPUT_FUNC) \
		-DOUTPUT_ALL=$(OUTPUT_ALL) \
		-DOUTPUT_TOTAL=$(OUTPUT_TOTAL) \
		-DCONDITION=$(CONDITION) \
		-DPARALLEL=$(PARALLEL) \
		-DPARALLEL_PRINT_FINISHED=$(PARALLEL_PRINT_FINISHED)

all: generate randgen

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(OBJ): globals.h config.mk

clean:
	rm -f $(OBJ)

generate: generate.o polynomial.o sequence.o functions.o
	$(CC) generate.o polynomial.o sequence.o functions.o -o $@ $(CFLAGS)

randgen: randgen.o polynomial.o sequence.o functions.o
	$(CC) randgen.o polynomial.o sequence.o functions.o  -o $@ $(CFLAGS)

.PHONY: all generate randgen
