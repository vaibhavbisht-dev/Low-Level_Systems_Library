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

    ~Test() {
        std::cout << "Destroy " << value << "\n";
    }
};
    DynamicArray<Test> arr;
    arr.emplace_back(1);
    arr.emplace_back(2);
    arr.emplace_back(3); // triggers reallocation

    

	return 0;
}

