#pragma once
#include "Containers\Mask.h"
#include "Utilities\types.h"

namespace fdk
{
namespace Rendering
{
  class Buffer;

	class RenderInterface
	{
	public:
    enum ERenderFlags : u32
    {
      kPendingResize = 1 << 0
    };

    static RenderInterface* create();

    void init();

    void release();

    bool is_pending_resize() const { m_flags.is_enable(kPendingResize); };

    void on_resize();

    void create_buffer(Buffer& rBuffer);

    void destroy_buffer(Buffer& rBuffer);

    void copy_buffer(Buffer& rFrom, Buffer& rTo);

    void send_buffer_memory_to_gpu(Buffer& rBuffer);

	protected:
		RenderInterface();

    Containers::Mask<u32> m_flags;
	};

}
}