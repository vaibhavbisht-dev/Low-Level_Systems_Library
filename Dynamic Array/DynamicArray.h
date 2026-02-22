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
	{
	}
	
	DynamicArray(const DynamicArray& other):
		data_(nullptr),
		size_(0),
		capacity_(0)
	{
		if (other.capacity_ == 0)
			return;
		data_ = static_cast<T*>(:: operator new(sizeof(T) * other.capacity_));
		capacity_ = other.capacity_;
		size_ = other.size_;

		for (size_t i = 0; i < size_; i++) {
			new(data_ + i) T(other.data_[i]);
		}
	}


	// destriction of each object in the data_
	~DynamicArray() {
		for (size_t i = 0; i < size_; i++) {
			data_[i].~T();
		}

		::operator delete(data_);
	}

	/**
	* Overloaded Assignment Operator
	* handles deep copying of one dynamic array to another 
	*/
	
	DynamicArray& operator=(const DynamicArray& other) {
		// 1. Self-assignment guard: check if we are assigning the object to itself
		if (this == &other)
			return *this;

		// 2. Cleanup: Manually call destructors for existing elements
		for (size_t i = 0; i < size_; ++i)
		{
			data_[i].~T();
		}

		// 3. Free the raw memory block
		::operator delete(data_);

		// Reset state to ensure consistency in case the rest of the function fails
		data_ = nullptr;
		size_ = 0;
		capacity_ = 0;

		// 4. If the source array is empty, we are done
		if (other.capacity_ == 0)
			return *this;

		// 5. Allocation: Allocate a new block of raw memory based on the source's capacity
		data_ = static_cast<T*>(::operator new(sizeof(T) * other.capacity_));
		capacity_ = other.capacity_;
		size_ = other.size_;


		// 6. Copy Construction: Use "placement new" to call the copy constructor 
		// for each element into the newly allocated raw memory
		for (size_t i = 0; i < size_; ++i)
		{
			new (data_ + i) T(other.data_[i]);
		}

		return *this;

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

	void push_back(T&& value) {
		
		// Check if size_ == capacity_ 
		// increase capacity_ exponentianally if true
		if (size_ == capacity_) {
			size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
			reserve(new_capacity);
		}

		// add new data 
		// data_ + 0: Address 0x1000 (First element)
		// data_ + 1: Address 0x1004 (Second element)
		new (data_ + size_) T(std::move(value));
		// increase size
		++size_;
	}

	void push_back(const T& value) {

		// Check if size_ == capacity_ 
		// increase capacity_ exponentianally if true
		if (size_ == capacity_) {
			size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
			reserve(new_capacity);
		}

		// add new data 
		// data_ + 0: Address 0x1000 (First element)
		// data_ + 1: Address 0x1004 (Second element)
		new (data_ + size_) T(value);
		// increase size
		++size_;
	}


	void resize(size_t new_size) {
		if (new_size < size_) {
			for (size_t i = new_size; i < size_; ++i) {
				data_[i].~T();
			}
			size_ = new_size;
		}
		else if (new_size > size_) {
			if (new_size > capacity_) {
				reserve(new_size);
			}

			for (size_t i = size_; i < new_size; ++i) {
				new (data_ + i) T();
			}

			size_ = new_size;
		}
	}

private:
	T* data_;
	size_t size_;
	size_t capacity_;

};
