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

		/// <summary>
		/// Initializes the arena with a pre-allocated block of memory.
		/// </summary>
		/// <param name="memory">Pointer to the start of the memory block. Must be aligned to k_max_alignment.</param>
		/// <param name="size">Total size of the memory block in bytes.</param>
		/// <param name="handler">Optional callback for Out of Memory events. Defaults to aborting.</param>
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

		/// <summary>
		/// Move constructor: Transfers ownership of the memory block from another arena.
		/// </summary>
		/// <param name="other">The arena to move from. It will be nullified after the move.</param>
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
#endif 
		}

		/// <summary>
		/// Move assignment operator: Transfers ownership of the memory block.
		/// </summary>
		/// <param name="other">The arena to move from.</param>
		/// <returns>Reference to this arena.</returns>
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

		/// <summary>
		/// Calculates the remaining capacity in the arena.
		/// </summary>
		/// <returns>Number of bytes available for future allocations.</returns>
		size_t LinearArena::remaining() const noexcept {
			LLS_ASSERT(m_begin != nullptr, "LinearArena used after moved or not initialized");
			return static_cast<size_t>(m_end - m_current);
		}

		/// <summary>
		/// Calculates the total bytes currently allocated.
		/// </summary>
		/// <returns>Number of bytes used, including alignment padding.</returns>
		size_t LinearArena::used() const noexcept {
			LLS_ASSERT(m_begin != nullptr, "LinearArena using after move or not initialized");
			return static_cast<size_t>(m_current - m_begin);
		}

		/// <summary>
		/// Internal helper to trigger the OOM handler when an allocation fails.
		/// </summary>
		/// <param name="requested_size">The size of the allocation that failed.</param>
		void LinearArena::handle_oom(size_t requested_size) const {
			LLS_ASSERT(m_begin != nullptr, "LinearArena used after moved or not initialized");
			const size_t remaining_bytes = static_cast<size_t>(m_end - m_current);

			if (m_oom_handler) {
				m_oom_handler(requested_size, remaining_bytes);
			}
			std::abort();
		}

		/// <summary>
		/// Allocates a block of memory with specific alignment.
		/// </summary>
		/// <param name="size">The number of bytes to allocate.</param>
		/// <param name="alignment">The required alignment (must be a power of two).</param>
		/// <returns>A pointer to the allocated memory.</returns>
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
			}

			m_current = new_current;
#if defined(LLS_DEBUG)
			++m_allocation_count;
			size_t used_bytes = static_cast<size_t>(m_current - m_begin);
			if (used_bytes > m_high_water_mark) {
				m_high_water_mark = used_bytes;
			}
#endif
			return aligned;
		}

		/// <summary>
		/// Captures the current state of the arena's allocation pointer.
		/// </summary>
		/// <returns>A marker representing the current allocation position.</returns>
		LinearArena::marker_t LinearArena::get_marker() const noexcept {
			LLS_ASSERT(m_begin != nullptr, "LinearArena is used after move or not initialized");
			return m_current;
		}

		/// <summary>
		/// Rewinds the arena's allocation pointer to a previously captured marker.
		/// </summary>
		/// <param name="marker">The position to reset to. Must be valid and within this arena's range.</param>
		void LinearArena::reset_to_marker(marker_t marker) noexcept {
			LLS_ASSERT(m_begin != nullptr, "LinearArena is used after move or not initialized");
			LLS_ASSERT(marker >= m_begin, "Marker is before arena begin");
			LLS_ASSERT(marker <= m_current, "Marker is ahead of current allocation pointer");

#if defined(LLS_DEBUG)
			if constexpr (config::enable_debug_poison)
			{
				const size_t bytes_to_poison = static_cast<size_t>(m_current - marker);
				std::memset(marker, config::debug_fill_pattern, bytes_to_poison);
			}
#endif
			m_current = marker;
		}

		/// <summary>
		/// Resets the arena to its initial state, effectively freeing all allocations.
		/// </summary>
		void LinearArena::reset() noexcept {
			LLS_ASSERT(m_begin != nullptr, "LinearArena used after move or not initialized");
#if defined(LLS_DEBUG)
			if constexpr (config::enable_debug_poison) {
				size_t used = static_cast<size_t>(m_current - m_begin);
				std::memset(m_begin, config::debug_fill_pattern, used);
			}
			m_allocation_count = 0;
#endif
			m_current = m_begin;
		}
	}
}