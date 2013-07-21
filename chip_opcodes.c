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
  if (emu->registers[VX(opcode)] >= emu->registers[VY(opcode)])
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
  if (emu->registers[VX(opcode)] <= emu->registers[VY(opcode)])
    emu->registers[0xF] = 1;
  emu->registers[VX(opcode)] = emu->registers[VY(opcode)] - emu->registers[VX(opcode)];

}

int _8XYE(s_emu *emu, uint16_t opcode)
{
  emu->registers[0xF] = (emu->registers[VX(opcode)] & 0x80) >> 7;
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

int _DXYN(s_emu *emu, uint16_t opcode)
{
  if (N(opcode) == 0 && emu->extended)
    //schip
    return _DXY0(emu, opcode);

  uint8_t x = emu->registers[VX(opcode)];
  uint8_t y = emu->registers[VY(opcode)];
  emu->registers[0xF] = 0;
  for (uint8_t i = 0; i < N(opcode); ++i)
    for (uint8_t j = 0; j < 8; ++j) {
      //takes j bit from mem[rI+i]
      //example: j=2 from 0b11(0)10101 => 0 (taken bit in brackets)
      int t = (emu->mem[emu->rI + i] & (1 << (7 - j))) >> (7 - j);
      //check for collision
      if (emu->display[i+y][j+x] && emu->display[i+y][j+x] & t)
        emu->registers[0xF] = 1;
      emu->display[i+y][j+x] ^= t;
    }
  emu->display_changed = 1;
}

int _EX9E(s_emu *emu, uint16_t opcode)
{
  if (key_pressed(emu) == emu->registers[VX(opcode)])
    emu->MC += 2;
}

int _EXA1(s_emu *emu, uint16_t opcode)
{
  if (key_pressed(emu) != emu->registers[VX(opcode)])
    emu->MC += 2;
}

int _FX07(s_emu *emu, uint16_t opcode)
{
  emu->registers[VX(opcode)] = emu->delay_timer;
}

int _FX0A(s_emu *emu, uint16_t opcode)
{
  uint8_t t = key_pressed(emu);
  if (t < 16)
    emu->registers[VX(opcode)] = t;
  else
    emu->MC -= 2;
}

int _FX15(s_emu *emu, uint16_t opcode)
{
  emu->delay_timer = emu->registers[VX(opcode)];
}

int _FX18(s_emu *emu, uint16_t opcode)
{
  emu->sound_timer = emu->registers[VX(opcode)];
}

int _FX1E(s_emu *emu, uint16_t opcode)
{
  //undocumented feature used in Spacefight 2019! 
  emu->registers[0xF] = 0;
  int t = emu->rI + emu->registers[VX(opcode)];
  if (t > 4095)
    emu->registers[0xF] = 1;
  emu->rI += emu->registers[VX(opcode)];
}

int _FX29(s_emu *emu, uint16_t opcode)
{
  emu->rI = emu->registers[VX(opcode)] * 5 + 160;
}

int _FX33(s_emu *emu, uint16_t opcode)
{
  emu->mem[emu->rI] = emu->registers[VX(opcode)] / 100;
  emu->mem[emu->rI + 1] = (emu->registers[VX(opcode)] % 100) / 10;
  emu->mem[emu->rI + 2] = emu->registers[VX(opcode)] % 10;
}

int _FX55(s_emu *emu, uint16_t opcode)
{
  memcpy(emu->mem+emu->rI, emu->registers, VX(opcode)+1);
}

int _FX65(s_emu *emu, uint16_t opcode)
{
  memcpy(emu->registers, emu->mem + emu->rI, VX(opcode)+1);
}
