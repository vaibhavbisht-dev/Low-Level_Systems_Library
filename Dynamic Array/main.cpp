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

    Test(Test&& other) noexcept : value(other.value) {
        std::cout << "Move " << value << "\n";
    }

    ~Test() {
        std::cout << "Destroy " << value << "\n";
    }
};
    DynamicArray<Test> arr;
    arr.emplace_back(1);
    arr.emplace_back(2);
    arr.emplace_back(3);

    std::cout << arr[1].value << "\n";     // should print 2

    arr[1].value = 42;
    std::cout << arr[1].value << "\n";     // should print 42

    try
    {
        arr.at(10);
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "Exception caught\n";
    }

    

	return 0;
}

