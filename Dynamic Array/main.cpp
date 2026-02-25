#include<iostream>
#include"DynamicArray.h"
#include <algorithm>

int main() {
struct Test {
    int value;

    Test(int v) : value(v) {
        std::cout << "Construct " << value << "\n";
    }

    Test(const Test& other) : value(other.value) {
        std::cout << "Copy " << value << "\n";
    }

    Test(Test&& other)
{
    std::cout << "Move " << other.value << "\n";

    if (other.value == 2)
        throw std::runtime_error("Move failed");

    value = other.value;
}

Test& operator=(const Test& other)
{
    std::cout << "Copy Assign " << other.value << "\n";
    value = other.value;
    return *this;
}

Test& operator=(Test&& other) noexcept
{
    std::cout << "Move Assign " << other.value << "\n";
    value = other.value;
    return *this;
}

    ~Test() {
        std::cout << "Destroy " << value << "\n";
    }
};
    DynamicArray<Test> arr;

    arr.emplace_back(1);
    arr.emplace_back(2);
    arr.emplace_back(3);
    arr.emplace_back(4);
    arr.emplace_back(5);

    arr.erase(arr.begin() + 2); // remove 3

    for (auto& x : arr)
        std::cout << x.value << "\n";

    std::cout << "---\n";

    arr.erase(arr.begin(), arr.begin() + 2); // remove 1,2

    for (auto& x : arr)
        std::cout << x.value << "\n";

    return 0;

    

	return 0;
}

