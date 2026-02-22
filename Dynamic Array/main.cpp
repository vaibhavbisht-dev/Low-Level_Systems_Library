#include<iostream>
#include"DynamicArray.h"

int main() {
struct Test {
    int value;

    Test() : value(0) {
        std::cout << "Default Construct\n";
    }

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
    arr.resize(3);

    arr.resize(1);

    arr.resize(5);

	return 0;
}

