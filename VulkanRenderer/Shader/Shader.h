#pragma once
#include <fstream>
#include <vector>

#include "../Common/Common.h"

namespace shader {
   class Shader {
   public:
      Shader();
      ~Shader();

      std::vector<char> ReadFile(const std::string& filename);
      VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code);
   };
}

