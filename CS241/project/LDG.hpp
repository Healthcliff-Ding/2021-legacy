#include "Graph.hpp"
#include "Utility.hpp"
#include "binHeap.hpp"
#ifndef LDG_HPP
#define LDG_HPP
// -------------------------- //

// An Implement of LDG Algorithm

// ---- Hyper Parameters ---- //

constexpr double BETA = 1.00; // this hyperparameter does harm to the outcome www
constexpr unsigned int CAPACITY = 10;

// -------------------------- //

unsigned int stride = 0;
double crossWeight = 0, minWeight = int(0x7fffffff);
unsigned int initialSeed = time(NULL);

// -------------------------- //

int partition(const Vertex& v, vector<Graph>& G) { // vertex number id v, number of subGraph
    binHeap score;
    vector<int> crossEdge;
    unsigned int ind = 0, sum = 0;
    for(auto& iter : G) {
        // neighbours in a partition
        int cutEdge = iter.cutEdge(v);
        crossEdge.push_back(cutEdge);
        sum += cutEdge;
        double penalty = 1 - (iter.vertex_num() + 0.5) / CAPACITY;
        if(penalty > 0) score.push_back(make_pair(ind, cutEdge * penalty));
        ++ind;
    }
    ind = score.top().first;
    crossWeight += (sum - crossEdge[ind]);
    return ind;
}


vector<Graph> LDG(Graph& _graph) { // implement a LDG algorithm on a subGraph
    vector<Graph> _res;
    vector<Vertex> _all_nodes = _graph.to_vector();
    //map<int,Vertex> _all_nodes = _graph.to_map();
    unsigned int tot = (_graph.vertex_num()-1) / CAPACITY + 1; // number of derived subGraphs
    _res.resize(tot);
    // initialization: select tot-initial subGraph
    /**TODO
     * problem is that we need different initializations
     * but we don't know where or what to initialize
     */
    int i = 0;
    /*
    auto iter = _all_nodes.begin();
    for(; i < tot; ++i, ++iter) {
        vector<Vertex> tmp = { iter->second };
        _res[i] = Graph(tmp);
    }
    for(; i < _all_nodes.size(); ++i, ++iter) {
        int ind = partition(iter->second, _res);
        _res[ind].merge_one_node(iter->second);
    }
    */
    vector<bool> fetched;
    vector<Vertex> _initial;
    vector<int> _randomize = random(_all_nodes.size(), tot);
    putchar('\n');
    fetched.resize(_all_nodes.size());
    for(; i < tot; ++i) {
        vector<Vertex> tmp = { _all_nodes[_randomize[i]] };
        fetched[_randomize[i]] = true;
        _res[i] = Graph(tmp);
        _initial.emplace_back(tmp[0]);
    }
    /*
    for(; i < tot - 1; ++i) {
        vector<Vertex> tmp = { _all_nodes[i * CAPACITY + stride] };
        fetched[i * CAPACITY + stride] = true;
        _res[i] = Graph(tmp);
        _initial.emplace_back(tmp[0]);
    }
    if(i * CAPACITY + stride < _all_nodes.size()) {
        vector<Vertex> tmp = { _all_nodes[i * CAPACITY + stride] };
        fetched[i * CAPACITY + stride] = true;
        _res[i] = Graph(tmp);
        _initial.emplace_back(tmp[0]);
    }
    else {
        vector<Vertex> tmp = { _all_nodes[i * CAPACITY] };
        fetched[i * CAPACITY] = true;
        _res[i] = Graph(tmp);
        _initial.emplace_back(tmp[0]);
    } */

    // add initial loss
    for(auto& vv : _initial)
        for(auto& iter : _res)
            crossWeight += iter.cutEdge(vv);
    // main -loop
    for(i = 0; i < _all_nodes.size(); ++i) {
        if(!fetched[i]) {
            int ind = partition(_all_nodes[i],  _res);
            _res[ind].merge_one_node(_all_nodes[i]);
        }
    }
    return _res;
}

// -------------------------- //

#endif //LDG_HPP