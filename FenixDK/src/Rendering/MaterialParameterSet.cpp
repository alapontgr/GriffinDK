#include "MaterialParameterSet.h"

namespace fdk
{
namespace Rendering
{

  static inline u32 get_offset(const MaterialParameterSet::ParameterCollection& rParamaterList) 
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

	void MaterialParameterSet::add_parameter(const ParameterDefinition& rParameterDefinition)
	{
    u32 offset = get_offset(m_parameters);
    u32 alignmentOffset = Memory::get_alignment_offset(offset, rParameterDefinition.m_alignment);
    m_parameters.push_back(rParameterDefinition);
    auto& rParam = m_parameters[m_parameters.size() - 1];
    rParam.m_offset = offset + alignmentOffset;
	}

  ParameterDefinition* MaterialParameterSet::get_parameter(const Utilities::Name& rParamName)
  {
    for (auto& rParam : m_parameters) 
    {
      if (rParam.m_paramName == rParamName) 
      {
        return &rParam;
      }
    }
    return nullptr;
  }

  u32 MaterialParameterSet::total_size()
  {
    auto& rParam = m_parameters[m_parameters.size() - 1];
    return rParam.m_offset + rParam.m_size;
  }

}
}
