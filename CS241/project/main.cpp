#include <iostream>
#include "Utility.hpp"
#include "Graph.hpp"
#include "LDG.hpp"
#include "codeRun.hpp"
#include "Parser.hpp"

string phase = "senario 3"; // "senario 2", "senario 3", "debug"

int main()
{
    // ============= Module 1 ============== //
    if(phase == "senario 1" || phase == "debug") {

        // Useful code when checking

        // Version 1
        /*
        cout << "Input max number of vertices" << endl;
        cin >> MXN;
        cout << "Input max value of edge weight" << endl;
        cin >> MXV;
        cout << "Constructing 10 graphs......\n=========================\n";
        for(int i = 0; i < 10; ++i) {
            Graph tmpGraph;
            cout << "Graph " << i+1 << endl; 
                cout << "Construct an input file with x lines info. Input x " << endl;
                int x;
                cin >> x;
                tmpGraph.add_all_info(x);
            if(skip(string("Add y lines to the graph file <Y/N>\n"))) {
                int y;
                cout << "Input parameter y:\n";
                cin >> y;
                tmpGraph.add_all_info(y);
            }
            cout << "Vertex: " << tmpGraph.vertex_num() << "  Edge: " << tmpGraph.edge_num() << std::endl;
            //tmpGraph.print_in_Terminal();
            string tmpStr = "graphs\\graph" + to_string(i+1);
            tmpGraph.save_as_file(tmpStr);
        } */

        // Version 2
        /*
        cout << "Input max number of vertices" << endl;
        cin >> MXN;
        cout << "Input max value of edge weight" << endl;
        cin >> MXV;
        cout << "Input number of info item" << endl;
        int info;
        cin >> info;
        cout << "Constructing 10 graphs......\n=========================\n";
        for(int i = 0; i < 10; ++i) {
            Graph tmpGraph;
            cout << "Graph " << i+1 << endl; 
                tmpGraph.add_all_info(info);
            }
            cout << "Vertex: " << tmpGraph.vertex_num() << "  Edge: " << tmpGraph.edge_num() << std::endl;
            //tmpGraph.print_in_Terminal();
            string tmpStr = "graphs\\graph" + to_string(i+1);
            tmpGraph.save_as_file(tmpStr);
        } */

        // code for debugging
        if(skip(string("Construct an input file with x lines input <Y/N>\n"))) {
            int x;
            std::cout << "Input parameter x:\n";
            std::cin >> x;
            graph.add_all_info(x);
            std::cout << "Vertex: " << graph.vertex_num() << "  Edge: " << graph.edge_num() << std::endl;
        }
        if(skip(string("Add y lines to the graph file <Y/N>\n"))) {
            int y;
            std::cout << "Input parameter y:\n";
            std::cin >> y;
            graph.add_all_info(y);
            std::cout << "Vertex: " << graph.vertex_num() << "  Edge: " << graph.edge_num() << std::endl;
        }
        graph.save_as_file("originGraph.txt");
        
    }
    else {
        // please make input filename "data.txt", or change it in codeRun.hpp
        graph = parser.to_graph();
        cout << "number of vertices: " << graph.vertex_num() << "\nnumber of edges: " << graph.edge_num()
             << endl;
    }
    
    if(phase == "senario 1") return 0;

    // ============= Module 2 ============== //
    // step 1
    // perform a WCC search
    vector<pair<Graph,bool>> wccInfo;
    graph.WCC(wccInfo);
    // step 2
    // put an wcc with proper size into a sub-graph
    for(auto& component : wccInfo) {
        if(component.first.vertex_num() <= CAPACITY)
            component.second = true;
    }
    auto constWccInfo = wccInfo;
    // step 3
    // perform a LDG on each connected component
    int epoch = 30 * graph.vertex_num();
    for(int k = 0; k < epoch; ++k) {
        crossWeight = 0;
        subGraph.clear();
        wccInfo = constWccInfo;
        for(auto& iter : wccInfo) {
            if(iter.second) {
                subGraph.emplace_back(iter.first);
            }
            else {
                vector<Graph> tmp = LDG(iter.first);
                vector_concat<Graph>(subGraph, tmp);
            }
        }
        int i = 0;
        // demonstrate in Terminal
        if(phase != "debug") {}
        else if(skip(string("Demonstrate subGraph partition in Terminal? <Y/N>\n"))) {
            for(auto& iter : subGraph) {
                string str = "subGraph" + to_string(++i);
                cout << '\n' << str << '\n';
                iter.print_in_Terminal();
            }
            putchar('\n');
            i = 0;
        }
        // choose the best
        if(crossWeight < minWeight) {
            minWeight = crossWeight;
            runTimeSubGraph = subGraph;
        }
        cout << "Epoch" << k+1 << ":\n";
        cout << "Total Cross Edge Weight: " << crossWeight << endl;
        cout << "Total Edge Weight:" << graph.totLoss() << endl; 
    }
    // main-loop end && save the best
    subGraph = runTimeSubGraph;
    int i = 0;
    for(auto& iter : subGraph) {
        if(!iter.empty()) {
            string str = "subGraphs\\subGraph" + to_string(++i) + ".txt";
            iter.save_as_file(str);
        }
        else {
            cout << "An empty subGraph!\n";
        }
    }
    save_to_check("subGraphs\\check.txt", subGraph);
    cout << "\nTotal Minimum Cross Edge Weight: " << minWeight << endl;
    cout << "Total Edge Weight: " << graph.totLoss() << endl; 

    if(phase == "senario 2") return 0;

    // ============= Module 3 ============== //
    // step 1
    // construct an indexTable from verteices to index of subGraph
    for(int i = 0; i < subGraph.size(); ++i) {
        map<int,Vertex> tmp = subGraph[i].to_map();
        for(auto& v : tmp) {
            indexTable.insert(make_pair(v.first, i));
        }
    }
    // step 2
    // Perform a dfs
    if(skip(string("Find all reachable vertices of an assigned vertex? <Y/N>\n"))) {
        int assign;
        cout << "Input the id of vertex queried:\n";
        cin >> assign;
        while(indexTable.find(assign)==indexTable.end()) {
            cout << "Assigned Vertex doen't exist.\n";
            cout << "Input the id of vertex queried:\n";
            cin >> assign;
        }
        Vertex v = subGraph[indexTable[assign]].to_map().find(assign)->second;
        vector<Vertex> reach = reachable(v);
        // demostrate in Terminal
        if(skip(string("Demonstrate all vertices reachable? <Y/N>\n"))) {
            cout << "All vertices reachable to vertex<" << assign << ">:\n";
            for(auto& iter : reach) {
                string str = '<' + to_string(iter.id) + '>';
                cout << str << ' ';
            }
            putchar('\n');
        }
    }
    // step 3
    // Perform a Dijkstra
    if(skip(string("Find shortest path between two assigned vertices? <Y/N>\n"))) {
        int x, y;
        cout << "Input the id of vertices x1, x2 queried for shortest path:\n";
        cin >> x >> y;
        while(indexTable.find(x)==indexTable.end()) {
            cout << "Assigned Vertex x1 doen't exist.\n";
            cout << "Input the id of vertex x1 queried:\n";
            cin >> x;
        }
        while(indexTable.find(y)==indexTable.end()) {
            cout << "Assigned Vertex x2 doen't exist.\n";
            cout << "Input the id of vertex x2 queried:\n";
            cin >> y;
        }
        map<int,Vertex> reach;
        Vertex v1 = subGraph[indexTable[x]].to_map().find(x)->second;
        vector<Vertex> tmp = reachable(v1);
        for(auto& iter : tmp) {
            reach.insert(make_pair(iter.id, iter));
        }
        if(reach.find(v1.id)==reach.end()) {
            cout << "There isn't path from <" << x << "> to <" << y << ">.\n";
            return -2;
        }
        else {
            Vertex v2 = subGraph[indexTable[y]].to_map().find(y)->second;
            cout << "Length of shortest path from <" << x << "> to <" << y << "> is: " << Dijkstra(v1, v2, reach);
        }
    }
    
    return 0;

}
