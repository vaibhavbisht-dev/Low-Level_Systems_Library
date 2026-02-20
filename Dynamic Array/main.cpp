#include<iostream>
#include"DynamicArray.h"

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
	arr.push_back(Test(1));
	arr.push_back(Test(2));
	arr.push_back(Test(3));

	return 0;
}

