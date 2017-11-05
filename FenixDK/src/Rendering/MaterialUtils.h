#pragma once
#include "Framework\VertexLayout.h"
#include "RenderConfiguration.h"
#include "RenderPass.h"

#include <string>

namespace fdk
{
  namespace Rendering 
  {
    struct MaterialDesc
    {
      Framework::VertexLayout* m_pVertexLayout;
      BlendState* m_pBlendState;
      RasterState* m_pRasterState;
      MultiSampleState* m_pMultiSamplingState;
      RenderPass* m_pRenderPass; // Render pass where this material will be used
                                 // Stages paths
      std::string m_vsPath;
      std::string m_psPath;
      // ...
    };

    enum EParameterSetFramerateType : u32
    {
      Framerate_Scene = 0,
      Framerate_Material = 1,
      Framerate_Instance = 2,
      Framerate_Count = 3
    };
  }
}