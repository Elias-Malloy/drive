#include "emm_vulkan.h"

VkResult initializeVulkanApp(VulkanApp *app) {
	VkResult res;

	// step one : create instance 

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
		// todo check layer availibility and only enable if they are availible
		instanceCreateInfo.enabledLayerCount = 1;
		instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
	}

	res = vkCreateInstance(&instanceCreateInfo, app->allocator, &app->instance);
	
	if (res != VK_SUCCESS) {
		return res;
	}

	if (app->validate && app->debugCallback) {
		
		VkDebugReportCallbackCreateInfoEXT debugCallbackCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
			.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | 
					 VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | 
					 VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT,
			.pfnCallback = app->debugCallbackFunction 
		};
		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = 
			(PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(app->instance, 
			"vkCreateDebugReportCallbackEXT");

		vkCreateDebugReportCallbackEXT(app->instance, &debugCallbackCreateInfo, 
									   app->allocator, &app->debugCallback);
	}

	// step two : 


	return VK_SUCCESS;
}

void quitVulkanApp(VulkanApp *app) {
	if (app == NULL || app->instance == VK_NULL_HANDLE) return;

	if (app->debugCallback != VK_NULL_HANDLE) {
		PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = 
			(PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(app->instance, 
			"vkDestroyDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT(app->instance, app->debugCallback, app->allocator);
	}
	vkDestroyInstance(app->instance, app->allocator);
}
