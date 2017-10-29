#include "MaterialParameterSet.h"

namespace fdk
{
namespace Rendering
{

  static inline u32 get_offset(const ParameterCollection& rParamaterList) 
  {
    u32 offset = 0;
    for (const auto& rParam : rParamaterList) 
    {
      offset += Memory::get_alignment_offset(offset, rParam.m_alignment); // Add alignment offset
      offset += rParam.m_size; // Add size
    }
    return offset;
  }

	MaterialParameterSet::MaterialParameterSet() {}

	void MaterialParameterSet::clear()
  {
    m_parameters.clear();
	}

  void MaterialParameterSet::add_parameter(const u32 bindingSlot, const Utilities::Name& rParamName, const EShaderParameterType paramType, const ShaderStageMask stages)
  {
    u32 offset = get_offset(m_parameters);
    ParameterDefinition parameter;
    parameter.m_paramName = rParamName;
    parameter.m_paramType = paramType;
    parameter.m_stages = stages;
    parameter.m_bindingSlot = bindingSlot;
    parameter.m_size = sizeof(u32*);
    parameter.m_alignment = alignof(u32*);
    u32 alignmentOffset = Memory::get_alignment_offset(offset, parameter.m_alignment);
    m_parameters.push_back(parameter);
    auto& rParam = m_parameters[m_parameters.size() - 1];
    rParam.m_offset = offset + alignmentOffset;
  }

  ParameterSlot MaterialParameterSet::get_parameter(const Utilities::Name& rParamName) const
  {
    for (const auto& rParam : m_parameters) 
    {
      if (rParam.m_paramName == rParamName) 
      {
        return {rParam.m_offset, rParam.m_size};
      }
    }
    return {0,0};
  }

  u32 MaterialParameterSet::total_size() const
  {
    auto& rParam = m_parameters[m_parameters.size() - 1];
    return rParam.m_offset + rParam.m_size;
  }

  void MaterialParameterSet::create()
  {
    BaseT::create();
  }

}
}
