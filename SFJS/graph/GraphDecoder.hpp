
#ifndef GraphDecoder_hpp
#define GraphDecoder_hpp
#include <random>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include "Graph.hpp"


class GraphDecoder
{
public:
	GraphDecoder();
	~GraphDecoder();
	static Graph* Decode(const char* fname, bool wheighted);
	static Graph* DecodeNotSeq(const char* fname, bool wheighted);
	static Graph* BuildGraph(int nodes, float density, std::mt19937& mt_rand);
	//static Graph* Decode(TInstancia* instancia);
	static bool VeryfyGraph(const Graph &g);

};

GraphDecoder::GraphDecoder(){}

Graph* GraphDecoder::Decode(const char* fname, bool wheighted)
{
	std::ifstream f(fname);

	if (f.fail()) {
		std::cout << "Error loading instance\n";
		return 0;
	}

	// Declare variables:
	unsigned int M, N, L;

	// Ignore headers and comments:
	while (f.peek() == '%')
		f.ignore(2048, '\n');

	// Read defining parameters:
	f >> M >> N >> L;

	Graph *g = new Graph();
	bool multy = false;
	if (M == N)
	{
		for (unsigned int l = 0; l < L; l++)
		{
			int m, n; double e;
			//double data;
			if(wheighted)
				f >> m >> n >> e;
			else
			{
				f >> m >> n;
			}
			if(m != n)
			{
				g->AddEdge(m,n);
			}
			else{ multy = true; }
		}
		
	}
	f.close();
	return g;
}

Graph* GraphDecoder::DecodeNotSeq(const char* fname, bool wheighted)
{
	std::ifstream f(fname);

	if(f.fail())
		return 0;

	// Declare variables:
	unsigned int M, N, L;

	// Ignore headers and comments:
	while (f.peek() == '%')
		f.ignore(2048, '\n');

	// Read defining parameters:
	f >> M >> N >> L;

	Graph *g = new Graph();
	std::unordered_map <int,int> vertex_map;

	bool multy = false;
	int index = 1;
	if (M == N)
	{
		for (unsigned int l = 0; l < L; l++)
		{
			int m, n; double e;
			//double data;
			if(wheighted)
				f >> m >> n >> e;
			else
			{
				f >> m >> n;
			}
			
			if(vertex_map.count(m) == 0)
			{
				vertex_map[m] = index;
				index++;
			}

			if(vertex_map.count(n) == 0)
			{
				vertex_map[n] = index;
				index++;
			}

			int v = vertex_map[m];
			int w = vertex_map[n];

			if(m != n)
			{
				g->AddEdge(v, w);
			}
			else{ multy = true; }
		}
		
	}
	f.close();
	return g;
}


GraphDecoder::~GraphDecoder()
{
}

Graph* GraphDecoder::BuildGraph(int nodes, float density, std::mt19937& mt_rand)
{
	Graph *g = new Graph();
	int E_max = (int)(density*(nodes*(nodes-1))/2.0f);
	
	int k = mt_rand.max();
	float max = (float)abs(k);
	int edg = 0;
	std::vector<std::pair<int,int> > rest;
	
	for(int i = 1; i <= nodes; i++)
	{
		for(int j = i+1; j <= nodes; j++)
		{
			int lf = mt_rand();
			float f = (float)lf;
			if(f/max <= density)
			{
				g->AddEdge(i,j);
				edg++;
				if(edg == E_max)
					return g;
			}
			else 
			{
				rest.push_back(std::pair<int,int>(i,j));
			}
		}
	}

	while(edg < E_max || rest.size() == 0)
	{
		int r = mt_rand();
		int pos = r % rest.size();
		g->AddEdge(rest[pos].first, rest[pos].second);
		rest.erase(rest.begin() + pos);
		edg++;
	}
	return g;
	
}


#endif
