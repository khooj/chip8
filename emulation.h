#ifndef EMUL_H
#define EMUL_H
#include <stdint.h>
#include "SDL/SDL.h"

#define VX(x) (((x)&0x0F00)>>8)
#define VY(x) (((x)&0x00F0)>>4)
#define N(x) ((x)&0x000F)
#define NN(x) ((x)&0x00FF)
#define NNN(x) ((x)&0x0FFF)
#define X 64
#define Y 32

typedef struct
{
	uint8_t display[Y][X];
	uint8_t mem[4096];
	uint16_t stack[16];
	uint8_t registers[16];
	uint8_t schip_reg[7];
	uint16_t rI;
	uint8_t SP;
	uint16_t MC;
	uint8_t delay_timer;
	uint8_t sound_timer;
	int display_changed;
	uint8_t key_pressed[16];
	const char keys[16];
	int extended;
} s_emu;

// emulation
int init(s_emu *emu);
int load_file(s_emu *emu, const char *path);
void handle_key(s_emu *emu, SDL_Event ev);
uint8_t key_pressed(s_emu *emu);
uint16_t get_opcode(s_emu *emu);
int execute_opcode(s_emu *emu, uint16_t op);
int step(s_emu *emu);
void clear_screen(s_emu *emu);
void draw_opcode(s_emu *emu, uint16_t op);

//stack
int push(s_emu *emu, uint16_t val);
uint16_t pop(s_emu *emu);

//chip opcodes
int _00E0(s_emu *emu, uint16_t opcode);
int _00EE(s_emu *emu, uint16_t opcode);
int _0NNN(s_emu *emu, uint16_t opcode);
int _1NNN(s_emu *emu, uint16_t opcode);
int _2NNN(s_emu *emu, uint16_t opcode);
int _3XKK(s_emu *emu, uint16_t opcode);
int _4XKK(s_emu *emu, uint16_t opcode);
int _5XY0(s_emu *emu, uint16_t opcode);
int _6XKK(s_emu *emu, uint16_t opcode);
int _7XKK(s_emu *emu, uint16_t opcode);
int _8XY0(s_emu *emu, uint16_t opcode);
int _8XY1(s_emu *emu, uint16_t opcode);
int _8XY2(s_emu *emu, uint16_t opcode);
int _8XY3(s_emu *emu, uint16_t opcode);
int _8XY4(s_emu *emu, uint16_t opcode);
int _8XY5(s_emu *emu, uint16_t opcode);
int _8XY6(s_emu *emu, uint16_t opcode);
int _8XY7(s_emu *emu, uint16_t opcode);
int _8XYE(s_emu *emu, uint16_t opcode);
int _9XY0(s_emu *emu, uint16_t opcode);
int _ANNN(s_emu *emu, uint16_t opcode);
int _BNNN(s_emu *emu, uint16_t opcode);
int _CXKK(s_emu *emu, uint16_t opcode);
int _DXYN(s_emu *emu, uint16_t opcode);
int _EX9E(s_emu *emu, uint16_t opcode);
int _EXA1(s_emu *emu, uint16_t opcode);
int _FX07(s_emu *emu, uint16_t opcode);
int _FX0A(s_emu *emu, uint16_t opcode);
int _FX15(s_emu *emu, uint16_t opcode);
int _FX18(s_emu *emu, uint16_t opcode);
int _FX1E(s_emu *emu, uint16_t opcode);
int _FX29(s_emu *emu, uint16_t opcode);
int _FX33(s_emu *emu, uint16_t opcode);
int _FX55(s_emu *emu, uint16_t opcode);
int _FX65(s_emu *emu, uint16_t opcode);

//schip opcodes
int _00CN(s_emu *emu, uint16_t opcode);
int _00FB(s_emu *emu, uint16_t opcode);
int _00FD(s_emu *emu, uint16_t opcode);
int _00FE(s_emu *emu, uint16_t opcode);
int _00FF(s_emu *emu, uint16_t opcode);
int _DXY0(s_emu *emu, uint16_t opcode);
int _FX30(s_emu *emu, uint16_t opcode);
int _FX75(s_emu *emu, uint16_t opcode);
int _FX85(s_emu *emu, uint16_t opcode);

#endif //EMUL_H
