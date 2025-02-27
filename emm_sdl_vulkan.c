#include "emm_sdl_vulkan.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    SDL_Log("Validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

uint32 startupSDLVulkanApp(SDLVulkanApp *app) {
	if (app == NULL) return 1;

	// Startup SDL subsystems
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
		return -1;
	}
	
	// open main window
	app->window = SDL_CreateWindow(
		app->vkapp.name,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI
	);
	
	if (app->window == NULL) {
		SDL_Log("SDL window creation failed: %s\n", SDL_GetError());
		return -1;
	}

	if (app->vkapp.validate) {
		app->vkapp.debugCallbackFunction = debugCallback;
	}
	
	SDL_Vulkan_GetInstanceExtensions(app->window, &app->vkapp.extensionCount, NULL);
	app->vkapp.extensionNames = malloc((app->vkapp.extensionCount + 2) * sizeof(char *));
	if (app->vkapp.extensionNames == NULL) return 1;
	SDL_Vulkan_GetInstanceExtensions(app->window, &app->vkapp.extensionCount, app->vkapp.extensionNames);	
	app->vkapp.extensionNames[app->vkapp.extensionCount++] = "VK_KHR_portability_enumeration";
	app->vkapp.extensionNames[app->vkapp.extensionCount++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; 

	app->vkapp.deviceExtensionCount = 1;
	app->vkapp.deviceExtensionNames = malloc(sizeof(const char *) * app->vkapp.deviceExtensionCount);
	if (app->vkapp.deviceExtensionNames == NULL) return 1;
	app->vkapp.deviceExtensionNames[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	uint32 res = initializeVulkanApp(&app->vkapp);
	
	free(app->vkapp.deviceExtensionNames);
	free(app->vkapp.extensionNames);
	
	if (res != 0) {
		SDL_Log("Vulkan App init failed with code: %d\n", res);
		return 1;
	}	

	if (SDL_Vulkan_CreateSurface(app->window, app->vkapp.instance, &app->vkapp.surface) == SDL_FALSE) {
		SDL_Log("SDL window creation failed: %s\n", SDL_GetError());	
		return 1;
	}

	return 0;
}

void mainloopSDLVulkanApp(SDLVulkanApp *app) {
	SDL_Event e;
	while (1) {
		// process events
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					return;	
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							return;
					}
			}
		}
		// render
	}
}

void quitSDLVulkanApp(SDLVulkanApp *app) {
	quitVulkanApp(&app->vkapp);
	if (app->window) {
		SDL_DestroyWindow(app->window);
	}
	SDL_Quit();
}
