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


