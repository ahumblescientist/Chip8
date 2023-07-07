#include "cpu.h"
#include <stdlib.h>
#include <math.h>

CPU cpu;

void write(uint16_t addr, uint8_t b) {
	RAM_write(cpu.mem, addr, b);
}

uint8_t read(uint16_t addr) {
	return RAM_read(cpu.mem, addr);
}

void v_write(uint16_t addr, uint8_t b) {
	VIDEO_write(cpu.mem, addr, b);
}

uint8_t v_read(uint16_t addr) {
	return VIDEO_read(cpu.mem, addr);
}

void fetch() {
	cpu.opcode = cpu.pc++;
}

void CPU_init(char *ROM, uint16_t size) {
	srandom(random());
	cpu.mem = malloc(sizeof(Memory));
	RAM_init(cpu.mem);
	cpu.pc = 0x200;
	for(uint16_t i=0;i<size;i++) {
		write(0x200+i, ROM[i]);
	}
}


#define getnnn() (cpu.opcode & 0x0fff)
#define getx() ( (cpu.opcode & 0x0f00) >> 8)
#define gety() ( ( cpu.opcode & 0x00f0) >> 4)
#define getkk() (uint8_t)(cpu.opcode & 0x00ff)

void I00E0() {
	for(uint16_t i=0;i<VIDEO_SIZE;i++) {
		v_write(i, 0);
	}
}

void I00EE() {
	cpu.pc = cpu.stack[cpu.sp];
	cpu.sp--;
}

void I1nnn() {
	cpu.pc = getnnn();
}

void I2nnn() {
	cpu.stack[cpu.sp] = cpu.pc;
	cpu.pc = getnnn();
}

void I3xkk() {
	if(cpu.regs[getx()] == getkk()) {
		cpu.pc += 2;
	}
}

void I4xkk() {
	if(cpu.regs[getx()] != getkk()) {
		cpu.pc += 2;
	}
}

void I5xy0() {
	if(cpu.regs[getx()] == cpu.regs[gety()]) {
		cpu.pc+=2;
	}
}

void I6xkk() {
	cpu.regs[getx()] = getkk();
}

void I7xkk() {
	cpu.regs[getx()] += getkk();
}

void I8xy0() {
	cpu.regs[getx()] = cpu.regs[gety()];
}

void I8xy1() {
	cpu.regs[getx()] |= cpu.regs[gety()];
}

void I8xy2() {
	cpu.regs[getx()] &= cpu.regs[gety()];
}

void I8xy3() {
	cpu.regs[getx()] ^= cpu.regs[gety()];
}

void I8xy4() {
	if(( (uint16_t)cpu.regs[getx()] + (uint16_t)cpu.regs[gety()]) > 255) {
		cpu.regs[0x0F] = 1;
	} else {
		cpu.regs[0x0F] = 0;
	}
	cpu.regs[getx()] += cpu.regs[gety()];
}

void I8xy5() {
	if(cpu.regs[getx()] > cpu.regs[gety()]) {
		cpu.regs[0x0F] = 1;
	} else {
		cpu.regs[0x0F] = 0;
	}
	cpu.regs[getx()] -= cpu.regs[gety()];
}

void I8xy6() {
	if(cpu.regs[getx()] & 0x01) {
		cpu.regs[0x0F] = 1;
	} else {
		cpu.regs[0x0F] = 0;
	}
	cpu.regs[getx()] >>= 1;
}

void I8xy7() {
	if(cpu.regs[gety()] > cpu.regs[getx()]) {
		cpu.regs[0x0F] = 1;
	} else {
		cpu.regs[0x0F] = 0;
	}
	cpu.regs[getx()] = cpu.regs[gety()] - cpu.regs[getx()];
}

void I8xyE() {
	if(cpu.regs[getx()] & 0x01) {
		cpu.regs[0x0F] = 1;
	} else {
		cpu.regs[0x0F] = 0;
	}
	cpu.regs[getx()] <<= 1;
}

void I9xy0() {
	if(cpu.regs[getx()] != cpu.regs[gety()]) {
		cpu.pc+=2;
	}
}

void IAnnn() {
	cpu.i = getnnn();
}

void IBnnn() {
	cpu.pc = (uint16_t)(cpu.regs[0]) + (getnnn());
}

void ICxkk() {
	cpu.regs[getx()] = rand() & getkk();
}

void Dxyn() {
	uint16_t xpos = cpu.regs[getx()] % 64;
	uint16_t ypos = cpu.regs[gety()] % 32;
	uint16_t n = cpu.opcode & 0x000f;
	cpu.regs[0x0F] = 0;
	for(uint16_t I=0; I<n; I++) {
			uint8_t sprite = read(cpu.i + I);
			uint16_t index = ypos * 32 + xpos;
			for(uint16_t shift=0;shift<8;shift++) {
				uint8_t bit = sprite & (128 >> shift);
				uint8_t *vbit = &(cpu.mem->VIDEO[index]);
				if(( *vbit & (128 >> shift) ) == 1 && ( ( (*vbit & (128 >> shift)) ^ bit) == 0) ) {
					cpu.regs[0x0F] = 0;
				}
				*vbit ^= bit;
			}
	}
}

void IEx9E() {
	// TODO
}

void IExA1() {
	cpu.pc+=2;
}

void IFx07() {
	cpu.regs[getx()] = cpu.DT;
}

void IFx0A() {
	// TODO
}

void IFx15() {
	cpu.DT = cpu.regs[getx()];
}

void IFx18() {
	// TODO
}

void IFx1E() {
	cpu.i += (uint16_t)(cpu.regs[getx()]);
}

void IFx29() {
	cpu.i = cpu.regs[getx()] * 5;
}

// basically subtracting the digit until its zero and finding how many times subtraction occures
void IFx33() {
	uint8_t value = cpu.regs[getx()];
	uint8_t hs=0, ts=0, s=0;
	while(value >= 100) {
		value -= 100;
		hs++;
	}
	write(cpu.i+0, hs);
	while(value >= 10) {
		value -= 10;
		ts++;
	}
	write(cpu.i+1, ts);
	while(value >= 0) {
		value -= 1;	
		s++;
	}
	write(cpu.i+2, s);
}

void IFx55() {
	for(uint16_t i=0;i<= getx();i++) {
		write(cpu.i + i, cpu.regs[i]);
	}
}

void IFx65() {
	for(uint16_t i=0;i<= getx();i++) {
		cpu.regs[i] = read(cpu.i + i);
	}
}
