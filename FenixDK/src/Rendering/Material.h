#pragma once
#include "Utilities\types.h"
#include "Containers\Mask.h"
#include "Framework\VertexLayout.h"

#include <string>
#include "RenderPass.h"
#include "RenderConfiguration.h"

namespace fdk
{
  namespace Rendering 
  {
    class Material 
    {
    
    public:

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

      ~Material();

      static Material* create();

      void init(const MaterialDesc& rDesc);

    protected:
      
      Material();
      
      MaterialDesc m_desc;
      Containers::Mask<u32> m_flags;
    };

  }
}