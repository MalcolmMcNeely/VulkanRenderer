#include "Application.h"

#include <thread>
#include <chrono>

using namespace std;

namespace application {

   void Application::Initialise()
   {
      pWindow = new RenderWindow();
   }

   void Application::MainLoop()
   {
      while (!glfwWindowShouldClose(pWindow->Get()))
      {
         glfwPollEvents();

         this_thread::sleep_for(chrono::milliseconds(250));
      }
   }

   void Application::Destroy()
   {
      pWindow->Destroy();
   }
}