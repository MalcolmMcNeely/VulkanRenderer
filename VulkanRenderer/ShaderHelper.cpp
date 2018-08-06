#include "ShaderHelper.h"
#include <stdexcept>

using namespace std;

ShaderHelper::ShaderHelper()
{
}


ShaderHelper::~ShaderHelper()
{
}

vector<char> ShaderHelper::ReadFile(const string& filename)
{
   ifstream file(filename, ios::ate | ios::binary);

   if (!file.is_open())
   {
      throw runtime_error("Failed to open shader file");
   }

   size_t fileSize = (size_t)file.tellg();
   vector<char> buffer(fileSize);

   file.seekg(0);
   file.read(buffer.data(), fileSize);
   file.close();

   return buffer;
}

VkShaderModule ShaderHelper::CreateShaderModule(const VkDevice& device, const vector<char>& code)
{
   VkShaderModuleCreateInfo createInfo = {};
   createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   createInfo.codeSize = code.size();
   createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

   VkShaderModule shaderModule;

   if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
   {
      throw runtime_error("Failed to create shader module");
   }

   return shaderModule;
}
