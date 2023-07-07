#ifndef CPU_H
#define CPU_H

#include "ram.h"

#define START_ADDRESS 0x200
#define STACK_SIZE 0x10
#define REGS_SIZE 0x10

typedef struct {
	uint8_t regs[16];
	uint8_t delay;
	uint16_t i;
	uint8_t DT;
	uint16_t pc;
	uint8_t sp;
	uint16_t stack[16];
	uint16_t opcode;
	Memory *mem;
} CPU;

void initCPU();

#endif
