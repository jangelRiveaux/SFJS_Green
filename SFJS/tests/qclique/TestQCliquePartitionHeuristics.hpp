#pragma once

#ifndef TestQCliquePartitionHeuristics_hpp
#define TestQCliquePartitionHeuristics_hpp

#include <stdio.h>
#include <string>
#include <random>
#include <assert.h>
#include "../../qclique_partition/DecoderHCB.hpp"
#include "../../qclique_partition/GeneraPDecoder.hpp"
#include "../../qclique_partition/DynamicGeneralPDecoder.hpp"
#include "../../qclique_partition/DynamicGeneralDecoder0.hpp"
#include "../../qclique_partition/DynamicFGeneralDecoder0.hpp"
#include "../../qclique_partition/DynamicFGeneralDecoder.hpp"
#include "../../graph/Graph.hpp"
#include "../../graph/GraphDecoder.hpp"
#include "../../LRKGA/MTRand.h"
#include "GraphInstancePool.h"
#include "../CPUTimer.h"
#include "DecoderHCBTest.hpp"
#include "../TestLogManager.h"
#include "../../LRKGA/LRKGA_Executor.hpp"
#include "../../qclique_partition/IteratedGreedy.hpp"

class TestQCliquePartitionHeuristics {

public:

	static void TestDecoders(std::string instance_src, std::string result_dir);

    static void TestDecoderBRKGA(std::string instance_src, std::string result_dir,  float time = 300);
    
    static void TestDecoderLRKGA(std::string instance_src, std::string result_dir);

	static void SettingBRKGA(std::string instance_src, std::string result_dir);

	static void TestIterationGreedyVsBRKGA(std::string instance_src, std::string result_dir);
	
	static void SettingLRKGA(std::string instance_src, std::string result_dir);

	static void TestBRKGAVsLRKGA(std::string instance_src, std::string result_dir);

};



