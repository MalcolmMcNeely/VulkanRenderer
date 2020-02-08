#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "Window/HelloTriangle.h"

using namespace window;

int main() 
{
	HelloTriangle app;
	int n;

	try 
	{
		app.Run();
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		std::cin >> n;
		return EXIT_FAILURE;
	}

	std::cin >> n;
	return EXIT_SUCCESS;
}