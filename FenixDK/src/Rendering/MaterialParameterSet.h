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
  
    const ParameterDefinition* get_parameter(const Utilities::Name& rParamName) const;

    u32 total_size() const;

  private:
   
    ParameterCollection m_parameters;
	};

}
}