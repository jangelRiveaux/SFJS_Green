//
//  sfjs_localSearchs.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 06/12/23.
//

#ifndef sfjs_localSearchsConditional_h
#define sfjs_localSearchsConditional_h


#include "sfjs_localSearchs.h"

class SFJS_simpleInsertionConditionalLS: public  SFJS_LocalSearch{
private:
    const SFJS_Instance & _instance;
    bool _firstImrpovment;
public:
#ifdef  _WIN32 //|| _WIN64
    SFJS_simpleInsertionConditionalLS(const SFJS_Instance &instance, std::mt19937& random,bool firstImrpovment = false, double timeToStop = DBL_MAX)
        : SFJS_LocalSearch(random,false, timeToStop),
        _instance(instance),   _firstImrpovment(firstImrpovment){};
#else
    SFJS_simpleInsertionConditionalLS(const SFJS_Instance &instance, bool firstImrpovment = false, double timeToStop = __DBL_MAX__)
        : SFJS_LocalSearch(false, timeToStop),
        _instance(instance),  _firstImrpovment(firstImrpovment){};
#endif // _WIN32 || _WIN64
   
    
    void shake(SFJS_Solution& solution,  std::mt19937& rnd, float epsilon) const override {
        
        //SFJS_Solution solution_best = solution;
        int n =  _instance.getProcessCount();
        int neigs = 0;
        int iter = 0;
        std::vector<std::set<int> > predecessorsLst, succesorsLst;
        std::vector<int> topologyList, invertedTopologyList;
        solution.BuildPredecesorsMoveLists(predecessorsLst,topologyList);
        //_solution.BuildPredecesorsLists();
        solution.BuildSucessorsMoveLists(succesorsLst,invertedTopologyList);
        for(int op =0; op < n; op++)
        {
            //int procMachine = _solution.getProcessMachineScheduled(i);
            //int procMachinePos  = _solution.getProcessMachineScheduledPosition(i);
            int i  = rnd()%n;
            auto predecessors= predecessorsLst[i];
            auto sucessors = succesorsLst[i];
            auto machines = _instance.getOperationMachineInfo(i);
            int k = rnd()%machines.size();
            std::vector<std::pair<int, int>> __aux(machines.begin(), machines.end());
            auto kInfo = __aux[k];
            auto sch = solution.getCstSchedule(kInfo.first);
            ///Get first and last position in k where  v can be inserted
            int begin = 0, end = (int)sch.size()-1;
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
            int r = begin < end? begin + rnd()%(end - begin): begin;
            //for(int r = begin; r <= end; r++)
            if(kInfo.first != solution.getProcessMachineScheduled(i) ||r != solution.getProcessMachineScheduledPosition(i))
            {
                if(iter == 0) neigs++;
                //std::cout << neigs << "\n";
                solution.move( i,  kInfo.first,  r);
                //bool isFeasible = aux.isFeasible() && abs(aux.fo() - aux.value()) <= 0.001;
                //assert(isFeasible);
            }
        }
        
       
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
            for(int i =0; i < n && notImproved; i++)
            {
                int procMachine = _solution.getProcessMachineScheduled(i);
               // int procMachinePos  = _solution.getProcessMachineScheduledPosition(i);
                
                auto predecessors= predecessorsLst[i];
                auto sucessors = succesorsLst[i];
                
                for(auto kInfo : _instance.getOperationMachineInfo(i))
                {
                    auto sch = _solution.getCstSchedule(kInfo.first);
                    ///Get first and last position in k where  v can be inserted
                    int begin = 0, end = 0;
                    for(int r =  (int)sch.size()-1; r > -1 && notImproved; r--)
                    {
                        int w =sch[r];
                        if(predecessors.find(w) != predecessors.end())
                        {
                            begin= r+1;
                            break;
                        }
                        if(sucessors.find(w)!= sucessors.end()) end = r;
                    }
                    //if(kInfo.first == procMachine && begin <= procMachinePos) begin = procMachinePos+1;
                    for(int r = begin; r <= end; r++)
                    {
                        double gamma1 = _instance.getProcessEnergyConsumtion(procMachine);
                        double gamma2 = _instance.getProcessEnergyConsumtion(kInfo.first);
                        if(_solution.getProcessRealTime(i)*gamma1 < _instance.f(i,kInfo.first,r)*gamma2 &&
                          _solution.getMacTime(procMachine) < _solution.getTotalTime() ) continue;
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
                            notImproved =  !_firstImrpovment;
                        }
                        
                    }
                    if(!notImproved) break;
                }//for maq
            }//for op
            iter++;
        }while(!oneIteration && solution.value() < bestObj && !exceedsTime(startTime));
        solution.ls_neigborhoodSize += neigs;
        solution.best_iteration = solution.iterations += iter;
    };
};

#endif /* sfjs_localSearchs_h */
