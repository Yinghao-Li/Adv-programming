//
// Created by bpswe on 9/24/2018.
// Modified by YLi on 10/05/2018
//

#pragma once

#include <iostream>
#include <assert.h>
#include <math.h>
#include "array.h"

using size_t = std::size_t;
using std::cout;
using std::endl;

//#define VERBOSE

template<typename T> class array_iterator;

template<typename T>
class array{
public:
	//default constructor
	array();

	//initialize array with elements in initializer
	array(std::initializer_list<T>);

	//copy constructor
	array(const array&);

	//move constructor
	array(array&&);

	//construct with initial "reserved" size
	array(size_t);

	//construct with n copies of t
	array(size_t n, const T& t);

	//destructor
	~array();

	//ensure enough memory for n elements
	void reserve(size_t n);

	//add to end of vector
	void push_back(const T&);

	//add to front of vector
	void push_front(const T&);

	//remove last element
	void pop_back();

	//remove first element
	void pop_front();

	//return reference to first element
	T& front() const;

	//return reference to last element
	T& back() const;

	//return reference to specified element
	const T& operator[](size_t) const;

	//return reference to specified element
	T& operator[](size_t);

	//return number of elements
	size_t length() const;

	//returns true if empty
	bool empty() const;

	//remove all elements
	void clear();

	//obtain iterator to first element
	array_iterator<T> begin() const;

	//obtain iterator to one beyond element
	array_iterator<T> end() const;

	//remove specified element
	void erase(const array_iterator<T>&);

	//insert element right before itr
	void insert(const T&, const array_iterator<T>&);

public:
	T * m_elements;              //points to actual elements
	size_t m_size;              //number of elements
	size_t m_reserved_size;     //number of reserved elements
};

template<typename T>
class array_iterator {
public:
	array_iterator();
	array_iterator(T*);
	array_iterator(const array_iterator&);
	T& operator*() const;
	array_iterator operator++();
	array_iterator operator++(int __unused);
	bool operator != (const array_iterator&) const;
	bool operator == (const array_iterator&) const;

private:
	T * m_current;
	// I want my array class to be able to access m_current even though it's private
	// 'friend' allows this to happen
	friend class array<T>;
};

//----------------------------------------------------------------------------------

// default constructor
template<class T>
array<T>::array() :
	m_elements(nullptr), m_size(0), m_reserved_size(0) {
#ifdef VERBOSE
	cout << "array<T> default constructor is called." << endl;
#endif // VERBOSE
}

// initialize array with elements in initializer
template<class T>
array<T>::array(std::initializer_list<T> l) : m_size(l.size()) {

	//m_reserved_size = (size_t)pow(2, ((int)log2(l.size()) + 1));
	m_reserved_size = l.size();
	m_elements = (T*)malloc(sizeof(T) * m_reserved_size);
	assert(m_elements);
	auto iter = l.begin();
	for (size_t i = 0; i < m_size; ++i) {
		new(m_elements + i) T(*(iter + i));
	}

#ifdef VERBOSE
	cout << "array<T> initializer constructor is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif // VERBOSE
}

