#include "intdef.h"
#include "emm_sdl_vulkan.h"

int main(void) {
	SDLVulkanApp app = { 
		.vkapp = {
			.name = "catan",
			.validate = 1,
		}	
	};
	uint32 result = startupSDLVulkanApp(&app);
	if (result == 0) {
		mainloopSDLVulkanApp(&app);
	}
	quitSDLVulkanApp(&app);
	return result;
}
