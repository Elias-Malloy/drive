#include "emm_vulkan.h"

VkResult createInstance(VulkanApp *app) {
	VkResult res;

	VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = app->name,
		.pEngineName = app->name,
		.apiVersion = VK_API_VERSION_1_3
	};

	VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
		.pApplicationInfo = &applicationInfo,
		.enabledExtensionCount = app->extensionCount,
		.ppEnabledExtensionNames = app->extensionNames
	};
	const char *validationLayerNames[] = {
		"VK_LAYER_KHRONOS_validation"
	};
	
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, 
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
					   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = app->debugCallbackFunction 
	};

	if (app->validate) {
		// todo check layer availibility and only enable if they are availible
		instanceCreateInfo.enabledLayerCount = 1;
		instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
		if (app->debugCallbackFunction) {
			instanceCreateInfo.pNext = &debugMessengerCreateInfo;
		}
	}

	res = vkCreateInstance(&instanceCreateInfo, app->allocator, &app->instance);
	
	if (res != VK_SUCCESS) {
		return res;
	}
	
	if (app->validate && app->debugCallbackFunction) {
		// register the debug messenger function

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
			(PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
			app->instance, "vkCreateDebugUtilsMessengerEXT");
		if (vkCreateDebugUtilsMessengerEXT == NULL) return VK_ERROR_EXTENSION_NOT_PRESENT;	
		vkCreateDebugUtilsMessengerEXT(app->instance, &debugMessengerCreateInfo, 
									   app->allocator, &app->debugMessenger);
	}

	return VK_SUCCESS;
}

VkResult selectPhysicalDevice(VulkanApp *app) {
	VkResult res;
	uint32 physicalDeviceCount;
	vkEnumeratePhysicalDevices(app->instance, &physicalDeviceCount, NULL);
	VkPhysicalDevice *physicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	if (physicalDevices == NULL) return VK_ERROR_OUT_OF_HOST_MEMORY;
	vkEnumeratePhysicalDevices(app->instance, &physicalDeviceCount, physicalDevices);

	uint32 queueFamilyCount = 0;
	VkQueueFamilyProperties *queueFamilyProperties;

	for (uint32 i = 0; i < physicalDeviceCount; i++) {
		// query queue family properties
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, NULL);
		queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
		if (queueFamilyProperties == NULL) return VK_ERROR_OUT_OF_HOST_MEMORY;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, 
			queueFamilyProperties);
		bool deviceSupportsRequiredQueueOperations = 0;
		
		app->presentQueueFamily = INVALID_INDEX;
		app->graphicsQueueFamily = INVALID_INDEX;

		for (uint32 j = 0; j < queueFamilyCount; j++) {
			VkBool32 surfaceSupported = VK_FALSE;
			if (app->surface != VK_NULL_HANDLE) {
				res = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], j, app->surface, &surfaceSupport);
				if (res != VK_SUCCESS) surfaceSupported = VK_FALSE;
			}

			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				// the user wants as many queues as possible but no specific number
				
				// does the queue family contain at lease the requested number of queues
				if (app->queueCount == 0 || app->queueCount >= queueFamilyProperties[i].queueCount) {
					if (app->queueCount == 0) app->queueCount = queueFamilyProperties[i].queueCount;
					queueFamilyIndex = j;
					deviceSupportsRequiredQueueOperations = 1;
					break;
				}
			}
		}
		free(queueFamilyProperties);
		if (deviceSupportsRequiredQueueOperations) {
			// check for requested device extension support
			uint32 deviceExtensionCount = 0;
			vkEnumerateDeviceExtensionProperties(physicalDevices[i], NULL, &deviceExtensionCount, NULL);
			if (deviceExtensionCount > 0) {
				VkExtensionProperties *deviceExtensions = malloc(
					sizeof(VkExtensionProperties) * deviceExtensionCount);
				if (deviceExtensions != NULL) {
					vkEnumerateDeviceExtensionProperties(
						physicalDevices[i], NULL, &deviceExtensionCount, deviceExtensions);
					bool allRequestedExtensionsSupported = 1;
					for (uint32 j = 0; j < app->deviceExtensionCount; j++) {
						bool extensionIsSupported = 0;
						for (uint32 k = 0; k < deviceExtensionCount; k++) {
							if (strcmp(app->deviceExtensionNames[j], deviceExtensions[k].extensionName) == 0) {
								extensionIsSupported = 1;
								break;
							}
						}
						if (!extensionIsSupported) {
							allRequestedExtensionsSupported = 0;
							break;
						}
					}
					if (allRequestedExtensionsSupported) {
						app->physicalDevice = physicalDevices[i];
					}
					free(deviceExtensions);
				}
			} 
		}
		// if a suitable device has been found, stop searching
		if (app->physicalDevice != VK_NULL_HANDLE) break;
	}

	free(physicalDevices);
	
	// when a suitible device is found, it will be assigned to app->physicalDevice	
	if (app->physicalDevice == VK_NULL_HANDLE) return VK_ERROR_FEATURE_NOT_PRESENT;
	
	return VK_SUCCESS;
}	

VkResult createLogicalDevice(VulkanApp *app) {
	float *graphicsQueuePriorities = malloc(sizeof(float) * app->graphicsQueueCount);
	if (graphicsQueuePriorities == NULL) return VK_ERROR_OUT_OF_HOST_MEMORY;
	float *presentQueuePriorities = malloc(sizeof(float) * app->presentQueueCount);
	if (presentQueuePriorities == NULL) return VK_ERROR_OUT_OF_HOST_MEMORY;
	
	for (uint32 i = 0; i < app->graphicsQueueCount; i++) {
		graphicsQueuePriorities[i] = 1.0f / app->graphicsQueueCount;
	}
	for (uint32 i = 0; i < app->presentQueueCount; i++) {
		presentQueuePriorities[i] = 1.0f / app->presentQueueCount;
	}

	VkDeviceQueueCreateInfo deviceQueueCreateInfos[2] = {
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = app->graphicsQueueFamilyIndex,
			.queueCount = app->graphicsQueueCount,
			.pQueuePriorities = graphicsQueuePriorities
		},
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = app->presentQueueFamilyIndex,
			.queueCount = app->presentQueueCount,
			.pQueuePriorities = presentQueuePriorities
		}
	};

	VkDeviceCreateInfo deviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 2,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = app->deviceExtensionCount,
		.ppEnabledExtensionNames = app->deviceExtensionNames,
	};
	
	res = vkCreateDevice(app->physicalDevice, &deviceCreateInfo, app->allocator, &app->device);

	free(graphicsQueuePriorities);
	free(presentQueuePriorities);

	return res;
}
/*
VkResult createSwapChain(VulkanApp *app) {
	
}
*/

void quitVulkanApp(VulkanApp *app) {
	if (app == NULL || app->instance == VK_NULL_HANDLE) return;

	if (app->debugMessenger != VK_NULL_HANDLE) {
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
			(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
			app->instance, "vkDestroyDebugUtilsMessengerEXT");
		if (vkDestroyDebugUtilsMessengerEXT != NULL)
			vkDestroyDebugUtilsMessengerEXT(app->instance, app->debugMessenger, app->allocator);
	}
	if (app->surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(app->instance, app->surface, app->allocator);
	}

	if (app->device != VK_NULL_HANDLE) {
		vkDestroyDevice(app->device, app->allocator);
	}
	vkDestroyInstance(app->instance, app->allocator);
}
