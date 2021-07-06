#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <queue>
#ifndef UTILITY_HPP
#define UTILITY_HPP
using std::vector; using std::set; using std::map; using std::priority_queue;
using std::make_pair; using std::pair;
using std::ofstream; using std::string; using std::to_string;
using std::cout; using std::cin; using std::endl;
// -------------------------- //

unsigned int MXN = 10;
double MXV = 100;
static unsigned int randSeed = time(NULL);

// -------------------------- //

bool inline skip(const string& str) {
    char ch;
    while(true) {
        cout << str;
        std::cin >> ch;
        if(ch == 'Y' || ch == 'y') return true;
        if(ch == 'N' || ch == 'n') return false;
    }
}

int inline random(int hi) {
    if(hi <= 0) return 0;
    static std::default_random_engine e(randSeed++);
    return e() % hi;
}

vector<int> inline random(int hi, int num) {
    vector<int> _res;
    set<int> _dedup;
    _res.resize(num);
    if(hi <= 0) return _res;
    static std::default_random_engine e(randSeed++);
    int i = 0;
    while(i < num) { 
        int tmp = e() % hi;
        if(_dedup.find(tmp) == _dedup.end()) {
            _res[i++] = tmp;
            _dedup.insert(tmp);
        }
    }
    return _res;
}

// concat vector2 to vector1
template<typename T> void inline vector_concat(vector<T>& vec1, const vector<T>& vec2) {
    for(auto& iter : vec2) {
        vec1.emplace_back(iter);
    }
}

struct greater2 {
    bool operator() (const pair<int,int>& obj1, const pair<int,int>& obj2) {
        return obj1.second >= obj2.second;
    }
};

// -------------------------- //

#endif //UTILITY_HPP
