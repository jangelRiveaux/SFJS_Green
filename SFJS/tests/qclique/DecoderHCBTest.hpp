//
//  DecoderHCBTest.hpp
//  LRKGA
//
//  Created by Jose Angel Riveaux on 03/09/22.
//

#ifndef DecoderHCBTest_hpp
#define DecoderHCBTest_hpp

#include <stdio.h>
#include <random>
#include <assert.h>
#include "../../qclique_partition/DecoderHCB.hpp"
#include "../../qclique_partition/GeneraPDecoder.hpp"
#include "../../qclique_partition/DynamicGeneralPDecoder.hpp"
#include "../../graph/Graph.hpp"
#include "../../graph/GraphDecoder.hpp"
#include "../../LRKGA/MTRand.h"
#include "../CPUTimer.h"


class DecoderHCBTest{
private:
    static Graph* createTestGraph(int graphId);
    
public:
    static std::vector<double> createChromosome(int n, MTRand& rand);
    
    static bool  testSelectBest();
    
    static bool testGet_Laux();
    
    static bool testHCB();
    
    static bool verify_solution(Graph *g, float gamma, const std::vector<std::set<int> > &sol);
    
    static bool verify_solution(Graph *g, float gamma, const std::vector<std::vector<int>> &sol);
    
    static bool testGetSolution();
    
};

Graph* DecoderHCBTest::createTestGraph(int graphId)
{
    std::mt19937 mt_rand(graphId);
    if(graphId == 0)
        return GraphDecoder::Decode("C:/Users/jange/Desktop/LRKGA_framework/LRKGA/LRKGA/test_instances/test_graph.mtx", false);
    if(graphId == 1)
        return GraphDecoder::BuildGraph(50, 0.50f, mt_rand);
    if(graphId == 2)
        return GraphDecoder::BuildGraph(100, 0.25f, mt_rand);
    if(graphId == 3)
        return GraphDecoder::BuildGraph(200, 0.20f, mt_rand);
    else
        return GraphDecoder::BuildGraph(300, 0.25f, mt_rand);
}

std::vector<double> DecoderHCBTest::createChromosome(int n, MTRand &rand) {
    std::vector<double> chromosome(n, 0);
    for(int c = 0; c < n; c++)
    {
        chromosome[c] =  rand.randExc();
        assert(chromosome[c] >= 0 && chromosome[c] <= 1);
    }
    return chromosome;
}

bool DecoderHCBTest::testSelectBest(){
    for(int i = 0; i < 4; i++)
    {
        Graph *g = createTestGraph(i);
        DecoderHCB decoder(*g, 0.8f, 0.5f);
        int n = g->getNodesCount();
        
        for(int j = 0; j < 30; j++){
            MTRand rand(j+i*100);
            std::vector<double> chromosome = createChromosome(n, rand);
            std::set<int> set;
            for(int v = 0; v < 10; v++){
                int x = (int)rand.randInt(n)+1;
                set.insert(x);
                assert(x > 0 && x <= n);
            }
            int selected = decoder.selectBest(chromosome,  set);
            for(int v : set)
            {
                int index = g->getIndex(v);
                int sIndex = g->getIndex(selected);
                if(chromosome[index] < chromosome[sIndex] )
                {
                    return false;
                }
            }
            
        }
        delete g;
    }
    return true;
}

bool DecoderHCBTest::testGet_Laux(){
    for(int i = 0; i < 4; i++)
    {
        Graph *g = createTestGraph(i);
        DecoderHCB decoder(*g, 0.8f, 0.5f);
        std::map<int,int> comparator;
        for(auto v : g->Vericeses())
        {
            comparator[v] = g->Degree(v);
        }
        std::set<int> l_aux;
        decoder.get_laux(comparator,l_aux);
        //Verifying that all vertices with greater value of comparator than the minimum in l_aux is in l_aux else there is an erro on the method
        int min = g->getNodesCount();
        for(int v : l_aux){
            if(comparator[v] < min){
                min = comparator[v];
            }
        }
        
        for(auto v : comparator){
            if(v.second >  min && l_aux.find(v.first) == l_aux.end())
                return false;
        }
        delete g;
    }
    return true;
}


