#pragma once
#include <vector>

#include "Common.h"
#include "ShaderHelper.h"

struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool IsComplete()
	{
		return graphicsFamily >= 0 && 
			    presentFamily >= 0;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangle
{

public:

	HelloTriangle();
	~HelloTriangle();

	void Run();

private:

	void InitialiseWindow();
	void InitialiseVulkan();
	void CleanUp();
	void MainLoop();

	void CreateInstance();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugCallback();
	void PickPhysicalDevice();
	bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	//int RateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateGraphicsPipeline();

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	// Window variables
	const int _windowWidth = 800;
	const int _windowHeight = 600;
	const char* _windowTitle = "Vulkan Triangle";
	GLFWwindow* _window;

	// Vulkan variables
	VkInstance _instance;

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

	// Devices
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	VkDevice _device;

	VkQueue _graphicsQueue;
	VkQueue _presentationQueue;

	VkSurfaceKHR _surface;

	VkSwapchainKHR _swapChain;
	std::vector<VkImage> _swapChainImages;
	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	std::vector<VkImageView> _swapChainImageViews;

   // Shaders
   ShaderHelper _shaderHelper;

};

