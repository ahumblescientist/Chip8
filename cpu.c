#include "cpu.h"
#include <stdlib.h>
#include <math.h>

CPU cpu;

void execute();

void write(uint16_t addr, uint8_t b) {
	RAM_write(&cpu.mem, addr, b);
}

uint8_t read(uint16_t addr) {
	return RAM_read(&cpu.mem, addr);
}

void v_write(uint16_t addr, uint32_t b) {
	VIDEO_write(&cpu.mem, addr, b);
}

uint32_t v_read(uint16_t addr) {
	return VIDEO_read(&cpu.mem, addr);
}

uint32_t *videoMem() {
	return (cpu.mem.VIDEO);
}

void CPU_init(char *ROM, size_t size) {
	srandom(random());
	cpu.pc = 0x200;
	for(uint16_t i=0;i<size;i++) {
		write(0x200+i, ROM[i]);
	}
}

uint8_t *getkeys() {
	return cpu.keys;
}


void cycle() {
	cpu.opcode = (read(cpu.pc) << 8) | (read(cpu.pc+1));
	cpu.pc+=2;
	execute();
	if(cpu.DT > 0) {
		cpu.DT--;
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

void IDxyn() {
	uint16_t xpos = cpu.regs[getx()] % 64;
	uint16_t ypos = cpu.regs[gety()] % 32;
	uint16_t n = cpu.opcode & 0x000f;
	cpu.regs[0x0F] = 0;
	for(uint16_t I=0; I<n; I++) {
			uint8_t sprite = read(cpu.i + I);
			for(uint16_t shift=0;shift<8;shift++) {
				uint16_t index = (ypos + I) * 64 + xpos + shift;
				uint8_t bit = sprite & (128 >> shift);
				uint32_t *vbit = &(cpu.mem.VIDEO[index]);
				if(bit) {
					if(*vbit) {
						cpu.regs[0x0F] = 1;
					}
					*vbit ^= 0xFFFFFFFF;
				}
			}
	}
}

void IEx9E() {
	if(cpu.keys[cpu.regs[getx()]]) {
		cpu.pc+=2;
	}
}

void IExA1() {
	if(!cpu.keys[cpu.regs[getx()]]) {
		cpu.pc+=2;
	}
}

void IFx07() {
	cpu.regs[getx()] = cpu.DT;
}

void IFx0A() {
	int isPressed = 0;
	for(int i=0;i<16;i++) {
		if(cpu.keys[i]) {
			cpu.regs[getx()] = cpu.keys[i];
			isPressed = 1;
		}
	}
	if(!isPressed) {
		cpu.pc-=2;
	}
}

void IFx15() {
	cpu.DT = cpu.regs[getx()];
}

void IFx18() {
	// i DONT give a shit about sound timer
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
	while(value > 0) {
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

void execute() {
#define MSD() ( (cpu.opcode & 0xF000) >> 12)
#define LSD() (cpu.opcode & 0x000F)

	if(cpu.opcode == 0x00E0) {
		I00E0();
	}
	if(cpu.opcode == 0x00EE) {
		I00EE();
	}
	switch(MSD()) {
		case 1: I1nnn();break;
		case 2: I2nnn();break;
		case 3: I3xkk();break;
		case 4: I4xkk();break;
		case 5: I5xy0();break;
		case 6: I6xkk();break;
		case 7: I7xkk();break;
		case 8: {
			switch(LSD()) {
				case 0: I8xy0(); break;
				case 1: I8xy1(); break;
				case 2: I8xy2(); break;
				case 3: I8xy3(); break;
				case 4: I8xy4(); break;
				case 5: I8xy5(); break;
				case 6: I8xy6(); break;
				case 7: I8xy7(); break;
				case 0xE: I8xyE(); break;
				default: break;
			} break; 
		}
		case 9: I9xy0(); break;
		case 0xA: IAnnn(); break;
		case 0xB: IBnnn(); break;
		case 0xC: ICxkk(); break;
		case 0xD: IDxyn(); break;
		case 0xE: {
			switch(LSD()) {
				case 0xE: IEx9E(); break;
				case 1: IExA1(); break;
			}
		} break;
		case 0xF: {
			switch((cpu.opcode & 0x00FF)) {
				case 0x07: IFx07();break;
				case 0x0A: IFx0A();break;
				case 0x15: IFx15();break;
				case 0x18: IFx18();break;
				case 0x1E: IFx1E();break;
				case 0x29: IFx29();break;
				case 0x33: IFx33();break;
				case 0x55: IFx55();break;
				case 0x65: IFx65();break;
			}
		} break;
		default: break;
	}
}
