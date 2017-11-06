#include "MaterialParameterSet.h"

#include <algorithm>

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

  void MaterialParameterSet::add_parameter(
    const EParameterSetFramerateType slotType, 
    const u32 bindingSlot,
    const Utilities::Name& rParamName, 
    const EShaderParameterType paramType, 
    const ShaderStageMask stages)
  {
    //u32 offset = get_offset(m_parameters);
    ParameterDefinition parameter;
    parameter.m_paramName = rParamName;
    parameter.m_paramType = paramType;
    parameter.m_stages = stages;
    parameter.m_setSlot = slotType;
    parameter.m_bindingSlot = bindingSlot;
    parameter.m_size = sizeof(u32*);
    parameter.m_alignment = alignof(u32*);
    //u32 alignmentOffset = Memory::get_alignment_offset(offset, parameter.m_alignment);
    m_parameters.push_back(parameter);
    //auto& rParam = m_parameters[m_parameters.size() - 1];
    //rParam.m_offset = offset + alignmentOffset;
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
    // Order the parameters by set
    std::sort(m_parameters.begin(), m_parameters.end(), [](const ParameterDefinition& rLParam, const ParameterDefinition& rRParam)
    {
      return (rLParam.m_setSlot == rRParam.m_setSlot) ? 
        (rLParam.m_bindingSlot < rRParam.m_bindingSlot) : 
        (rLParam.m_setSlot < rRParam.m_setSlot);
    });
    
    // Adjust offsets
    FDASSERT(m_parameters.size() > 0, "Invalid set of parameters");
    u32 currentSet = m_parameters[0].m_setSlot;
    u32 offset = 0;
    m_setCount = 1;
    for (auto& rParam : m_parameters) 
    {
      if (rParam.m_setSlot != currentSet) // Restart the offset when the set changes
      {
        currentSet = rParam.m_setSlot;
        offset = 0;
        m_setCount++;
      }
      u32 alignmentOffset = Memory::get_alignment_offset(offset, rParam.m_alignment);
      rParam.m_offset = offset + alignmentOffset;
      offset = rParam.m_offset + rParam.m_size;
    }

    BaseT::create();
  }

}
}
