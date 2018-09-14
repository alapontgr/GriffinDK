#include "ParameterBufferBase.h"
#include "ParameterDefinition.h"

namespace fdk
{
  namespace Rendering 
  {
    
    ParameterBufferBase::ParameterBufferBase()
      : m_framerate{EParameterSetFramerateType::Framerate_Scene}
    {
    }

    Memory::mem_ptr_t ParameterBufferBase::get_parameter_data(const ParameterDefinition& rParameter)
    {
      FDASSERT(m_pParametersData, "Data for this parameter buffer has not been allocated yet");
      return m_pParametersData + rParameter.m_offset;
    }

  }
}

