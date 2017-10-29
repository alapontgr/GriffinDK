#include "ParameterBuffer.h"
#include "RenderInterface.h"
#include "MaterialParameterSet.h"
#include "ParameterBufferGroup.h"

namespace fdk
{
namespace Rendering
{

	ParameterBuffer::ParameterBuffer()
	{
	}

	void ParameterBuffer::create(
			const MaterialParameterSet& rParameterSetLayout,
			const ParameterBufferGroup& rAllocationGroup,
			Memory::MemAllocator& rAllocator)
	{
		m_pParameterSetLayout = &rParameterSetLayout;
		m_pAllocationGroup = &rAllocationGroup;
		u32 size = m_pParameterSetLayout->total_size();
		m_pParametersData = rAllocator.allocate(size, 16);
    memset(m_pParametersData, 0, size); // Set default value to the data of 0 (NULL)
		BaseT::create();
	}

	void ParameterBuffer::update(Memory::MemAllocator& rTmpAllocator)
	{
		FDASSERT(m_pParameterSetLayout, "Parameter layout has not been set");
		BaseT::update(rTmpAllocator);
	}

  void ParameterBuffer::set_parameter(const ParameterSlot& rSlot, const ConstantBuffer* pConstantBuffer)
  {
    FDASSERT(m_pParametersData, "The parameter buffer has not been created and initialized yet");
    memcpy(m_pParametersData + rSlot.m_offset, &pConstantBuffer, sizeof(ConstantBuffer*));
  }

  void ParameterBuffer::release()
	{
		BaseT::release();
	}
}
}
