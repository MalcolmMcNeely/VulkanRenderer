#pragma once
#include <fstream>
#include <vector>

#include "Common.h"

class ShaderHelper
{
public:
   ShaderHelper();
   ~ShaderHelper();

   std::vector<char> ReadFile(const std::string& filename);
   VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code);
};

