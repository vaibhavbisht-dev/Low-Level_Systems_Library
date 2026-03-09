#include <iostream>
#include <cassert>
#include <cstddef>
#include <cstdint>

// Ensure this path matches your project structure
#include "lls/memory/LinearArena.h"

void test_basic_allocation()
{
    std::cout << "Running: Basic Allocation... ";
    alignas(64) std::byte buffer[1024];
    lls::memory::LinearArena arena(buffer, sizeof(buffer));

    void* a = arena.allocate(16, 8);
    void* b = arena.allocate(32, 8);

    assert(a != nullptr);
    assert(b != nullptr);
    assert(a != b);
    std::cout << "Passed!\n";
}

void test_alignment()
{
    std::cout << "Running: Alignment... ";
    alignas(64) std::byte buffer[1024];
    lls::memory::LinearArena arena(buffer, sizeof(buffer));

    // Requesting 32-byte alignment
    void* ptr = arena.allocate(32, 32);

    assert(reinterpret_cast<uintptr_t>(ptr) % 32 == 0);
    std::cout << "Passed!\n";
}

void test_marker()
{
    std::cout << "Running: Marker/Rewind... ";
    alignas(64) std::byte buffer[1024];
    lls::memory::LinearArena arena(buffer, sizeof(buffer));

    arena.allocate(64, 8);
    auto marker = arena.get_marker(); // Save current position

    arena.allocate(128, 8);
    arena.reset_to_marker(marker); // Rewind back

    // The arena should think only the first 64 bytes are used
    assert(arena.remaining() == sizeof(buffer) - 64);
    std::cout << "Passed!\n";
}

void test_reset()
{
    std::cout << "Running: Full Reset... ";
    alignas(64) std::byte buffer[1024];
    lls::memory::LinearArena arena(buffer, sizeof(buffer));

    arena.allocate(256, 8);
    arena.reset(); // Wipe everything

    assert(arena.remaining() == sizeof(buffer));
    std::cout << "Passed!\n";
}

void test_stress()
{
    std::cout << "Running: Stress Test... ";
    alignas(64) std::byte buffer[4096];
    lls::memory::LinearArena arena(buffer, sizeof(buffer));

    for (int i = 0; i < 100; ++i)
    {
        void* p = arena.allocate(32, 8);
        assert(p != nullptr);
    }
    std::cout << "Passed!\n";
}

int main()
{
    std::cout << "--- Starting LinearArena Tests ---\n";

    try {
        test_basic_allocation();
        test_alignment();
        test_marker();
        test_reset();
        test_stress();
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "--- All Tests Passed Successfully! ---\n";
    return 0;
}