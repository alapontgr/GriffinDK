#pragma once
#include "Utilities\types.h"
#include "BufferUtils.h"

namespace fdk
{
  namespace Rendering 
  {
    class Texture2D;

    struct ConstantBufferParameter
    {
      u32 m_slot;
      BufferChunk m_dataChunk;
    };

    struct Texture2DParameter 
    {
      u32 m_slot;
      Texture2D* m_boundTexture;
    };
  }
}