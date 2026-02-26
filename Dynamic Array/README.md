# DynamicArray

A generic, heap-allocated dynamic array implemented in C++ using manual memory management and placement new. Mirrors the behavior of `std::vector` without relying on the standard library container.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Getting Started](#getting-started)
- [API Reference](#api-reference)
  - [Type Aliases](#type-aliases)
  - [Constructors & Destructor](#constructors--destructor)
  - [Assignment](#assignment)
  - [Capacity](#capacity)
  - [Element Access](#element-access)
  - [Modifiers](#modifiers)
  - [Iterators](#iterators)
- [Implementation Notes](#implementation-notes)
- [Example Usage](#example-usage)

---

## Overview

`DynamicArray<T>` is a templated contiguous container that grows dynamically as elements are added. Memory is managed manually using `::operator new` and `::operator delete`, and elements are constructed in-place via placement new — giving full control over object lifetimes separate from raw memory allocation.

---

## Features

- **Generic** — works with any type `T` via templates
- **Deep copy semantics** — copy constructor and copy assignment operator perform full deep copies
- **Manual memory management** — uses raw allocation + placement new for precise lifetime control
- **Exception-safe `reserve`** — rolls back cleanly if an element constructor throws during reallocation
- **`emplace_back`** — constructs elements in-place with perfect forwarding, avoiding unnecessary copies
- **Range erase** — supports erasing a single element or a range of elements
- **STL-compatible iterators** — pointer-based iterators compatible with range-for loops and standard algorithms

---

## Getting Started

Since `DynamicArray` is a header-only template, simply include it in your project:

```cpp
#include "DynamicArray.h"
```

No build system configuration or linking is required.

**Requirements:** C++11 or later.

---

## API Reference

### Type Aliases

| Alias | Type | Description |
|---|---|---|
| `iterator` | `T*` | Mutable pointer-based iterator |
| `const_iterator` | `const T*` | Read-only pointer-based iterator |

---

### Constructors & Destructor

#### Default Constructor
```cpp
DynamicArray()
```
Constructs an empty array. No memory is allocated. `size` and `capacity` are both `0`.

---

#### Copy Constructor
```cpp
DynamicArray(const DynamicArray& other)
```
Creates a deep copy of `other`. Allocates a new buffer of the same capacity and copy-constructs each element from `other`.

---

#### Destructor
```cpp
~DynamicArray()
```
Calls the destructor on each element in the array, then frees the raw memory block.

---

### Assignment

#### Copy Assignment Operator
```cpp
DynamicArray& operator=(const DynamicArray& other)
```
Performs a deep copy of `other` into `this`. Handles self-assignment safely. Destroys and deallocates the current contents before copying.

---

### Capacity

#### `reserve`
```cpp
void reserve(size_t new_capacity)
```
Pre-allocates memory for at least `new_capacity` elements without changing the logical size. If `new_capacity` is less than or equal to the current capacity, this is a no-op. Elements are migrated to the new buffer using `std::move_if_noexcept`. If an exception is thrown during migration, the new buffer is cleaned up and the exception is re-thrown, leaving the original array intact.

---

#### `resize`
```cpp
void resize(size_t new_size)
```
Resizes the container to hold exactly `new_size` elements.

- If `new_size < size`: excess elements are destroyed.
- If `new_size > size`: new elements are default-constructed. Memory is reserved if needed.

---

### Element Access

#### `operator[]`
```cpp
T& operator[](size_t index) noexcept
const T& operator[](size_t index) const noexcept
```
Accesses the element at `index`. **No bounds checking.** Undefined behavior if `index >= size`.

---

#### `at`
```cpp
T& at(size_t index)
const T& at(size_t index) const
```
Accesses the element at `index` **with bounds checking**. Throws `std::out_of_range` if `index >= size`.

---

### Modifiers

#### `push_back` (copy)
```cpp
void push_back(const T& value)
```
Appends a copy of `value` to the end of the array. Triggers reallocation (doubling capacity) if the array is full.

---

#### `push_back` (move)
```cpp
void push_back(T&& value)
```
Appends `value` to the end by moving it. Triggers reallocation if needed.

---

#### `emplace_back`
```cpp
template 
void emplace_back(Args&&... args)
```
Constructs an element directly at the end of the array by perfectly forwarding `args` to `T`'s constructor. More efficient than `push_back` when constructing from multiple arguments, as it avoids an intermediate temporary.

---

#### `erase` (single element)
```cpp
iterator erase(iterator pos)
```
Removes the element pointed to by `pos`. Shifts all subsequent elements left by one using move assignment. The last duplicate element is then destroyed and the size is decremented. Returns an iterator to the position where the element was removed. Returns `end()` if `pos` is out of bounds.

---

#### `erase` (range)
```cpp
iterator erase(iterator first, iterator last)
```
Removes all elements in the half-open range `[first, last)`. Shifts subsequent elements left to fill the gap, destroys the trailing duplicates, and updates the size. Returns an iterator to the position of `first` after the removal. Returns `first` unchanged if `first == last`.

---

### Iterators

| Method | Description |
|---|---|
| `begin()` | Returns a mutable iterator to the first element |
| `end()` | Returns a mutable iterator to one past the last element |
| `begin() const` | Returns a const iterator to the first element |
| `end() const` | Returns a const iterator to one past the last element |
| `cbegin()` | Returns a const iterator to the first element |
| `cend()` | Returns a const iterator to one past the last element |

All iterator methods are marked `noexcept`. The iterators are raw pointers and are fully compatible with range-for loops and standard algorithms like `std::sort`, `std::find`, etc.

---

## Implementation Notes

**Memory model:** Raw memory is allocated with `::operator new` (no constructors called) and freed with `::operator delete`. Constructors are invoked separately via placement new, giving full control over object lifetimes.

**Growth strategy:** Capacity doubles on each reallocation (starting from 1 if previously 0). This amortizes the cost of `push_back` to O(1).

**`std::move_if_noexcept`:** Used in `reserve` and `erase` to guarantee strong exception safety. If `T`'s move constructor is not `noexcept`, elements are copied instead of moved during reallocation to preserve the original on failure.

**No shrink-to-fit:** Capacity never automatically decreases. Erase operations reduce `size` but not `capacity`.

---

## Example Usage

```cpp
#include "DynamicArray.h"
#include 
#include 

int main() {
    DynamicArray arr;

    // Adding elements
    arr.push_back(10);
    arr.push_back(20);
    arr.emplace_back(30);

    // Range-for loop
    for (int x : arr) {
        std::cout << x << " ";  // 10 20 30
    }
    std::cout << "\n";

    // Indexed access
    std::cout << arr[0] << "\n";   // 10
    std::cout << arr.at(2) << "\n"; // 30

    // Resize
    arr.resize(5);  // Adds two default-constructed ints (0)

    // Erase single element
    arr.erase(arr.begin());  // Removes 10

    // Erase range
    arr.erase(arr.begin(), arr.begin() + 2);  // Removes next two elements

    // Deep copy
    DynamicArray copy = arr;

    return 0;
}
```

---

## License

This project is released for educational purposes. Feel free to use, modify, and distribute it.