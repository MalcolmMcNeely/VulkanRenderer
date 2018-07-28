#include "HelloTriangle.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <algorithm>
#include <vector>

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
}

void HelloTriangle::CleanUp()
{
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
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Check that required extension is one of the available extensions
	{
		VkExtensionProperties tempLastElement = { "temp", 0 };
		extensions.push_back(tempLastElement);

		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			auto results = find_if(extensions.begin(), extensions.end(),
				[glfwExtensions, i](const VkExtensionProperties& extension)
			{
				return strcmp(extension.extensionName, glfwExtensions[i]) == 0;
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
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
	{
		throw runtime_error("Failed to create Vulkan instance");
	}
}
