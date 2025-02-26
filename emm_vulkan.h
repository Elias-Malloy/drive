#ifndef EMM_VULKAN_H
#define EMM_VULKAN_H

#include <vulkan/vulkan.h>
#include <stdlib.h>
#include "intdef.h"

typedef struct VulkanApp {
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;	

	
	const char **extensionNames;
	uint32 extensionCount;

	PFN_vkDebugUtilsMessengerCallbackEXT debugCallbackFunction;
	VkDebugUtilsMessengerEXT debugMessenger;

	
	VkAllocationCallbacks *allocator;
	const char *name;
	bool validate;
} VulkanApp;

VkResult initializeVulkanApp(VulkanApp *app);
void quitVulkanApp(VulkanApp *app);

#endif
