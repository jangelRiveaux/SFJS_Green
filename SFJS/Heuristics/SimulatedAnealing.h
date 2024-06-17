//
//  SimulatedAnealing.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 05/01/24.
//

#ifndef SimulatedAnnealing_h
#define SimulatedAnnealing_h


#include <set>
#include <cmath>
/**/

typedef struct _SimulatedAnnealingSetting : Setting{
    
    double tm = 0.73; // (0,1)
    double tp = 0.74; //)0,1)
    double tf = 0.001f; // 0.001,0.01,0.1,...10
    double gamma = 0.999;// 0.8, 0.85, 0.9, 0.95, 0.99, 0.999
    int maxExcecutions = 10; //1,10,20,30,40,..100
    float alpha = 0;
    _SimulatedAnnealingSetting(){};
    
    void setFromMap(const std::map<std::string,std::string>& setting) override {
        if(setting.find("-tm") != setting.end())
             tm = std::atof(setting.at("-tm").c_str());
        if(setting.find("-tp") != setting.end())
             tp = std::atof(setting.at("-tp").c_str());
        if(setting.find("-tf") != setting.end())
             tf = std::atof(setting.at("-tf").c_str());
        if(setting.find("-gamma") != setting.end())
             gamma = std::atof(setting.at("-gamma").c_str());
       if(setting.find("-maxExcs") != setting.end())
            maxExcecutions = std::atoi(setting.at("-maxExcs").c_str());
        if(setting.find("-alpha") != setting.end())
            alpha = atof(setting.at("-alpha").c_str());
    }
    
    std::string toString() override{
        char res[500];
        std::snprintf(res, 500, "%.4f\t%.4f\t%.4f\t%.4f\t%d\t%.4f\t", tm,tp,tf,gamma,maxExcecutions,alpha);
        std::string result(res);
        return result;
    }
}SimulatedAnnealingSetting;

template< class Solution, class Builder, class LocalSearch >
class SimulatedAnnealing : public Metaheuristic<Solution> {
private:
    Builder* _builder;
    LocalSearch* _localSearch;
    
public:
    SimulatedAnnealing(Builder* builder, LocalSearch* localSearch, StopRule rule,double stop_argument, std::string name = "SA"):
    Metaheuristic<Solution>(rule, stop_argument, name),
    _builder(builder), _localSearch(localSearch){};
    ~SimulatedAnnealing(){
    };

    
    
    Solution runSimulatedAnnealing(const SimulatedAnnealingSetting& _setting,  std::mt19937& _rnd){
        double start = get_cpu_time();
        Solution* initialSolution =  this->isNotInitialized ? this->_builder->constructSolution(_rnd,_setting.alpha) :  new Solution(this->lastBestSolution);
        unsigned long iteration = 0;
        unsigned long lastUpdate = 0;
        unsigned long updDistance = 0;
        bool exc = true;
        
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double bestOverallFitness = initialSolution->value();
        double temp = -_setting.tm/ log(_setting.tp);
        //std::cout << "temp: " << temp << "\n";
        //double deltaTf= sig(_tf) - sig(initialTemp);
        Solution current = *initialSolution;
        double last_improvment_time = get_cpu_time();
        while(exc)
        {
            //std::cout << "current: " <<current.value() << "\n";
            iteration++;
            for(int i = 0;  exc && i < _setting.maxExcecutions; i++)
            {
                
                Solution  x_ = current;
                _localSearch->shake(x_,_rnd,10.0);
                //assert(x_.isFeasible());
                double deltaC = x_.value() - bestOverallFitness;
                double rho = dist(_rnd);
                //std::cout << " rho: " << rho  << "\n";
                if(std::exp(-deltaC/(bestOverallFitness*temp)) >= rho)
                {
                    current = x_;
                    if (current.value() < bestOverallFitness)
                    {
                        if (initialSolution != 0) delete initialSolution;
                        initialSolution = new Solution(current);
                        bestOverallFitness = x_.value();
                        updDistance = int(fmaxf(iteration - lastUpdate, updDistance));
                        lastUpdate = iteration;
                        last_improvment_time = get_cpu_time();
                        this->_stats._history.push_back(std::pair<double, double>(bestOverallFitness,last_improvment_time -start));
                    }
                }
                
                temp = std::fmaxf(_setting.gamma*temp,_setting.tf);
                //this->_stats.sum += current.value();
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
        double    time_elapsed_in_seconds = end - start;
        double best_iter_time = last_improvment_time - start;
        
        this->_stats.generations = (int)iteration;
        this->_stats.solution = bestOverallFitness;
        this->_stats.bestSolution = bestOverallFitness;
        //stats.restarts = cont_reset;
        this->_stats.last_improvment_time = best_iter_time;
        this->_stats.last_improvmentt_iteration = (int)lastUpdate;
        this->_stats.grater_inprovment_distnace = (int)updDistance;
        this->_stats.time = time_elapsed_in_seconds;
        Solution sol = *initialSolution;
        delete initialSolution;
        return sol;
    };
    
    void run( int seed, void* setting) override {
        SimulatedAnnealingSetting _setting =*(SimulatedAnnealingSetting*) setting;
        std::mt19937 rand(seed);
        this->lastBestSolution =  runSimulatedAnnealing(_setting,rand);
    };
};

#endif /* SimulatedAnealing_h */
