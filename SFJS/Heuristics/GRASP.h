//
//  GRASP.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 05/12/23.
//

#ifndef GRASP_h
#define GRASP_h

#include <set>
#include "../CPUTimer.h"
#include "Metaheuristic.h"

typedef struct _GRASPSetting : Setting{
    
    _GRASPSetting(){}
    void setFromMap(const std::map<std::string,std::string>& setting) override{
        if(setting.find("-alpha") == setting.end()) throw std::exception();
        alpha = atof(setting.at("-alpha").c_str());
    }
    _GRASPSetting(const _GRASPSetting& sett):
    alpha(sett.alpha){};
    float alpha = 0;
    std::string toString() override {
        char res[20];
        std::snprintf(res, 20, "%.4f\t", alpha);
        std::string result(res);
        return result;
    }
}GRASPSetting;

template< class Solution,class Builder, class LocalSearch >
class GRASP : public Metaheuristic<Solution>{
private:
    LocalSearch* _localSearch;
    Builder* _builder;
    
public:
    GRASP(Builder* builder, LocalSearch* localSearch, StopRule rule,double stop_argument, std::string name = "GRASP"):
         Metaheuristic<Solution>(rule, stop_argument,name),
        _builder(builder),  _localSearch(localSearch){};
        ~GRASP(){ };
    

    Solution runGrasp(std::mt19937& rand, float alpha){
        double start = get_cpu_time();
        unsigned long iteration = 0;
        unsigned long lastUpdate = 0;
        unsigned long updDistance = 0;
        double last_improvment_time = get_cpu_time();
        bool exc = true;
#ifdef _WIN32 ////|| _WIN64
        double bestOverallFitness = DBL_MAX;
#else
        double bestOverallFitness = __DBL_MAX__;
#endif
      
        Solution *bestSolution = 0;// = _builder->constructSolution(_alpha);
        
        
        while(exc)
        {
            //std::cout << "current: " <<current.value() << "\n";
            iteration++;
            //int k = 0;
            
            Solution current = iteration > 1   || this->isNotInitialized ? _builder->buildSolution(rand, alpha) :  this->lastBestSolution;
          
            _localSearch->search(current, rand, start);
            //assert(ls_solution.isFeasible());
           
            if(current.value() < bestOverallFitness)
            {
                if (bestSolution != 0) delete bestSolution;
                bestSolution = new Solution(current);
                //assert(bestSolution->isFeasible());
                bestOverallFitness = current.value();
                updDistance = int(fmaxf(iteration - lastUpdate,updDistance));
                lastUpdate = iteration;
                last_improvment_time = get_cpu_time();
                this->_stats._history.push_back(std::pair<double, double>(bestOverallFitness,last_improvment_time -start));
                //assert(bestSolution.isFeasible());
            }

           this->_stats.sum += current.value();
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
        
        
        double end = get_cpu_time();
        double    time_elapsed_in_seconds = (end - start);
        double best_iter_time = (last_improvment_time - start);
        
        this->_stats.generations = (int)iteration;
        this->_stats.solution = bestOverallFitness;
        this->_stats.bestSolution = bestOverallFitness;
        //stats.restarts = cont_reset;
        this->_stats.last_improvment_time = best_iter_time;
        this->_stats.last_improvmentt_iteration = (int)lastUpdate;
        this->_stats.grater_inprovment_distnace = (int)updDistance;
        this->_stats.time = time_elapsed_in_seconds;
        Solution sol = *bestSolution;
        if(bestSolution) delete bestSolution;
        //assert(bestSolution.isFeasible());
        return sol;
    };
    
    void run(int seed, void* setting) override {
        GRASPSetting& _setting = *(GRASPSetting*)setting;
        std::mt19937 _rand(seed);
        this->lastBestSolution =  runGrasp(_rand, _setting.alpha);
    };
    
};


#endif /* GRASP_h */
