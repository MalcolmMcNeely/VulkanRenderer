#include "Renderer.h"
#include "../Shader/Shader.h"

using namespace std;

namespace renderer {
   void Renderer::Initialise(GLFWwindow* pWindow)
   {
      window = pWindow;
   }

   void Renderer::InitialiseVulkan()
   {

   }

   void Renderer::CreateInstance()
   {
      // Providing details is technically optional, but driver
      // may be provided with useful optimisation info
      VkApplicationInfo appInfo = {};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Hello Triangle";
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "No Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_0;

      // Get all available extensions
      uint32_t extensionCount = 0;
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
      vector<VkExtensionProperties> extensions(extensionCount);
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

      // Get required extensions to interface with our window system
      vector<const char*> requiredExtensions = GetRequiredExtensions();

      // Check that required extension is one of the available extensions
      {
         VkExtensionProperties tempLastElement = { "temp", 0 };
         extensions.push_back(tempLastElement);

         for (const auto& requiredExtension : requiredExtensions)
         {
            auto results = find_if(extensions.begin(), extensions.end(),
               [requiredExtension](const VkExtensionProperties& extension)
               {
                  return strcmp(extension.extensionName, requiredExtension) == 0;
               });

            if (strcmp(results[0].extensionName, tempLastElement.extensionName) == 0)
            {
               throw runtime_error("Required extensions are not supported");
            }
         }

         extensions.pop_back();
      }

      // Create Vulkan Instance
      VkInstanceCreateInfo createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;
      createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
      createInfo.ppEnabledExtensionNames = requiredExtensions.data();

      if (_enableValidationLayers)
      {
         createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
         createInfo.ppEnabledLayerNames = _validationLayers.data();
      }
      else
      {
         createInfo.enabledLayerCount = 0;
      }

      if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
      {
         throw runtime_error("Failed to create Vulkan instance");
      }
   }

   vector<const char*> Renderer::GetRequiredExtensions()
   {
      // Setup the Vulkan extensions required by this application
      uint32_t glfwExtensionCount = 0;
      const char** glfwExtensions;
      glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

      vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

      if (_enableValidationLayers)
      {
         extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
      }

      return extensions;
   }
}