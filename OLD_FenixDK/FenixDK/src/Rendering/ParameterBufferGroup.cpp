#include "ParameterBufferGroup.h"

namespace fdk
{
namespace Rendering
{

	ParameterBufferGroup::ParameterBufferGroup()
	{
	}

	void ParameterBufferGroup::create()
	{
		FDASSERT(m_maxElementCount == 0, "This group has already been initialized");
		m_maxElementCount = 1;
		BaseT::create();
	}
}
}
