#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "SDL/SDL.h"
#include "emulation.h"

int init(s_emu *emu) {
	uint8_t f8[80] = {0xF0,0x90,0x90,0x90,0xF0,0x20,0x60,0x20,0x20,
				0x70,0xF0,0x10,0xF0,0x80,0xF0,0xF0,0x10,0xF0,
				0x10,0xF0,0x90,0x90,0xF0,0x10,0x10,0xF0,0x80,
				0xF0,0x10,0xF0,0xF0,0x80,0xF0,0x90,0xF0,0xF0,
				0x10,0x20,0x40,0x40,0xF0,0x90,0xF0,0x90,0xF0,
				0xF0,0x90,0xF0,0x10,0xF0,0xF0,0x90,0xF0,0x90,
				0x90,0xE0,0x90,0xE0,0x90,0xE0,0xF0,0x80,0x80,
				0x80,0xF0,0xE0,0x90,0x90,0x90,0xE0,0xF0,0x80,
				0xF0,0x80,0xF0,0xF0,0x80,0xF0,0x80,0x80,0xF0,
				0x80,0xF0,0x80,0x80};
	uint8_t fs[160] = {//fish'n'chips chip8/schip emu copy-paste
/* 0 */	0x7c, 0xc6, 0xce, 0xde, 0xd6, 0xf6, 0xe6, 0xc6, 0x7c, 0x00,
/* 1 */	0x10, 0x30, 0xf0, 0x30, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x00,
/* 2 */	0x78, 0xcc, 0xcc, 0xc,  0x18, 0x30, 0x60, 0xcc, 0xfc, 0x00,
/* 3 */	0x78, 0xcc, 0x0c, 0x0c, 0x38, 0x0c, 0x0c, 0xcc, 0x78, 0x00,
/* 4 */	0x0c, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x0c, 0x1e, 0x00,
/* 5 */	0xfc, 0xc0, 0xc0, 0xc0, 0xf8, 0x0c, 0x0c, 0xcc, 0x78, 0x00,
/* 6 */	0x38, 0x60, 0xc0, 0xc0, 0xf8, 0xcc, 0xcc, 0xcc, 0x78, 0x00,
/* 7 */	0xfe, 0xc6, 0xc6, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x00,
/* 8 */	0x78, 0xcc, 0xcc, 0xec, 0x78, 0xdc, 0xcc, 0xcc, 0x78, 0x00,
/* 9 */	0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x18, 0x18, 0x30, 0x70, 0x00,
/* A */	0x30, 0x78, 0xcc, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0xcc, 0x00,
/* B */	0xfc, 0x66, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0xfc, 0x00,
/* C */	0x3c, 0x66, 0xc6, 0xc0, 0xc0, 0xc0, 0xc6, 0x66, 0x3c, 0x00,
/* D */	0xf8, 0x6c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00,
/* E */	0xfe, 0x62, 0x60, 0x64, 0x7c, 0x64, 0x60, 0x62, 0xfe, 0x00,
/* F */	0xfe, 0x66, 0x62, 0x64, 0x7c, 0x64, 0x60, 0x60, 0xf0, 0x00
};
	/*
	chip8		pc
	123c		1234
	456d		qwer
	789e		asdf
	a0bd		zxcv
	*/

	const char k[16] = "x123qweasdzc4rfv";
	memset(emu->display, 0, sizeof(emu->display));
	memset(emu->mem, 0, sizeof(emu->mem));
	memset(emu->stack, 0, sizeof(emu->stack));
	memset(emu->registers, 0, sizeof(emu->registers));
	memset(emu->schip_reg, 0, sizeof(emu->registers));
	memset(emu->key_pressed, 0, sizeof(emu->key_pressed));
	memcpy(emu->mem, fs, 160);
	memcpy(emu->mem + 160, f8, 80);
	emu->rI = 0x200;
	emu->MC = 0x200;
	emu->SP = 0;
	emu->delay_timer = 0;
	emu->sound_timer = 0;
	emu->display_changed = 0;
	emu->extended = 0;
	memcpy(emu->keys, k, 16);
	srand(time(NULL));
}

int open_file(s_emu *emu, const char *path) {
	FILE *pFile;
	uint16_t fSize;
	pFile = fopen(path, "rb");
	if (!pFile) return -1;
	fseek(pFile, 0, SEEK_END);
	fSize = ftell(pFile);
	rewind(pFile);
	fread(emu->mem + emu->MC, 1, fSize, pFile);
	fclose(pFile);
	return 0;
}