bool DecoderHCBTest::testHCB(){
    for(int i = 0; i < 4; i++)
    {
        Graph *g = createTestGraph(i);
        DecoderHCB decoder(*g, 0.8f, 0.5f);
        int n = g->getNodesCount();
        
        for(int j = 0; j < 30; j++){
            MTRand rand(j+i*100);
            std::vector<double> chromosome = createChromosome(n, rand);
            auto sol = decoder.HCB(chromosome, *g);
            //std::cout << "solution size: " << sol.size() << "\n";
            if(!g->isQClique(0.8f, sol)) return false;
        }
    }
    return true;
}


bool DecoderHCBTest::verify_solution(Graph *g, float gamma, const std::vector<std::set<int>> &sol) {
    std::set<int> visited;
    int n = g->getNodesCount();
    for(auto qclique : sol)
    {
        if(!g->isQClique(gamma, qclique))
        {
            std::cout<< "Is not a valid qclique partition\n";
            return false;
        }
        for(auto v :qclique)
        {
            if(!visited.insert(v).second)
            {
                std::cout<< "There are same vertices in more than one partition\n";
                return false;
            }
        }
    }
    if(visited.size() != n) {
        std::cout<< "Not all vertices in the partition\n";
        return false;}
    return  true;
}

bool DecoderHCBTest::verify_solution(Graph *g, float gamma, const std::vector<std::vector<int>> &sol) {
    std::vector<std::set<int>> partition;
    for(auto q : sol){
        partition.push_back(std::set<int>(q.begin(),q.end()));
    }
    
    return  verify_solution(g,  gamma, partition);
}

bool DecoderHCBTest::testGetSolution(){
    for(int i = 0; i < 4; i++)
    {
        Graph *g = createTestGraph(i);

        DecoderHCB decoder(*g, 0.8f, 0.5f);
        GeneralDecoder genDecoder(*g,0.8f,false);
        GeneralDecoder genDecoder2(*g, 0.8f, true);
        DynamicGeneralDecoder genDecoder3(*g, 0.8f);
        int n = g->getNodesCount();
        std::cout << "Graph size: " << n << "\n";
        for (int j = 0; j < 30; j++) {
            MTRand rand(j + i * 100);
            std::vector<double> chromosome = createChromosome(n, rand);
            double start = get_cpu_time();
            auto sol = decoder.getSolution(chromosome);
            double t1 = get_cpu_time() - start;
            start = get_cpu_time();
            auto sol2 = genDecoder.getSolution(chromosome);
            double t2 = get_cpu_time() - start;
            start = get_cpu_time();
            auto sol3 = genDecoder2.getSolution(chromosome);
            double t3 = get_cpu_time() - start;
            start = get_cpu_time();
            auto dsol3 = genDecoder3.getSolution(chromosome);
            double t4 = get_cpu_time() - start;
            bool dynamicOk = sol3.size() == dsol3.size();

            if (dynamicOk)
            {
                for (int i = 0; i < sol3.size() && dynamicOk; i++)
                {
                    dynamicOk = sol3[i] == dsol3[i];
                    if (!dynamicOk) break;
                }
            }
            std::cout << "hcb partition size: " << sol.size() <<  "\t"<< t1 << "\n";
            std::cout << "general partition size: " << sol2.size() << "\t" << t2 << "\n";
            std::cout << "general partition with union size: " << sol3.size() << "\t" << t3 << "\n";
            std::cout << "general partition with union size: " << dsol3.size() << "\t" << t4 << "\tvalid:\t" << dynamicOk << "\n";
            if(!verify_solution(g, 0.75f, sol) ||
               !verify_solution(g, 0.75f, sol2) || !verify_solution(g, 0.75f, sol3)) return false;
        }
    }
    return true;
}


#endif /* DecoderHCBTest_hpp */
