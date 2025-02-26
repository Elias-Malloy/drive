#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include "intdef.h"

#include "emm_vulkan.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    SDL_Log("Validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

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

	VulkanApp app = {
		.name = "drive",
		.validate = 1,
		.debugCallbackFunction = debugCallback
	};

	SDL_Vulkan_GetInstanceExtensions(window, &app.extensionCount, NULL);
	app.extensionNames = malloc((app.extensionCount + 2) * sizeof(char *));
	SDL_Vulkan_GetInstanceExtensions(window, &app.extensionCount, app.extensionNames);	
	app.extensionNames[app.extensionCount++] = "VK_KHR_portability_enumeration";
	app.extensionNames[app.extensionCount++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; 

	uint32 res = initializeVulkanApp(&app);
	if (res != 0) SDL_Log("Vulkan App init failed with code: %d\n", res);

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
		// render
	}


cleanup:
	free(app.extensionNames);
	quitVulkanApp(&app);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
