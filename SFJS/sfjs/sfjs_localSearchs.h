//
//  sfjs_localSearchs.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 06/12/23.
//

#ifndef sfjs_localSearchs_h
#define sfjs_localSearchs_h

#include <random>
#include "sfjs_solution.hpp"
#include "../CPUTimer.h" 
#include "utils.hpp"

class SFJS_LocalSearch{
protected:
    bool _useCritical = false;
    double _timeToStop;
public:
#ifdef  _WIN32 //|| _WIN64
    SFJS_LocalSearch(bool useCritical = false,double timeToStop = DBL_MAX) :
     _useCritical(useCritical) ,  _timeToStop(timeToStop){}
#else
    SFJS_LocalSearch(bool useCritical = false,double timeToStop = __DBL_MAX__) :
     _useCritical(useCritical) ,  _timeToStop(timeToStop){}
#endif // _WIN32 || _WIN64
    

    virtual void shake(SFJS_Solution& solution, std::mt19937& rnd, float epsilon = 0) const {
    };
    
    virtual void search(SFJS_Solution& solution, std::mt19937& rnd, double startTime=0, bool oneIteration = false) const{
    };
    
    bool exceedsTime(double startTime) const {
        return  get_cpu_time() - startTime > _timeToStop;
    }
};

class SFJS_simpleInsertionLS: public  SFJS_LocalSearch{
private:
    const SFJS_Instance & _instance;
    bool _firstImrpovment;
public:
    SFJS_simpleInsertionLS(const SFJS_Instance &instance,
        bool firstImrpovment = false, bool useCritical=false, double timeToStop = __DBL_MAX__):
    SFJS_LocalSearch(useCritical,timeToStop),
        _instance(instance),  _firstImrpovment(firstImrpovment){};
    
    void shake(SFJS_Solution& solution,  std::mt19937& rnd, float epsilon = 0) const override {
        int n =  _instance.getProcessCount();
        int neigs = 0;
        int iter = 0;
        int bestObj =0;
        do{
            SFJS_Solution _solution = solution;
            bestObj = solution.value();
            std::vector<std::set<int> > predecessorsLst, succesorsLst;
            std::vector<int> topologyList, invertedTopologyList;
            _solution.BuildPredecesorsMoveLists(predecessorsLst,topologyList);
            //_solution.BuildPredecesorsLists();
            _solution.BuildSucessorsMoveLists(succesorsLst,invertedTopologyList);
            bool notImproved = true;
            
            std::vector<int> operations;
            if (_useCritical)
            {
                operations = solution.getCriticalPathOperations();
                n = (int)operations.size();
            }

            for (int op = 0; op < n && notImproved; op++)
            {
                int _rnd = rnd()%n;
                int i = (_useCritical) ? operations[_rnd] : _rnd;
           
                auto predecessors= predecessorsLst[i];
                auto sucessors = succesorsLst[i];
                auto machines = _instance.getOperationMachineInfo(i);
                int k = rnd()%machines.size();
                std::vector<std::pair<int, int>> __aux(machines.begin(), machines.end());
                auto kInfo = __aux[k];
                auto sch = solution.getCstSchedule(kInfo.first);
            
                ///Get first and last position in k where  v can be inserted
                int begin = 0, end = 0;
                for(int r =  (int)sch.size()-1; r > -1; r--)
                {
                    int w =sch[r];
                    if(predecessors.find(w) != predecessors.end())
                    {
                        begin= r+1;
                        break;
                    }
                    if(sucessors.find(w)!= sucessors.end()) end = r;
                }
                if(end - begin > 0)
                {
                    //std::cout << begin<< "  :begin\n";
                    //std::cout << end << "  :end\n";
                    int r = begin + rnd()%(end-begin);
                    //std::cout << r << "  :r\n";
                    if(iter == 0) neigs++;
                    //std::cout << neigs << "\n";
                    SFJS_Solution aux = _solution;
                    //bool isFeasible = aux.isFeasible() && abs(aux.fo() - aux.value()) <= 0.001;
                    //assert(isFeasible);
                    //std::cout << i << " ; "<< kInfo.first << "\n";
                    aux.move( i,  kInfo.first,  r);
                   // isFeasible = aux.isFeasible() && abs(aux.fo() - aux.value()) <= 0.001;
                    //assert(isFeasible);
                    if((aux.value()  - solution.value())/solution.value() <= epsilon)
                    {
                        solution = aux;
                        return;
                    }

                }//for maq
            }//for op
            iter++;
        }while(solution.value() < bestObj);
        solution.ls_neigborhoodSize += neigs;
        solution.best_iteration = solution.iterations += iter;
    };

    void search(SFJS_Solution& solution, std::mt19937& rnd, double startTime = 0, bool oneIteration = false) const override {

        int n =  _instance.getProcessCount();
        int neigs = 0;
        int iter = 0;
        int bestObj =0;
        do{
            SFJS_Solution _solution = solution;
            bestObj = solution.value();

            std::vector<std::set<int> > predecessorsLst, succesorsLst;
            std::vector<int> topologyList, invertedTopologyList;
            _solution.BuildPredecesorsMoveLists(predecessorsLst,topologyList);
            //_solution.BuildPredecesorsLists();
            _solution.BuildSucessorsMoveLists(succesorsLst,invertedTopologyList);
            bool notImproved = true;
            
            std::vector<int> operations;
            if (_useCritical)
            {
                operations = solution.getCriticalPathOperations();
                n = (int)operations.size();
            }

            for (int op = 0; op < n && notImproved; op++)
            {
                int i = (_useCritical) ? operations[op] : op;
                //int procMachine = _solution.getProcessMachineScheduled(i);
                //int procMachinePos  = _solution.getProcessMachineScheduledPosition(i);
                
                auto predecessors= predecessorsLst[i];
                auto sucessors = succesorsLst[i];
                
                
                for(auto kInfo : _instance.getOperationMachineInfo(i))
                {
                    auto sch = _solution.getCstSchedule(kInfo.first);
                    ///Get first and last position in k where  v can be inserted
                    int begin = 0, end = 0;
                    for(int r =  (int)sch.size()-1; r > -1; r--)
                    {
                        int w =sch[r];
                        if(predecessors.find(w) != predecessors.end())
                        {
                            begin= r+1;
                            break;
                        }
                        if(sucessors.find(w)!= sucessors.end()) end = r;
                    }
                    for(int r = begin; r <= end; r++)
                    {
                        if(iter == 0) neigs++;
                        //std::cout << neigs << "\n";
                        SFJS_Solution aux = _solution;
                        //bool isFeasible = aux.isFeasible() && abs(aux.fo() - aux.value()) <= 0.001;
                        //assert(isFeasible);
                        //std::cout << i << " ; "<< kInfo.first << "\n";
                        aux.move( i,  kInfo.first,  r);
                       // isFeasible = aux.isFeasible() && abs(aux.fo() - aux.value()) <= 0.001;
                        //assert(isFeasible);
                        if(aux.value() < solution.value())
                        {
                            solution = aux;
                            notImproved = !_firstImrpovment;
                            break;
                           
                        }
                        
                    }

                    if (!notImproved)break;
                }//for maq
            }//for op
            iter++;
        }while(!oneIteration && solution.value() < bestObj && !exceedsTime(startTime));
        solution.ls_neigborhoodSize += neigs;
        solution.best_iteration = solution.iterations += iter;
    };
};

#endif /* sfjs_localSearchs_h */
