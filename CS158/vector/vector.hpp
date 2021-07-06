#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <malloc.h>
#include <memory.h>

// 注意细节,先填主体,注意要看测试点
namespace sjtu {
/*
 * a data container like std::vector
 * store data in a successive memory and support random access iterator.
 */
const int DEFAULT_CAPACITY = 20;
template<typename T>
class vector {
private:
	T *elem;
	int _size;
	int _capacity;
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 * a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		int _rank;
		vector<T> *prev;
	public:
		iterator(vector<T> *p, int r) : prev(p), _rank(r)  {}
		~iterator() {}
		int rank() const {return _rank;}
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {	// 是否考虑加上out_of_range
			iterator tmp(this->prev, _rank + n);
			return tmp;		// 需不需要处理 this ?? "return a new iterator which pointer n-next element"
		}
		iterator operator-(const int &n) const {	// 是否考虑加上out_of_range
			iterator tmp(this->prev, _rank - n);
			return tmp;
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			if(this->prev != rhs.prev)	throw invalid_iterator();
			return (this->_rank - rhs._rank);
		}
		iterator& operator+=(const int &n) {
			_rank += n;
			return *this;
		}
		iterator& operator-=(const int &n) {
			_rank -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) 
		{
			iterator tmp(this->prev, _rank);
			++_rank;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() 
		{
			++_rank;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) 
		{
			iterator tmp(this->prev, _rank);
			--_rank;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() 
		{
			--_rank;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const
		{
			return (*prev)[_rank];
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const 
		{
			if(this->prev == rhs.prev && this->_rank == rhs._rank)	return true;
			else	return false;
		}
		bool operator==(const const_iterator &rhs) const 
		{
			if(this->prev == rhs.prev && this->_rank == rhs._rank)	return true;
			else	return false;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const 
		{
			if(*this == rhs)	return false;
			else 	return true;
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			if(*this == rhs)	return false;
			else 	return true;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object 只读迭代器.
	 */
	class const_iterator {
	private:
		int _rank;
		const vector<T> *prev;
	public:
		const_iterator(const vector<T> *p, int r) : prev(p), _rank(r)	{}
		~const_iterator() {}
		int rank() const {return _rank;}
		const_iterator operator-(const int &n) const {	// 是否考虑加上out_of_range
			const_iterator tmp(this->prev, _rank - n);
			return tmp;
		}
		const_iterator operator+(const int &n) const {	// 是否考虑加上out_of_range
			const_iterator tmp(this->prev, _rank + n);
			return tmp;		// 需不需要处理 this ?? "return a new iterator which pointer n-next element"
		}
		int operator-(const const_iterator &rhs) const {
			if(this->prev != rhs.prev)	throw invalid_iterator();
			return (this->_rank - rhs._rank);
		}
		const_iterator& operator+=(const int &n) {
			_rank += n;
			return *this;
		}
		const_iterator& operator-=(const int &n) {
			_rank -= n;
			return *this;
		}
		const_iterator operator++(int) 	// iter++
		{
			iterator tmp(this->prev, _rank);
			++_rank;
			return tmp;
		}
		const_iterator& operator++() 	// ++iter
		{
			++_rank;
			return *this;
		}
		const_iterator operator--(int) 	// iter--
		{
			iterator tmp(this->prev, _rank);
			--_rank;
			return tmp;
		}
		const_iterator& operator--() 	// --iter
		{
			--_rank;
			return *this;
		}
		const T& operator*() const
		{
			return (*prev)[_rank];
		}
		bool operator==(const iterator &rhs) const 
		{
			if(this->prev == rhs.prev && this->_rank == rhs._rank)	return true;
			else	return false;
		}
		bool operator==(const const_iterator &rhs) const 
		{
			if(this->prev == rhs.prev && this->_rank == rhs._rank)	return true;
			else	return false;
		}
		bool operator!=(const iterator &rhs) const 
		{
			if(*this == rhs)	return false;
			else 	return true;
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			if(*this == rhs)	return false;
			else 	return true;
		}

	};

	vector(int c = DEFAULT_CAPACITY) // default constructor
	{
		_capacity = c;
		_size = 0;
		//elem = new T[_capacity];   // 就是说现在的问题是new操作没考虑是否存在默认构造函数,故需要使用malloc函数
		elem = (T*)malloc(sizeof(T) * _capacity);
		// 老子不初始化了...md还得初始化
		memset(elem, 0, sizeof(T) * _capacity);
	}	
	vector(const vector &other) // copy constructor  这块错了
	{
		_size = 0;
		_capacity = 2 * other.size();
		//elem = new T[_capacity];
		elem = (T*)malloc(sizeof(T) * _capacity);
		while(_size < other.size()){
			elem[_size] = other[_size];
			++_size;
		}
	}

	~vector() 	// 其他的内部变量由操作系统回收
	{ 
		//delete []elem;	// 需要知道万一 T 含有指针,那么需要让T等先析构
		for( int i = 0; i < _size; ++i)	elem[i].~T();
		free(elem);
	}

	vector &operator=(const vector &other) // vector整体复制
	{
		if(*this == other)	return *this;
		//delete []elem;
		free(elem);
		_size = 0;
		_capacity = 2 * other.size();
		//elem = new T[_capacity];
		elem = (T*)malloc(sizeof(T) * _capacity);
		while(_size < other.size()){
			elem[_size] = other[_size];
			++_size;
		}
	}

	bool operator==(vector &other)	// 这个地方可以考虑设置一个接口,如果elem指针指向内存完全相同会不会报错
	{
		if(this->elem == other.elem)	return true;
		else return false;
	}
	bool operator==(const vector &other)
	{
		if(this->elem == other.elem)	return true;
		else return false;
	}
	/**
	 * 得先写一个expand()和shrink()
	 * shrink()就先不写了吧
	 */
	void expand() 
	{
		if(_size < _capacity)	return;
		T *tmp = elem;
		_capacity *= 2;
		//elem = new T[_capacity];
		elem = (T*)malloc(sizeof(T) * _capacity);
		for(int i = 0; i < _size; ++i)	elem[i] = tmp[i];
		//delete []tmp;
		free(tmp);
		} 
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) 
	{
		if(pos < 0 || pos >= _size)	throw index_out_of_bound();
		return elem[pos];
	}
	const T & at(const size_t &pos) const 
	{
		if(pos < 0 || pos >= _size)	throw index_out_of_bound();
		return elem[pos];
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) 
	{
		if(pos < 0 || pos >= _size)	throw index_out_of_bound();
		return elem[pos];
	}
	const T & operator[](const size_t &pos) const 
	{
		if(pos < 0 || pos >= _size)	throw index_out_of_bound();
		return elem[pos];
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const 	
	{
		if(_size == 0)	throw container_is_empty();
		return elem[0];
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const 
	{
		if(_size == 0)	throw container_is_empty();
		return elem[_size-1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() 
	{
		iterator tmp(this, 0);
		return tmp;
	}
	const_iterator cbegin() const 
	{
		const_iterator tmp(this, 0);
		return tmp;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() 
	{
		iterator tmp(this, _size);	// 没有检查行不行? 也就是说需要检查是的_size总是小于_capacity
		return tmp;
	}
	const_iterator cend() const 
	{
		const_iterator tmp(this, _size);
		return tmp;
	}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const 
	{
		if(_size == 0)	return true;
		else return false;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const 
	{
		return _size;
	}
	/**
	 * clears the contents
	 */
	void clear() // 是否要单显式独调用每个元素的析构函数?
	{
		//delete []elem;
		free(elem);
		_size = 0;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) 
	{
		expand();
		int ind = pos.rank();
		for( int i = _size; i > ind; --i)	elem[i] = elem[i-1];
		elem[ind] = value;
		++_size;
		iterator tmp(this, ind);
		return tmp;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) 
	{
		expand();
		for( int i = _size; i > ind; --i)	elem[i] = elem[i-1];
		elem[ind] = value;
		++_size;
		iterator tmp(this, ind);
		return tmp;
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) 
	{
		int ind = pos.rank();
		if(ind == (_size - 1))	return end();
		for( int i = ind; i < _size - 1; ++i)	elem[i] = elem[i+1];
		--_size;
		iterator tmp(this, ind);
		return tmp;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) 
	{
		if(ind >= _size)	throw index_out_of_bound();
		for( int i = ind; i < _size - 1; ++i)	elem[i] = elem[i+1];
		--_size;
		iterator tmp(this, ind);
		return tmp;
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) 
	{
		expand();
		elem[_size] = value;
		++_size;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() 
	{
		if(_size == 0)	throw container_is_empty();
		--_size;
	}
};


}

#endif
