CC := cc
CFLAGS := -O3 -pipe -march=native

generate: generate.c genrandom.c
	${CC} ${CFLAGS} -o generate generate.c
	${CC} ${CFLAGS} -o genrandom genrandom.c
