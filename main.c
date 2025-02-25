#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define PINK 0.97, 0.86, 0.89, 1.0 

int main(void) {
	// Startup SDL subsystems
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
		return -1;
	}
	
	// request opengl version 4.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);	

	// enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	

	// open main window
	SDL_Window *window = SDL_CreateWindow(
		"drive",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
	);
	
	if (window == NULL) {
		SDL_Log("SDL window creation failed: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	// query the coordinate and pixel width of the main window
	int coordW, coordH, pixelW, pixelH;
	SDL_GetWindowSize(window, &coordW, &coordH);
	SDL_GL_GetDrawableSize(window, &pixelW, &pixelH);

	// create opengl context for main window
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		SDL_Log("GL context creation failed: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	// load opengl procedure addresses using glad with the SDL loader
	gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);	

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
		// set background color
		glClearColor(PINK);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// swap buffers	
		SDL_GL_SwapWindow(window);
	}

cleanup:
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
