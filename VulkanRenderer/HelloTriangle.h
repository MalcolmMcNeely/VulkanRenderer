#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

	// Window variables
	const int _windowWidth = 800;
	const int _windowHeight = 600;
	const char* _windowTitle = "Vulkan Triangle";
	GLFWwindow* _window;

	// Vulkan variables
	VkInstance _instance;


};

