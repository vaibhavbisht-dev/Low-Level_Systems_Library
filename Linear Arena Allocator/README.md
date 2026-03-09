# Low-Level Systems Library (LLS)

A high-performance, header-friendly C++17 library providing low-level memory management primitives for systems programming, game development, and performance-critical applications.

## 🚀 Features

### Linear Arena Allocator
- **Ultra-fast allocation** with O(1) time complexity
- **Zero fragmentation** - sequential memory layout
- **Marker-based checkpointing** for efficient rollback
- **Configurable debug instrumentation** (allocation tracking, high-water mark, memory poisoning)
- **Custom OOM handling** for graceful error management
- **Move semantics** for efficient ownership transfer
- **Alignment support** up to 64-byte boundaries

### Memory Alignment Utilities
- Power-of-two alignment validation
- Forward alignment for both integers and pointers
- Padding calculation for efficient memory layout

### Type System
- Platform-independent fixed-width integer types (`u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64`)
- Size type (`usize`) for memory operations
- Type-safe `std::byte` wrapper

## 📚 Table of Contents
- [Installation](#installation)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
- [Configuration](#configuration)
- [Performance Characteristics](#performance-characteristics)
- [Best Practices](#best-practices)
- [Examples](#examples)
- [License](#license)

## 🛠️ Installation

### Requirements
- **C++17** or later
- CMake 3.10+ (if using CMake build system)
- Any modern C++ compiler (MSVC, GCC, Clang)

### Header-Only Integration
Simply copy the `include/lls` directory into your project and add it to your include path:

```cpp
#include "lls/memory/LinearArena.h"
```

## ⚡ Quick Start

### Basic Usage

```cpp
#include "lls/memory/LinearArena.h"
#include <vector>

int main() {
    // 1. Allocate a memory buffer (must be 64-byte aligned)
    constexpr size_t arena_size = 1024 * 1024; // 1MB
    alignas(lls::memory::LinearArena::k_max_alignment) 
    std::byte buffer[arena_size];

    // 2. Create the arena
    lls::memory::LinearArena arena(buffer, arena_size);

    // 3. Allocate memory
    int* numbers = static_cast<int*>(arena.allocate(10 * sizeof(int), alignof(int)));

    // 4. Use the memory
    for (int i = 0; i < 10; ++i) {
        numbers[i] = i * i;
    }

    // 5. Reset when done (instant deallocation)
    arena.reset();

    return 0;
}
```

### Using Markers for Temporary Allocations

```cpp
lls::memory::LinearArena arena(buffer, buffer_size);

// Allocate some persistent data
auto* persistent = arena.allocate(100, 8);

// Save checkpoint
auto marker = arena.get_marker();

// Allocate temporary data
auto* temp1 = arena.allocate(200, 8);
auto* temp2 = arena.allocate(300, 8);

// Rollback to checkpoint - temp1 and temp2 are "freed"
arena.reset_to_marker(marker);

// Persistent data is still valid
```

## 📖 API Reference

### `LinearArena` Class

#### Constructor
```cpp
LinearArena(void* memory, size_t size, oom_handler_t oom_handler = nullptr) noexcept;
```
- `memory`: Pointer to pre-allocated buffer (must be aligned to `k_max_alignment`)
- `size`: Size of the buffer in bytes
- `oom_handler`: Optional callback invoked on out-of-memory (defaults to abort)

#### Allocation Methods

```cpp
void* allocate(size_t size, size_t alignment);
```
Allocates `size` bytes with the specified alignment.

```cpp
template<typename T>
T* allocate();
```
Type-safe allocation for trivially destructible types.

#### Arena Control

```cpp
void reset() noexcept;
```
Resets the arena to its initial state (in debug mode, poisons freed memory).

```cpp
marker_t get_marker() const noexcept;
```
Returns a marker representing the current allocation position.

```cpp
void reset_to_marker(marker_t marker) noexcept;
```
Rewinds allocation to a previously saved marker.

```cpp
size_t remaining() const noexcept;
```
Returns the number of bytes remaining.

```cpp
size_t used() const noexcept;
```
Returns the number of bytes currently allocated.

#### Debug Methods (Available in `LLS_DEBUG` builds)

```cpp
size_t allocation_count() const noexcept;
```
Returns the total number of allocations made.

```cpp
size_t high_water_mark() const noexcept;
```
Returns the peak memory usage.

### Alignment Utilities

```cpp
namespace lls::memory
```

```cpp
constexpr bool is_power_of_two(size_t value) noexcept;
```
Checks if a value is a power of two.

```cpp
constexpr size_t align_forward(size_t value, size_t alignment) noexcept;
```
Aligns a value forward to the nearest alignment boundary.

```cpp
std::byte* align_forward(std::byte* ptr, size_t alignment) noexcept;
```
Aligns a pointer forward to the nearest alignment boundary.

```cpp
constexpr size_t align_padding(size_t value, size_t alignment) noexcept;
```
Calculates padding needed to align a value.

## ⚙️ Configuration

Configuration is controlled via preprocessor defines in `ArenaConfig.h`:

### Debug Features (Enabled when `LLS_DEBUG` is defined)

```cpp
// In ArenaConfig.h
namespace lls::memory::config {
    constexpr bool enable_allocation_tracking = true;  // Track allocation count
    constexpr bool enable_high_watermark = true;       // Track peak usage
    constexpr bool enable_debug_poison = true;         // Fill freed memory
    constexpr unsigned char debug_fill_pattern = 0xCD; // Memory poison pattern
}
```

### Custom Assertions

Define `LLS_DEBUG` to enable runtime assertions:

```cpp
#define LLS_DEBUG
#include "lls/memory/LinearArena.h"
```

## 🎯 Performance Characteristics

| Operation | Time Complexity | Space Overhead |
|-----------|----------------|----------------|
| Allocation | O(1) | 0-63 bytes (alignment padding) |
| Reset | O(1)* | 0 bytes |
| Reset to Marker | O(1)* | 0 bytes |
| Get Marker | O(1) | sizeof(void*) |

\* O(n) when debug poisoning is enabled, where n is the freed memory size

### Memory Layout
```
[allocated][padding][allocated][padding][...][free space]
^                                            ^           ^
m_begin                                   m_current    m_end
```

## 💡 Best Practices

### ✅ Do's
- Use for frame-based allocations in game loops
- Allocate temporary data structures that share the same lifetime
- Use markers for nested scopes
- Pre-allocate the buffer with proper alignment
- Handle OOM gracefully with custom handlers

### ❌ Don'ts
- Don't store individual allocations long-term (use heap instead)
- Don't allocate types with non-trivial destructors
- Don't mix with manual `delete`/`free` (no per-allocation deallocation)
- Don't use the arena after moving from it

## 📋 Examples

### Custom OOM Handler

```cpp
void my_oom_handler(size_t requested, size_t remaining) {
    std::cerr << "Out of memory! Requested: " << requested 
              << " bytes, remaining: " << remaining << " bytes\n";
    // Custom recovery logic or logging
}

lls::memory::LinearArena arena(buffer, size, my_oom_handler);
```

### Frame-Based Allocation Pattern (Game Development)

```cpp
class Game {
    alignas(64) std::byte frame_memory[1024 * 1024]; // 1MB
    lls::memory::LinearArena frame_arena;

public:
    Game() : frame_arena(frame_memory, sizeof(frame_memory)) {}

    void update() {
        // Allocate temporary data for this frame
        auto* particles = frame_arena.allocate<ParticleData>();
        auto* collision_cache = frame_arena.allocate(1024, 16);

        // Use data...

        // At end of frame, free everything instantly
        frame_arena.reset();
    }
};
```

### Hierarchical Allocation

```cpp
void process_level(lls::memory::LinearArena& arena) {
    auto level_marker = arena.get_marker();

    // Load level data
    auto* level = arena.allocate<LevelData>();

    for (auto& room : level->rooms) {
        auto room_marker = arena.get_marker();

        // Allocate room-specific temp data
        auto* temp_geometry = arena.allocate(room.size, 16);
        process_room(temp_geometry);

        // Free room temp data
        arena.reset_to_marker(room_marker);
    }

    // Free entire level
    arena.reset_to_marker(level_marker);
}
```

## 🏗️ Project Structure

```
Low-Level_Systems_Library/
├── include/lls/
│   ├── core/
│   │   ├── Assert.h          # Debug assertion macros
│   │   └── Types.h           # Platform-independent types
│   └── memory/
│       ├── Alignment.h       # Memory alignment utilities
│       ├── ArenaConfig.h     # Compile-time configuration
│       └── LinearArena.h     # Linear arena allocator
├── src/memory/
│   └── LinearArena.cpp       # Implementation
├── tests/
│   └── LinearArenaTests.cpp  # Unit tests
└── README.md
```

## 🔬 Testing

Tests are located in the `tests/` directory. Run the test suite to verify functionality:

```bash
# Build and run tests (example using your build system)
```

## 🤝 Contributing

Contributions are welcome! Please ensure:
- Code follows existing style conventions
- All tests pass
- New features include appropriate tests
- Documentation is updated

## 📄 License

[Specify your license here]

## 🙏 Acknowledgments

This library is designed for high-performance systems where allocation speed and memory layout control are critical. It draws inspiration from industry practices in game development and real-time systems programming.

## 📞 Contact

**Author**: Vaibhav Bisht  
**GitHub**: [vaibhavbisht-dev](https://github.com/vaibhavbisht-dev)  
**Repository**: [Low-Level_Systems_Library](https://github.com/vaibhavbisht-dev/Low-Level_Systems_Library)

---

⭐ If you find this library useful, please consider giving it a star on GitHub!
