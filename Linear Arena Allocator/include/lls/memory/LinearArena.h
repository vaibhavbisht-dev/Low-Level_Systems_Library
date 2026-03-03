#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

// Ensure std::byte is available (C++17)
#if !defined(__cpp_lib_byte) || __cpp_lib_byte < 201603L
namespace std {
	enum class byte : unsigned char {};
}
#endif

#include "lls/memory/Alignment.h"
#include "lls/memory/ArenaConfig.h"
#include "lls/core/Assert.h"


namespace lls 
{
namespace memory 
{
	class LinearArena {
	public:

		//============================================================================
		// Types
		//============================================================================

		using oom_handler_t = void(*)(size_t request_size, size_t remaining_size);
		static constexpr size_t k_max_alignment = 64;

		//============================================================================
		// Construction/ Lifetime
		//============================================================================

		LinearArena(void* memory,
			size_t size,
			oom_handler_t oom_handler = nullptr) noexcept;

		LinearArena(const LinearArena&) = delete;
		LinearArena& operator=(const LinearArena&) = delete;

		LinearArena(LinearArena&& other) noexcept;
		LinearArena& operator=(LinearArena&& other) noexcept;

		~LinearArena() = default;

		//============================================================================
		// Alloction Interface
		//============================================================================

		void* allocate(size_t size, size_t alignment);

		template<typename T>
		T* allocate() {
			static_assert(std::is_trivially_destructible_v<T>,
				"LinearArena only supports trivially destructable types");

			return static_cast<T*>(allocate(sizeof(T*), alignof(T*)));
		}

		//============================================================================
		// ARENA Control
		//============================================================================

		void reset() noexcept;
		size_t remaining() const noexcept;

		//============================================================================
		// Debug / Telemetry
		//============================================================================

#if defined(LLS_DEBUG)
		size_t allocation_count() const noexcept { return m_allocation_count; }
		size_t high_water_mark() const noexcept { return m_high_water_mark; }
#endif // defined(LLS_DEBUG)

	private:
		void handle_oom(size_t requested_size) const;
		// ============================================================
		// Data Layout
		// ============================================================

		std::byte* m_begin;
		std::byte* m_current;
		std::byte* m_end;

		oom_handler_t m_oom_handler;

#if defined(LLS_DEBUG)
		size_t m_allocation_count;
		size_t m_high_water_mark;
#endif // defined(LLS_DEBUG)

	};
}
}