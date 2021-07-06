#include "Utility.hpp"
#ifndef GRAPH_HPP
#define GRAPH_HPP
// -------------------------- //

struct token {
    int u, v; 
    double w;
    bool operator==(const token& other) const {
        return (u == other.u && v == other.v && w == other.w);
    }
    token& operator=(const token& other) {
        u = other.u, v = other.v, w = other.w;
        return *this;
    }
    token() = default;
    token(int uu, int vv, double ww): u(uu), v(vv), w(ww) {}
    token(const token& other) { u = other.u, v = other.v, w = other.w; }
};


struct Vertex {
    //template
    int id;
    vector<token> inDeg, outDeg;
    Vertex() = default;
    explicit Vertex(int _id): id(_id) {}
    Vertex(const Vertex& other) {
        id = other.id;
        inDeg = other.inDeg, outDeg = other.outDeg;
    }
    bool operator==(const Vertex& other) const {
        return (id == other.id);
    }
    Vertex& operator=(const Vertex& other) {
        id = other.id;
        inDeg = other.inDeg, outDeg = other.outDeg;
        return *this;
    }
};


class Graph {
private:
    // info holds all Vertices
    map<int, Vertex> V; //template
    set<int> idTable;   //template
    vector<Vertex> _all_vertices; //template

    // -------------- CALLEE --------------- //

    // ============= Module 1 ============== //
    // Add information callee
    // functions titled "add_" should only be the callee of functions titled "add_"
    void add_vertex(const int ID) {
        if(idTable.insert(ID).second)
            V.insert(make_pair(ID, Vertex(ID)));
    }
    bool add_edge(const token& obj) {
        // judge a self-ring or not
        if(obj.u == obj.v) return false;
        // judge an edge exists or not
        if(idTable.find(obj.u)==idTable.end() || idTable.find(obj.v)==idTable.end())
            return false;
        else {
            Vertex tmp = V[obj.u];
            for(auto& iter : tmp.outDeg)
                if(iter.v == obj.v) return false;
        }
        // the edge doesn't exist
        // implement an insertion
        V[obj.u].outDeg.push_back(obj);
        V[obj.v].inDeg.push_back(obj);
        return true;
    }

    // ============= Module 2 ============== //
    // graph DFS callee
    void dfs(int u, map<int,bool>& vis, vector<Vertex>& component) const
    {
        vis[u] = true;
        Vertex tmp = V.find(u)->second;
        component.push_back(tmp);
        for(auto& iter : tmp.outDeg)
            if(!vis[iter.v]) dfs(iter.v, vis, component);
        for(auto& iter : tmp.inDeg)
            if(!vis[iter.u]) dfs(iter.u, vis, component);
    }

public:
    // Constructor and Destructor
    Graph() = default;
    Graph(const Graph& other) {
        V = other.V;
        idTable = other.idTable;
        _all_vertices = other._all_vertices;
    }
    explicit Graph(const vector<Vertex>& nodes) {
        for(auto& iter : nodes) {
            idTable.insert(iter.id);
            V.insert(make_pair((iter.id),iter));
        }
        _all_vertices = nodes;
    }
    Graph& operator=(const Graph& other) = default;

    // Masked value access
    [[nodiscard]] size_t vertex_num() const { return V.size(); }
    [[nodiscard]] size_t edge_num() const {
        size_t sz = 0;
        for(auto& iter : V) {
            auto& inDeg = iter.second.inDeg;
            for(auto& in : inDeg)
                sz += (idTable.find(in.u)==idTable.end())? 0: 1;
        }
        return sz;
    }
    [[nodiscard]] bool empty() const { return V.empty(); }
    [[nodiscard]] map<int,Vertex> to_map() const { return V; }
    [[nodiscard]] vector<Vertex> to_vector() { return _all_vertices; }

    // -------------- CALLER --------------- //
    // ============= Module 1 ============== //

