#include "Graph.hpp"
#include "LDG.hpp"
#include "Utility.hpp"
#include "Parser.hpp"
#ifndef CODERUN_HPP
#define CODERUN_HPP
// -------------------------- //
Graph graph;
vector<Graph> subGraph, runTimeSubGraph; 
map<int, int> indexTable;
string filename = "14.txt";
Parser parser(filename);
// -------------------------- //

void _dfs(int u, map<int,bool>& vis, map<int,Vertex>& thisGraph, vector<Vertex>& component) {
    vis[u] = true;
    // the other end of caller's edge is a CUT
    if(thisGraph.find(u) == thisGraph.end()) {
        map<int,Vertex> nextGraph = subGraph[indexTable[u]].to_map();
        // save outcome and continue dfs in next graph
        Vertex& tmp = nextGraph[u];
        component.emplace_back(tmp);
        for(auto& iter : tmp.outDeg)
            if(!vis[iter.v]) _dfs(iter.v, vis, nextGraph, component);
    }
    else {
        // save outcome and continue dfs in this graph
        Vertex& tmp = thisGraph[u];
        component.emplace_back(tmp);
        for(auto& iter : tmp.outDeg)
            if(!vis[iter.v]) _dfs(iter.v, vis, thisGraph, component);
    }
}

vector<Vertex> reachable(const Vertex& v) { // implement a dfs
    // initializition: visit table
    map<int,bool> _vis;
    vector<Vertex> component;
    for(auto& iter : subGraph) {
        map<int,Vertex> tmp = iter.to_map();
        for(auto& iter2 : tmp)
            _vis.insert(make_pair(iter2.first, false));
    }
    // initialization: via nodes ADT gain the nodes
    Vertex thisVertex = v;
    map<int,Vertex> thisGraph = subGraph[indexTable[thisVertex.id]].to_map();
    _dfs(thisVertex.id, _vis, thisGraph, component);
    return component;
}

// -------------------------- //

 double Dijkstra(const Vertex& u, const Vertex& v, const map<int,Vertex>& _component) {
    map<int,double> dist = { make_pair(u.id, 0) };
    while(dist.size() < _component.size()) {
        priority_queue<pair<int,double>,vector<pair<int,double>>,greater2> _len;
        for(auto& iter : dist) {
            for(auto& iter2 : _component.find(iter.first)->second.outDeg) {
                if(dist.find(iter2.v) == dist.end())
                    _len.emplace(make_pair(iter2.v, iter.second + iter2.w));
            }
        }
        pair<int,double> tmp = _len.top();
        dist.insert(make_pair(_component.find(tmp.first)->first, tmp.second));
    }
    return dist[v.id];
}

// -------------------------- //

#endif //CODERUN_HPP
