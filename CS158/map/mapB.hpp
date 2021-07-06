/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP
#define LEFT 1
#define RIGHT 2

// only for std::less<T>
// 不看这句话呜呜呜
#include <functional>
#include <cstddef>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less<Key>
> class map {
public:
    typedef pair<const Key, T> value_type;
private:
    Compare _less = Compare();
    int max(int a, int b)
    {
        return (a > b)? a: b;
    }
    struct link{
        value_type* data;
        link *next, *last;
        link(value_type x): next(NULL), last(NULL) 
        {
            data = new value_type(x);
        }
        link(): data(NULL), next(NULL), last(NULL) {}
    };
    struct node{
        int _height;
        link* obj;
        node* lc, *rc;
        node(): lc(NULL), rc(NULL), _height(0) {}
    };
    node *root, *pointer;
    link *header, *tailer, *build;
    size_t _size;
    bool linkage;
    int height(node *t) const
    {
        if(t == NULL)   return -1;
        else    return t->_height;
    }
    // rotating
    void LL(node* &t)
    {
        node *tmp = t->lc;
        t->lc = tmp->rc;
        tmp->rc = t;
        t->_height = max(height(t->lc), height(t->rc)) + 1;
        tmp->_height = max(height(tmp->lc), height(t)) + 1;
        t = tmp;
    }
    void RR(node* &t)
    {
        node* tmp = t->rc;
        t->rc = tmp->lc;
        tmp->lc = t;
        t->_height = max(height(t->lc), height(t->rc)) + 1;
        tmp->_height = max(height(tmp->rc), height(t)) + 1;
        t = tmp;
    }
    void LR(node* &t)
    {
        RR(t->lc);
        LL(t);
    }
    void RL(node* &t)
    {
        LL(t->rc);
        RR(t);
    }
    // handle unbalance situation
    bool adjust(node* &t, int sub)
    {
        if(sub == RIGHT){
            if(height(t->lc) - height(t->rc) == 1)  return true;
            if(height(t->rc) == height(t->lc)){
                --t->_height;
                return false;
            }
            if(height(t->lc->rc) > height(t->lc->lc)){
                LR(t);
                return false;
            }
            LL(t);
            if(height(t->rc) == height(t->lc))  return false;
            else    return true;
        }
        else{
            if(height(t->rc) - height(t->lc) == 1)  return true;
            if(height(t->rc) == height(t->lc)){
                --t->_height;
                return false;
            }
            if(height(t->rc->lc) > height(t->rc->rc)){
                RL(t);
                return false;
            }
            RR(t);
            if(height(t->rc) == height(t->lc))  return false;
            else    return true;
        }
    }
    // use modify() to modify leaves' father node's linkage
    void modify(node* &t)
    {
        if(!linkage)  return; // 不用再调整了
        if(t->lc && t->obj->last != t->lc->obj){
            t->lc->obj->next = t->obj;
            t->lc->obj->last = t->obj->last;
            t->obj->last->next = t->lc->obj;
            t->obj->last = t->lc->obj;
        }
        if(t->rc && t->obj->next != t->rc->obj){
            t->rc->obj->last = t->obj;
            t->rc->obj->next = t->obj->next;
            t->obj->next->last = t->rc->obj;
            t->obj->next = t->rc->obj;
        }
        linkage = false;
        return;
    }
    // find & locate the pointer
    bool _find(const Key &key)
    {
        pointer = root;
        while(pointer && (_less(key, pointer->obj->data->first) || _less(pointer->obj->data->first, key))){
            if(_less(key, pointer->obj->data->first)) 
                pointer = pointer->lc;
            else                               
                pointer = pointer->rc;
        }
        if(!pointer)  return false;
        else          return true; 
    }
    // find & get a pointer
    bool _find(const Key &key, node* &rhs) const
    {
        rhs = root;
        while(rhs && (_less(key, rhs->obj->data->first) || _less(rhs->obj->data->first, key))){
            if(_less(key, rhs->obj->data->first)) 
                rhs = rhs->lc;
            else                           
                rhs = rhs->rc;
        }
        if(!rhs)  return false;
        else      return true; 
    }
    // just tell whether an element exists
    bool _cfind(const Key &key) const
    {
        node* t = root;
        while(t && (_less(key, t->obj->data->first) || _less(t->obj->data->first, key))){
            if(_less(key, t->obj->data->first)) 
                t = t->lc;
            else                         
                t = t->rc;
        }
        if(!t)  return false;
        else    return true; 
    }
    // insert a tree node
    void _insert(const value_type &value, node* &t)
    {
        if(t == NULL){
            t = new node();
            t->obj = new link(value);
            linkage = true;
            pointer = t;
            return;
        }
        if(_less(value.first, t->obj->data->first)){
            _insert(value, t->lc);
            // rearrange linklist
            modify(t);
            if(height(t->lc) - height(t->rc) == 2)
                (_less(value.first, t->lc->obj->data->first))? LL(t): LR(t);
            t->_height = max(height(t->lc), height(t->rc)) + 1;
            return;
        }
        if(_less(t->obj->data->first, value.first)){
            _insert(value, t->rc);
            // rearrange linklist
            modify(t);
            if(height(t->rc) - height(t->lc) == 2)
                (_less(t->rc->obj->data->first, value.first))?  RR(t): RL(t);
            t->_height = max(height(t->lc), height(t->rc)) + 1;
            return;
        }
    }
    // just remove a tree node
    bool _remove(const Key &key, node* &t)
    {
        if(t == NULL)   return true;
        if(!_less(key, t->obj->data->first) && !_less(t->obj->data->first, key)){
            if(!t->lc || !t->rc){
                node *tmp = t;
                t = (t->lc != NULL)? t->lc: t->rc;
                delete tmp;
                return false;
            }
            else{
                node *tmp = t->lc;
                while(tmp->rc)  tmp = tmp->rc;
                t->obj = tmp->obj;
                if(_remove(tmp->obj->data->first, t->lc))   return true;
                return adjust(t, LEFT);
            }
        }
        if(_less(key, t->obj->data->first)){
            if(_remove(key, t->lc))   return true;
            else           return adjust(t, LEFT);
        }
        else{
            if(_remove(key, t->rc))   return true;
            else          return adjust(t, RIGHT);
        }
    }
    // just clear tree nodes
    void _clear(node* t)
    {
        if(t == NULL)   return;
        _clear(t->lc), _clear(t->rc);
        delete t;
    }
    // just copy tree nodes
    void _copy(node* &t, const node* rhs)
    {
        if(rhs == NULL){
            t = NULL;
            return;
        }
        t = new node;
        t->_height = rhs->_height;
        _copy(t->lc, rhs->lc);
        _copy(t->rc, rhs->rc);
    }
    // use for tightening tree node to link node
    void _mid_build(node* &t)
    {
        if(t == NULL)   return;
        _mid_build(t->lc);
        t->obj = build;
        build = build->next;
        _mid_build(t->rc);
    }
    // functions for visualization
    void _print_tree(node *&t)
    {
        Key *ll, *rr;
        if(t->lc)
            ll = new Key(t->lc->obj->data->first);
        else
            ll = NULL;
        if(t->rc)
            rr = new Key(t->rc->obj->data->first);
        else
            rr = NULL;
        std::cout << t->obj->data->first << ' ';
        std::cout << ll << ' ' << rr << std::endl;
        if(t->lc)    _print_tree(t->lc);
        if(t->rc)    _print_tree(t->rc);
    }
    void _print_list()
    {
        link *tmp = header;
        while(tmp->next != tailer){
            tmp = tmp->next;
            std::cout << tmp->data->first;
        }
        while(tmp != header){
            std::cout << tmp->data->first;
            tmp = tmp->last;
        }
        std::cout << '\n';
    }
public:
    class const_iterator;
    class iterator {
        friend class const_iterator;
        private:
            link* obj;
        public:
            iterator() {
                obj = NULL;
            }
            iterator(link *rhs) {
                obj = rhs;
            }
            iterator(const iterator &other) {
                obj = other.obj;
            }
            iterator operator++(int) 
            {
                iterator tmp(*this);
                if(!obj->next)    throw invalid_iterator();
                obj = obj->next;
                return  tmp;
            }
            iterator & operator++() 
            {
                if(!obj->next)    throw invalid_iterator();
                obj = obj->next;
                return  *this;
            }
            iterator operator--(int) 
            {
                iterator tmp(*this); 
                if(!obj->last->last)  throw invalid_iterator();
                obj = obj->last;
                return  tmp;
            }
            iterator & operator--() 
            {
                if(!obj->last->last)  throw invalid_iterator();
                obj = obj->last;
                return  *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type & operator*() const 
            {
                return *obj->data;
            }
            iterator & operator=(const iterator &rhs)
            {
                obj = rhs.obj;
                return *this;
            }
            bool operator==(const iterator &rhs) const 
            {
                return (obj == rhs.obj)? true: false;  
            }
            bool operator==(const const_iterator &rhs) const 
            {
                return (obj == rhs.obj)? true: false;
            }
            bool operator!=(const iterator &rhs) const 
            {
                return (obj == rhs.obj)? false: true;
            }
            bool operator!=(const const_iterator &rhs) const 
            {
                return (obj == rhs.obj)? false: true;
            }
            value_type* operator->() const noexcept 
            {
                return obj->data;
            }
    };
    class const_iterator {
        friend class iterator;
        private:
            link* obj;
        public:
            const_iterator() {
                obj = NULL;
            }
            const_iterator(link *rhs) {
                obj = rhs;
            }
            const_iterator(const const_iterator &other) {
                obj = other.obj;
            }
            const_iterator(const iterator &other) {
                obj = other.obj;
            }
            const_iterator operator++(int) 
            {
                const_iterator tmp(*this);
                if(!obj->next)    throw invalid_iterator();
                obj = obj->next;
                return  tmp;
            }
            const_iterator & operator++() 
            {
                if(!obj->next)    throw invalid_iterator();
                obj = obj->next;
                return  *this;
            }
            const_iterator operator--(int) 
            {
                const_iterator tmp(*this);
                if(!obj->last->last)  throw invalid_iterator();
                obj = obj->last;
                return  tmp;
            }
            const_iterator & operator--() 
            {
                if(!obj->last->last)  throw invalid_iterator();
                obj = obj->last;
                return  *this;
            }
            value_type & operator*() const
            {
                return *obj->data;
            }
            const_iterator & operator=(const const_iterator &rhs)
            {
                obj = rhs.obj;
                return *this;
            }
            bool operator==(const iterator &rhs) const 
            {
                return (obj == rhs.obj)? true: false;  
            }
            bool operator==(const const_iterator &rhs) const 
            {
                return (obj == rhs.obj)? true: false;
            }
            bool operator!=(const iterator &rhs) const 
            {
                return (obj == rhs.obj)? false: true;
            }
            bool operator!=(const const_iterator &rhs) const 
            {
                return (obj == rhs.obj)? false: true;
            }
            value_type* operator->() const noexcept 
            {
                return obj->data;
            }
    };
    map(): root(NULL), _size(0) 
    {
        header = new link();
        tailer = new link();
        header->next = tailer;
        tailer->last = header;
        linkage = false;
    }
    map(const map &other)
    {
        _size = other._size;
        _copy(root, other.root);
        header = new link();
        tailer = new link();
        link *tmp = header, *p = other.header->next;
        for(int i = 0; i < _size; ++i){
            tmp->next = new link(*p->data);
            // 这句也没问题啊QAQ
            tmp->next->last = tmp;
            tmp = tmp->next;
            p = p->next;
        }
        tailer->last = tmp;
        tmp->next = tailer;
        build = header->next;
        _mid_build(root);
    }
    map & operator=(const map &other) 
    {
        if(root == other.root)  return *this;
        clear();
        _size = other._size;
        _copy(root, other.root);
        link *tmp = header, *p = other.header->next;
        for(int i = 0; i < _size; ++i){
            tmp->next = new link(*p->data);
            tmp->next->last = tmp;
            tmp = tmp->next;
            p = p->next;
        }
        tailer->last = tmp;
        tmp->next = tailer;
        build = header->next;
        _mid_build(root);
        return *this;
    }
    ~map() 
    {
        _clear(root);
        link *tmp = header;
        while(tmp){
            tmp = header->next;
            delete header->data;
            delete header;
            header = tmp;
        }
    }
    T & at(const Key &key) 
    {
        if(_find(key))  return pointer->obj->data->second;
        else            throw index_out_of_bound();
    }
    const T & at(const Key &key) const 
    {
        node *p;
        if(_find(key, p))   return p->obj->data->second;
        else                throw index_out_of_bound();
    }
    // performing an insertion if such key does not already exist.
    T & operator[](const Key &key) 
    {
        if(_find(key))  return pointer->obj->data->second;
        else{
            T tmp;
            value_type value(key, tmp);
            insert(value);
            return pointer->obj->data->second;
        }
    }
    const T & operator[](const Key &key) const 
    {
        node *p;
        if(_find(key, p))   return p->obj->data->second;
        else                throw index_out_of_bound();
    }
    iterator begin() 
    {
        return iterator(header->next);
    }
    const_iterator cbegin() const 
    {
        return const_iterator(header->next);
    }
    iterator end() 
    {
        return iterator(tailer);
    }
    const_iterator cend() const 
    {
        return const_iterator(tailer);
    }
    bool empty() const
    {
        return(_size)? false: true;
    }
    size_t size() const
    {
        return _size;
    }
    void clear() 
    {
        // clear不应该到把header和tailer都删掉的地步
        _clear(root);
        link *p1 = header->next, *p2;
        for(int i = 0; i < _size; ++i){
            p2 = p1->next;
            delete p1->data;
            delete p1;
            p1 = p2;
        }
        header->next = tailer;
        tailer->last = header;
        _size = 0;
        root = NULL;
    }
    pair<iterator, bool> insert(const value_type &value) 
    {
        if(_find(value.first)) {
            iterator tmp(pointer->obj);
            pair<iterator, bool> res(tmp, false);
            return res; 
        }
        _insert(value, root);
        // if map is empty
        if(linkage && !_size){
            root->obj->last = header;
            header->next = root->obj;
            root->obj->next = tailer;
            tailer->last = root->obj;
            pointer = root;
            linkage = false;
        }
        ++_size;
        iterator tmp(pointer->obj);
        pair<iterator, bool> res(tmp, true);
        return res;
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) 
    {
        // 这个地方有问题
        if(pos == end() || !(find((*pos).first) == pos)){
            throw invalid_iterator();
            return;
        }
        pointer->obj->last->next = pointer->obj->next;
        pointer->obj->next->last = pointer->obj->last;
        link *tmp = pointer->obj;
        _remove((*pos).first, root);
        delete tmp->data;
        delete tmp;
        --_size;
    }
    size_t count(const Key &key) const 
    {
        return (_cfind(key))? 1: 0;
    }
    iterator find(const Key &key) 
    {
        if(_find(key))
            return iterator(pointer->obj);
        else
            return end();
    }
    const_iterator find(const Key &key) const 
    {
        node *p;
        if(_find(key, p))
            return const_iterator(p->obj);
        else
            return cend();
    }
};

}

#endif