#pragma once

#include "BufferUtils.h"
#include "ParameterDefinition.h"

namespace fdk
{
  namespace Rendering 
  {
    class Buffer;

    // TODO: Complete this class
    class ConstantBuffer 
    {
    public:

      ConstantBuffer();

      void initialize(const BufferChunk& rdataDescriptor);

      EShaderParameterType type() const { return Type_ConstantBuffer; }

      BufferChunk buffer_chunk() const { return m_gpuData; }

    private:

      BufferChunk m_gpuData;
    };

  }
}