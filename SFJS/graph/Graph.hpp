#ifndef Graph_hpp
#define Graph_hpp

#include <set> 
#include <string>
#include <vector>
#include <map>

class Graph
{
public:
	Graph();

    const std::set<int>& Vericeses() const {return _nodesSet;};
    
   //x: lavel of fiirst vertex
	std::set<int> Neighbords(int x) const { 
        int v = vertex_map.at(x);
        return adjacent_list[v]; 
    }

	std::set<int> NeighbordsIn(int x, const std::set<int>& vertices) const;
    
    std::set<int> NeighbordsIn(int x, const std::vector<int>& vertices) const{
        return  NeighbordsIn(x,  std::set<int>(vertices.cbegin(), vertices.cend()));
    };

	int Degree(int x) const {
        int v = vertex_map.at(x);
        return (int)adjacent_list[v].size();
        
    };

    int getNodesCount() const {return (int)_nodesSet.size();}

	int getEdgeCount() const {return _edges;};
	
	int getEdgeCount(const std::vector<int>& vertices) const ;
    int getEdgeCount(const std::set<int>& vertices)const ;

    
    const std::set<int>& Nodes() const {
        return  _nodesSet;
    }
   //x: lavel of fiirst vertex
   //y: label of secon vertex
	void AddEdge(int x, int y){
        std::pair<std::set<int>::iterator, bool> result;
        _nodesSet.insert(x);
        _nodesSet.insert(y);
        std::map<int, int>::iterator iter1 = vertex_map.find(x);
        std::map<int, int>::iterator iter2 = vertex_map.find(y);

        int v = (int)adjacent_list.size();
        if(iter1 != vertex_map.end())
        { v = iter1->second; }
        else {
            vertex_map[x] = v;
            adjacent_list.push_back(std::set<int>());
        }
        int w = (int)adjacent_list.size();
        if(iter2 != vertex_map.end())
        { w = iter2->second; }
        else {
            vertex_map[y] = w;
            adjacent_list.push_back(std::set<int>());
        }
        
        result = adjacent_list[v].insert(y);
        result = adjacent_list[w].insert(x);

        if(adjacent_list[v].size() > max_degree)
            max_degree = (int)adjacent_list[v].size();
        if(adjacent_list[w].size() > max_degree)
            max_degree = (int)adjacent_list[w].size();

        if(result.second) _edges++;
    };

    int EdgesBetween(const std::set<int>& set1, const std::set<int>& set2) const;

	bool ExistEdge(int x, int y) const{

        int v = vertex_map.at(x);
        return adjacent_list[v].find(y) != adjacent_list[v].end();
    }

    Graph* SubGraph(std::set<int> vertices) const;
    
    int getIndex(int x) const {return vertex_map.at(x); };
    
	bool IsSorted() const
	{
		return is_sorted;
	};
    
    bool isQClique(float gamma, std::set<int> vertices) const {
        int m = getEdgeCount(std::vector<int>(vertices.begin(), vertices.end()));
        int n = (int)vertices.size();
        //std::cout << "n: " << n  << " m: " << m<<"\n";
        return 2.0f*m >= gamma*n*(n-1)-0.1f;
    }
    
	bool Test_Sorted() const
	{
		for(unsigned int i = 1; i < _nodesSet.size(); i++)
		{
			if(Degree(i) < Degree(i+1))
             return false;
		}
        return true;
	};

	std::string name;
private:
    Graph(std::set<int>verices);
	std::vector<std::set<int> > adjacent_list;
    std::set<int> _nodesSet;
	bool is_sorted;
	std::map<int,int> vertex_map; //vertex, index
    int max_degree;
	int _edges;
};

Graph::Graph(){
    max_degree = 0;
}

Graph::Graph(std::set<int> vertices){
    _nodesSet = vertices;
    for(int v : _nodesSet){
        int index = (int)adjacent_list.size();
        adjacent_list.push_back(std::set<int>());
        vertex_map[v] = index;
    }
    max_degree = 0;
}

std::set<int> Graph::NeighbordsIn(int x, const std::set<int>& vertices) const{
    std::set<int> result;
    for(auto y : vertices)
    {
        if(ExistEdge(x,y))
            result.insert(y);
    }
    return result;
}

Graph* Graph::SubGraph(std::set<int> vertices) const{
    Graph* result = new Graph(vertices);
    if(vertices.size() > max_degree){ // otimizing with O(n*max(maxDegree,n) where n = |verttices|
        for(std::set<int>::iterator itr =vertices.begin(); itr != vertices.end(); itr++){
            std::set<int> neighbords = Neighbords(*itr);
            for(std::set<int>::iterator itr2 =neighbords.begin(); itr2 != neighbords.end(); itr2++){
                if(*itr < *itr2 && vertices.find(*itr2) != vertices.end()){
                    result->AddEdge(*itr,*itr2);
                }
            }
        }
    }
    else{
        std::vector<int> vect(vertices.begin(), vertices.end());
        for(std::vector<int>::iterator itr =vect.begin(); itr != vect.end(); itr++){
            for(std::vector<int>::iterator itr2 = vect.begin()+1; itr2 != vect.end(); itr2++){
                if(ExistEdge(*itr,*itr2)){
                    result->AddEdge(*itr,*itr2);
                }
            }
        }
    }
    return result;
}

int Graph::getEdgeCount(const std::vector<int>& vertices)const {
    int m = 0;
    for(std::vector<int>::const_iterator itr = vertices.begin(); itr != vertices.end(); itr++)
    {
        std::vector<int>::const_iterator itr2 = itr + 1;
        for(; itr2 != vertices.end(); itr2++){
            if(ExistEdge(*itr, *itr2)) m++;
        }
    }
    return m;
}

int Graph::getEdgeCount(const std::set<int>& vertices)const {
    std::vector<int> vect(vertices.begin(), vertices.end());
    return getEdgeCount(vect);
}

int Graph::EdgesBetween(const std::set<int>& set1, const std::set<int>& set2) const {
    int m = 0;
    for (int v : set1)
    {
        m += (int)NeighbordsIn(v, set2).size();
    }
    return m;
}

#endif
