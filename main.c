#include "cpu.h"
#include "ram.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

#define WIDTH 640
#define HEIGHT 320


int init() {
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Failed to inti SDL2: %s", SDL_GetError());
		return 1;
	}
	window = SDL_CreateWindow("Chip-8", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(window == NULL) {
		printf("Failed to create SDL2 window: %s", SDL_GetError());
		return 1;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	return 0;
}

void destroy() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_Quit();
}

void proccess() {
	SDL_UpdateTexture(texture, NULL, videoMem(), sizeof(uint32_t) * 64);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int input(uint8_t *keys) {
	SDL_Event e;
	int quit = 0;
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
			case SDL_QUIT:
				quit = 1;
				return quit;
				break;
			case SDL_KEYDOWN: {
				switch(e.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = 1;
						break;
					case SDLK_1:
						keys[0] = 1;
						break;
					case SDLK_2:
						keys[1] = 1;
						break;
					case SDLK_3:
						keys[2] = 1;
						break;
					case SDLK_4:
						keys[3] = 1;
						break;
					case SDLK_q:
						keys[4] = 1;
						break;
					case SDLK_w:
						keys[5] = 1;
						break;
					case SDLK_e:
						keys[6] = 1;
						break;
					case SDLK_r:
						keys[7] = 1;
						break;
					case SDLK_a:
						keys[8] = 1;
						break;
					case SDLK_s:
						keys[9] = 1;
						break;
					case SDLK_d:
						keys[10] = 1;
						break;
					case SDLK_f:
						keys[11] = 1;
						break;
					case SDLK_z:
						keys[12] = 1;
						break;
					case SDLK_x:
						keys[13] = 1;
						break;
					case SDLK_c:
						keys[14] = 1;
						break;
					case SDLK_v:
						keys[15] = 1;
						break;
				}
			}break;
			case SDL_KEYUP: {
				switch(e.key.keysym.sym) {
					case SDLK_1:
						keys[0] = 0;
						break;
					case SDLK_2:
						keys[1] = 0;
						break;
					case SDLK_3:
						keys[2] = 0;
						break;
					case SDLK_4:
						keys[3] = 0;
						break;
					case SDLK_q:
						keys[4] = 0;
						break;
					case SDLK_w:
						keys[5] = 0;
						break;
					case SDLK_e:
						keys[6] = 0;
						break;
					case SDLK_r:
						keys[7] = 0;
						break;
					case SDLK_a:
						keys[8] = 0;
						break;
					case SDLK_s:
						keys[9] = 0;
						break;
					case SDLK_d:
						keys[10] = 0;
						break;
					case SDLK_f:
						keys[11] = 0;
						break;
					case SDLK_z:
						keys[12] = 0;
						break;
					case SDLK_x:
						keys[13] = 0;
						break;
					case SDLK_c:
						keys[14] = 0;
						break;
					case SDLK_v:
						keys[15] = 0;
						break;
				}
			} break;
		}
	}
	return quit;
}


int main(int argc, char **argv) {
	if(init()!=0) {
		return 1;
	}
	FILE *ptr;

	char *filename = argv[1];
	if((ptr = fopen(filename, "r")) == NULL){ 
		printf("Error: cant open file: %s", filename);
		return 1;
	}
	fseek(ptr, 0l, SEEK_END);	
	size_t filesize = ftell(ptr);
	rewind(ptr);
	char *ROM = malloc(filesize);
	fread(ROM, 1, filesize, ptr);
	CPU_init(ROM, filesize);
	int quit = 0;
	while(!quit) {
		uint64_t start = SDL_GetPerformanceCounter();
		cycle();
		proccess();
		quit = input(getkeys());
		uint64_t end = SDL_GetPerformanceCounter();
		float seconds = (end - start) / (float)(SDL_GetPerformanceFrequency()) * 1000.0f;
		SDL_Delay(floor(8.3333f - seconds));
	}
	destroy();
	return 0;
}
