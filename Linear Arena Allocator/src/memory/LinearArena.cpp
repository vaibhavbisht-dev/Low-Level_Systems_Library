#include "lls/memory/LinearArena.h"
#include "lls/memory/Alignment.h"
#include "lls/memory/ArenaConfig.h"
#include <cstring>
#include <cstdlib>

namespace lls 
{
namespace memory 
{
	namespace 
	{
		void default_oom_handler(size_t, size_t) {
			LLS_ASSERT(false, "Linear Arena Out Of Memory");
			std::abort();
		}

	}

	LinearArena::LinearArena(void* memory, size_t size, oom_handler_t handler) noexcept :
		m_begin(nullptr),
		m_current(nullptr),
		m_end(nullptr),
		m_oom_handler(handler ? handler : default_oom_handler)

#if defined(LLS_DEBUG)
		, m_allocation_count(0)
		, m_high_water_mark(0)
#endif
	{
		LLS_ASSERT(memory != nullptr, "LinearArena Memory Must Not Be Null.");
		LLS_ASSERT(size > 0, "LinearArena Size Must Be Greater Than 0");

		const uintptr_t addr = reinterpret_cast<uintptr_t>(memory);
		LLS_ASSERT(addr % k_max_alignment == 0,
			"LinearArena base memory must be aligned to k_max_alignment");


		m_begin = static_cast<std::byte*>(memory);
		m_current = m_begin;
		m_end = m_begin + size;

	}


	LinearArena::LinearArena(LinearArena&& other) noexcept
		:m_begin(other.m_begin)
		, m_current(other.m_current)
		, m_end(other.m_end)
		, m_oom_handler(other.m_oom_handler)
#if defined(LLS_DEBUG)
		, m_allocation_count(other.m_allocation_count)
		, m_high_water_mark(other.m_high_water_mark)
#endif
	{
		other.m_begin = nullptr;
		other.m_current = nullptr;
		other.m_end = nullptr;
		other.m_oom_handler = nullptr;

#if defined(LLS_DEBUG)
		other.m_allocation_count = 0;
		other.m_high_water_mark = 0;
#endif // defined(LLS_DEBUG)


	}

	LinearArena& LinearArena::operator=(LinearArena&& other) noexcept {
		if (this == &other)
			return *this;

		m_begin = other.m_begin;
		m_current = other.m_current;
		m_end = other.m_end;
		m_oom_handler = other.m_oom_handler;

#if defined(LLS_DEBUG)
		m_allocation_count = other.m_allocation_count;
		m_high_water_mark = other.m_high_water_mark;
#endif

		other.m_begin = nullptr;
		other.m_current = nullptr;
		other.m_end = nullptr;
		other.m_oom_handler = nullptr;
#if defined(LLS_DEBUG)
		other.m_allocation_count = 0;
		other.m_high_water_mark = 0;
#endif

		return *this;
	}


	size_t LinearArena::remaining() const noexcept {
		LLS_ASSERT(m_begin != nullptr, "LinearArena used after moved or not initialized");

		return static_cast<size_t>(m_end - m_current);
	}


	void LinearArena::handle_oom(size_t requested_size) const {
		LLS_ASSERT(m_begin != nullptr, "LinearArena used after moved or not initialized");
		const size_t remaining_bytes = static_cast<size_t>(m_end - m_current);

		if (m_oom_handler) {
			m_oom_handler(requested_size, remaining_bytes);
		}

		// Safety: allocator contract says we never return on OOM
		std::abort();
	}


	void* LinearArena::allocate(size_t size, size_t alignment) {
		
		LLS_ASSERT(m_begin != nullptr, "LinearArena used after moved or not initialized");
		LLS_ASSERT(size > 0, "LinearArena Size should be greater that 0");
		LLS_ASSERT(alignment > 0, "LinearArena alignment should be greater than 0");
		LLS_ASSERT(alignment <= k_max_alignment,
			"Requested alignment exceeds maximum supported alignment");
		LLS_ASSERT(is_power_of_two(alignment), "Alignment must be a power of two");

		std::byte* aligned = align_forward(m_current, alignment);

		std::byte* new_current = aligned + size;

		if (new_current > m_end) {
			handle_oom(size);
			//unreachable, handle_oom should never return
		}

		m_current = new_current;

#if defined(LLS_DEBUG)
		++m_allocation_count;

		size_t used_bytes = static_cast<size_t> (m_current - m_begin);
		if (used_bytes > m_high_water_mark) {
			m_high_water_mark = used_bytes;
		}

#endif // defined(LLS_DEBUG)
		return aligned;

	}

	LinearArena::marker_t LinearArena::get_marker() const noexcept {
		LLS_ASSERT(m_begin != nullptr, "LinearArena is used after move or not initialized");
		return m_current;
	}

	void LinearArena::reset_to_marker(marker_t marker) noexcept {
		LLS_ASSERT(m_begin != nullptr,
			"LinearArena is used after move or not initialized");

		LLS_ASSERT(marker >= m_begin,
			"Marker is before arena begin");

		LLS_ASSERT(marker <= m_current,
			"Marker is ahead of current allocation pointer");

#if defined(LLS_DEBUG)
		if constexpr (config::enable_debug_poison)
		{
			const size_t bytes_to_poison =
				static_cast<size_t>(m_current - marker);

			std::memset(marker,
				config::debug_fill_pattern,
				bytes_to_poison);
		}
#endif

		m_current = marker;
	}



	void LinearArena::reset() noexcept {
		LLS_ASSERT(m_begin != nullptr, "LinearArena used after move or not initialized");
#if defined(LLS_DEBUG)
		if constexpr (config::enable_debug_poison) {
			size_t used = static_cast<size_t>(m_current - m_begin);
			std::memset(m_begin, config::debug_fill_pattern, used);
		}

		m_allocation_count = 0;
#endif // defined(LLS_DEBUG)
		m_current = m_begin;
	}

}
}