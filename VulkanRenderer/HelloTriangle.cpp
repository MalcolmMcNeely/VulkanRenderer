#include "HelloTriangle.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <algorithm>
#include <map>

#include "ValidationCallbacks.h"

using namespace std;

HelloTriangle::HelloTriangle()
{
}

HelloTriangle::~HelloTriangle()
{
}

void HelloTriangle::Run()
{
	InitialiseWindow();
	InitialiseVulkan();
	MainLoop();
	CleanUp();
}

void HelloTriangle::InitialiseWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Do not create an OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable window resizing

	_window = glfwCreateWindow(_windowWidth, _windowHeight, _windowTitle, nullptr, nullptr);
}

void HelloTriangle::InitialiseVulkan()
{
	CreateInstance();
	SetupDebugCallback();
}

void HelloTriangle::CleanUp()
{
	if (_enableValidationLayers)
	{
		ValidationCallbacks::DestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
	}

	vkDestroyInstance(_instance, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void HelloTriangle::MainLoop()
{
	while (!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();
	}
}

void HelloTriangle::CreateInstance()
{
	if (_enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw runtime_error("Validation layers requested, but not available");
	}

	// Providing details is technically optional, but driver
	// may be provided with useful optimisation info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Get all available extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Output available extensions
	cout << "Available extensions:" << endl;
	for (const auto& extension : extensions)
	{
		cout << "\t" << extension.extensionName << endl;
	}

	// Get required extensions to interface with our window system
	vector<const char*> requiredExtensions = GetRequiredExtensions();

	// Check that required extension is one of the available extensions
	{
		VkExtensionProperties tempLastElement = { "temp", 0 };
		extensions.push_back(tempLastElement);

		for (const auto &requiredExtension : requiredExtensions)
		{
			auto results = find_if(extensions.begin(), extensions.end(),
				[requiredExtension](const VkExtensionProperties& extension)
			{
				return strcmp(extension.extensionName, requiredExtension) == 0;
			});

			if (strcmp(results[0].extensionName, tempLastElement.extensionName) == 0)
			{
				throw runtime_error("Required extension not supported");
			}
		}

		extensions.pop_back();
	}

	// Create Vulkan Instance
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	if (_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
		createInfo.ppEnabledLayerNames = _validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
	{
		throw runtime_error("Failed to create Vulkan instance");
	}
}

bool HelloTriangle::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Check if all layers in validation layers exist in the 
	// availabe layers
	for (const char* layerName : _validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> HelloTriangle::GetRequiredExtensions()
{
	// Setup the Vulkan extensions required by this application
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (_enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

void HelloTriangle::SetupDebugCallback()
{
	if (!_enableValidationLayers) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = ValidationCallbacks::DebugCallback;

	if (ValidationCallbacks::CreateDebugReportCallbackEXT(_instance, &createInfo, nullptr, &_debugCallback) != VK_SUCCESS)
	{
		throw runtime_error("Failed to setup debug callback");
	}
}

void HelloTriangle::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw runtime_error("Failed to find GPUs with Vulkan support");
	}

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

#pragma region Rate devices by suitability

	//// Use an ordered map to automatically sort candidates by increasing score
	//std::multimap<int, VkPhysicalDevice> candidates;

	//for (const auto& device : devices) 
	//{
	//	int score = RateDeviceSuitability(device);
	//	candidates.insert(std::make_pair(score, device));
	//}

	//// Check if the best candidate is suitable at all
	//if (candidates.rbegin()->first > 0) 
	//{
	//	_physicalDevice = candidates.rbegin()->second;
	//}
	//else 
	//{
	//	throw std::runtime_error("Failed to find a suitable GPU");
	//}

#pragma endregion

	for (const auto& device : devices)
	{
		if (IsPhysicalDeviceSuitable(device))
		{
			_physicalDevice = device;
			break;
		}
	}

	if (_physicalDevice == VK_NULL_HANDLE)
	{
		throw runtime_error("Failed to find a suitable GPU");
	}
}

bool HelloTriangle::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
{
	// Get device information
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	
	// Ensure the device has the correct queue family
	QueueFamilyIndices indices = FindQueueFamilies(device);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			 deviceFeatures.geometryShader &&
			 indices.IsComplete();
}

//int HelloTriangle::RateDeviceSuitability(VkPhysicalDevice device)
//{
//	// Get device information
//	VkPhysicalDeviceProperties deviceProperties;
//	vkGetPhysicalDeviceProperties(device, &deviceProperties);
//	VkPhysicalDeviceFeatures deviceFeatures;
//	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
//
//	int score = 0;
//
//	// Discrete GPUs have a significant performance advantage
//	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
//	{
//		score += 1000;
//	}
//
//	// Maximum possible size of textures affects graphics quality
//	score += deviceProperties.limits.maxImageDimension2D;
//
//	// Application can't function without geometry shaders
//	if (!deviceFeatures.geometryShader) 
//	{
//		return 0;
//	}
//
//	return score;
//}

QueueFamilyIndices HelloTriangle::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.GraphicsFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}