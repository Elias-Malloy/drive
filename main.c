#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#define PINK 0.97, 0.86, 0.89, 1.0 

int main(void) {
	// Startup SDL subsystems
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
		return -1;
	}
	
	// open main window
	SDL_Window *window = SDL_CreateWindow(
		"drive",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI
	);
	
	if (window == NULL) {
		SDL_Log("SDL window creation failed: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	// query the coordinate and pixel width of the main window
	int coordW, coordH, pixelW, pixelH;
	SDL_GetWindowSize(window, &coordW, &coordH);
	SDL_Vulkan_GetDrawableSize(window, &pixelW, &pixelH);
	
	SDL_Event e;
	while (1) {
		// process events
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					goto cleanup;	
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							goto cleanup;
					}
			}
		}
	}

cleanup:
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
