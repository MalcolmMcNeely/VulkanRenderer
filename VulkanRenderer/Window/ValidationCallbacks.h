#pragma once
#include <vulkan/vulkan.h>
#include <iostream>

namespace window {

	class ValidationCallbacks
	{
	public:
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,				// Message type
			VkDebugReportObjectTypeEXT objType,	// Typoe of object that is the subject of the message
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData)								// Use to pass my own data to the callback
		{
			std::cerr << "validation layer: " << msg << std::endl;
			return VK_FALSE; // Returning true will abort the call with VK_ERROR_VALIDATION_FAILED_EXT error
		}

		static VkResult CreateDebugReportCallbackEXT(VkInstance instance,
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugReportCallbackEXT* pCallback)
		{
			auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

			if (func)
			{
				return func(instance, pCreateInfo, pAllocator, pCallback);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		static void DestroyDebugReportCallbackEXT(VkInstance instance,
			VkDebugReportCallbackEXT callback,
			const VkAllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

			if (func)
			{
				func(instance, callback, pAllocator);
			}
		}
	};
}
