#ifndef EMUL_H
#define EMUL_H
#include <stdint.h>
#include "SDL/SDL.h"

#define VX ((op&0x0F00)>>8)
#define VY ((op&0x00F0)>>4)
#define N (op&0x000F)
#define NN (op&0x00FF)
#define NNN (op&0x0FFF)
#define X 128
#define Y 64

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

#endif //EMUL_H
