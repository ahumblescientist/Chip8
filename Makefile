CFILES = main.c ram.c cpu.c
HFILES = Makefile ram.h cpu.h
FILES = $(CFILES) $(HFILES)
CC = gcc
OUT = bin/main
FLAGS = -ggdb3 -O0 -lm `sdl2-config --cflags --libs` -o $(OUT)


main: $(FILES)
	$(CC) $(CFILES) $(FLAGS)
