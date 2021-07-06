#include "Utility.hpp"
#include "Graph.hpp"
#include "LDG.hpp"
#include "codeRun.hpp"
#include "Parser.hpp"

int main()
{
    string str = "data.txt";
    Parser parser(str);
    Graph tmpGraph = parser.to_graph();
    cout << tmpGraph.edge_num();

}