// copy constructor
template<class T>
array<T>::array(const array<T>& a) : m_size(a.m_size), m_reserved_size(a.m_reserved_size) {
	m_elements = (T*)malloc(sizeof(T) * m_reserved_size);
	assert(m_elements);
	for (size_t i = 0; i < m_size; ++i) {
		new(m_elements + i) T(*(a.m_elements + i));
	}

#ifdef VERBOSE
	cout << "array<T> copy constructor is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

// move constructor
template<class T>
array<T>::array(array&& a) : m_size(a.m_size), m_reserved_size(a.m_reserved_size) {
	assert(a.m_elements);
	m_elements = a.m_elements;
	a.m_elements = nullptr;
	a.m_size = 0;
	a.m_reserved_size = 0;

#ifdef VERBOSE
	cout << "array<T> move constructor is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

// construct with initial "reserved" size
template<class T>
array<T>::array(size_t n) : m_reserved_size(n), m_size(0) {
	m_elements = (T*)malloc(sizeof(T) * m_reserved_size);
	assert(m_elements);

#ifdef VERBOSE
	cout << "array<T> \"reserved\" size constructor is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

// construct with n copies of t
template<class T>
array<T>::array(size_t n, const T& t) : m_size(n) {
	// m_reserved_size = (size_t)pow(2, ((int)log2(n) + 1));
	m_reserved_size = n;
	m_elements = (T*)malloc(sizeof(T) * m_reserved_size);
	assert(m_elements);
	for (size_t i = 0; i < n; ++i) {
		new(m_elements + i) T(t);
	}

#ifdef VERBOSE
	cout << "array<T> n copies of t constructor is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

//destructor
template<class T>
array<T>::~array() {

#ifdef VERBOSE
	cout << "array<T> destructor is called." << endl;
#endif
	if (m_elements != nullptr) {

		for (size_t i = 0; i < m_size; ++i) {
			(m_elements + i)->~T();
		}
		free(m_elements);
		m_elements = nullptr;
		m_size = 0;
		m_reserved_size = 0;
	}
}

//ensure enough memory for n elements
template<class T>
void array<T>::reserve(size_t n) {
	m_reserved_size = n;
	if (m_size == 0) {
		m_elements = (T*)malloc(sizeof(T) * m_reserved_size);
		assert(m_elements);
	}
	else {
		T* temp = (T*)malloc(sizeof(T) * m_reserved_size);
		assert(temp);

		for (size_t i = 0; i < m_size; ++i) {
			new(temp) T(std::move(*(m_elements + i)));
			(m_elements + i)->~T();
		}
		free(m_elements);
		m_elements = temp;
		temp = nullptr;
	}

#ifdef VERBOSE
	cout << "array<T> reserve function is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

//add to end of vector
template<class T>
void array<T>::push_back(const T& a) {
	if (m_reserved_size == 0) {
		m_reserved_size = 2;
		m_elements = (T*)malloc(sizeof(T) * m_reserved_size);
		assert(m_elements);
	}
	else {
		if (m_size == m_reserved_size) {
			m_reserved_size *= 2;
			T* temp = (T*)malloc(sizeof(T) * m_reserved_size);
			assert(temp);

			for (size_t i = 0; i < m_size; ++i) {
				new(temp + i) T(std::move(*(m_elements + i)));
				(m_elements + i)->~T();
			}
			free(m_elements);
			m_elements = temp;
			temp = nullptr;
		}
	}
	new(m_elements + m_size) T(a);
	++m_size;

#ifdef VERBOSE
	cout << "array<T> push_back function is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

//add to front of vector
template<class T>
void array<T>::push_front(const T& a) {
	if (m_size == m_reserved_size) {
		if (m_reserved_size == 0) m_reserved_size = 2;
		else m_reserved_size *= 2;
	}
	T* temp = (T*)malloc(sizeof(T) * (m_reserved_size + 1));
	assert(temp);
	new(temp) T(a);

	for (size_t i = 0; i < m_size; ++i) {
		new(temp + i + 1) T(std::move(*(m_elements + i)));
		(m_elements + i)->~T();
	}
	free(m_elements);
	m_elements = temp;
	temp = nullptr;
	++m_size;

#ifdef VERBOSE
	cout << "array<T> push_front function is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

//remove last element
template<class T>
void array<T>::pop_back() {
	if (m_size == 0) return;
	(m_elements + m_size - 1)->~T();
	--m_size;

#ifdef VERBOSE
	cout << "array<T> pop_back function is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

//remove first element
template<class T>
void array<T>::pop_front() {
	if (m_size == 0) return;
	m_elements->~T();
	for (size_t i = 0; i < m_size - 1; ++i) {
		*(m_elements + i) = std::move(*(m_elements + i + 1));
	}
	--m_size;

#ifdef VERBOSE
	cout << "array<T> pop_back function is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

//return reference to first element
template<class T>
T& array<T>::front() const {
#ifdef VERBOSE
	cout << "array<T> front function is called." << endl;
#endif
	return *m_elements;
}

//return reference to last element
template<class T>
T& array<T>::back() const {
#ifdef VERBOSE
	cout << "array<T> back function is called." << endl;
#endif
	return *(m_elements + m_size - 1);
}

//return reference to specified element
template<class T>
const T& array<T>::operator[](size_t n) const {
	assert(n < m_size);

#ifdef VERBOSE
	cout << "array<T> const operator[] is called." << endl;
#endif
	return *(m_elements + n);
}

//return reference to specified element
template<class T>
T& array<T>::operator[](size_t n) {
	assert(n < m_size);

#ifdef VERBOSE
	cout << "array<T> operator[] is called." << endl;
#endif
	return *(m_elements + n);
}

//return number of elements
template<class T>
size_t array<T>::length() const {

#ifdef VERBOSE
	cout << "array<T> length function is called." << endl;
#endif
	return m_size;
}

//returns true if empty
template<class T>
bool array<T>::empty() const {

#ifdef VERBOSE
	cout << "array<T> empty function is called." << endl;
#endif
	return m_size == 0;
}

//remove all elements
template<class T>
void array<T>::clear() {
	if (m_elements == nullptr);
	else {
		for (size_t i = 0; i < m_size; ++i) {
			(m_elements + i)->~T();
		}
		free(m_elements);
		m_elements = nullptr;
		m_reserved_size = 0;
		m_size = 0;
	}
#ifdef VERBOSE
	cout << "array<T> clear function is called." << endl;
#endif
}

//obtain iterator to first element
template<class T>
array_iterator<T> array<T>::begin() const {
	assert(m_elements);
	array_iterator<T> iter(m_elements);

#ifdef VERBOSE
	cout << "array<T> begin function is called." << endl;
#endif
	return iter;
}

//obtain iterator to one beyond element
template<class T>
array_iterator<T> array<T>::end() const {
	assert(m_elements);
	array_iterator<T> iter(m_elements + m_size);

#ifdef VERBOSE
	cout << "array<T> end function is called." << endl;
#endif
	return iter;
}

//remove specified element
template<class T>
void array<T>::erase(const array_iterator<T>& i) {
	assert(m_elements);
	array_iterator<T> iter(i);
	i.m_current->~T();
	++iter;
	for (iter; iter != this->end(); ++iter) {
		*(iter.m_current - 1) = std::move(*iter);
	}
	--m_size;

#ifdef VERBOSE
	cout << "array<T> erase function is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif
}

//insert element right before itr
template<class T>
void array<T>::insert(const T& a, const array_iterator<T>& iter) {
	assert(m_elements);
	if (m_size == m_reserved_size) {
		m_reserved_size *= 2;

		T* temp = (T*)malloc(sizeof(T) * m_reserved_size);
		auto it(this->begin());
		size_t i = 0;
		for (it; it != iter; ++it, ++i) {
			new(temp + i) T(std::move(*it));
			it.m_current->~T();
		}
		new(temp + i) T(a);
		++i;
		for (it; it != this->end(); ++it, ++i) {
			new(temp + i) T(std::move(*it));
			it.m_current->~T();
		}

		free(m_elements);
		m_elements = temp;
		temp = nullptr;
	}
	else {
		T* it(m_elements + m_size - 1);
		T* temp = (T*)malloc(sizeof(T));
		new(temp) T(a);
		new(m_elements + m_size) T(std::move(*(m_elements + m_size - 1)));
		for (it; it != iter.m_current; --it){
			*(it) = std::move(*(it - 1));
		}
		*(iter.m_current) = std::move(*temp);
		temp->~T();
		free(temp);
		temp = nullptr;
	}
	++m_size;

#ifdef VERBOSE
	cout << "array<T> insert function is called. Reserved size is " <<
		m_reserved_size << ". Used size is " << m_size << endl;
#endif

}

//--------------------------------------------------------------------

template<class T>
array_iterator<T>::array_iterator() : m_current(nullptr) {}

template<class T>
array_iterator<T>::array_iterator(T* iter) : m_current(iter) {}

template<class T>
array_iterator<T>::array_iterator(const array_iterator& iter) : m_current(iter.m_current) {}

template<class T>
T& array_iterator<T>::operator*() const {
	return *m_current;
}

template<class T>
array_iterator<T> array_iterator<T>::operator++() {
	++m_current;
	return *this;
}

template<class T>
array_iterator<T> array_iterator<T>::operator++(int __unused) {
	array_iterator<T> temp(m_current);
	++m_current;
	return temp;
}

template<class T>
bool array_iterator<T>::operator!=(const array_iterator& iter) const {
	if (m_current != iter.m_current)
		return true;
	else
		return false;
}

template<class T>
bool array_iterator<T>::operator==(const array_iterator& iter) const {
	if (m_current == iter.m_current)
		return true;
	else
		return false;
}