    // Add information caller
    bool add_one_info() {
        token tmp(random(MXN), random(MXN), random(MXV));
        if(V.size() < 2) {
            add_vertex(tmp.u);
            add_vertex(tmp.v);
            add_edge(tmp);
            return true;
        }
        else if(add_edge(tmp)) return true;
        // ensure no single node
        else {
            if(idTable.find(tmp.u) == idTable.end()) {
                add_vertex(tmp.u);
            }
            else if(idTable.find(tmp.v) == idTable.end()) {
                add_vertex(tmp.v);
            }
            return add_edge(tmp);
        }
        // allow single node exsit
        /*
        else {
            if(idTable.find(tmp.u) == idTable.end()) {
                add_vertex(tmp.u);
                return true;
            }
            else if(idTable.find(tmp.v) == idTable.end()) {
                add_vertex(tmp.v);
                return true;
            }
            return false;
        }
        */
    }
    void add_all_info(int num) {
        int i = 0;
        while(i < num) {
            if(add_one_info()) ++i;
        }
    }

    // ============= Module 2 ============== //

    // Connected Component CALLER
    void WCC(vector<std::pair<Graph,bool>>& _res) {
        map<int,bool> _vis;
        for(auto& iter : idTable)
            _vis.insert(make_pair(iter, false));
        // one loop construct one component
        vector<std::pair<Graph,bool>> res;
        for(auto& iter : V) {
            if(!_vis[iter.first]) {
                vector<Vertex> component;
                dfs(iter.first, _vis, component);
                res.emplace_back(Graph(component),false);
            }
        }
        _res = res;
    }

    // Linear Deterministic Greedy CALLER
    [[nodiscard]] double cutEdge(const Vertex& _obj) const { // compute sum of cutEdge value
        // target vertex is in the subgraph
        if(idTable.find(_obj.id) != idTable.end()) return 0;
        // traverse and compute
        double _res = 0;
        for(auto& iter : _obj.outDeg)
            if(idTable.find(iter.v) != idTable.end()) _res += iter.w;
        for(auto& iter : _obj.inDeg )
            if(idTable.find(iter.u) != idTable.end()) _res += iter.w;
        return _res;
    }

    void merge_one_node(const Vertex& obj) {
        if(idTable.find(obj.id) != idTable.end()) return;
        idTable.insert(obj.id);
        V.insert(make_pair(obj.id, obj));
    }

    [[nodiscard]] double totLoss() const {
        double _res = 0;
        for(auto& v : V) {
            for(auto& iter : v.second.outDeg) {
                _res += iter.w;
            }
        }
        return _res;
    }

    // ======== Assistant Functions ======== //
    
    void save_as_file(const string& filename) {
        ofstream ofile(filename);
        string line;
        for(auto& iter : V) {
            if(iter.second.inDeg.empty() && iter.second.outDeg.empty()) {
                line = '<' + to_string(iter.first) + '>';
                ofile << line << '\n';
                line.clear();
            }
            else for(auto& ii : iter.second.outDeg) {
                if(idTable.find(ii.v) != idTable.end()) { // An inner edge
                    line = '<' + to_string(ii.u) + ',' + to_string(ii.v) + ',' + to_string(ii.w) + '>';
                    ofile << line << '\n';
                    line.clear();
                }
                else { // a cross edge
                    line = '<' + to_string(ii.u) + ",*,<" + to_string(ii.v) + ',' + to_string(ii.w) + ">>";
                    ofile << line << '\n';
                    line.clear();
                }
            }
        }
        ofile.close();
    }

    void print_in_Terminal() {
        string line;
        cout << "Vertices:\n";
        for(auto& iter : idTable) {
            line += '<' + to_string(iter) + '>' + ' ';
        }
        cout << line << '\n';
        line.clear();
        cout << "Edges:\n";
        for(auto& iter : V) {
            for(auto& ii : iter.second.outDeg) {
                if(idTable.find(ii.v) != idTable.end()) {
                    line += '<' + to_string(ii.u) + ',' + to_string(ii.v) + ',' + to_string(ii.w) + '>';
                    cout << line << '\n';
                }
                line.clear();
            }
        }
    }
    friend void save_to_check(const string& filename, const vector<Graph>& subGraph);
};

void save_to_check(const string& filename, const vector<Graph>& subGraph) {
    ofstream ofile(filename);
    string line;
    for(auto& _graph : subGraph) {
        if(!_graph.empty()) {
            for(auto& i : _graph.idTable) {
                line += '<' + to_string(i) + '>' + ' ';
            }
            ofile << line << '\n';
            line.clear();
        }
    }
    ofile.close();
}

// -------------------------- //

#endif //GRAPH_HPP