void TestQCliquePartitionHeuristics::TestDecoders(std::string instance_src, std::string result_dir)
{
    std::vector<std::string> instanes = GraphInstancePool::getLInstances();
    std::vector<float> gammas = {/*0.999f, 0.95f, 0.90f, 0.80f, 0.70f, 0.60f, 0.50f,*/ 0.40f, 0.30f};
    int instanceNumber = 0;
    std::string format = "%s\t%d\t%d\t%.3f\t%.3f\t";
    std::string format2 = "%d";
    for (int i = 0; i < 4; i++)
    {
        format += "\t%d\t%d\t%d\t%.6f";
        format2 += "\t%d\t%.6f";
    }
    format += "\n";
    format2 += "\n";
    TestLogManager generalManager(result_dir + "TDTotalResults.log",format,true);
    generalManager.PrintMessage("Testing decoders DecoderHCB, GeneralDecoder (without merge)\n");
    generalManager.PrintMessage("and DynamicGeneralDecoder(with dynamic merge)\n");
    generalManager.PrintMessage("DecoderHCB is testing with alpha = 0.999, 0.95, 0.90, 0.80, 0.70, 0.60, 0.50, 0.40, 0.30\n");
    generalManager.PrintMessage("Result show instance name min solution, solution avg, max solution, avg time, total time for 100 runs\n");
    generalManager.PrintMessage("for HCB in the configuration order shows above, GeneralDecodersand DynamicDeneralDecoder\n");
    
    int executions = 10;

    for (std::string iname : instanes)
    {
        std::string instance_dir = instance_src + iname + ".mtx";
        Graph* g = GraphDecoder::Decode(instance_dir.c_str(),iname !=  "bcspwr06" && iname != "man_5976");
        int n = g->getNodesCount();
        for (float gamma : gammas)
        {

            TestLogManager localManager(result_dir +"td_"+ iname + std::to_string(gamma)+".log", format2, true);
            localManager.PrintMessage("Testing decoders DecoderHCB, GeneralDecoder (without merge)\nand DynamicGeneralDecoder(with dynamic merge)\nDecoderHCB is testing with alpha = 0.999, 0.95, 0.90, 0.80, 0.70, 0.60, 0.50, 0.40, 0.30\nResult show iteration solution, time\nfor HCB in the configuration order shows above, GeneralDecoders and DynamicDeneralDecoder\n");

            std::vector<int> solutions(11,0);
            std::vector<double> times(11,0);
            std::vector<int> minSolution(11,n+1);
            std::vector<int> maxSolution(11,-1);
            instanceNumber++;
            std::vector<const QCliquePartitionDecoder*> decoders;
            /*for (int i = 1; i < 10; i++)
            {
                decoders.push_back(new DecoderHCB(*g, gamma, 0.1f * i));
            }*/
            
            decoders.push_back(new DynamicGeneralDecoder0(*g, gamma));
            decoders.push_back(new DynamicGeneralDecoder(*g, gamma));
            decoders.push_back(new DynamicFGeneralDecoder0(*g, gamma));
            decoders.push_back(new DynamicFGeneralDecoder(*g, gamma));
            std::cout << "Graph size: " << n << "\n";
            for (int j = 0; j < executions; j++) {
                MTRand rand(j + instanceNumber * 100);
                std::vector<double> chromosome = DecoderHCBTest::createChromosome(n, rand);
                std::vector<double> _times;
                std::vector<int> _solution;
                for (int dec = 0; dec < decoders.size(); dec++)
                {
                    const QCliquePartitionDecoder& decoder = *decoders[dec];
                    double start = get_cpu_time();
                    auto sol = decoder.getSolution(chromosome);
                    double t1 = get_cpu_time() - start;
                    _times.push_back((float)t1);
                    _solution.push_back((int)sol.size());
                    if (!DecoderHCBTest::verify_solution(g, gamma, sol)) {
                        char buff[100];
                        snprintf(buff, sizeof(buff), "Error in decorer %d iteration %d", dec, j);
                        localManager.PrintMessage(std::string(buff));
                    }
                }
                char str[300];
                snprintf(str, 300, format2.c_str(), j, _solution[0], _times[0], _solution[1], _times[1],
                         _solution[2], _times[2], _solution[3], _times[3], _solution[4], _times[4]);
                         /*_solution[5], _times[5], _solution[6], _times[6], _solution[7], _times[7],
                         _solution[8], _times[8], _solution[9], _times[9], _solution[10], _times[10]*/
            
                localManager.PrintMessage(std::string(str));
                for (int i = 0; i < 11; i++)
                {

                    solutions[i] += _solution[i];
                    times[i] += _times[i];
                    if(_solution[i] < minSolution[i])
                        minSolution[i] = _solution[i];
                    if (_solution[i] > maxSolution[i])
                        maxSolution[i] = _solution[i];
                }
            }//end run decoders
            int n = g->getNodesCount();
            int m =  g->getEdgeCount();
            char str2[300];
            snprintf(str2, 300, format.c_str(),iname.c_str(),n ,m ,2.0f*m/(n*(n-1)),gamma,
                minSolution[0], solutions[0], maxSolution[0], times[0],
                minSolution[1], solutions[1], maxSolution[1], times[1],
                minSolution[2], solutions[2], maxSolution[2], times[2],
                     minSolution[3], solutions[3], maxSolution[3], times[3]);
                /*minSolution[4], solutions[4], maxSolution[4], times[4],
                minSolution[5], solutions[5], maxSolution[5], times[5],
                minSolution[6], solutions[6], maxSolution[6], times[6],
                minSolution[7], solutions[7], maxSolution[7], times[7],
                minSolution[8], solutions[8], maxSolution[8], times[8],
                minSolution[9], solutions[9], maxSolution[9], times[9],
                minSolution[10], solutions[10], maxSolution[10], times[10]);*/
            generalManager.PrintMessage(str2);
            for (int i = 0; i < decoders.size(); i++)
                delete decoders[i];
        }
        
    delete g;
    }
}

