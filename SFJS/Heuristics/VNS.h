//
//  VNS.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 27/09/23.
//

#ifndef VNS_h
#define VNS_h

#include <set>
#include "../CPUTimer.h"
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


typedef struct _VNSSetting : Setting{
    
    _VNSSetting(){}

    
    _VNSSetting(const _VNSSetting& sett):
    alpha(sett.alpha), epsilon(sett.epsilon){};
    
    float alpha = 0; float epsilon = 0;
    
    void setFromMap(const std::map<std::string,std::string>& setting) override {
        if(setting.find("-alpha") == setting.end()) throw std::exception();
        if(setting.find("-epsilon") == setting.end()) throw std::exception();
        alpha = atof(setting.at("-alpha").c_str());
        alpha = atof(setting.at("-epsilon").c_str());
    }
    
    std::string toString() override {
        char res[20];
        std::snprintf(res, 20, "%.4f\t%.4f\t", alpha,epsilon);
        std::string result(res);
        return result;
    }
}VNSSetting;

template< class Solution, class Builder, class LocalSearch >
class VNS : public Metaheuristic<Solution>{

protected:
    Builder* _builder;
    std::vector<LocalSearch*> _localSearchs;
    
public:
    VNS(Builder* builder, std::vector<LocalSearch*> localSearchs, StopRule rule,double stop_argument, std::string name = "VNS"):
    Metaheuristic<Solution>(rule, stop_argument,name), _builder(builder), _localSearchs(localSearchs){};
    ~VNS(){
    };

    virtual Solution runVNS(float alpha, std::mt19937& rand,float epsilon){
        double start = get_cpu_time();
        Solution* initialSolution =  this->isNotInitialized ? _builder->constructSolution(rand, alpha) :  new Solution(this->lastBestSolution);
        unsigned long iteration = 0;
        unsigned long lastUpdate = 0;
        unsigned long updDistance = 0;
        bool exc = true;
        
        double bestOverallFitness = initialSolution->value();
       
        double last_improvment_time = get_cpu_time();
        while(exc)
        {
            //std::cout << "current: " <<current.value() << "\n";
            int k = 0;
            Solution current = *initialSolution;
            while(exc && k < _localSearchs.size())
            {
                iteration++;
               _localSearchs[k]->shake(current,rand,epsilon);
               // assert(current.isFeasible());
               // std::cout << "x_: " << current.value() << "\n";
                _localSearchs[k]->search(current,rand, start);
               // assert(current.isFeasible());
               // std::cout << "x__: " << current.value() << "\n";
                if(current.value() < bestOverallFitness)
                {
                    bestOverallFitness = current.value();
                    if (initialSolution != 0) delete initialSolution;
                    initialSolution = new Solution(current);
                    updDistance = int(fmaxf(iteration - lastUpdate,updDistance));
                    lastUpdate = iteration;
                    last_improvment_time = get_cpu_time();
                    this->_stats._history.push_back(std::pair<double, double>(bestOverallFitness,last_improvment_time -start));
                    k=0;
                }
                else
                {
                    k++;
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
    
    void run( int seed, void* setting) override{
        VNSSetting &_setting = *(VNSSetting*)setting;
        std::mt19937 rand(seed);
        this->lastBestSolution =  runVNS(_setting.alpha, rand, _setting.epsilon);
    };
    
};


#endif /* VNS_h */
