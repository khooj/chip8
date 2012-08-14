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

// DXYN* Show N-byte sprite from M(I) at coords (VX,VY), VF :=
// collision. If N=0 and extended mode, show 16x16 sprite.

void draw_opcode(s_emu *emu, uint16_t op) {//tl;dr draws a sprite
	uint8_t x = emu->registers[VX];
	uint8_t y = emu->registers[VY];
	emu->registers[0xF] = 0;
	if (N == 0 && emu->extended) {
		for (uint8_t i = 0; i < N; ++i)
			for (uint8_t j = 0; j < 16; ++j) {
				int t = (emu->mem[emu->rI + i + (uint8_t)(j / 8)] & (1 << (7 - j % 8))) >> (7 - j % 8);
				if (emu->display[i+y][j+x] && emu->display[i+y][j+x] ^ t)
					emu->registers[0xF] = 1;
				emu->display[i+y][j+x] ^= t;
			}
	} else {
		for (uint8_t i = 0; i < N; ++i)
			for (uint8_t j = 0; j < 8; ++j) {
				int t = (emu->mem[emu->rI + i] & (1 << (7 - j))) >> (7 - j);
				if (emu->display[i+y][j+x] && emu->display[i+y][j+x] ^ t)
					emu->registers[0xF] = 1;
				emu->display[i+y][j+x] ^= t;
			}
	}
	emu->display_changed = 1;
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
// 	00CN* Scroll display N lines down
// 00FB* Scroll display 4 pixels right
// 00FC* Scroll display 4 pixels left
// 00FD* Exit CHIP interpreter
// 00FE* Disable extended screen mode
// 00FF* Enable extended screen mode for full-screen graphics
// DXYN* Show N-byte sprite from M(I) at coords (VX,VY), VF :=
// collision. If N=0 and extended mode, show 16x16 sprite.

// FX30* Point I to 10-byte font sprite for digit VX (0..9)
// FX75* Store V0..VX in RPL user flags (X <= 7)
// FX85* Read V0..VX from RPL user flags (X <= 7)
	switch (op & 0xF000) {
		case 0x0000: {
			switch (op & 0x00FF) {
				case 0x00E0: //clearscreen
					clear_screen(emu);
					emu->display_changed = 1;
					break;
				case 0x00EE:
					emu->MC = pop(emu);
					break;
				case 0x00FB:
					for (uint8_t j = 0; j < Y; ++j)
						for (int i = X - 4 - 1; i >= 0; --i)
							emu->display[j][i+4] = emu->display[j][i];
					for (uint8_t j = 0; j < Y; ++j)
						for (uint8_t i = 0; i < 4; ++i)
							emu->display[j][i] = 0;
					emu->display_changed = 1;
					break;
				case 0x00FC:
					for (uint8_t j = 0; j < Y; ++j)
						for (uint8_t i = 4; i < X; ++i)
							emu->display[j][i-4] = emu->display[j][i];
					for (uint8_t j = 0; j < Y; ++j)
						for (uint8_t i = X - 1; i > X - 4 - 1; --i)
							emu->display[j][i] = 0;
					emu->display_changed = 1;
					break;
				case 0x00FD:
					return -1;
				case 0x00FE:
					emu->extended = 0;
					break;
				case 0x00FF:
					emu->extended = 1;
					break;
				default:
					if (VY == 0x00C0) { //case 0x00CN
						for (uint8_t j = Y - N - 1; j > 0; --j)
							for (uint8_t i = 0; i < X; ++i)
								emu->display[j+N][i] = emu->display[j][i];
						memset(emu->display, 0, X*N);
					}
					break;
			}
		}
		break;
		case 0x1000:
			emu->MC = NNN - 2;
			break;
		case 0x2000: //calls subroutine at NNN
			push(emu, emu->MC);
			emu->MC = NNN - 2;
			break;
		case 0x3000: //skips the next instruction if VX equals NN
			if (emu->registers[VX] == NN)
				emu->MC += 2;
			break;
		case 0x4000: //skips the next instruction if VX doesnt equals NN
			if (emu->registers[VX] != NN)
				emu->MC += 2;
			break;
		case 0x5000: //skips the next instruction if VX equals VY (5XY0)
			if (emu->registers[VX] == emu->registers[VY])
				emu->MC += 2;
			break;
		case 0x6000: //sets VX to NN
			emu->registers[VX] = NN;
			break;
		case 0x7000: //adds NN to VX
			emu->registers[VX] += NN;
			break;
		case 0x8000: {
			switch (op & 0xF00F) {
				case 0x8000: //VX = VY
					emu->registers[VX] = emu->registers[VY];
					break;
				case 0x8001: //VX = VX | VY
					emu->registers[VX] |= emu->registers[VY];
					break;
				case 0x8002: //VX = VX & VY
					emu->registers[VX] &= emu->registers[VY];
					break;
				case 0x8003: //VX = VX ^ VY
					emu->registers[VX] ^= emu->registers[VY];
					break;
				case 0x8004: {//VX + VY, if carry then VF = 1 else 0
					int t = emu->registers[VX] + emu->registers[VY];
					if (t > 255)
						emu->registers[0xF] = 1;
					else
						emu->registers[0xF] = 0;
				}
				break;
				case 0x8005: {//VX - VY, if borrow then VF = 0 else 1
					int t = emu->registers[VX] - emu->registers[VY];
					if (t < 0)
						emu->registers[0xF] = 0;
					else
						emu->registers[0xF] = 1;
				}
				break;
				case 0x8006: //VX >> 1, VF is set to the value of the least significant bit of VX before the shift
					emu->registers[0xF] = emu->registers[VX] & 0x1;
					emu->registers[VX] = emu->registers[VX] >> 1;
					break;
				case 0x8007: {//VX = VY - VXm VF = 0 if borrow else 1
					int t = emu->registers[VY] - emu->registers[VX];
					if (t < 0)
						emu->registers[0xF] = 0;
					else
						emu->registers[0xF] = 1;
					emu->registers[VX] = t;
				}
				break;
				case 0x800E: //VX << 1, VF is set like in 0x8006 but most significant bit
					emu->registers[0xF] = emu->registers[VX] & 0x80;
					emu->registers[VX] = emu->registers[VX] << 1;
					break;
				}
			}
			break;
		case 0x9000: // skips the next instruction if VX != VY
			if (emu->registers[VX] != emu->registers[VY])
				emu->MC += 2;
			break;
		case 0xA000: //sets rI to the NNN
			emu->rI = NNN;
			break;
		case 0xB000: //jumps to the NNN + V0
			emu->MC = NNN + emu->registers[0x0] - 2;
			break;
		case 0xC000: //sets VX to a random number & NN
			emu->registers[VX] = (rand() % 256) & (NN);
			break;
		case 0xD000:
			draw_opcode(emu, op);
			break;
		case 0xE000: {
			switch (op & 0xF0FF) {
				case 0xE09E: //skips the next instruction if key stored in VX is pressed(EX9E)
					if (key_pressed(emu) == emu->registers[VX])
						emu->MC += 2;
					break;
				case 0xE0A1:
					if (key_pressed(emu) != emu->registers[VX])
						emu->MC += 2;
					//skips the next instruction if key stored in Vx isnt pressed
			}
		}
		break;
		case 0xF000:
			switch (op & 0xF0FF) {
				case 0xF007: //VX = delay_timer
					emu->registers[VX] = emu->delay_timer;
					break;
				case 0xF00A: {//wait for key press then store it in VX (need implement)
					uint8_t t = key_pressed(emu);
					if (t != 16)
						emu->registers[VX] = t;
					else
						emu->MC -= 2;
				}
				break;
				case 0xF015: //delay_timer = VX
					emu->delay_timer = emu->registers[VX];
					break;
				case 0xF018: //sound_timer = VX
					emu->sound_timer = emu->registers[VX];
					break;
				case 0xF01E: //rI += VX
					emu->rI += emu->registers[VX];
					break;
				case 0xF029:
					emu->rI = 160 + 5 * emu->registers[VX];
					break;
				case 0xF033:
					emu->mem[emu->rI] = emu->registers[VX] / 100;
					emu->mem[emu->rI + 1] = (emu->registers[VX] % 100) / 10;
					emu->mem[emu->rI + 2] = emu->registers[VX] % 10;
					break;
				case 0xF055: //stores V0-VX in memory starting at rI
					for (uint8_t i = 0; i < 16; ++i)
						emu->mem[emu->rI + i] = emu->registers[i];
					break;
				case 0xF065: //fills V0-VX with values from memory starting from rI
					for (uint8_t i = 0; i < 16; ++i)
						emu->registers[i] = emu->mem[emu->rI + i];
					break;
					// FX30* Point I to 10-byte font sprite for digit VX (0..9)
					// FX75* Store V0..VX in RPL user flags (X <= 7)
					// FX85* Read V0..VX from RPL user flags (X <= 7)
				case 0xF030:
					emu->rI = emu->registers[VX]*10;
					break;
				case 0xF075:
					for (uint8_t i = 0; i < VX - 1; ++i)
						emu->schip_reg[i] = emu->registers[i];
					break;
				case 0xF085:
					for (uint8_t i = 0; i < VX - 1; ++i)
						emu->registers[i] = emu->schip_reg[i];
					break;
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
