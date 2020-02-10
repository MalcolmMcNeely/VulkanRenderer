#include "RenderWindow.h"

namespace renderer {
   GLFWwindow* RenderWindow::Get()
   {
      if (!pWindow)
      {
         Initialise();
      }

      return pWindow;
   }

   void RenderWindow::Destroy()
   {
      glfwDestroyWindow(pWindow);
      glfwTerminate();
   }

   void RenderWindow::Initialise()
   {
      glfwInit();
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Do not create an OpenGL context
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable window resizing

      pWindow = glfwCreateWindow(windowWidth, windowHeight, pWindowTitle, nullptr, nullptr);
   }
}