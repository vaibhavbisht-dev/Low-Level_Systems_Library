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
    arr.emplace_back(4);
    arr.emplace_back(5);

    for (auto& x : arr)
    {
        std::cout << x.value << "\n";
    }

    std::for_each(arr.begin(), arr.end(), [](const Test& t) {
    std::cout << t.value << "\n";
    });

	return 0;
}

