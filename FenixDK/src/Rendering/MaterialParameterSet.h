#pragma once

#include "ParameterDefinition.h" 
#include <vector>

namespace fdk
{
namespace Rendering
{

	class MaterialParameterSet
	{

	public:

    using ParameterCollection = std::vector<ParameterDefinition>;

    MaterialParameterSet();

    void clear();

    void add_parameter(const ParameterDefinition& rParameterDefinition);

    u32 slot() const { return m_bindingSlot; }
    
    void set_bindingSlot( const u32 slot) { m_bindingSlot = slot; }
  
    ParameterDefinition* get_parameter(const Utilities::Name& rParamName);

  private:
    
    u32 m_bindingSlot; 
    ParameterCollection m_parameters;
	};

}
}