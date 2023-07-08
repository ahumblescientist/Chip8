#ifndef RAM_H
#define RAM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define VIDEO_SIZE 64*32
#define RAM_SIZE 0x1000

typedef struct {
	uint8_t RAM[RAM_SIZE];
	uint32_t VIDEO[VIDEO_SIZE]; 
} Memory;

void RAM_init(Memory *);
void RAM_write(Memory*, uint16_t, uint8_t);
uint8_t RAM_read(Memory*, uint16_t);
void VIDEO_write(Memory*, uint16_t, uint32_t);
uint32_t VIDEO_read(Memory*, uint16_t);

#endif
