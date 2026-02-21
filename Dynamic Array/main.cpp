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
	DynamicArray<Test> a;
a.push_back(Test(1));
a.push_back(Test(2));

DynamicArray<Test> b;
b.push_back(Test(99));

b = a;
	return 0;
}

