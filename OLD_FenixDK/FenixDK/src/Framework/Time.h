#pragma once

namespace fdk
{
namespace Framework
{
	namespace Time
	{
		f64 delta_time_secs();
		f64 delta_time_mili();
		f64 delta_time_micro();

		f64 total_time_secs();
		f64 total_time_mili();
		f64 total_time_micro();

		f64 update_time();
		f64 render_time();
	}
}
}