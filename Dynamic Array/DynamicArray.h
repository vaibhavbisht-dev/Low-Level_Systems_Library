#pragma once

#include <cstddef>  
#include <new>  
#include <utility>   

template<typename T>
class DynamicArray {

public:
	// Construction of The object
	DynamicArray():
		data_(nullptr),
		size_(0),
		capacity_(0)
	{}


	// destriction of each object in the data_
	~DynamicArray() {
		for (size_t i = 0; i < size_; i++) {
			data_[i].~T();
		}

		::operator delete(data_);
	}

	void reserve(size_t new_capacity) {
		// 1. if the requested capacity is less than or equal to the old capacity do nothing
		if (new_capacity <= capacity_)
			return;

		//2. allocate raw memory for new capacity
		T* new_data = static_cast<T*>(::operator new(sizeof(T) * new_capacity));

		// 3. Transfer existing elements to the new memory location
		for (size_t i = 0; i < size_; i++) {
			// Uses "Placement New" to construct the object in the new memory 
			// using the Move Constructor to avoid expensive copies
			new (new_data + i) T(std::move(data_[i]));
		}

		// 4. Explicitly call destructors for the objects in the old memory block
		for (size_t i = 0; i < size_; i++) {

			data_[i].~T();
		}

		// 5. Free the raw memory of the old buffer
		::operator delete(data_);

		// 6. Update the internal pointers and capacity value
		data_ = new_data;
		capacity_ = new_capacity;


	}

private:
	T* data_;
	size_t size_;
	size_t capacity_;

};
