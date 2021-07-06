#include "Utility.hpp"
#include "Graph.hpp"
#ifndef PARSER_HPP
#define PARSER_HPP
using std::fstream;

class Parser {
private:
	vector<token> _all_tokens;
	map<int,Vertex> _all_vertices;
	vector<string> to_token(const string& _str) {
		int len = _str.length(), posi = 0;
		string res;
		vector<string> ans;
		while(posi != len) {
			while(posi != len && !isdigit(_str[posi])) ++posi;
			while(posi != len && (isdigit(_str[posi]) || _str[posi] == '.')) {
				res += _str[posi];
				++posi;
			}
			ans.emplace_back(res);
			res.clear();
		}
		if(ans.back().empty())
			ans.pop_back();
		return move(ans);
	}
	void add_one_token(const vector<string>& _vec) {
		int size = _vec.size(), u(-1), v(-1);
		double w = 0;
		// use for-loop avoid error, if it's a single node
		for(int i = 0; i < size; ++i) {
			if(i == 0)
				u = atoi(_vec[0].c_str());
			else if (i == 1)
				v = atoi(_vec[1].c_str());
			else
				w = atof(_vec[2].c_str());
		}
		if(v == -1)	return;
		else
			_all_tokens.emplace_back(token(u, v, w));
	}
	void to_vertex(const vector<token>& _vec) {
		for(auto& iter : _vec) {
			int u = iter.u, v = iter.v;
			double w = iter.w;
			if(_all_vertices.find(u)==_all_vertices.end())
				_all_vertices.insert(make_pair(u, Vertex(u)));
			if(_all_vertices.find(v)==_all_vertices.end())
				_all_vertices.insert(make_pair(v, Vertex(v)));
			_all_vertices.find(u)->second.outDeg.emplace_back(token(u, v, w));
			_all_vertices.find(v)->second.inDeg.emplace_back(token(u, v, w));
		}
	}
	void read_in(const string &str) {
		fstream originGraph(str);
		if(!originGraph)
			cout << "file stream failed!" << endl;
		string tmp;
		while(getline(originGraph, tmp))
			add_one_token(to_token(tmp));
		to_vertex(_all_tokens);
	}

public:
	Parser(const string& str) {
		read_in(str);
	}
	[[nodiscard]] vector<token> all_tokens() const {
		return _all_tokens;
	}
	[[nodiscard]] map<int,Vertex> all_vertices() const {
		return _all_vertices;
	}
	[[nodiscard]] Graph to_graph() const {
		vector<Vertex> tmp;
		for(auto& iter : _all_vertices) {
			tmp.emplace_back(iter.second);
		}
		return Graph(tmp);
	}
};

#endif //PARSER_HPP