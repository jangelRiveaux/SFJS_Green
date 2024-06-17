//
//  VNS.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 27/09/23.
//

#ifndef VNS_1_h
#define VNS_1_h

#include <set>
#include "../CPUTimer.h"
#include "VNS.h"
#include "Metaheuristic.h"
/*Function VNS (x, kmax, tmax)
 1: repeat
 2:    k ← 1
 3:    repeat
 4:       x' ← Shake(x, k)                   // Shaking
 5:       x'' ← BestImprovement(x' )         // Local search
 6:       x ← NeighbourhoodChange(x, x'', k) // Change neighbourhood
 7:    until k = kmax
 8:    t ← CpuTime()
 9: until t > tmax*/

template< class Solution, class Builder, class LocalSearch >
class VNS1:  public VNS< Solution,Builder,LocalSearch > {

public:
    VNS1(Builder* builder, std::vector<LocalSearch*> localSearchs, StopRule rule, double stop_argument, std::string name = "VNS_v1"):
    VNS< Solution,Builder,LocalSearch >(builder,localSearchs,rule, stop_argument,name){};
    ~VNS1(){};

    Solution runVNS(float alpha, std::mt19937& rand, float epsilon) override{
        double start = get_cpu_time();
        Solution* initialSolution =  this->isNotInitialized ? this->_builder->constructSolution(rand, alpha) :  new Solution(this->lastBestSolution);
        unsigned long iteration = 0;
        unsigned long lastUpdate = 0;
        unsigned long updDistance = 0;
        bool exc = true;
        
        double bestOverallFitness = initialSolution->value();
       
        double last_improvment_time = get_cpu_time();
        while(exc)
        {
            //std::cout << "current: " <<current.value() << "\n";
            int i = 0;
            while(exc && i < this->_localSearchs.size())
            {
                iteration++;
                Solution current = *initialSolution;
               this->_localSearchs[i]->shake(current,rand,epsilon);
                int j = 0;
               // assert(current.isFeasible());
                while(exc && j < this->_localSearchs.size())
                {
                    double current_val = current.value();
                    // std::cout << "x_: " << current.value() << "\n";
                    this->_localSearchs[j]->search(current,rand,start,true);
                    // assert(current.isFeasible());
                    // std::cout << "x__: " << current.value() << "\n";
                    if(current.value() < current_val)
                    {
                        j=0;
                    }
                    else
                    {
                        j++;
                    }
                   
                }
                
                if(current.value() < bestOverallFitness)
                {
                    bestOverallFitness = current.value();
                    if (initialSolution != 0) delete initialSolution;
                    initialSolution = new Solution(current);
                    updDistance = int(fmaxf(iteration - lastUpdate,updDistance));
                    lastUpdate = iteration;
                    last_improvment_time = get_cpu_time();
                    this->_stats._history.push_back(std::pair<double, double>(bestOverallFitness,last_improvment_time -start));
                    i=0;
                }
                else
                {
                    i++;
                }
                
               // stats.sum += current.value();
               double actual_cputime = get_cpu_time() - start;
                switch (this->_rule) {
                    case StopRule::GENERATIONS:
                        if (iteration >= this->_stop_argument) { exc = false; }
                        break;
                    case StopRule::TARGET:
                        if (bestOverallFitness <= this->_stop_argument || actual_cputime > 180)
                        { exc = false; }
                        break;
                        
                    case StopRule::IMPROVEMENT:
                        if (iteration >= this->_stop_argument) { exc = false; } //if(iterWithoutImprovement >= stopArg) { run = false; }
                        break;
                        //}
                    default:
                        if (actual_cputime >= this->_stop_argument) { exc = false; }
                        break;
                }
            }
        }
        
        
        double end = get_cpu_time();
        double  time_elapsed_in_seconds = (end - start);
        double best_iter_time = (last_improvment_time - start);
        
       this->_stats.generations = (int)iteration;
       this->_stats.solution = bestOverallFitness;
        this->_stats.bestSolution = bestOverallFitness;
        //stats.restarts = cont_reset;
        this->_stats.last_improvment_time = best_iter_time;
        this->_stats.last_improvmentt_iteration = (int)lastUpdate;
        this->_stats.grater_inprovment_distnace = (int)updDistance;
        this->_stats.time = time_elapsed_in_seconds;
        
        //assert(bestSolution.isFeasible());
        Solution sol = *initialSolution;
        delete initialSolution;
        return sol;
    };
};


#endif /* VNS_h */
