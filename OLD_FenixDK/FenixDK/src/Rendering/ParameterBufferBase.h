#pragma once

#include "Memory\memallocator.h"
#include "MaterialUtils.h"

namespace fdk
{
namespace Rendering
{
	class MaterialParameterSet;
	class ParameterBufferGroup;
	struct ParameterDefinition;

	class ParameterBufferBase
	{
	public:
	
    ParameterBufferBase();

    const MaterialParameterSet* layout() const { return m_pParameterSetLayout; };

    EParameterSetFramerateType framerate() const { return m_framerate; }

	protected:
		Memory::mem_ptr_t get_parameter_data(const ParameterDefinition& rParameter);

		Memory::mem_ptr_t m_pParametersData;
    EParameterSetFramerateType m_framerate;
		const MaterialParameterSet* m_pParameterSetLayout;
		const ParameterBufferGroup* m_pAllocationGroup;
	};
}
}