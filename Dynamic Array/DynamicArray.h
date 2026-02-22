#pragma once

#include <cstddef>  
#include <new>  
#include <utility>   

template<typename T>
class DynamicArray {

public:
	/// <summary>
	/// An iterator type that provides pointer-based access to elements of type T.
	/// </summary>
	using iterator = T*;
	/// <summary>
	/// A constant iterator type for traversing elements without modification.
	/// </summary>
	using const_iterator = const T*;

	/// <summary>
	/// Constructs an empty DynamicArray with no allocated memory.
	/// </summary>
	DynamicArray(): 
		data_(nullptr),
		size_(0),
		capacity_(0)
	{
	}
	/// <summary>
	/// Copy constructor that creates a deep copy of another DynamicArray.
	/// </summary>
	/// <param name="other">The DynamicArray to copy from.</param>
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


	/// <summary>
	/// Destructor that cleans up the dynamic array by destroying all elements and deallocating memory.
	/// </summary>
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

	/// <summary>
	/// Reserves storage capacity for the container without changing its size.
	/// </summary>
	/// <param name="new_capacity">The new capacity to reserve. If less than or equal to the current capacity, no action is taken.</param>
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
	/// <summary>
	/// Adds an element to the end of the container by moving the provided value.
	/// </summary>
	/// <param name="value">An rvalue reference to the element to be moved into the container.</param>
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
	/// <summary>
	/// Adds an element to the end of the container by moving the provided value.
	/// </summary>
	/// <param name="value">A reference to the element to be moved into the container.</param>
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

	/// <summary>
	/// Resizes the container to contain the specified number of elements.
	/// </summary>
	/// <param name="new_size">The new size of the container.</param>
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
	/// <summary>
	/// Constructs an element in-place at the end of the container.
	/// This avoids unnecessary copy/move operations by forwarding arguments directly to the constructor.
	/// </summary>
	/// <typeparam name="Args">Variadic template parameter pack for the constructor arguments.</typeparam>
	/// <param name="args">The arguments to forward to the constructor of type T.</param>
	template <typename... Args>
	void emplace_back(Args&&... args) {
		// Check if the internal buffer is full
		if (size_ == capacity_) {
			// Simple geometric expansion: start at 1 or double the current capacity
			size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
			reserve(new_capacity);
		}

		/**
		 * Placement New:
		 * Constructs the object of type T directly at the memory address (data_ + size_).
		 * std::forward preserves the value category (lvalue/rvalue) of the arguments.
		 */
		new (data_ + size_) T(std::forward<Args>(args)...);

		// Increment logical size after successful construction
		++size_;
	}


	/// <summary>
	/// Returns an iterator to the beginning of the container.
	/// </summary>
	/// <returns>An iterator pointing to the first element.</returns>
	iterator begin() noexcept {
		return data_;
	}
	/// <summary>
	/// Returns an iterator to the end of the container.
	/// </summary>
	/// <returns>An iterator pointing to one past the last element.</returns>
	iterator end() noexcept {
		return data_ + size_;
	}

	/// <summary>
	/// Returns a constant iterator to the beginning of the container.
	/// </summary>
	/// <returns>A const_iterator pointing to the first element, or the underlying data pointer.</returns>
	const_iterator begin() const noexcept {
		return data_;
	}
	
	/// <summary>
	/// Returns a const iterator to the end of the container.
	/// </summary>
	/// <returns>A const iterator pointing to the position one past the last element.</returns>
	const_iterator end() const noexcept {
		return data_ + size_;
	}

	/// <summary>
	/// Returns a constant iterator to the beginning of the container.
	/// </summary>
	/// <returns>A constant iterator pointing to the first element.</returns>
	const_iterator cbegin() const noexcept {
		return data_;
	}

	/// <summary>
	/// Returns a constant iterator to the end of the container.
	/// </summary>
	/// <returns>A constant iterator pointing to the position one past the last element.</returns>
	const_iterator cend() const noexcept {
		return data_ + size_;
	}





private:
	T* data_;
	size_t size_;
	size_t capacity_;


};
