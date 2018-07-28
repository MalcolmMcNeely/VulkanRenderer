#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

struct QueueFamilyIndices
{
	int GraphicsFamily = -1;

	bool IsComplete()
	{
		return GraphicsFamily >= 0;
	}
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
	//int RateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

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

	VkDebugReportCallbackEXT _debugCallback;

	// Device
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

};

