#pragma once
// ----------------------------------------
// React Engine Utility
// Basic data structures
// 
// Created: 18.07.2022
// Modified: 18.07.2022
// Author: Deryabin K.
// ----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h> //TODO: CHANGE THIS SHIT ASSERT TO NORMAL !!!

namespace rutl
{
	// ------------------------------------------ 
	// DYNAMIC HEAP
	// ------------------------------------------ 
	template <typename Type>
	class dheap
	{
		size_t size;
		size_t capacity;
		size_t reserve;
		Type *p_data;
	public:
		dheap() : p_data(NULL), size(0), capacity(0), reserve(1) {}
		~dheap() {}

		size_t get_size() { return size; }
		size_t get_capacity() { return capacity; }
		size_t get_reserve() { return reserve; }
		Type  *get_data() { return p_data; }

		// 
		// resize
		// Resize heap
		// 
		bool resize(size_t new_size) {
			p_data = (Type *)realloc(p_data, new_size * sizeof(Type));
			return p_data != NULL;
		}

		// 
		// alloc
		// Initialize container
		// Memory will be automatic reallocated
		// 
		bool alloc(size_t start_size = 0, size_t start_cap = 1, size_t res = 1) {
			size = start_size;
			reserve = res;
			capacity = start_cap;
			if (capacity)
				if (!resize(capacity))
					return false;

			return true;
		}

		// 
		// push
		// Add new element to container
		// Memory will be automatic reallocated
		// 
		bool push(const Type elem) {
			if (size >= capacity) {
				capacity += reserve;
				if (!resize(capacity))
					return false;
			}
			p_data[size] = elem;
			size++;
			return true;
		}

		bool push(const Type *p_elem) {
			if (size >= capacity) {
				capacity += reserve;
				if (!resize(capacity))
					return false;
			}
			p_data[size] = *p_elem;
			size++;
			return true;
		}


		Type &pop() {
			Type data;
			if (size <= 0) // empty container
				return data;

			data = p_data[size - 1];
			size--;
			return data;
		}

		bool find(size_t *p_dst_index, Type value) {
			for (size_t i = 0; i < get_size(); i++) {
				if (p_data[i] == value) {
					*p_dst_index = i;
					return true;
				}
			}
			return false;
		}

		// 
		// remove_fast
		// Move end element to removed position
		// 
		bool remove_fast(size_t index) {
			// check empty container
			if (!size)
				return false;

			// check bounds
			if (index < 0 || index > size)
				return false;

			// decrement size
			if (size > 0)
				size--;

			p_data[index] = p_data[size];
			return true;
		}

		// 
		// remove_from
		// Remove 'count' elements starts at 'index'
		// Move all elements to back after 'index'
		// 
		void remove_from(int index, int count) {
			if (count >= size) {
				size = 0;
				return;
			}

			for (int i = index; i < (size - count); i++)
				p_data[i] = p_data[i + count];

			size -= count;
		}

		// 
		// mfree
		// Free memory block
		// 
		bool mfree() {
			if (is_allocated()) {
				free(p_data);
				p_data = NULL;
				return true;
			}
			return false;
		}

		// 
		// clear
		// Reset position to 0
		// Rewrite all elements as a new element is added 
		// 
		void clear() { size = 0; }

		// 
		// operator[]
		// 
		Type &operator[](size_t index) { return p_data[index]; }

		Type *get_at_ptr(size_t index) { return &p_data[index]; }
		Type &get_at_ref(size_t index) { return p_data[index]; }

		inline bool is_valid_index(size_t index) { return (index >= 0 && index < get_size()); }

		inline bool is_allocated() { return (p_data != NULL); }
	};


	// ------------------------------------------ 
	// LINKED LIST
	// ------------------------------------------ 
	//template <typename Type>
	//class linked_list
	//{
	//public:

	//	// Linked list node class
	//	class ll_node
	//	{
	//		friend class linked_list;
	//		Type data;
	//		ll_node *p_last;
	//		ll_node *p_next;
	//	public:
	//		inline void     set_last(ll_node *p_ll) { p_last = p_ll; }
	//		inline ll_node *get_last() { return p_last; }
	//		inline void     set_next(ll_node *p_nl) { p_next = p_nl; }
	//		inline ll_node *get_next() { return p_next; }
	//		inline void     set_data(Type d) { data = d; }
	//		inline Type     get_data() { return data; }

	//		// unlink me from list
	//		inline void     unlink() {
	//			if (p_last)
	//				p_last->p_next = p_next;
	//			if (p_next)
	//				p_next->p_last = p_last;
	//		}
	//	};

	//private:
	//	linked_list<Type> *p_begin, *p_end;
	//	size_t num_of_nodes;

	//public:
	//	linked_list() : p_begin(NULL), p_end(NULL), num_of_nodes(0) {}
	//	~linked_list() {}

	//	// 
	//	// alloc_node
	//	// Alloc new node
	//	// For internally use
	//	// 
	//	ll_node *alloc_node() { return (ll_node *)calloc(1, sizeof(ll_node)); }
	//	void     free_node(ll_node *p_node) { free(p_node); }

	//	ll_node *begin() { return p_begin; }
	//	ll_node *end() { return p_end; }
	//	size_t   get_nodes_count() { return num_of_nodes; }

