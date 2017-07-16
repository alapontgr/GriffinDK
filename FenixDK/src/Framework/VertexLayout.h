#pragma once
#include "Utilities\types.h"
#include <vector>
#include "Utilities\GeneralDefinitions.h"

namespace fdk
{
  namespace Framework 
  {
    class VertexLayout 
    {
    public:

      struct VertexParameter 
      {
        u32 m_type;
        u32 m_size;
        u32 m_offset;
      };

      VertexLayout();

      void add_parameter(EVertexParameterType parameterType);

      void clear();

      u32 parameter_count() const;

    private:

      std::vector<VertexParameter> m_parameters;
    };
  }
}