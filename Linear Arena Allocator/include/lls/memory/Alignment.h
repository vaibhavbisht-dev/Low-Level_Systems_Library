#pragma once

#include <cstddef>
#include <cstdint>
#include "lls/core/Assert.h"

namespace lls::memory 
{
	/// <summary>
	/// Checks if a given value is a power of two.
	/// </summary>
	/// <param name="value">The value to verify.</param>
	/// <returns>True if the value is a power of two and non-zero.</returns>
	constexpr bool is_power_of_two(size_t value) noexcept {
		return value != 0 && (value & (value - 1)) == 0;
	}

	/// <summary>
	/// Aligns an integer value forward to the nearest multiple of the specified alignment.
	/// </summary>
	/// <param name="value">The original value to align.</param>
	/// <param name="alignment">The alignment boundary (must be a power of two).</param>
	/// <returns>The value rounded up to the nearest alignment boundary.</returns>
	constexpr size_t align_forward(size_t value, size_t alignment) noexcept {
		LLS_ASSERT(alignment != 0, "Aligment Should Not be 0");
		LLS_ASSERT(is_power_of_two(alignment), "Alignemnt should be a power of two");
		const size_t mask = alignment - 1;
		return (value + mask) & ~mask;
	}

	/// <summary>
	/// Aligns a memory pointer forward to the nearest address multiple of the specified alignment.
	/// </summary>
	/// <param name="ptr">The raw pointer to align.</param>
	/// <param name="alignment">The alignment boundary (must be a power of two).</param>
	/// <returns>A pointer to the aligned memory address.</returns>
	inline std::byte* align_forward(std::byte* ptr, size_t alignment) noexcept {
		LLS_ASSERT(ptr != nullptr, "Pointer Must Not Be NULL");
		const uintptr_t value = reinterpret_cast<uintptr_t>(ptr);
		const uintptr_t aligned = static_cast<uintptr_t>(align_forward(static_cast<size_t>(value), alignment));
		return reinterpret_cast<std::byte*>(aligned);
	}

	/// <summary>
	/// Calculates the number of padding bytes needed to align a value.
	/// </summary>
	/// <param name="value">The current value or offset.</param>
	/// <param name="alignment">The target alignment boundary.</param>
	/// <returns>The difference between the aligned value and the original value.</returns>
	constexpr size_t align_padding(size_t value, size_t alignment) noexcept {
		return align_forward(value, alignment) - value;
	}
}