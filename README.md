# Low Level Systems Library

A C++ low-level systems programming library focused on building core engine-level data structures and memory systems from scratch, without relying on the STL for internal implementation.

This repository documents my progression toward AAA-level engine programming by implementing fundamental systems components manually with attention to memory safety, performance, and architectural clarity.

---

## Completed Components

### 1. [DynamicArray](Dynamic%20Array/README.md)

A custom implementation of a dynamically resizable contiguous container similar in functionality to `std::vector`, built using raw memory management.

#### Features

- Manual memory allocation using `::operator new`
- Placement new for element construction
- Explicit destructor calls for element destruction
- Full Rule of Five implementation:
  - Destructor
  - Copy constructor
  - Copy assignment
  - Move constructor
  - Move assignment
- Strong exception safety guarantee in `reserve`
- Amortized constant-time `push_back`
- Element access via `operator[]`
- Iterators (pointer-based)
- `erase` support
- Capacity growth strategy (geometric growth)
- Clear ownership and lifetime semantics

#### Design Goals

- Understand low-level container internals
- Control memory layout and allocation strategy
- Practice RAII and strong exception guarantees
- Avoid hidden allocations or STL abstractions
- Build foundation for engine-level data structures

#### Complexity Guarantees

| Operation        | Complexity        |
|------------------|------------------|
| `push_back`      | Amortized O(1)   |
| `pop_back`       | O(1)             |
| `operator[]`     | O(1)             |
| `reserve`        | O(n)             |
| `erase`          | O(n)             |

#### Memory Model

- Contiguous memory layout
- Capacity managed separately from size
- Reallocation performs move construction when possible
- Old memory destroyed and deallocated only after successful transfer

---


### 2. [Linear Arena Allocator](Linear%20Arena%20Allocator/README.md)

A high-performance linear arena allocator designed for engine-level systems where allocations are frequent, short-lived, and freed in bulk. The allocator operates on a contiguous memory block and provides extremely fast $O(1)$ allocations by moving a cursor forward through the memory region.

This allocator is intended for use in performance-critical scenarios such as frame memory, temporary buffers, ECS scratch memory, and job system allocations.

---

## Overview

The Linear Arena Allocator manages a fixed block of memory and fulfills allocation requests by advancing a pointer through the block. Individual deallocation is not supported; instead, memory is reclaimed all at once via a reset operation or by rewinding to a previously recorded marker.

This design minimizes allocation overhead and avoids fragmentation, making it well-suited for real-time systems such as game engines.

---

## Features

* **$O(1)$ allocation** using pointer bumping.
* **Contiguous memory layout** for cache-friendly access.
* **Alignment-safe allocations** for SIMD and platform requirements.
* **Custom Out-of-Memory (OOM) handler** support.
* **Marker system** for scoped temporary allocations.
* **Full reset capability.**
* **Debug instrumentation** and safety checks.
* **Debug memory poisoning** to detect use-after-reset bugs.
* **Move semantics** for safe ownership transfer.
* **Stress-tested** for alignment correctness and allocation safety.

---

## Core Design

The allocator operates using a three-pointer model:

* `m_begin`   → Start of the memory block.
* `m_current` → Current allocation cursor.
* `m_end`     → End of the memory block.

Allocations proceed by aligning the current cursor and advancing it by the requested size.



---

## Allocation Algorithm

1.  **Align** the current pointer to the requested alignment.
2.  **Compute** the new pointer after allocation.
3.  **Check** for overflow against the arena boundary.
4.  **Commit** the allocation by advancing the cursor.

This process requires only a few pointer operations and avoids expensive bookkeeping.

---

## Marker System

The arena supports markers to enable scoped temporary allocations. A marker captures the current allocation position:

```cpp
auto marker = arena.get_marker();
```

Later, allocations can be rewound to this point:

```cpp
arena.reset_to_marker(marker);
```

This allows nested temporary allocation scopes without resetting the entire arena.

---

## Reset Behavior

The arena can be reset entirely via `arena.reset()`. This operation:

* Optionally poisons previously used memory in debug builds.
* Resets the allocation cursor to the beginning.
* Clears debug allocation counters.

The reset operation runs in constant time.

---

## Debug Instrumentation

In debug builds, the allocator tracks:

* Total allocation count.
* High water mark (maximum memory usage).

These metrics assist with memory budgeting and performance profiling.

---

## Memory Safety

Debug builds include additional safeguards:

* Assertions for invalid arena usage.
* Marker boundary validation.
* Debug memory poisoning to catch stale pointer usage.

---

## Alignment Guarantees

All allocations support power-of-two alignment requirements up to the maximum supported alignment. This ensures compatibility with:

* SIMD types.
* Platform-specific alignment constraints.
* Cache line boundaries.

---

## Complexity

| Operation | Complexity |
|---|---|
| `allocate` | $O(1)$ |
| `reset` | $O(1)$ |
| `get_marker` | $O(1)$ |
| `reset_to_marker` | $O(1)$ |
| `remaining` | $O(1)$ |

---

## Typical Use Cases

Linear arenas are widely used in engine systems for:

* Frame allocators
* Temporary ECS buffers
* Job system scratch memory
* Parsing and serialization buffers
* Physics contact generation
* AI pathfinding scratch memory

---

## Future Integrations

The arena allocator will serve as a foundation for:

* Arena-backed containers.
* Stack and Pool allocators.
* ECS memory subsystems.

---
## Current Scope

At this stage, the repository contains:

- DynamicArray implementation

Future components will include:

- Linear Arena Allocator
- Stack Allocator
- Pool Allocator
- Custom String
- Hash Map
- Job System prototypes

---

## Purpose

This repository serves as a public record of building foundational engine systems from scratch to develop deep expertise in:

- Memory management
- Object lifetime control
- Cache-aware design
- Exception safety
- Systems-level C++ architecture

---

More systems will be added incrementally as they are designed and implemented.