int push(s_emu *emu, uint16_t val) {
	if (emu->SP > 15) return -1;
	emu->stack[emu->SP] = val;
	emu->SP += 1;
	return 0;
}

uint16_t pop(s_emu *emu) {
	if (!emu->SP) return -1;
	emu->SP -= 1;
	return emu->stack[emu->SP];
}

void clear_screen(s_emu *emu) {
	memset(emu->display, 0, sizeof(emu->display));
}

void handle_key(s_emu *emu, SDL_Event ev) {
	switch (ev.type) {
		case SDL_KEYDOWN:
			for (uint8_t i = 0; i < 16; ++i)
				if (ev.key.keysym.unicode == emu->keys[i])
					emu->key_pressed[i] = 1;
			break;
		case SDL_KEYUP:
			memset(emu->key_pressed, 0, sizeof(emu->key_pressed));
			break;
	}
}

uint8_t key_pressed(s_emu *emu) {
	uint8_t i = 255;
	while (!emu->key_pressed[++i] & (i<16));
	return i;
}

uint16_t get_opcode(s_emu *emu) {
	return (emu->mem[emu->MC] << 8) | emu->mem[emu->MC+1];
}

int execute_opcode(s_emu *emu, uint16_t op) {
	//printf("Current opcode: %X\n", op);
	switch (op & 0xF000) {
		case 0x0000: {
			switch (op & 0x00FF) {
				case 0x00E0: return _00E0(emu, op); 
				case 0x00EE: return _00EE(emu, op); 
				case 0x00FB: return _00FB(emu, op); 
				case 0x00FC: return _00FC(emu, op); 
				case 0x00FD: return _00FD(emu, op); 
				case 0x00FE: return _00FE(emu, op);
				case 0x00FF: return _00FF(emu, op);
				default:
					if (VY(op) == 0x00C0) 
						return _00CN(emu, op);
			}
		}
		case 0x1000: return _1NNN(emu, op);
		case 0x2000: return _2NNN(emu, op);
		case 0x3000: return _3XKK(emu, op);
		case 0x4000: return _4XKK(emu, op);
		case 0x5000: return _5XY0(emu, op);
		case 0x6000: return _6XKK(emu, op);
		case 0x7000: return _7XKK(emu, op);
		case 0x8000: {
			switch (op & 0xF00F) {
				case 0x8000: return _8XY0(emu, op);
				case 0x8001: return _8XY1(emu, op);
				case 0x8002: return _8XY2(emu, op);
				case 0x8003: return _8XY3(emu, op);
				case 0x8004: return _8XY4(emu, op);
				case 0x8005: return _8XY5(emu, op);
				case 0x8006: return _8XY6(emu, op);
				case 0x8007: return _8XY7(emu, op);
				case 0x800E: return _8XYE(emu, op);
				}
			}
		case 0x9000: return _9XY0(emu, op);
		case 0xA000: return _ANNN(emu, op);
		case 0xB000: return _BNNN(emu, op);
		case 0xC000: return _CXKK(emu, op);
		case 0xD000: return _DXYN(emu, op);
		case 0xE000: {
			switch (op & 0xF0FF) {
				case 0xE09E: return _EX9E(emu, op);
				case 0xE0A1: return _EXA1(emu, op);
			}
		}
		case 0xF000:
			switch (op & 0xF0FF) {
				case 0xF007: return _FX07(emu, op);
				case 0xF00A: return _FX0A(emu, op);
				case 0xF015: return _FX15(emu, op);
				case 0xF018: return _FX18(emu, op);
				case 0xF01E: return _FX1E(emu, op);
				case 0xF029: return _FX29(emu, op);
				case 0xF033: return _FX33(emu, op);
				case 0xF055: return _FX55(emu, op);
				case 0xF065: return _FX65(emu, op);
				case 0xF030: return _FX30(emu, op);
				case 0xF075: return _FX75(emu, op);
				case 0xF085: return _FX85(emu, op);
			}
			break;
		default: printf("%s %X\n", "Unknown opcode ", op);
	}
}

int step(s_emu *emu) {
	emu->display_changed = 0;
	if (execute_opcode(emu, get_opcode(emu)) < 0)
		return -1;
	emu->MC += 2;
	if (emu->delay_timer > 0) emu->delay_timer -= 1;
	if (emu->sound_timer > 0) emu->sound_timer -= 1;
}
