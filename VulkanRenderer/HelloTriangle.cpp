#include "HelloTriangle.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <set>

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
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateGraphicsPipeline();
}

void HelloTriangle::CleanUp()
{
	for (auto imageView : _swapChainImageViews)
	{
		vkDestroyImageView(_device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(_device, _swapChain, nullptr);
	vkDestroyDevice(_device, nullptr);

	if (_enableValidationLayers)
	{
		ValidationCallbacks::DestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
	}

	vkDestroySurfaceKHR(_instance, _surface, nullptr);
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

	// Get the first suitable device
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
	// Ensure the device has the correct queue family
	QueueFamilyIndices indices = FindQueueFamilies(device);

	// Check extensions
	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	// Check swap chain
	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() &&
								  !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && 
			 extensionsSupported && 
		    swapChainAdequate;

	// Get device information
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
	//		 deviceFeatures.geometryShader &&
	//		 indices.IsComplete();
}

bool HelloTriangle::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// "Cross-out" the names of extensions which we require
	set<string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());
	for (const auto& extension : availableExtensions) 
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
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
		// Check queue
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		// Check presentation
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentationSupport);
		if (queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

void HelloTriangle::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);

	// Specify queue infos
	vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
	set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Specify device features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// Logical device creation
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();	
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

	if (_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
		createInfo.ppEnabledLayerNames = _validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
	{
		throw runtime_error("Failed to create logical device");
	}

	// Get handles for queue
	vkGetDeviceQueue(_device, indices.graphicsFamily, 0, &_graphicsQueue);
	vkGetDeviceQueue(_device, indices.presentFamily, 0, &_presentationQueue);
}

void HelloTriangle::CreateSurface()
{
	if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS)
	{
		throw runtime_error("Failed to create window surface");
	}
}

void HelloTriangle::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice);
	
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	// A value of 0 for maxImageCount means that there is no limit besides memory requirements
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		 imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	// Amount of layers each image consists of. Always 1 unless doing stereoscopic 3D
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;		// Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
	{
		throw runtime_error("Failed to create swap chain");
	}

	// Get swap chain images
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
	_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

	// Cache swap chain member variables
	_swapChainImageFormat = surfaceFormat.format;
	_swapChainExtent = extent;
}

SwapChainSupportDetails HelloTriangle::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	// Get capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

	// Get formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

	if (formatCount != 0) 
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
	}

	// Get presentation modes
	uint32_t presentationModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentationModeCount, nullptr);

	if (presentationModeCount != 0)
	{
		details.presentModes.resize(presentationModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentationModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR HelloTriangle::ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats)
{
	// If surface has no preferred format
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	// Find our preferred format
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR HelloTriangle::ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D HelloTriangle::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// Max limit indicates that the window manager will let us render
	// at a resolution that differs from the resolution of the window
	if (capabilities.currentExtent.width != numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}

	VkExtent2D actualExtent = { (uint32_t)_windowWidth, (uint32_t)_windowHeight };
	actualExtent.width = max(capabilities.minImageExtent.width, 
		min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = max(capabilities.minImageExtent.height, 
		min(capabilities.maxImageExtent.height, actualExtent.height));
	return actualExtent;
}

void HelloTriangle::CreateImageViews()
{
	_swapChainImageViews.resize(_swapChainImages.size());

	for (size_t i = 0; i < _swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
		{
			throw runtime_error("Failed to create image view");
		}
	}
}

void HelloTriangle::CreateGraphicsPipeline()
{
   auto vertexShaderCode = _shaderHelper.ReadFile("Shaders/vert.spv");
   auto fragmentShaderCode = _shaderHelper.ReadFile("Shaders/frag.spv");

   VkShaderModule vertexShaderModule = _shaderHelper.CreateShaderModule(_device, vertexShaderCode);
   VkShaderModule fragmentShaderModule = _shaderHelper.CreateShaderModule(_device, fragmentShaderCode);

   VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
   vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
   vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
   vertexShaderStageInfo.module = vertexShaderModule;
   vertexShaderStageInfo.pName = "main";

   VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
   fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
   fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
   fragmentShaderStageInfo.module = fragmentShaderModule;
   fragmentShaderStageInfo.pName = "main";

   VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

   vkDestroyShaderModule(_device, vertexShaderModule, nullptr);
   vkDestroyShaderModule(_device, fragmentShaderModule, nullptr);
}