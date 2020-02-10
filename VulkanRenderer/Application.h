#pragma once
#include "Window/RenderWindow.h"

using namespace renderer;

namespace application {

   class Application {
   public:
      void Initialise();
      void MainLoop();
      void Destroy();

   private:

      RenderWindow* pWindow;
   };
}