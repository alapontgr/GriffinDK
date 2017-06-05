#include "VK_RenderInterface.h"

namespace fdk
{
namespace Rendering
{
	RenderInterface* RenderInterface::create()
	{
		static VKRenderInterface s_renderInterface;
		return &s_renderInterface;
	}

	VKRenderInterface::VKRenderInterface() {}
	VKRenderInterface::~VKRenderInterface() {}

  void VKRenderInterface::test()
  {

  }
}
}
