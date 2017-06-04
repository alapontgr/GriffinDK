#pragma once
#include "Utilities/platform.h"

#include "Utilities/VulkanInclude.h"
#include "VulkanAutoDeleter.h"

#include <utility>

namespace dk
{
  namespace Rendering 
  {
    class Context;

    class Shader 
    {
    public:
      using ShaderHandler = AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>;
      Shader();
      ~Shader();

      Shader(Shader&& shader) : m_shaderHandler{ std::move(shader.m_shaderHandler) } {}

      void load(const Context& context, PATH path /*TODO: Use Allocator*/);
      
      void release();
      
      VkShaderModule get() { return m_shaderHandler.get(); }
    private:
      ShaderHandler m_shaderHandler;
    };

  }
}