CFILES = main.c ram.c cpu.c
HFILES = Makefile ram.h cpu.h
FILES = $(CFILES) $(HFILES)
CC = clang
OUT = bin/main
FLAGS = -ggdb3 -O0 -lm -o $(OUT)


main: $(FILES)
	$(CC) $(CFILES) $(FLAGS)
