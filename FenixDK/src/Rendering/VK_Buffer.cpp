#include "VK_Buffer.h"

#include "RenderDefines.h"

namespace fdk
{
namespace Rendering
{

  VK_Buffer::VK_Buffer()
			//: m_gpuBuffer{VK_NULL_HANDLE}
	{
	}

#ifdef FENIX_VK_IMPL
	Buffer* Buffer::create(Memory::MemAllocator& rAllocator)
	{
    return rAllocator.create<VK_Buffer>();
	}
#endif // FENIX_VK_IMPL
}
}
