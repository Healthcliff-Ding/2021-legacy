#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cmath>

namespace sjtu { 

const unsigned int DEFAULT_MAX_SIZE = 32; 
template<class T>
class deque {
private:
    class node {
    public:
        node *last, *next;
        T *data;
    };
    class block {
    public:
        unsigned int cur_size;
        block *last, *next;
        node *start;
    };
    unsigned int _size, block_size;
    node *head, *rear;
    block *header, *tailer;
public:
    class const_iterator;
    class iterator {
    friend class deque<T>;
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        unsigned int _rank;
        node *cur_elem;
        deque<T> *prev;
    public:
        /**
         * return a new iterator which pointer n-next elements
         *   even if there are not enough elements, the behaviour is **undefined**.
         * as well as operator-
         */
        iterator() {}
        iterator(deque<T> *p, unsigned int r, node *e): prev(p), _rank(r), cur_elem(e) {}
        ~iterator() {}
        iterator operator+(const int &n) const {
            if(n <= prev->block_size){
                node *tmp_n = cur_elem;
                for(int i = 0; i < n; ++i){
                    tmp_n = tmp_n->next;
                }
                return iterator(prev, _rank + n, tmp_n);
            }
            else{
                unsigned int tmp_pos = 0, pos = _rank + n;
                node *target;
                block *pointer = prev->header->next;
                if(pos == prev->_size)    return iterator(prev, pos, prev->rear);

                while((tmp_pos + pointer->cur_size) <= pos){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < pos){
                    target = target->next;
                    ++tmp_pos;
                }
                return iterator(prev, pos, target);
            }
        }
        iterator operator-(const int &n) const {
            if(n <= prev->block_size){
                node *tmp_n = cur_elem;
                for(int i = 0; i < n; ++i){
                    tmp_n = tmp_n->last;
                }
                return iterator(prev, _rank - n, tmp_n);
            }
            else{
                unsigned int tmp_pos = 0, pos = _rank - n;
                node *target;
                block *pointer = prev->header->next;

                while((tmp_pos + pointer->cur_size) <= pos){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < pos){
                    target = target->next;
                    ++tmp_pos;
                }
                return iterator(prev, pos, target);
            }
        }
        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const iterator &rhs) const {
            if(prev != rhs.prev)   throw invalid_iterator();
            return _rank - rhs._rank;
        }
        iterator& operator+=(const int &n) {
            if(n <= prev->block_size){
                _rank += n;
                for(int i = 0; i < n; ++i){
                    cur_elem = cur_elem->next;
                }
                return *this;
            }
            else{
                unsigned int tmp_pos = 0;
                _rank += n;
                node *target;
                block *pointer = prev->header->next;
                if(_rank == prev->_size){
                    cur_elem = prev->rear;
                    return *this;
                }

                while((tmp_pos + pointer->cur_size) <= _rank){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < _rank){
                    target = target->next;
                    ++tmp_pos;
                }
                cur_elem = target;
                return *this;
            }
        }
        iterator& operator-=(const int &n) {
            if(n <= prev->block_size){
                _rank -= n;
                for(int i = 0; i < n; ++i){
                    cur_elem = cur_elem->last;
                }
                return *this;
            }
            else{
                unsigned int tmp_pos = 0;
                _rank -= n;
                node *target;
                block *pointer = prev->header->next;

                while((tmp_pos + pointer->cur_size) <= _rank){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < _rank){
                    target = target->next;
                    ++tmp_pos;
                }
                cur_elem = target;
                return *this;
            }
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) 
        {
            iterator tmp(prev, _rank, cur_elem);
            ++_rank;
            cur_elem = cur_elem->next;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() 
        {
            ++_rank;
            cur_elem = cur_elem->next;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) 
        {
            iterator tmp(prev, _rank, cur_elem);
            --_rank;
            cur_elem = cur_elem->last;
            return tmp;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() 
        {
            --_rank;
            cur_elem = cur_elem->last;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const 
        {
            if(_rank < 0 || _rank >= prev->_size) throw invalid_iterator();
            return *(cur_elem->data);
        }
        /**
         * TODO it->field
         */
        T* operator->() const noexcept 
        {
            return cur_elem->data;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return true;
            else    return false;
        }
        bool operator==(const const_iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return true;
            else    return false;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return false;
            else    return true;
        }
        bool operator!=(const const_iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return false;
            else    return true;
        }
    };
    class const_iterator {
        // it should has similar member method as iterator.
        //  and it should be able to construct from an iterator.
        friend class deque<T>;
        private:
            // data members.
            unsigned int _rank;
            node *cur_elem;
            const deque<T> *prev;
        public:
            const_iterator() {}
            const_iterator(const deque<T> *p, unsigned int r, node *e): prev(p), _rank(r), cur_elem(e) {}
            const_iterator(const const_iterator &other) {
                _rank = other._rank;
                prev = other.prev;
                cur_elem = other.cur_elem;
            }
            const_iterator(const iterator &other) {
                _rank = other._rank;
                prev = other.prev;
                cur_elem = other.cur_elem;
            }
            ~const_iterator() {}
            // And other methods in iterator.
            const_iterator operator+(const int &n) const {
            if(n <= prev->block_size){
                node *tmp_n = cur_elem;
                for(int i = 0; i < n; ++i){
                    tmp_n = tmp_n->next;
                }
                return const_iterator(prev, _rank + n, tmp_n);
            }
            else{
                unsigned int tmp_pos = 0, pos = _rank + n;
                node *target;
                block *pointer = prev->header->next;
                if(pos == prev->_size)    return const_iterator(prev, pos, prev->rear);

                while((tmp_pos + pointer->cur_size) <= pos){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < pos){
                    target = target->next;
                    ++tmp_pos;
                }
                return const_iterator(prev, pos, target);
            }
        }
        const_iterator operator-(const int &n) const {
            if(n <= prev->block_size){
                node *tmp_n = cur_elem;
                for(int i = 0; i < n; ++i){
                    tmp_n = tmp_n->last;
                }
                return const_iterator(prev, _rank - n, tmp_n);
            }
            else{
                unsigned int tmp_pos = 0, pos = _rank - n;
                node *target;
                block *pointer = prev->header->next;

                while((tmp_pos + pointer->cur_size) <= pos){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < pos){
                    target = target->next;
                    ++tmp_pos;
                }
                return const_iterator(prev, pos, target);
            }
        }
        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            if(prev != rhs.prev)   throw invalid_iterator();
            return _rank - rhs._rank;
        }
        const_iterator& operator+=(const int &n) {
            if(n <= prev->block_size){
                _rank += n;
                for(int i = 0; i < n; ++i){
                    cur_elem = cur_elem->next;
                }
                return *this;
            }
            else{
                unsigned int tmp_pos = 0;
                _rank += n;
                node *target;
                block *pointer = prev->header->next;
                if(_rank == prev->_size){
                    cur_elem = prev->rear;
                    return *this;
                }

                while((tmp_pos + pointer->cur_size) <= _rank){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < _rank){
                    target = target->next;
                    ++tmp_pos;
                }
                cur_elem = target;
                return *this;
            }
        }
        const_iterator& operator-=(const int &n) {
            if(n <= prev->block_size){
                _rank -= n;
                for(int i = 0; i < n; ++i){
                    cur_elem = cur_elem->last;
                }
                return *this;
            }
            else{
                unsigned int tmp_pos = 0;
                _rank -= n;
                node *target;
                block *pointer = prev->header->next;

                while((tmp_pos + pointer->cur_size) <= _rank){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;

                while(tmp_pos < _rank){
                    target = target->next;
                    ++tmp_pos;
                }
                return *this;
            }
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) 
        {
            iterator tmp(prev, _rank, cur_elem);
            ++_rank;
            cur_elem = cur_elem->next;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() 
        {
            ++_rank;
            cur_elem = cur_elem->next;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) 
        {
            iterator tmp(prev, _rank, cur_elem);
            --_rank;
            cur_elem = cur_elem->last;
            return tmp;
        }
        /**
         * TODO --iter
         */
        const_iterator& operator--() 
        {
            --_rank;
            cur_elem = cur_elem->last;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const 
        {
            if(_rank < 0 || _rank >= prev->_size) throw invalid_iterator();
            return *(cur_elem->data);
        }
        /**
         * TODO it->field
         */
        T* operator->() const noexcept 
        {
            return cur_elem->data;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return true;
            else    return false;
        }
        bool operator==(const const_iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return true;
            else    return false;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return false;
            else    return true;
        }
        bool operator!=(const const_iterator &rhs) const 
        {
            if(_rank == rhs._rank && prev == rhs.prev)   return false;
            else    return true;
        }
    };
    /**
     * TODO Constructors
     */
    deque() 
    {
        _size = 0;
        block_size = DEFAULT_MAX_SIZE;
        head = new node;
        rear = new node;
        header = new block;
        tailer = new block;

        // 给两对哨兵节点的元素赋了NULL
        head->next = rear;
        head->last = NULL;
        rear->last = head;
        rear->next = NULL;
        head->data = NULL;
        rear->data = NULL;
        header->last = NULL;
        header->next = tailer;
        tailer->next = NULL;
        tailer->last = header;
        header->start = NULL;
        tailer->start = NULL;
    }
    deque(const deque &other) 
    {
        _size = other._size;
        block_size = other.block_size;
        head = new node;
        rear = new node;
        header = new block;
        tailer = new block;

        head->next = rear;
        head->last = NULL;
        rear->last = head;
        rear->next = NULL;
        head->data = NULL;
        rear->data = NULL;
        header->last = NULL;
        header->next = tailer;
        tailer->next = NULL;
        tailer->last = header;
        header->start = NULL;
        tailer->start = NULL;

        if(other._size == 0)  return;

        node *tmp_n, *other_tmp_n = other.head->next, *target = head;
        block *tmp_b, *pointer = header;

        for(int i = 0; i < _size; ++i){
            tmp_n = new node;
            tmp_n->data = new T(*other_tmp_n->data);
            tmp_n->next = target->next;
            target->next->last = tmp_n;
            tmp_n->last = target;
            target->next = tmp_n;
            target = target->next;
            other_tmp_n = other_tmp_n->next;

            if(i % block_size == 0){
                tmp_b = new block;
                tmp_b->cur_size = 1;
                tmp_b->start = tmp_n;
                tmp_b->last = pointer;
                pointer->next = tmp_b;
                tmp_b->next = tailer;
                tailer->last = tmp_b;
                pointer = pointer->next;
            }
            else{
                ++pointer->cur_size;
            }
        }
    }
    /**
     * TODO Deconstructor
     */
    ~deque() 
    {
        node *tmp_n;
        block *tmp_b;
        while(head->next != rear){
            tmp_n = head->next;
            head->next = tmp_n->next;
            delete tmp_n->data;
            delete tmp_n;
        }
        delete head;
        delete rear;

        while(header->next != tailer){
            tmp_b = header->next;
            header->next = tmp_b->next;
            delete tmp_b;
        }
        delete header;
        delete tailer;
    }
    /**
     * TODO assignment operator
     */
    deque &operator=(const deque &other) 
    {
        if(this->head == other.head)  return *this;

        clear();
        _size = other._size;
        block_size = other.block_size;
        node *tmp_n, *other_tmp_n = other.head->next, *target = head;
        block *tmp_b, *pointer = header;

        for(int i = 0; i < _size; ++i){
            tmp_n = new node;
            tmp_n->data = new T(*other_tmp_n->data);
            tmp_n->next = target->next;
            target->next->last = tmp_n;
            tmp_n->last = target;
            target->next = tmp_n;
            target = target->next;
            other_tmp_n = other_tmp_n->next;

            if(i % block_size == 0){
                tmp_b = new block;
                tmp_b->cur_size = 1;
                tmp_b->start = tmp_n;
                tmp_b->last = pointer;
                pointer->next = tmp_b;
                tmp_b->next = tailer;
                tailer->last = tmp_b;
                pointer = pointer->next;
            }
            else{
                ++pointer->cur_size;
            }
        }
        return *this;
    }
    /**
     * access specified element with bounds checking
     * throw index_out_of_bound if out of bound.
     */
    T & at(const size_t &pos) 
    {
        if(pos < 0 || pos >= _size) throw index_out_of_bound();
        unsigned int tmp_pos = 0;
        node *target;
        block *pointer = header->next;
        while((tmp_pos + pointer->cur_size) <= pos){
            tmp_pos += pointer->cur_size;
            pointer = pointer->next;
        }
        target = pointer->start;
        while(tmp_pos < pos){
            target = target->next;
            ++tmp_pos;
        }
        return *(target->data);
    }
    const T & at(const size_t &pos) const 
    {
        if(pos < 0 || pos >= _size) throw index_out_of_bound();
        unsigned int tmp_pos = 0;
        node *target;
        block *pointer = header->next;

        while((tmp_pos + pointer->cur_size) <= pos){
            tmp_pos += pointer->cur_size;
            pointer = pointer->next;
        }
        target = pointer->start;

        while(tmp_pos < pos){
            target = target->next;
            ++tmp_pos;
        }
        return *(target->data);
    }
    T & operator[](const size_t &pos) 
    {
        if(pos < 0 || pos >= _size) throw index_out_of_bound();
        unsigned int tmp_pos = 0;
        node *target;
        block *pointer = header->next;

        while((tmp_pos + pointer->cur_size) <= pos){
            tmp_pos += pointer->cur_size;
            pointer = pointer->next;
        }
        target = pointer->start;

        while(tmp_pos < pos){
            target = target->next;
            ++tmp_pos;
        }
        return *(target->data);
    }
    const T & operator[](const size_t &pos) const 
    {
        if(pos < 0 || pos >= _size) throw index_out_of_bound();
        unsigned int tmp_pos = 0;
        node *target;
        block *pointer = header->next;
        while((tmp_pos + pointer->cur_size) <= pos){
            tmp_pos += pointer->cur_size;
            pointer = pointer->next;
        }
        target = pointer->start;
        while(tmp_pos < pos){
            target = target->next;
            ++tmp_pos;
        }
        return *(target->data);
    }
    /**
     * access the first element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const 
    {
        if(_size == 0)  throw container_is_empty();
        return *(head->next->data);
    }
    /**
     * access the last element
     * throw container_is_empty when the container is empty.
     */
    const T & back() const 
    {
        if(_size == 0)  throw container_is_empty();
        return *(rear->last->data);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() 
    {
        return iterator(this, 0, head->next);
    }
    const_iterator cbegin() const 
    {
        return const_iterator(this, 0, head->next);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() 
    {
        return iterator(this, _size, rear);
    }
    const_iterator cend() const 
    {
        return const_iterator(this, _size, rear);
    }
    /**
     * checks whether the container is empty.
     */
    bool empty() const 
    {
        if(_size == 0)  return true;
        else  return false;
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
    void clear() 
    {
        node *tmp_n;
        block *tmp_b;
        _size = 0;
        while(head->next != rear){
            tmp_n = head->next;
            head->next = tmp_n->next;
            delete tmp_n->data;
            delete tmp_n;
        }
        rear->last = head;

        while(header->next != tailer){
            tmp_b = header->next;
            header->next = tmp_b->next;
            delete tmp_b;
        }
        tailer->next = header;
    }
    /**
     * inserts elements at the specified locat on in the container.
     * inserts value BEFORE pos
     * returns an iterator pointing to the INSERTED value
     *     throw if the iterator is invalid or it point to a wrong place.
     */
    iterator insert(iterator pos, const T &value) 
    {
        if(pos.prev != this  || pos._rank < 0 || pos._rank > _size) throw invalid_iterator();
        if(pos == begin()){
            push_front(value);
            return iterator(this, 0, head->next);
        }
        else{
            unsigned int tmp_pos, _pos = pos._rank - 1;
            ++_size;
            block_size = (sqrt(_size) > DEFAULT_MAX_SIZE)? sqrt(_size): DEFAULT_MAX_SIZE;
            node *target, *tmp_n = new node;
            block *pointer;

            // _pos是pos前一个元素,这样插入以后插进去的就成_pos了
            if(_pos < (0.618 * _size)){
                tmp_pos = 0;
                pointer = header->next;
                while((tmp_pos + pointer->cur_size) <= _pos){
                    tmp_pos += pointer->cur_size;
                    pointer = pointer->next;
                }
                target = pointer->start;
            }
            else{
                tmp_pos = _size - 1;
                pointer = tailer;
                while(tmp_pos > _pos){
                    tmp_pos -= pointer->last->cur_size;
                    pointer = pointer->last;
                }
                target = pointer->start;
            }
            while(tmp_pos < _pos){
                target = target->next;
                ++tmp_pos;
            }

            tmp_n->data = new T(value);
            target->next->last = tmp_n;
            tmp_n->next = target->next;
            target->next = tmp_n;
            tmp_n->last = target;
            ++pointer->cur_size;
            split(pointer);

            return iterator(this, _pos + 1, tmp_n);
        }

    }
    /**
     * removes specified element at pos.
     * removes the element at pos.
     * returns an iterator pointing to the FOLLOWING element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid or it points to a wrong place.
     */
    iterator erase(iterator pos) 
    {
        if(_size == 0)  throw container_is_empty();
        if(pos.prev != this  || pos._rank < 0 || pos._rank >= _size) throw invalid_iterator();
        unsigned int tmp_pos, _pos = pos._rank;
        --_size;
        block_size = (sqrt(_size) > DEFAULT_MAX_SIZE)? sqrt(_size): DEFAULT_MAX_SIZE;
        node *target, *tmp_n;
        block *pointer;

        // 先找到pos所在的block并将target定位到该cell的头
        if(_pos < (0.618 * _size)){
            tmp_pos = 0;
            pointer = header->next;
            while((tmp_pos + pointer->cur_size) <= _pos){
                tmp_pos += pointer->cur_size;
                pointer = pointer->next;
            }
            target = pointer->start;
        }
        else{
            tmp_pos = _size + 1;
            pointer = tailer;
            while(tmp_pos > _pos){
                tmp_pos -= pointer->last->cur_size;
                pointer = pointer->last;
            }
            target = pointer->start;
        }
        // 特判要删除的头是block的头
        if(tmp_pos == _pos){
            target->last->next = target->next;
            target->next->last = target->last;
            tmp_n = target->next;
            delete target->data;
            delete target;

            pointer->start = tmp_n;
            --pointer->cur_size;
            merge(pointer);
            return iterator(this, _pos, tmp_n);
        }
        // 反之只是blcok里面的一节
        while(tmp_pos < _pos){
            target = target->next;
            ++tmp_pos;
        }
        target->last->next = target->next;
        target->next->last = target->last;
        tmp_n = target->next;
        delete target->data;
        delete target;

        --pointer->cur_size;
        merge(pointer);
        return iterator(this, _pos, tmp_n);
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) 
    {
        if(_size == 0){
            _size = 1;
            node *tmp_n = new node;
            head->next = tmp_n;
            rear->last = tmp_n;
            tmp_n->last = head;
            tmp_n->next = rear;
            tmp_n->data = new T(value);

            block *tmp_b = new block;
            tmp_b->cur_size = 1;
            header->next = tmp_b;
            tailer->last = tmp_b;
            tmp_b->last = header;
            tmp_b->next = tailer;
            tmp_b->start = tmp_n;

        }
        else{
            ++_size;
            block_size = (sqrt(_size) > DEFAULT_MAX_SIZE)? sqrt(_size): DEFAULT_MAX_SIZE;
            node *tmp_n = new node;
            rear->last->next = tmp_n;
            tmp_n->last = rear->last;
            rear->last = tmp_n;
            tmp_n->next = rear;
            tmp_n->data = new T(value);

            ++tailer->last->cur_size;
            split(tailer->last);
        }
            
    }
    /**
     * removes the last element
     *     throw when the container is empty.
     */
    void pop_back() 
    {
        if(_size == 0)  throw container_is_empty();
        --_size;
        block_size = (sqrt(_size) > DEFAULT_MAX_SIZE)? sqrt(_size): DEFAULT_MAX_SIZE;
        node *tmp_n = rear->last;
        tmp_n->last->next = rear;
        rear->last = tmp_n->last;
        delete tmp_n->data;
        delete tmp_n;

        --tailer->last->cur_size;  
        merge(tailer->last);
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) 
    {
        if(_size == 0){
            _size = 1;
            node *tmp_n = new node;
            head->next = tmp_n;
            rear->last = tmp_n;
            tmp_n->last = head;
            tmp_n->next = rear;
            tmp_n->data = new T(value);

            block *tmp_b = new block;
            tmp_b->cur_size = 1;
            header->next = tmp_b;
            tailer->last = tmp_b;
            tmp_b->last = header;
            tmp_b->next = tailer;
            tmp_b->start = tmp_n;

        }
        else{
            ++_size;
            block_size = (sqrt(_size) > DEFAULT_MAX_SIZE)? sqrt(_size): DEFAULT_MAX_SIZE;
            node *tmp_n = new node;
            head->next->last = tmp_n;
            tmp_n->next = head->next;
            head->next = tmp_n;
            tmp_n->last = head;
            tmp_n->data = new T(value);

            ++header->next->cur_size;
            header->next->start = tmp_n;
            split(header->next);
        }
    }
    /**
     * removes the first element.
     *     throw when the container is empty.
     */
    void pop_front() 
    {
        if(_size == 0)  throw container_is_empty();
        --_size;
        block_size = (sqrt(_size) > DEFAULT_MAX_SIZE)? sqrt(_size): DEFAULT_MAX_SIZE;
        node *tmp_n = head->next;
        head->next = tmp_n->next;
        tmp_n->next->last = head;
        delete tmp_n->data;
        delete tmp_n;

        --header->next->cur_size;
        if(header->next->cur_size)  header->next->start = head->next;
        merge(header->next);
    }

    void split(block *b) 
    {
        if(b->cur_size < (2 * block_size))   return;
        else{
            block *tmp_b = new block;
            tmp_b->next = b->next;
            b->next->last = tmp_b;
            tmp_b->last = b;
            b->next = tmp_b;
            tmp_b->cur_size = b->cur_size / 2;
            b->cur_size -= tmp_b->cur_size;
            
            node *tmp_n = b->start;;
            for(int i = 0; i < b->cur_size ; ++i){
                tmp_n = tmp_n->next;
            }
            tmp_b->start = tmp_n;
        }
    }
    void merge(block *b) 
    {
        if(b->cur_size >  (block_size / 2)) return;
        else{
            if(b->cur_size == 0){
                b->last->next = b->next;
                b->next->last = b->last;
                delete b;
                return;
            }
            block *tmp_b = b;
            while(b->cur_size < (2 * block_size) && b->last != header){
                tmp_b = b->last;
                b->cur_size += tmp_b->cur_size;
                b->start = tmp_b->start;
                tmp_b->last->next = b;
                b->last = tmp_b->last;
                delete tmp_b;
            }
            // 如果左边没弄满，到右边继续弄，然而目前这里是有问题的
            while(b->cur_size < (2 * block_size) && b->next != tailer){
                tmp_b = b->next;
                b->cur_size += tmp_b->cur_size;
                tmp_b->next->last = b;
                b->next = tmp_b->next;
                delete tmp_b;
            }
            split(b);
        }
    }
    /**
     * the following functions are just for self test
     */
};

}

#endif
