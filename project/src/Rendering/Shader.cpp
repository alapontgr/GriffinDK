#include "Shader.h"
#include "IO\FileUtils.h"
#include "context.h"
#include "Utilities\ClassHelpers.h"

namespace fdk
{
namespace Rendering
{
  
  Shader::Shader()
  {

  }

  Shader::~Shader()
  {

  }

  void Shader::load(const Context& context, PATH path /*TODO: Use Allocator*/)
  {
    IO::File source;
    source.load(path, 0, alignof(u32));
    CHECK(source.size() > 0, "Loaded empty file");

    VkShaderModuleCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.codeSize = source.size();
    info.pCode = reinterpret_cast<u32*>(source.data());

    VkShaderModule shader;
    auto result = vkCreateShaderModule(context.device(), &info, nullptr, &shader);
    VK_CHECK(result, "Failed to create shader");

    // Assign a new handler. If it existed previously it will be released automatically
    m_shaderHandler = ShaderHandler(shader, vkDestroyShaderModule, context.device());
    
    source.release();
  }

  void Shader::release()
  {
    m_shaderHandler.release();
  }

}
}
