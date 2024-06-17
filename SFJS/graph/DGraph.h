//
//  DGraph.h
//  Models
//
//  Created by Jose Angel Riveaux on 29/06/23.
//
#ifndef DGraph_h
#define DGraph_h

#include <set>
#include <string>
#include <vector>
#include <map>

class DGraph
{
public:
    DGraph();
    DGraph(const DGraph&);
    ~DGraph(){};
    const std::set<int>& Vertices() const {return _nodesSet;};
    
   //x: lavel of fiirst vertex
    const std::set<int>& NeighbordsOut(int x) const {
        int v = vertex_map.at(x);
        return out_adjacent_list[v];
    }
        
    //x: lavel of fiirst vertex
     const std::set<int>& NeighbordsIn(int x) const {
         int v = vertex_map.at(x);
         return in_adjacent_list[v];
     }
    
    int DegreeOut(int x) const {
        int v = vertex_map.at(x);
        return (int)out_adjacent_list[v].size();
        
    };
    
    const std::map<int,int>& AllDegreeOut() const {
        return  degrees_out;
    };
    
    const std::map<int,int>& AllDegreeIn() const {
   
        return degrees_in;
        
    };
    
    int DegreeIn(int x) const {
        int v = vertex_map.at(x);
        return (int)in_adjacent_list[v].size();
        
    };

    int getNodesCount() const {return (int)_nodesSet.size();}

    int getArcCount() const {return _edges;};
    

    
    const std::set<int>& Nodes() const {
        return  _nodesSet;
    }
   //x: lavel of fiirst vertex
   //y: label of secon vertex
    void AddArc(int x, int y){
        std::pair<std::set<int>::iterator, bool> result;
        _nodesSet.insert(x);
        _nodesSet.insert(y);
        std::map<int, int>::iterator iter1 = vertex_map.find(x);
        std::map<int, int>::iterator iter2 = vertex_map.find(y);

        int v = (int)out_adjacent_list.size();
        if(iter1 != vertex_map.end())
        {
            v = iter1->second;
            
        }
        else {
            vertex_map[x] = v;
            degrees_in[x] = degrees_out[x] = 0;
            out_adjacent_list.push_back(std::set<int>());
            in_adjacent_list.push_back(std::set<int>());
        }
        int w = (int)out_adjacent_list.size();
        if(iter2 != vertex_map.end())
        { w = iter2->second; }
        else {
            vertex_map[y] = w;
            degrees_in[y] = degrees_out[y] = 0;
            out_adjacent_list.push_back(std::set<int>());
            in_adjacent_list.push_back(std::set<int>());
        }
        
        result = out_adjacent_list[v].insert(y);
        result = in_adjacent_list[w].insert(x);

        if(out_adjacent_list[v].size() > out_max_degree)
            out_max_degree = (int)out_adjacent_list[v].size();
        if(in_adjacent_list[w].size() > in_max_degree)
            in_max_degree = (int)in_adjacent_list[w].size();
        arcs.insert(std::pair<int,int>(x,y));
        if(result.second) _edges++;
        degrees_out[x]++;
        degrees_in[y]++;
    };

     bool ExistArc(int x, int y) const{

        int v = vertex_map.at(x);
        return out_adjacent_list[v].find(y) != out_adjacent_list[v].end();
    }

    
    int getIndex(int x) const {return vertex_map.at(x); };
    
    const std::set<std::pair<int, int>>& getArcs() const {return arcs;};
    
    
    void BuildPredecesorsLists(){
        std::map<int,int> degs= degrees_in;
        predecessors =  std::vector<std::set<int> > (in_adjacent_list.size(),std::set<int>());
        std::vector<int> C;
        for(auto v: degs)
        {
            if(v.second == 0)
            {
                C.push_back(v.first);
                topologyList.push_back(v.first);
            }
        }
        
        while(!C.empty())
        {
            int v = C[0];
            C.erase(C.begin());
            for(int w : out_adjacent_list[vertex_map[v]])
            {
                degs[w]--;
                predecessors[w].insert(predecessors[v].begin(), predecessors[v].end());
                predecessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    topologyList.push_back(w);
                }
            }
        }
    };
    
    void BuildSucessorsLists(){
        std::map<int,int> degs = degrees_out;
        sucessors =  std::vector<std::set<int> > (out_adjacent_list.size(),std::set<int>());
        std::vector<int> C;
        for(auto v: degs)
        {
            if(v.second == 0)
            {
                C.push_back(v.first);
                invertedTopologyList.push_back(v.first);
            }
        }
        
        while(!C.empty())
        {
            int v = C[0];
            C.erase(C.begin());
            for(int w : in_adjacent_list[vertex_map[v]])
            {
                degs[w]--;
                sucessors[w].insert(sucessors[v].begin(), sucessors[v].end());
                sucessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    invertedTopologyList.push_back(w);
                }
            }
        }
    };
    
    
    const std::set<int>& getPredecessors(int v) const { return predecessors[v]; };
    
    const std::set<int>& getSucessors(int v) const {return sucessors[v]; };
    
    
    const std::vector<int>& getTopologyList() const{return topologyList; };
    
    
    const std::vector<int>& getInvertedTopologyList() const {return invertedTopologyList; };
    
    std::string name;
private:
    DGraph(std::set<int>verices);
    std::vector<std::set<int> > out_adjacent_list, in_adjacent_list, sucessors, predecessors;
    std::vector<int> topologyList;
    std::vector<int> invertedTopologyList;
    std::set<std::pair<int, int>> arcs;
    std::set<int> _nodesSet;
    std::map<int,int> vertex_map;
    std::map<int,int> degrees_in;
    std::map<int,int> degrees_out; //vertex, index
    int out_max_degree, in_max_degree;
    int _edges;
};

DGraph::DGraph(){
    out_max_degree = in_max_degree = 0;
}

DGraph::DGraph(std::set<int> vertices){
    _nodesSet = vertices;
    for(int v : _nodesSet){
        int index = (int)out_adjacent_list.size();
        out_adjacent_list.push_back(std::set<int>());
        in_adjacent_list.push_back(std::set<int>());
        vertex_map[v] = index;
    }
    out_max_degree = in_max_degree = 0;
}

DGraph::DGraph(const DGraph& graph){
    _nodesSet = graph._nodesSet;
    out_adjacent_list = graph.out_adjacent_list;
    in_adjacent_list = graph.in_adjacent_list;
    vertex_map = graph.vertex_map;
    out_max_degree = graph.out_max_degree;
    in_max_degree = graph.in_max_degree;
    degrees_in = graph.degrees_in;
    degrees_out = graph.degrees_out;
    _edges = graph._edges;
    predecessors = graph.predecessors;
    sucessors = graph.sucessors;
    topologyList = graph.topologyList;
    invertedTopologyList = graph.invertedTopologyList;
}
#endif /* DGraph_h */
