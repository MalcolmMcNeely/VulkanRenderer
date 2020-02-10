#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "Application.h"

using namespace application;

int main() 
{
	Application app;
	int exitCode = EXIT_SUCCESS;

	app.Initialise();

	int n;

	try 
	{
		app.MainLoop();
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		std::cin >> n;

		
		exitCode = EXIT_FAILURE;
	}

	std::cin >> n;

	app.Destroy();

	return exitCode;
}