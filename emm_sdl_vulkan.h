#ifndef EMM_SDL_VULKAN
#define EMM_SDL_VULKAN

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "emm_vulkan.h"
#include "intdef.h"

typedef struct SDLVulkanApp {
	SDL_Window *window;
	VulkanApp vkapp;
} SDLVulkanApp;

uint32 startupSDLVulkanApp(SDLVulkanApp *app);
void mainloopSDLVulkanApp(SDLVulkanApp *app);
void quitSDLVulkanApp(SDLVulkanApp *app);


#endif
