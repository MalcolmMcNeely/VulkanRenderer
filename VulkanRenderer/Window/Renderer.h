#pragma once
#include "../Common/Common.h"

#include <vector>

namespace renderer {

   class Renderer {
   public:
      void Initialise(GLFWwindow* pWindow);


   private:
      void InitialiseVulkan();
      void CreateInstance();
		std::vector<const char*> GetRequiredExtensions();

      GLFWwindow* window;
      VkInstance instance;

		// Validation
		const std::vector<const char*> _validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

#ifdef NDEBUG
		const bool _enableValidationLayers = false;
#else
		const bool _enableValidationLayers = true;
#endif

		// Extensions
		const std::vector<const char*> _deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkDebugReportCallbackEXT _debugCallback;
   };

}