void TestQCliquePartitionHeuristics::TestDecoderBRKGA(std::string instance_src, std::string result_dir, float time)
{
    std::vector<std::string> instanes = GraphInstancePool::getLInstances();
    std::vector<float> gammas = {/*0.999f, */0.95f/*, 0.90f, 0.80f, 0.70f, 0.60f, 0.50f,0.40f, 0.30f */};
    int instanceNumber = 0;
    std::string format = "%s\t%.3f\t";
    std::string format2 = "%d";
    for (int i = 0; i < 5; i++)
    {
        format += "\t%d\t%d\t%d\t%.6f\t%d\t%d";
        format2 += "\t%d\t%.6f\t%.6f\t%d\t%d";
    }
    format += "\n";
    format2 += "\n";
    TestLogManager generalManager(result_dir + "TotalResults.log", format,true);
    generalManager.PrintMessage("#Testing BRKGA  (with and without merge)\n");
    generalManager.PrintMessage("#Result show instance name min solution, solution avg, max solution, avg time, total time for 10 runs\n");

    for (std::string iname : instanes)
    {
        std::string instance_dir = instance_src + iname + ".mtx";
        Graph* g = GraphDecoder::Decode(instance_dir.c_str(), iname !=  "bcspwr06" && iname != "man_5976");
        
        int n = g->getNodesCount();
        int m = g->getEdgeCount() ;
        double dens = 2.0*m / (n*(n-1));
        std::cout << iname << " nodes: "  <<  n << " edges: " << m  <<  " density: " << dens << "\n";
        LRKGASetting setting;
        setting.n = n;
        setting.p = 100;
        setting.pe = 0.2;
        setting.pm = 0.1;
        setting.MAX_THREADS = 1;
        setting.link_population = 0;
        setting.link_factor = 0.1;
        setting.rhoe = 0.7;
        setting.E = 100;
        setting.J = 0;
        setting.M = 0;
        setting.K = 1;
        setting.sense = 1;


        for (float gamma : gammas)
        {

           
                TestLogManager localManager(result_dir + iname + std::to_string(gamma) + ".log", format2, true);
                localManager.PrintMessage("#Testing BRKGA  (with and without merge)\n#Result show iteration solution, time\nfor BRKGA with each decoder\n");

                std::vector<Stats> solutions(11, Stats());
                std::vector<int> minSolution(11, n + 1);
                std::vector<int> maxSolution(11, -1);
                instanceNumber++;
                std::vector<QCliquePartitionDecoder*> decoders;
                //decoders.push_back(new DecoderHCB(*g, gamma, 0.1f));
                //decoders.push_back(new DecoderHCB(*g, gamma, 0.5f));
                //decoders.push_back(new DecoderHCB(*g, gamma, 0.9f));
                 decoders.push_back(new DynamicGeneralDecoder(*g, gamma));
                //decoders.push_back(new DynamicGeneralDecoder0(*g, gamma));
               // decoders.push_back(new DynamicFGeneralDecoder0(*g, gamma));
                //decoders.push_back(new DynamicFGeneralDecoder(*g, gamma));
                
                std::cout << "Graph size: " << n << "\n";
                for (int j = 0; j < 1; j++) {
                   // MTRand rand(j + instanceNumber * 100);
                    //std::mt19937 rnd(j + instanceNumber * 100);
                    std::vector<Stats> loc_solution;
                    loc_solution.push_back(Stats());
                    /*IteratedGreedy greedy(*g,  gamma, 0.09f, StopRule::TIME, time);
                    auto greedy_sol =  greedy.run(j);
                    Stats statistic = greedy.getStats();
                    loc_solution.push_back(statistic);
                    if (!DecoderHCBTest::verify_solution(g, gamma, greedy_sol)) {
                      
                        localManager.PrintMessage("Error in greedy");
                    }*/
                    for (int dec = 0; dec < decoders.size(); dec++)
                    {
                        
                        QCliquePartitionDecoder& decoder = *decoders[dec];
                        LRKGAExecutor<QCliquePartitionDecoder> executor(decoder, StopRule::GENERATIONS, 3);
                        executor.run(setting, j);
                        auto sol = decoder.getSolution(executor.getBestChromosome());
                        Stats statistic = executor.getStats();
                        loc_solution.push_back(statistic);

                        if (!DecoderHCBTest::verify_solution(g, gamma, sol) ||sol.size() != statistic.solution) {
                           // sol = ((DynamicGeneralDecoder*)decoders[0])->getSolution2(executor.getBestChromosome());
                            char buff[100];
                            snprintf(buff, sizeof(buff), "Error in decorer %d iteration %d", dec, j);
                            localManager.PrintMessage(std::string(buff));
                        }
                    }
                    char str[300];
                    snprintf(str, 300,format2.c_str(), j, int(loc_solution[0].solution), float(loc_solution[0].time), float(loc_solution[0].last_improvment_time), loc_solution[0].generations, loc_solution[0].last_improvmentt_iteration,
                            int(loc_solution[1].solution), float(loc_solution[1].time), float(loc_solution[1].last_improvment_time), loc_solution[1].generations, loc_solution[1].last_improvmentt_iteration,
                            int(loc_solution[2].solution), float(loc_solution[2].time), float(loc_solution[2].last_improvment_time), loc_solution[2].generations, loc_solution[2].last_improvmentt_iteration,
                    int(loc_solution[3].solution), float(loc_solution[3].time), float(loc_solution[3].last_improvment_time), loc_solution[3].generations, loc_solution[3].last_improvmentt_iteration,
                             int(loc_solution[4].solution), float(loc_solution[4].time), float(loc_solution[4].last_improvment_time), loc_solution[4].generations, loc_solution[4].last_improvmentt_iteration);
                
                    localManager.PrintMessage(std::string(str));
                        
                    for (int i = 0; i < 5; i++)
                    {
                        solutions[i] += loc_solution[i];
                        if (loc_solution[i].solution < minSolution[i])
                            minSolution[i] = int(loc_solution[i].solution);
                        if (loc_solution[i].solution > maxSolution[i])
                            maxSolution[i           ] = int(loc_solution[i].solution);
                    }
                }//end run LRKGA
                char str2[300];
                snprintf(str2, 300,format.c_str(),  iname.c_str(), gamma,
                        minSolution[0], int(solutions[0].solution), maxSolution[0], float(solutions[0].last_improvment_time), solutions[0].generations, solutions[0].last_improvmentt_iteration ,
                        minSolution[1], int(solutions[1].solution), maxSolution[1], float(solutions[1].last_improvment_time), solutions[1].generations, solutions[1].last_improvmentt_iteration,
                        minSolution[2], int(solutions[2].solution), maxSolution[2], float(solutions[2].last_improvment_time), solutions[2].generations, solutions[2].last_improvmentt_iteration,
                         minSolution[3], int(solutions[3].solution), maxSolution[3], float(solutions[3].last_improvment_time), solutions[3].generations, solutions[3].last_improvmentt_iteration,
                         minSolution[4], int(solutions[4].solution), maxSolution[4], float(solutions[4].last_improvment_time), solutions[4].generations, solutions[4].last_improvmentt_iteration);
                
                generalManager.PrintMessage(std::string(str2));
              
                
                for (int i = 0; i < decoders.size(); i++)
                    delete decoders[i];
            }
        }
    
}

