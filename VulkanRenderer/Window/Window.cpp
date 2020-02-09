#include "Window.h"

namespace window {
   GLFWwindow* Window::Get()
   {
      if (!pWindow)
      {
         Initialise();
      }

      return pWindow;
   }

   void Window::Destroy()
   {
      glfwDestroyWindow(pWindow);
   }

   void Window::Initialise()
   {
      glfwInit();
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Do not create an OpenGL context
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable window resizing

      pWindow = glfwCreateWindow(windowWidth, windowHeight, pWindowTitle, nullptr, nullptr);
   }
}