	//	// 
	//	// add
	//	// Add new element to linked list
	//	// 
	//	bool add(Type d) {

	//		// allocate new node
	//		ll_node *p_new_node = alloc_node();
	//		if (!p_new_node)
	//			return false; // failed to allocate memory

	//		// is not allocated previously
	//		if (!p_begin && !p_end) {
	//			p_begin = p_new_node;
	//			p_end = p_begin;
	//		}
	//		else {
	//			p_new_node->set_last(p_end);
	//			p_end->set_next(p_new_node);
	//			p_end = p_new_node; // save current node as end
	//		}
	//		p_new_node->set_data(d); //copy data
	//		num_of_nodes++;
	//		return true;
	//	}

	//	// 
	//	// remove
	//	// Remove node from linked list
	//	// 
	//	void remove(ll_node *p_node) {
	//		p_node->unlink();
	//		free_node(p_node);
	//		num_of_nodes--;
	//	}

	//	// 
	//	// remove all elements
	//	// 
	//	void remove_all() {
	//		ll_node *p_curr_node = begin();
	//		if (!p_curr_node) //empty ll
	//			return;

	//		ll_node *p_node_deleting;
	//		while (p_curr_node) {
	//			p_node_deleting = p_curr_node; //store address
	//			p_curr_node = p_curr_node->get_next(); // switch to next node
	//			free_node(p_node_deleting);
	//		}
	//		num_of_nodes = 0;
	//		p_begin = NULL;
	//		p_end = NULL;
	//	}

	//	// 
	//	// Find node at index
	//	// 
	//	ll_node *node_at(size_t idx) {
	//		ll_node *p_node = begin();
	//		if (!p_node)
	//			return NULL; //empty linked list

	//		size_t i = 0;
	//		while (p_node && i < idx) {
	//			p_node = p_node->get_next();
	//			i++;
	//		}
	//		return p_node;
	//	}
	//};

	// ------------------------------------------ 
	// FIXED CONTAINER
	// ------------------------------------------ 
	template<typename _Type, int _size>
	class fixed_container
	{
		size_t size;
		_Type data[_size];
		const size_t capacity = sizeof(data) / sizeof(data[0]);
	public:
		fixed_container() : size(0) {}
		~fixed_container() {}

		bool push(_Type &src_elem) {
			if (size == capacity)
				return false; //overflow

			data[size] = src_elem;
			size++;
			return true;
		}

		bool pop(_Type &dst_elem) {
			if (size == 0)
				return false; //underflow

			dst_elem = data[size];
			return true;
		}

		_Type &operator[](size_t idx) {
			assert(idx >= 0 && idx < get_capacity());
			return data[idx];
		}

		void clear() { size = 0; }

		size_t get_size() { return size; }
		size_t get_capacity() { return capacity; }
	};

	// ------------------------------------------ 
	// FIXED STACK
	// ------------------------------------------ 
	//template<typename _Type, int _size>
	//class fixed_stack : public fixed_container<_Type, _size>
	//{
	//	fixed_stack() {}
	//	~fixed_stack() {}


	//};

	// ------------------------------------------ 
	// 2D DYNAMIC HEAP
	// ------------------------------------------ 
	template<typename _Type>
	class dheap2dim
	{
		_Type *p_data;
		size_t idim_size;
		size_t jdim_size;
	public:
		dheap2dim() : p_data(NULL), idim_size(0), jdim_size(0) {}
		~dheap2dim() {}

		bool alloc(size_t isize, size_t jsize) {
			idim_size = isize;
			jdim_size = jsize;
			return (p_data = (_Type *)malloc(idim_size * jdim_size * sizeof(_Type)));
		}

		bool is_allocated() { return !!p_data; }
		bool memfree() { if (p_data) free(p_data); p_data = NULL; }

		size_t get_idim_size() { return idim_size; };
		size_t get_jdim_size() { return jdim_size; };
		_Type *get_ptr(size_t i, size_t j) { return &p_data[i * jdim_size + j]; }
	};


	// ------------------------------------------ 
	// 3D DYNAMIC HEAP
	// ------------------------------------------ 
	template<typename _Type>
	class dheap3dim
	{
		_Type *p_data;
		size_t idim_size;
		size_t jdim_size;
		size_t kdim_size;
	public:
		dheap3dim() : p_data(NULL), idim_size(0), jdim_size(0), kdim_size(0) {}
		~dheap3dim() {}

		bool alloc(size_t isize, size_t jsize, size_t ksize) {
			idim_size = isize;
			jdim_size = jsize;
			kdim_size = ksize;
			return (p_data = (_Type *)malloc(idim_size * jdim_size * kdim_size * sizeof(_Type)));
		}
		bool is_allocated() { return !!p_data; }
		bool memfree() { if (p_data) free(p_data); p_data = NULL; }

		size_t get_idim_size() { return idim_size; };
		size_t get_jdim_size() { return jdim_size; };
		size_t get_kdim_size() { return kdim_size; };
		_Type *get_ptr(size_t i, size_t j, size_t k) { return &p_data[(i * kdim_size + j) * jdim_size + k]; }
	};
};