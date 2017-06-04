#pragma once

namespace fdk
{
namespace Rendering
{
	class RenderInterface
	{
	public:
    static RenderInterface* create();

    void test();

    void init();

    void release();
	protected:
		RenderInterface();
	};

}
}