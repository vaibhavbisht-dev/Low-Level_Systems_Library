#pragma once


namespace lls 
{
namespace memory 
{
namespace config 
{


#if !defined(LLS_DEBUG)
	constexpr bool enable_allocation_tracking = true; // Enable or disable allocation tracking
	constexpr bool enable_high_watermark = true; // Enable or disable high watermark tracking
	constexpr bool enable_debug_poison = true; // Enable or disable debug poisoning of memory
#else
	constexpr bool enable_allocation_tracking = false; // Enable or disable allocation tracking
	constexpr bool enable_high_water_mark = false; // Enable or disable high watermark tracking
	constexpr bool enable_debug_poison = false; // Enable or disable debug poisoning of memory
#endif // !defined(LLS_DEBUG)

	constexpr unsigned char debug_fill_pattern = 0xCD; // Pattern used for debug poisoning
}
}
}