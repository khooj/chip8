#include "emulation.h"

int _00E0(s_emu *emu, uint16_t opcode)
{
  clear_screen(emu);
  emu->display_changed = 1;
}

int _00EE(s_emu *emu, uint16_t opcode)
{
  emu->MC = pop(emu);
}

int _0NNN(s_emu *emu, uint16_t opcode)
{
}

int _1NNN(s_emu *emu, uint16_t opcode)
{
  emu->MC = NNN(opcode) - 2;  
}

int _2NNN(s_emu *emu, uint16_t opcode)
{
  push(emu, emu->MC);
  emu->MC = NNN(opcode) - 2;
}

int _3XKK(s_emu *emu, uint16_t opcode)
{
  if (emu->registers[VX(opcode)] == NN(opcode))
    emu->MC += 2;
}

int _4XKK(s_emu *emu, uint16_t opcode)
{
  if (emu->registers[VX(opcode)] != NN(opcode))
    emu->MC += 2;
}

int _5XY0(s_emu *emu, uint16_t opcode)
{
  if (emu->registers[VX(opcode)] == emu->registers[VY(opcode)])
    emu->MC += 2;
}

int _6XKK(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] = NN(opcode);
}

int _7XKK(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] += NN(opcode);
}

int _8XY0(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] = emu->registers[VY(opcode)];
}

int _8XY1(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] |= emu->registers[VY(opcode)];
}

int _8XY2(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] &= emu->registers[VY(opcode)];
}

int _8XY3(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] ^= emu->registers[VY(opcode)];
}

int _8XY4(s_emu *emu, uint16_t opcode)
{
  emu->registers[0xF] = 0;
  int t = emu->registers[VX(opcode)] + emu->registers[VY(opcode)];
  if (t > 255)
    emu->registers[0xF] = 1;
  emu->registers[VX(opcode)] = (0xFF & t);
}

int _8XY5(s_emu *emu, uint16_t opcode)
{
  emu->registers[0xF] = 0;
  if (emu->registers[VX(opcode)] > emu->registers[VY(opcode)])
    emu->registers[0xF] = 1;
  emu->registers[VX(opcode)] -= emu->registers[VY(opcode)];
}

int _8XY6(s_emu *emu, uint16_t opcode) 
{
  emu->registers[0xF] = emu->registers[VX(opcode)] & 0x1;
  emu->registers[VX(opcode)] >>= 1;
}

int _8XY7(s_emu *emu, uint16_t opcode)
{
  emu->registers[0xF] = 0;
  if (emu->registers[VX(opcode)] < emu->registers[VY(opcode)])
    emu->registers[0xF] = 1;
  emu->registers[VX(opcode)] = emu->registers[VY(opcode)] - emu->registers[VX(opcode)];

}

int _8XYE(s_emu *emu, uint16_t opcode)
{
  emu->registers[0xF] = emu->registers[VX(opcode)] & 0x80;
  emu->registers[VX(opcode)] <<= 1;
}

int _9XY0(s_emu *emu, uint16_t opcode)
{
  if (emu->registers[VX(opcode)] != emu->registers[VY(opcode)])
    emu->MC += 2;
}

int _ANNN(s_emu *emu, uint16_t opcode)
{
  emu->rI = NNN(opcode);
}

int _BNNN(s_emu *emu, uint16_t opcode)
{
  emu->MC = NNN(opcode) + emu->registers[0x0];
}

int _CXKK(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] = ((rand() % 256) & NN(opcode));
}

int _DXYN(s_emu *emu, uint16_t opcode);
int _EX9E(s_emu *emu, uint16_t opcode);
int _EXA1(s_emu *emu, uint16_t opcode);
int _FX07(s_emu *emu, uint16_t opcode);
int _FX0A(s_emu *emu, uint16_t opcode);
int _FX15(s_emu *emu, uint16_t opcode);
int _FX18(s_emu *emu, uint16_t opcode);

int _FX1E(s_emu *emu, uint16_t opcode)
{
  emu->rI += emu->registers[VX(opcode)];
}

int _FX29(s_emu *emu, uint16_t opcode);
int _FX33(s_emu *emu, uint16_t opcode);
int _FX55(s_emu *emu, uint16_t opcode);
int _FX65(s_emu *emu, uint16_t opcode);
