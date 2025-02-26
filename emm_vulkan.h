#ifndef EMM_VULKAN_H
#define EMM_VULKAN_H

#include <vulkan/vulkan.h>
#include "intdef.h"

typedef struct VulkanApp {
	VkInstance instance;
	VkDevice device;	
	
	const char **extensionNames;
	uint32 extensionCount;
	
	VkAllocationCallbacks *allocator;
	const char *name;
	bool validate;
} VulkanApp;

VkResult initializeVulkanApp(VulkanApp *app);

#endif
