#include "emulation.h"

int _00CN(s_emu *emu, uint16_t opcode) 
{
  if (N(opcode) == 0)
    return;
  for (uint8_t i = SCHIP_Y_RES - 1; i >= N(opcode); --i)
    memcpy(emu->display[i], emu->display[i-N(opcode)], SCHIP_X_RES);
  for (uint8_t i = 0; i < N(opcode); ++i)
    memset(emu->display[i], 0, SCHIP_X_RES);
}

int _00FB(s_emu *emu, uint16_t opcode) 
{
  for (uint8_t i = 0; i < SCHIP_Y_RES; ++i) 
  {
    for (uint8_t j = SCHIP_X_RES - 1; j >= 4; --j)
      emu->display[i][j] = emu->display[i][j-4];
    memset(emu->display[i], 0, 4);
  }
}

int _00FC(s_emu *emu, uint16_t opcode) 
{
  for (uint8_t i = 0; i < SCHIP_Y_RES; ++i)
  {
    for (uint8_t j = 0; j <= SCHIP_X_RES - 4 - 1; ++j)
      emu->display[i][j] = emu->display[i][j+4];
    memset(emu->display[i] + SCHIP_X_RES - 4, 0, 4);
  }
}

int _00FD(s_emu *emu, uint16_t opcode)
{
  return -1;
}

int _00FE(s_emu *emu, uint16_t opcode)
{
  emu->extended = 0;
}

int _00FF(s_emu *emu, uint16_t opcode)
{
  emu->extended = 1;
}

int _DXY0(s_emu *emu, uint16_t opcode)
{
  uint8_t x = emu->registers[VX(opcode)];
  uint8_t y = emu->registers[VY(opcode)];
  emu->registers[0xF] = 0;
  for (uint8_t i = 0; i < 16; i+=2)
    for (uint8_t j = 0; j < 8; ++j) {
      //takes j bit from mem[rI+i]
      //example: j=2 from 0b11(0)10101 => 0 (taken bit in brackets)
      int t = (emu->mem[emu->rI + i] & (1 << (7 - j))) >> (7 - j);
      //check for collision
      if (emu->display[i+y][j+x] && emu->display[i+y][j+x] & t)
        emu->registers[0xF] = 1;
      emu->display[i+y][j+x] ^= t;

      t = (emu->mem[emu->rI + i+1] & (1 << (7 - j))) >> (7 - j);
      //check for collision
      if (emu->display[i+y][8+j+x] && emu->display[i+y][8+j+x] & t)
        emu->registers[0xF] = 1;
      emu->display[i+y][8+j+x] ^= t;
    }
  emu->display_changed = 1;
}

int _FX30(s_emu *emu, uint16_t opcode)
{
  emu->rI = emu->registers[VX(opcode)] * 10 + 0;
}

int _FX75(s_emu *emu, uint16_t opcode)
{
  memcpy(emu->mem + emu->rI, emu->schip_reg, VX(opcode)+1);
}

int _FX85(s_emu *emu, uint16_t opcode)
{
  memcpy(emu->schip_reg, emu->mem + emu->rI, VX(opcode)+1);
}
