#pragma once

#include "ParameterDefinition.h" 

namespace fdk
{
namespace Rendering
{  
	class MaterialParameterSet
	{

	public:

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