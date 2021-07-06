#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"
using namespace std;
#define LEFT 1
#define RIGHT 2

int abs(int a)
{
    return (a > 0)? a: -a;
}
int min(int a, int b)
{
    return (a > b)? b: a;
}
int max(int a, int b)
{
    return (a > b)? a: b;
}
// 先写一个int的AVL便于调试
// 事实上我们发现,如果想要提高效率的话必须引入父指针,有点麻烦QAQ
// 现在需要加入一个list,其实完全可以两者共用一组node
// 现在先实现一个int的迭代器
// 然后需要实现泛型编程
class AVL{
private:
    struct link{
        int data;
        link *next, *last;
        link(int x): data(x), next(NULL), last(NULL) {}
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
    // 现在需要注意,调整的时候,链之间的链接顺序不会改变,但是谁指谁是要注意的
    // 换句话说,只要不动data,应该都没什么问题
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
    // 使用modify()函数完成链接
    // modify()的问题在于没有考虑初始的情况
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
    bool _find(const int &x)
    {
        pointer = root;

        while(pointer && pointer->obj->data != x){
            if(pointer->obj->data > x) pointer = pointer->lc;
            else                       pointer = pointer->rc;
        }

        if(!pointer)  return false;
        else          return true; 
    }
    bool _find(const int &x, node* &t) const
    {
        t = root;

        while(t && t->obj->data != x){
            if(t->obj->data > x) t = t->lc;
            else                 t = t->rc;
        }

        if(!t)  return false;
        else    return true; 
    }
    bool _cfind(const int &x) const
    {
        node *p = root;

        while(p && p->obj->data != x){
            if(p->obj->data > x) p = p->lc;
            else                 p = p->rc;
        }

        if(!p)  return false;
        else    return true; 
    }
    // 检查link
    // insert函数怎么办啊啊啊
    void _insert(const int &x, node* &t)
    {
        if(t == NULL){
            t = new node();
            t->obj = new link(x);
            linkage = true;
            return;
        }
        if(x < t->obj->data){
            _insert(x, t->lc);
            // 需要重新排一下链
            modify(t);
            if(height(t->lc) - height(t->rc) == 2)
                (x < t->lc->obj->data)? LL(t): LR(t);
            t->_height = max(height(t->lc), height(t->rc)) + 1;
            return;
        }
        if(x > t->obj->data){
            _insert(x, t->rc);
            // 需要重新排一下链
            modify(t);
            if(height(t->rc) - height(t->lc) == 2)
                (x > t->rc->obj->data)?  RR(t): RL(t);
            t->_height = max(height(t->lc), height(t->rc)) + 1;
            return;
        }
    }
    // _insert应该也没问题
    bool _remove(const int x, node* &t)
    {
        if(t == NULL)   return true;
        if(x == t->obj->data){
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
                if(_remove(tmp->obj->data, t->lc))   return true;
                return adjust(t, LEFT);
            }
        }
        if(x < t->obj->data){
            if(_remove(x, t->lc))   return true;
            else         return adjust(t, LEFT);
        }
        else{
            if(_remove(x, t->rc))   return true;
            else        return adjust(t, RIGHT);
        }
    }
    void _clear(node* t)
    {
        if(t == NULL)   return;
        _clear(t->lc), _clear(t->rc);
        delete t;
    }
    // 借助oj_2111测试AVL树
    int _query(int x, node* t)
    {
        int res = abs(t->obj->data - x);
        if(!t->lc && !t->rc)    
            return res;
        if(t->obj->data == x)
            return 0;
        // 还是得 pruning
        // 0 <= x <= 1e8
        if(t->lc && t->rc){
            if(t->obj->data > x) // 剪掉右边
                return min(res, _query(x, t->lc));
            if(t->obj->data < x) // 剪掉左边
                return min(res, _query(x, t->rc));
        }
        else{
            if(t->lc)
                return min(res, _query(x, t->lc));
            else
                return min(res, _query(x, t->rc));
        }
    }
    void _copy(node* &t, const node* rhs)
    {
        if(rhs == NULL) return;
        t = new node;
        t->_height = rhs->_height;
        _copy(t->lc, rhs->lc);
        _copy(t->rc, rhs->rc);
    }
    void _mid(node* &t)
    {
        if(t == NULL)   return;
        _mid(t->lc);
        t->obj = build;
        build = build->next;
        _mid(t->rc);
    }
    void _print(node *&t)
    {
        int ll, rr;
        ll = (t->lc)? t->lc->obj->data: -1;
        rr = (t->rc)? t->rc->obj->data: -1;
        printf("%d %d %d\n", t->obj->data, ll, rr);
        if(t->lc)    _print(t->lc);
        if(t->rc)    _print(t->rc);
    }
    void _demo()
    {
        link *tmp = header;
        while(tmp->next != tailer){
            tmp = tmp->next;
            printf("%d ", tmp->data);
        }
        while(tmp != header){
            printf("%d ", tmp->data);
            tmp = tmp->last;
        }
        printf("\n");
    }
public:
    class const_iterator;
    class iterator {
    friend class const_iterator;
    private:
        link *obj;
    public:
        iterator():obj(NULL) {}
        iterator(link *rhs):obj(rhs) {}
        iterator(const iterator &other) 
        {
            obj = other.obj;
        }
        iterator operator++(int) 
        {
            iterator tmp(*this);
            obj = obj->next;
            return  tmp;
        }
        iterator & operator++() 
        {
            obj = obj->next;
            return  *this;
        }
        iterator operator--(int) 
        {
            iterator tmp(*this);
            obj = obj->last;
            return  tmp;
        }
        iterator & operator--() 
        {
            obj = obj->next;
            return  *this;
        }
        int & operator*() const 
        {
            return obj->data;
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
        int* operator->() const noexcept {}
    };
    class const_iterator {
        friend class iterator;
        private:
            link *obj;
        public:
            const_iterator():obj(NULL) {}
            const_iterator(link *rhs):obj(rhs) {}
            const_iterator(const const_iterator &other) 
            {
                obj = other.obj;
            }
            const_iterator(const iterator &other) 
            {
                obj = other.obj;
            }
            const_iterator operator++(int) 
            {
                const_iterator tmp(*this);
                obj = obj->next;
                return  tmp;
            }
            const_iterator & operator++() 
            {
                obj = obj->next;
                return  *this;
            }
            const_iterator operator--(int) 
            {
                const_iterator tmp(*this);
                obj = obj->last;
                return  tmp;
            }
            const_iterator & operator--() 
            {
                obj = obj->next;
                return  *this;
            }
            int & operator*() const 
            {
                return obj->data;
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
            int* operator->() const noexcept {}
    };
    AVL(): root(NULL), _size(0) 
    {
        header = new link(-1);
        tailer = new link(-1);
        header->next = tailer;
        tailer->last = header;
        linkage = false;
    }
    ~AVL()
    {
        _clear(root);
        link *tmp = header;
        while(tmp){
            tmp = header->next;
            delete header;
            header = tmp;
        }
    }
    // copy构造函数还没有测试！！
    AVL(const AVL &other)
    {
        // 先分别把链和树弄好
        // 然后对这个弄好的树一波中序遍历,妙哉
        _size = other._size;
        _copy(root, other.root);
        header = new link(-1);
        tailer = new link(-1);
        link *tmp = header, *p = other.header->next;
        for(int i = 0; i < _size; ++i){
            tmp->next = new link(p->data);
            tmp->next->last = tmp;
            tmp = tmp->next;
            p = p->next;
        }
        tailer->last = tmp;
        tmp->next = tailer;
        build = header->next;
        _mid(root);
    }
    AVL & operator=(const AVL &other)
    {
        clear();
        _size = other._size;
        _copy(root, other.root);
        header = new link(-1);
        tailer = new link(-1);
        link *tmp = header, *p = other.header->next;
        for(int i = 0; i < _size; ++i){
            tmp->next = new link(p->data);
            tmp->next->last = tmp;
            tmp = tmp->next;
            p = p->next;
        }
        tailer->last = tmp;
        tmp->next = tailer;
        build = header->next;
        _mid(root);
    }
    int & at(const int &x)
    {
        if(_find(x))      return pointer->obj->data;
        else       throw sjtu::index_out_of_bound();
    }
    const int & at(const int &x) const
    {
        node *t;
        if(_find(x, t))        return t->obj->data;
        else      throw sjtu::index_out_of_bound();
    }
    int & operator[](const int &x) 
    {
        if(_find(x))     return pointer->obj->data;
        else{
            insert(x);
            _find(x);
            return pointer->obj->data;
        }
    }
    const int & operator[](const int &x) const
    {
        node *t;
        if(_find(x, t))        return t->obj->data;
        else      throw sjtu::index_out_of_bound();
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
    void clear()
    {
        _size = 0;
        _clear(root);
        link *tmp = header;
        while(tmp){
            tmp = header->next;
            delete header;
            header = tmp;
        }
    }
    bool empty()
    {
        return(_size)? false: true;
    }
    size_t size()
    {
        return _size;
    }
    size_t count(const int &x) const
    {
        if(_cfind(x)) return 1;
        else          return 0;
    }
    void insert(const int &x)
    {
        if(_find(x)) return;
        _insert(x, root);
        // 可以在这个地方处理特殊情况
        if(linkage && !_size){
            root->obj->last = header;
            header->next = root->obj;
            root->obj->next = tailer;
            tailer->last = root->obj;
            linkage = false;
        }
        ++_size;
    }
    void remove(const int &x)
    {
        if(!_find(x)) return;
        // 先把链调整好
        pointer->obj->last->next = pointer->obj->next;
        pointer->obj->next->last = pointer->obj->last;
        link *tmp = pointer->obj;
        _remove(x, root);
        delete tmp;
        --_size;
    }
    int query(int x)
    {
        return _query(x, root);
    }
    void show_tree()
    {
        _print(root);
    }
    void show_list()
    {
        _demo();
    }
};

int M, opt, obj, x;
int main()
{
    AVL map;
    // 首先需要测试AVl
    for(int i = 1; i <= 10; ++i)
        map[i] = i;
    AVL map1(map), map2;
    map2 = map;
    map1.remove(1), map2.remove(10);
    map1.show_list();
    map2.show_list();
}
