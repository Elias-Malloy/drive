#include "emm_vulkan.h"

VkResult initializeVulkanApp(VulkanApp *app) {
	VkResult res;

	// step one : create instance ( if you're scared pretend this is in its own function )

	VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = app->name,
		.pEngineName = app->name,
		.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0)
	};

	VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &applicationInfo,
		.enabledExtensionCount = app->extensionCount,
		.ppEnabledExtensionNames = app->extensionNames
	};
	const char *validationLayerNames[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	if (app->validate) {
		instanceCreateInfo.enabledLayerCount = 1;
		instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
	}

	res = vkCreateInstance(&instanceCreateInfo, app->allocator, &app->instance);
	
	if (res != VK_SUCCESS) {
		return res;
	}

	// step two : 


	return VK_SUCCESS;
}
