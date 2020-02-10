#pragma once
#include "../Common/Common.h"

namespace renderer {
   class RenderWindow {
   public:
      void Destroy();
      GLFWwindow* Get();
      
      int Width() { return windowWidth; };
      int Height() { return windowHeight; };

   private:
      void Initialise();

      const int windowWidth = 800;
      const int windowHeight = 600;
      const char* pWindowTitle = "Vulkan Triangle";
      GLFWwindow* pWindow;
   };
}