void TestQCliquePartitionHeuristics::TestDecoderLRKGA(std::string instance_src, std::string result_dir)
{
    std::vector<std::string> instanes = GraphInstancePool::getLInstances();
    std::vector<float> gammas = {0.999f, 0.95f, 0.90f, 0.80f, 0.70f, 0.60f, 0.50f, 0.40f, 0.30f };
    int instanceNumber = 0;
    std::string format = "%s\t%.3f\t";
    std::string format2 = "%d";
    for (int i = 0; i < 10; i++)
    {
        format += "\t%d\t%d\t%d\t%.6f\t%d\t%d";
        format2 += "\t%d\t%.6f\t%.6f\t%d\t%d";
    }
    format += "\n";
    format2 += "\n";
    TestLogManager generalManager(result_dir + "TotalResults.log", format, true);
    generalManager.PrintMessage("#Testing BRKGA  (with and without merge)\n");
    generalManager.PrintMessage("#Result show instance name min solution, solution avg, max solution, avg time, total time for 10 runs\n");

    for (std::string iname : instanes)
    {
        std::string instance_dir = instance_src + iname + ".mtx";
        Graph* g = GraphDecoder::Decode(instance_dir.c_str(), false);

        int n = g->getNodesCount();
        LRKGASetting setting;
        setting.n = n;
        setting.p = 100;
        setting.pe = 0.2;
        setting.pm = 0.1;
        setting.MAX_THREADS = 1;
        setting.link_population = 0;
        setting.link_factor = 0.1;
        setting.rhoe = 0.7;
        setting.E = 100;
        setting.J = 0;
        setting.M = 0;
        setting.K = 1;
        setting.sense = 1;


        for (float gamma : gammas)
        {

            if( gamma < 0.45)
            {
                TestLogManager localManager(result_dir + iname + std::to_string(gamma) + ".log", format2, true);
                localManager.PrintMessage("#Testing BRKGA  (with and without merge)\n#Result show iteration solution, time\nfor BRKGA with each decoder\n");

                std::vector<Stats> solutions(11, Stats());
                std::vector<int> minSolution(11, n + 1);
                std::vector<int> maxSolution(11, -1);
                instanceNumber++;
                std::vector<QCliquePartitionDecoder*> decoders;
                //decoders.push_back(new DecoderHCB(*g, gamma, 0.1f));
                //decoders.push_back(new DecoderHCB(*g, gamma, 0.5f));
                //decoders.push_back(new DecoderHCB(*g, gamma, 0.9f));
                //decoders.push_back(new DynamicGeneralDecoder(*g, gamma));
                decoders.push_back(new DynamicGeneralDecoder0(*g, gamma));
                
                std::cout << "Graph size: " << n << "\n";
                for (int j = 0; j < 10; j++) {
                    MTRand rand(j + instanceNumber * 100);

                    std::vector<Stats> loc_solution;
                    for (int dec = 0; dec < decoders.size(); dec++)
                    {
                        QCliquePartitionDecoder& decoder = *decoders[dec];
                        for(int step_size = 0; step_size < 10; step_size++  )
                        {
                            LRKGASetting settiaux(setting);
                            if(step_size > 0)  settiaux.link_factor *= step_size;
                            settiaux.link_population = (step_size < 1 )? 0 :  0.1f;
                            LRKGAExecutor<QCliquePartitionDecoder> executor(decoder, StopRule::TIME, 60);
                            executor.run(settiaux, j);
                            auto sol = decoder.getSolution(executor.getBestChromosome());
                            Stats statistic = executor.getStats();
                            loc_solution.push_back(statistic);
                            
                            if (!DecoderHCBTest::verify_solution(g, gamma, sol) ||sol.size() != statistic.solution) {
                                // sol = ((DynamicGeneralDecoder*)decoders[0])->getSolution2(executor.getBestChromosome());
                                char buff[100];
                                snprintf(buff, sizeof(buff), "Error in decorer %d iteration %d", dec, j);
                                localManager.PrintMessage(std::string(buff));
                            }
                        }
                    }
                    char str[300];
                    snprintf(str,300, format2.c_str(), j, int(loc_solution[0].solution), float(loc_solution[0].time), float(loc_solution[0].last_improvment_time), loc_solution[0].generations, loc_solution[0].last_improvmentt_iteration,
                            int(loc_solution[1].solution), float(loc_solution[1].time), float(loc_solution[1].last_improvment_time), loc_solution[1].generations, loc_solution[1].last_improvmentt_iteration,
                             int(loc_solution[2].solution), float(loc_solution[2].time), float(loc_solution[2].last_improvment_time), loc_solution[2].generations, loc_solution[2].last_improvmentt_iteration,
                             int(loc_solution[3].solution), float(loc_solution[3].time), float(loc_solution[3].last_improvment_time), loc_solution[3].generations, loc_solution[3].last_improvmentt_iteration,
                             int(loc_solution[4].solution), float(loc_solution[4].time), float(loc_solution[4].last_improvment_time), loc_solution[4].generations, loc_solution[4].last_improvmentt_iteration,
                             int(loc_solution[5].solution), float(loc_solution[5].time), float(loc_solution[5].last_improvment_time), loc_solution[5].generations, loc_solution[5].last_improvmentt_iteration,
                             int(loc_solution[6].solution), float(loc_solution[6].time), float(loc_solution[6].last_improvment_time), loc_solution[6].generations, loc_solution[6].last_improvmentt_iteration,
                             int(loc_solution[7].solution), float(loc_solution[7].time), float(loc_solution[7].last_improvment_time), loc_solution[7].generations, loc_solution[7].last_improvmentt_iteration,
                             int(loc_solution[8].solution), float(loc_solution[8].time), float(loc_solution[8].last_improvment_time), loc_solution[8].generations, loc_solution[8].last_improvmentt_iteration,
                             int(loc_solution[9].solution), float(loc_solution[9].time), float(loc_solution[9].last_improvment_time), loc_solution[9].generations, loc_solution[9].last_improvmentt_iteration
                             );
                
                    localManager.PrintMessage(std::string(str));
                        
                    for (int i = 0; i < 2; i++)
                    {
                        solutions[i] += loc_solution[i];
                        if (loc_solution[i].solution < minSolution[i])
                            minSolution[i] = int(loc_solution[i].solution);
                        if (loc_solution[i].solution > maxSolution[i])
                            maxSolution[i           ] = int(loc_solution[i].solution);
                    }
                }//end run LRKGA
                char str2[300];
                snprintf(str2, 300,format.c_str(),  iname.c_str(), gamma,
                        minSolution[0], int(solutions[0].solution), maxSolution[0], float(solutions[0].last_improvment_time), solutions[0].generations, solutions[0].last_improvmentt_iteration ,
                        minSolution[1], int(solutions[1].solution), maxSolution[1], float(solutions[1].last_improvment_time), solutions[1].generations, solutions[1].last_improvmentt_iteration,
                        minSolution[2], int(solutions[2].solution), maxSolution[2], float(solutions[2].last_improvment_time), solutions[2].generations, solutions[2].last_improvmentt_iteration,
                        minSolution[3], int(solutions[3].solution), maxSolution[3], float(solutions[3].last_improvment_time), solutions[3].generations, solutions[3].last_improvmentt_iteration,
                        minSolution[4], int(solutions[4].solution), maxSolution[4], float(solutions[4].last_improvment_time), solutions[4].generations, solutions[4].last_improvmentt_iteration,
                         minSolution[5], int(solutions[5].solution), maxSolution[5], float(solutions[5].last_improvment_time), solutions[5].generations, solutions[5].last_improvmentt_iteration,
                         minSolution[6], int(solutions[6].solution), maxSolution[6], float(solutions[6].last_improvment_time), solutions[6].generations, solutions[6].last_improvmentt_iteration,
                         minSolution[7], int(solutions[7].solution), maxSolution[7], float(solutions[7].last_improvment_time),
                             solutions[7].generations, solutions[7].last_improvmentt_iteration,
                         minSolution[8], int(solutions[8].solution), maxSolution[8], float(solutions[8].last_improvment_time), solutions[8].generations, solutions[8].last_improvmentt_iteration,
                         minSolution[9], int(solutions[9].solution), maxSolution[9], float(solutions[9].last_improvment_time), solutions[9].generations, solutions[9].last_improvmentt_iteration);
                for (int i = 0; i < decoders.size(); i++)
                    delete decoders[i];
            }
        }
    }
}

void TestQCliquePartitionHeuristics::SettingBRKGA(std::string instance_src, std::string result_dir)
{}

void TestQCliquePartitionHeuristics::TestIterationGreedyVsBRKGA(std::string instance_src, std::string result_dir)
{}

void TestQCliquePartitionHeuristics::SettingLRKGA(std::string instance_src, std::string result_dir) 
{}

void TestQCliquePartitionHeuristics::TestBRKGAVsLRKGA(std::string instance_src, std::string result_dir) 
{}

#endif /* DecodersTest_hpp */
