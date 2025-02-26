#include "emm_vulkan.h"

VkResult initializeVulkanApp(VulkanApp *app) {
	VkResult res;

	// step one : create instance 

	VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = app->name,
		.pEngineName = app->name,
		.apiVersion = VK_API_VERSION_1_3
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
		// todo check layer availibility and only enable if they are availible
		instanceCreateInfo.enabledLayerCount = 1;
		instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
	}

	res = vkCreateInstance(&instanceCreateInfo, app->allocator, &app->instance);
	
	if (res != VK_SUCCESS) {
		return res;
	}

	if (app->validate && app->debugCallbackFunction) {
		// register the debug messenger function
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, 
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
			.pfnUserCallback = app->debugCallbackFunction 
		};

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
			(PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
			app->instance, "vkCreateDebugUtilsMessengerEXT");
		
		vkCreateDebugUtilsMessengerEXT(app->instance, &debugMessengerCreateInfo, 
									   app->allocator, &app->debugMessenger);
	}

	// step two : choose a physical device

	uint32 physicalDeviceCount;
	vkEnumeratePhysicalDevices(app->instance, &physicalDeviceCount, NULL);
	VkPhysicalDevice *physicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	if (physicalDevices == NULL) return VK_ERROR_OUT_OF_HOST_MEMORY;
	vkEnumeratePhysicalDevices(app->instance, &physicalDeviceCount, physicalDevices);
	
	// todo add real criteria
	app->physicalDevice = physicalDevices[0];
	free(physicalDevices);

	// step three : choose queue family

	// step four : create logical device

	

	return VK_SUCCESS;
}

void quitVulkanApp(VulkanApp *app) {
	if (app == NULL || app->instance == VK_NULL_HANDLE) return;

	if (app->debugMessenger != VK_NULL_HANDLE) {
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
			(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
			app->instance, "vkDestroyDebugUtilsMessengerEXT");
		vkDestroyDebugUtilsMessengerEXT(app->instance, app->debugMessenger, app->allocator);
	}
	vkDestroyInstance(app->instance, app->allocator);
}
