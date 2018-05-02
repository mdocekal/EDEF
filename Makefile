# makefile
# EDEF BIN.
# Autor: xdocek09

CC=g++
CFLAGS= -pedantic -Wall -Wextra -std=c++11 -g -O2
PROGS=edef

all: $(PROGS)
.PHONY: doc all clean

%.o: src/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@ 

edef: Image.o Config.o CGP.o main.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:
	rm -f *.o $(PROGS)
