#include <stdio.h>
#include <stdint.h>
#include "SDL/SDL.h"
#include "emulation.h"

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x;
    *p = pixel;
}

int main(int argc, char *argv[])
{
	s_emu *emu;
	SDL_Surface *screen;
	SDL_Event ev;
	int _exit = 0;

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(SCHIP_X_RES, SCHIP_Y_RES, 8, SDL_SWSURFACE);
	SDL_EnableUNICODE(1);
	emu = (s_emu*)malloc(sizeof(s_emu));
	init(emu);
	open_file(emu, argv[1]);
	Uint32 pxl = SDL_MapRGB(screen->format, 0, 255, 0);
	while (!_exit) {
		if (SDL_PollEvent(&ev)) {
			switch (ev.type) {
				case SDL_QUIT: _exit = 1; break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					if (ev.key.keysym.sym == SDLK_ESCAPE)
					{
						_exit = 1;
						break;
					}
					handle_key(emu, ev);
					break;
			}
		}
		if (step(emu) < 0)
			break;
		if (!emu->display_changed)
			continue;
		if (SDL_MUSTLOCK(screen))
			if (SDL_LockSurface(screen) < 0) {
				printf("Cant lock screen");
				return -1;
			}
		if (emu->extended) {
			for (uint8_t y = 0; y < SCHIP_Y_RES; ++y)
				for (uint8_t x = 0; x < SCHIP_X_RES; ++x)
					putpixel(screen, x, y, pxl * emu->display[y][x]);
		} else {
			for (uint8_t y = 0; y < CHIP_Y_RES; ++y)
				for (uint8_t x = 0; x < CHIP_X_RES; ++x) {
					putpixel(screen, x*2, y*2, pxl * emu->display[y][x]);
					putpixel(screen, x*2+1, y*2, pxl * emu->display[y][x]);
					putpixel(screen, x*2, y*2+1, pxl * emu->display[y][x]);
					putpixel(screen, x*2+1, y*2+1, pxl * emu->display[y][x]);
				}
		}
		if (SDL_MUSTLOCK(screen))
			SDL_UnlockSurface(screen);
		SDL_UpdateRect(screen,0,0,0,0);
		SDL_Delay(50);
	}
	SDL_Quit();
	free(emu);
	return 0;
}
