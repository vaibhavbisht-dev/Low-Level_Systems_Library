#include<iostream>
#include"DynamicArray.h"

int main() {
	struct Test {
    Test() { std::cout << "Construct\n"; }
    ~Test() { std::cout << "Destroy\n"; }
	};


	DynamicArray<int> arr;
	arr.reserve(5);
	DynamicArray<Test> arr2;
	arr2.reserve(3);

	return 0;
}

