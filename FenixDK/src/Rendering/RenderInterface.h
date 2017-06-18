#pragma once

namespace fdk
{
namespace Rendering
{
	class RenderInterface
	{
	public:
    static RenderInterface* create();

    void init();

    void release();

    void on_resize();

	protected:
		RenderInterface();
	};

}
}