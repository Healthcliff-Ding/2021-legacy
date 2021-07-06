#include "Utility.hpp"
#include "Graph.hpp"
#ifndef BINHEAP_HPP
#define BINHEAP_HPP

class binHeap{
private:
    vector<pair<int,double>> elem;
    void pclup(int posi) {
        auto value = elem[posi];
        while(posi > 1 && value.second > elem[posi/2].second){
            elem[posi] = elem[posi/2];
            posi /= 2;
        }
        elem[posi] = value;
    }
    void pcldown(int posi) {
        auto value = elem[posi];
        int tmp;
        while(2*posi <= elem.size()) {
            tmp = posi * 2;
            if(tmp != elem.size() && elem[tmp+1].second > elem[tmp].second)   ++tmp;
            if(elem[tmp].second > value.second)   
                elem[posi] = elem[tmp];
            else 
                break;
            posi = tmp;
        }
        elem[posi] = value;
    }
public:
    binHeap() {
        elem.emplace_back(make_pair(-1, -2147483648));
    }
    bool empty() const {
        return elem.size() == 1;
    }
    int size() const {
        return elem.size() - 1;
    }
    void clear() {
        elem.clear();
        elem.emplace_back(make_pair(-1, -1));
    }
    void push_back(pair<int,double> _value) {
        elem.emplace_back(_value);
        pclup(elem.size() - 1);
    }
    pair<int,double> pop_front() {
        auto tmp = elem[1];
        elem[1] = elem.back();
        elem.erase(elem.end() - 1);
        pcldown(1);
        return tmp;
    }
    pair<int,double> top() const {
        return elem[1];
    }
};

#endif //BINHEAP_HPP