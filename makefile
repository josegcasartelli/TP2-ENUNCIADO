VALGRIND_FLAGS=--leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes
CFLAGS = -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O2 -g
CC = gcc

all: tp2 pruebas_alumno

tp2: src/*.c tp2.c
	$(CC) $(CFLAGS) src/*.c tp2.c -o tp2

pruebas_alumno: src/*.c pruebas_alumno.c
	$(CC) $(CFLAGS) src/*.c pruebas_alumno.c -o pruebas_alumno

valgrind-alumno: pruebas_alumno
	valgrind $(VALGRIND_FLAGS) ./pruebas_alumno

valgrind-tp2: tp2
	valgrind $(VALGRIND_FLAGS) ./tp2

clean:
	rm -f tp2 pruebas_alumno
