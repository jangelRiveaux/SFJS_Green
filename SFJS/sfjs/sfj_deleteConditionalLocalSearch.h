//
//  sfj_deleteConditionalLocalSearch.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 08/01/24.
//

#ifndef sfj_deleteConditionalLocalSearch_h
#define sfj_deleteConditionalLocalSearch_h
#include "sfjs_localSearchs.h"
#include "sfjs_builder.h"
#include <random>

class SFJS_deleteConditionalLS: public  SFJS_LocalSearch{
private:
    const SFJS_Instance & _instance;
    SFJSBuilder &_builder;
    bool _firstImrpovment;
    float _deleteRestrictionFactor;
    
public:
#ifdef  _WIN32 //|| _WIN64
    SFJS_deleteConditionalLS(const SFJS_Instance &instance, SFJSBuilder& builder, float deleteRestrictionFactor = 0.5f,bool firstImrpovment = false, double timeToStop = DBL_MAX) :
        SFJS_LocalSearch(random,false, timeToStop),
        _instance(instance),  _builder(builder), _firstImrpovment(firstImrpovment), _deleteRestrictionFactor(deleteRestrictionFactor){};
#else
    SFJS_deleteConditionalLS(const SFJS_Instance &instance,SFJSBuilder& builder, float deleteRestrictionFactor = 0.5f,bool firstImrpovment = false, double timeToStop = __DBL_MAX__) :
        SFJS_LocalSearch(false, timeToStop),
        _instance(instance),  _builder(builder), _firstImrpovment(firstImrpovment), _deleteRestrictionFactor(deleteRestrictionFactor){};
#endif // _WIN32 || _WIN64
   
    
    void shake(SFJS_Solution& solution, std::mt19937& rnd, float epsilon = 0) const  override {
        
        int n =  _instance.getProcessCount();
        int neigs = 0;
        int iter = 0;
        int gamma_extra = _instance.getExtraEnergyConsumtion();
        std::vector<std::set<int> > predecessorsLst, succesorsLst;
        std::vector<int> topologyList, invertedTopologyList;
        solution.BuildPredecesorsMoveLists(predecessorsLst,topologyList);
        //_solution.BuildPredecesorsLists();
        solution.BuildSucessorsDeleteLists(succesorsLst,invertedTopologyList);
        //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
        for(int op =0; op < n; op++)
        {
            int i = rnd()%n;
            //int procMachine = _solution.getProcessMachineScheduled(i);
            //int procMachinePos  = _solution.getProcessMachineScheduledPosition(i);
            std::vector<bool> visited(n,true);
            SFJS_Solution _aux1 = solution;
            std::vector<int> _topology = invertedTopologyList;
            _aux1.deleteProcess(i,_topology,succesorsLst,visited);
            //assert(abs(_aux1.fo() - _aux1.value()) <= 0.001);
            auto predecessors= predecessorsLst[i];
            auto machines = _instance.getOperationMachineInfo(i);
            int k = rnd()%machines.size();
            std::vector<std::pair<int, int>> __aux(machines.begin(), machines.end());
            auto kInfo = __aux[k];
            
            //for(auto kInfo : _instance.getOperationMachineInfo(i))
            {
                auto sch = _aux1.getCstSchedule(kInfo.first);
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
                }
                int r = begin < end? begin + rnd()%(end - begin): begin;
                //for(int r = begin; r <= end; r++)
                if(kInfo.first != solution.getProcessMachineScheduled(i) ||r != solution.getProcessMachineScheduledPosition(i))
                {
                    if(iter == 0) neigs++;
                    //std::cout << neigs << "\n";
                    SFJS_Solution _aux2 = _aux1;
                    std::vector<bool> visited2 = visited;
                    std::vector<int> _topology2 = _topology;
                    if(sch.size() > r)_aux2.deleteProcess(sch[r],_topology2, succesorsLst,visited2);
                    //assert(abs(_aux2.fo() - _aux2.value()) <= 0.001);
                    int realTime =  _instance.f(i,kInfo.first,r);
                    auto sh2 = _aux2.getSchedule(kInfo.first);
                    //TODO: delete assert
                    //assert(r == sh2.size());
                    
                    int delta_energy = 0; int start = 0; int miu = 0;
                    for(int v : _instance.getDGraph().NeighbordsIn(i))
                    {
                        if(_aux2.getProcessStart(v) + _aux2.getProcessRealTime(v) > miu)
                            miu = _aux2.getProcessStart(v) + _aux2.getProcessRealTime(v);
                    }
                    bool turnOnOff = false;
                    if(sh2.size() == 0)
                    {
                        start = int(std::fmaxf(_instance.getmMachineTurnOnTime(kInfo.first),miu));
                        int ext = start + realTime + _instance.getmMachineTurnOffTime(kInfo.first) - _aux2.getTotalTime();
                        delta_energy =_instance.getProcessEnergyConsumtion(kInfo.first)*realTime +gamma_extra * (ext > 0? ext : 0) +_instance.getTurnOnEnergyConsumtion(kInfo.first)  + _instance.getTurnOffEnergyConsumtion(kInfo.first) ;
                    }
                    else{
                        int w = sh2[sh2.size()-1];
                        int r_kmac =   _aux2.getProcessStart(w)+ _aux2.getProcessRealTime(w);
                        int Zeta1 = r_kmac > miu? r_kmac: miu;
                        int ext = Zeta1 + realTime +  _instance.getmMachineTurnOffTime(kInfo.first)  - _aux2.getTotalTime();
                        int Gamma1 =gamma_extra * (ext > 0? ext : 0) + _instance.getIdleEnergyConsumtion(kInfo.first)*(Zeta1 - r_kmac);
                        int Zeta2 = r_kmac +  _instance.getmMachineTurnOffTime(kInfo.first)  +  _instance.getmMachineTurnOnTime(kInfo.first)  > miu ? r_kmac +_instance.getmMachineTurnOffTime(kInfo.first)  +  _instance.getmMachineTurnOnTime(kInfo.first)  : miu;
                        ext = Zeta2 + realTime + _instance.getmMachineTurnOffTime(kInfo.first) - _aux2.getTotalTime();
                        int Gamma2 =gamma_extra * (ext > 0? ext : 0) +_instance.getTurnOnEnergyConsumtion(kInfo.first)  + _instance.getTurnOffEnergyConsumtion(kInfo.first);
                       turnOnOff = Gamma1 > Gamma2 || (Gamma1 == Gamma2 && Zeta1 > Zeta2  );
                        if(!turnOnOff)
                        {
                            delta_energy = _instance.getProcessEnergyConsumtion(kInfo.first)*realTime + Gamma1;
                            start = Zeta1;
                        }
                        else{
                            delta_energy = _instance.getProcessEnergyConsumtion(kInfo.first)*realTime + Gamma2;
                            start = Zeta2;
                        }
                    }
                    _aux2.schedule(i, kInfo.first, start,realTime, delta_energy);
                    visited2[i] = true;
                    //assert(abs(_aux2.fo() - _aux2.value()) <= 0.001);
                    _builder.partialConstructSolution(_aux2,visited2,rnd);
                    //bool isFeasible = _aux2.isFeasible();
                   //assert(isFeasible && abs(_aux2.fo() - _aux2.value()) <= 0.001);
                    solution = _aux2;
                    return;
                    
                }
            }
        }
    };
    
    void search(SFJS_Solution& solution, std::mt19937& rnd, double startTime = 0, bool oneIteration=false) const override {

        int n =  _instance.getProcessCount();
        int neigs = 0;
        int iter = 0;
        int bestObj =0;
        int gamma_extra = _instance.getExtraEnergyConsumtion();
        
        do{
            SFJS_Solution _solution = solution;
            bestObj =solution.value();
            
            std::vector<std::set<int> > predecessorsLst, succesorsLst;
            std::vector<int> topologyList;
            std::vector<int> invertedTopologyList;
            solution.BuildPredecesorsMoveLists(predecessorsLst,topologyList);
            //_solution.BuildPredecesorsLists();
            solution.BuildSucessorsDeleteLists(succesorsLst,invertedTopologyList);
            //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
            bool notImproved = true;
            for(int i =0; i < n && notImproved; i++)
            {
                //int procMachine = _solution.getProcessMachineScheduled(i);
                //int procMachinePos  = _solution.getProcessMachineScheduledPosition(i);
                std::vector<bool> visited(n,true);
                SFJS_Solution _aux1 = _solution;
                std::vector<int> _topology = invertedTopologyList;
                _aux1.deleteProcess(i,_topology,succesorsLst,visited);
                //assert(abs(_aux1.fo() - _aux1.value()) <= 0.001);
                auto predecessors= predecessorsLst[i];
                auto sucessors = succesorsLst[i];
                if(sucessors.size() > n*_deleteRestrictionFactor)
                    continue;
                for(auto kInfo : _instance.getOperationMachineInfo(i))
                {
                    auto sch = _aux1.getCstSchedule(kInfo.first);
                    ///Get first and last position in k where  v can be inserted
                    int begin = 0, end = sch.size() > 0 ? (int)sch.size()-1: 0;
                    for(int r =  (int)sch.size()-1; r > -1; r--)
                    {
                        int w =sch[r];
                        if(predecessors.find(w) != predecessors.end())
                        {
                            begin= r+1;
                            break;
                        }
                    }
                    for(int r = begin; r <= end; r++)
                    {
                        neigs++;
                        //std::cout << neigs << "\n";
                        SFJS_Solution _aux2 = _aux1;
                        std::vector<bool> visited2 = visited;
                        if(sch.size() > r)
                        {
                            auto sucessors2 = succesorsLst[sch[r]];
                            if(sucessors2.size()> n*_deleteRestrictionFactor)
                                continue;
                            std::vector<int> _topology2 = _topology;
                            _aux2.deleteProcess(sch[r],_topology2,succesorsLst, visited2);
                            
                        }
                       // assert(abs(_aux2.fo() - _aux2.value()) <= 0.001);
                        int realTime =  _instance.f(i,kInfo.first,r);
                        auto sh2 = _aux2.getSchedule(kInfo.first);
                        //TODO: delete assert
                        //assert(r == sh2.size());
                        
                        int delta_energy = 0; int start = 0; int miu = 0;
                        for(int v : _instance.getDGraph().NeighbordsIn(i))
                        {
                            if(_aux2.getProcessStart(v) + _aux2.getProcessRealTime(v) > miu)
                                miu = _aux2.getProcessStart(v) + _aux2.getProcessRealTime(v);
                        }
                        bool turnOnOff = false;
                        if(sh2.size() == 0)
                        {
                            start = int(std::fmaxf(_instance.getmMachineTurnOnTime(kInfo.first),miu));
                            int ext = start + realTime + _instance.getmMachineTurnOffTime(kInfo.first) - _aux2.getTotalTime();
                            delta_energy =_instance.getProcessEnergyConsumtion(kInfo.first)*realTime +gamma_extra * (ext > 0? ext : 0) +_instance.getTurnOnEnergyConsumtion(kInfo.first)  + _instance.getTurnOffEnergyConsumtion(kInfo.first) ;
                        }
                        else{
                            int w = sh2[sh2.size()-1];
                            int r_kmac =   _aux2.getProcessStart(w)+ _aux2.getProcessRealTime(w);
                            int Zeta1 = r_kmac > miu? r_kmac: miu;
                            int ext = Zeta1 + realTime +  _instance.getmMachineTurnOffTime(kInfo.first)  - _aux2.getTotalTime();
                            int Gamma1 =gamma_extra * (ext > 0? ext : 0) + _instance.getIdleEnergyConsumtion(kInfo.first)*(Zeta1 - r_kmac);
                            int Zeta2 = r_kmac +  _instance.getmMachineTurnOffTime(kInfo.first)  +  _instance.getmMachineTurnOnTime(kInfo.first)  > miu ? r_kmac +_instance.getmMachineTurnOffTime(kInfo.first)  +  _instance.getmMachineTurnOnTime(kInfo.first)  : miu;
                            ext = Zeta2 + realTime + _instance.getmMachineTurnOffTime(kInfo.first) - _aux2.getTotalTime();
                            int Gamma2 =gamma_extra * (ext > 0? ext : 0) +_instance.getTurnOnEnergyConsumtion(kInfo.first)  + _instance.getTurnOffEnergyConsumtion(kInfo.first);
                           turnOnOff = Gamma1 > Gamma2 || (Gamma1 == Gamma2 && Zeta1 > Zeta2  );
                            if(!turnOnOff)
                            {
                                delta_energy = _instance.getProcessEnergyConsumtion(kInfo.first)*realTime + Gamma1;
                                start = Zeta1;
                            }
                            else{
                                delta_energy = _instance.getProcessEnergyConsumtion(kInfo.first)*realTime + Gamma2;
                                start = Zeta2;
                            }
                        }
                        _aux2.schedule(i, kInfo.first, start,realTime, delta_energy);
                        visited2[i] = true;
                        //assert(abs(_aux2.fo() - _aux2.value()) <= 0.001);
                        _builder.partialConstructSolution(_aux2, visited2, rnd);
                        //bool isFeasible = _aux2.isFeasible();
                       //assert(isFeasible && abs(_aux2.fo() - _aux2.value()) <= 0.001);
                        if(_aux2.value() <  solution.value())
                        {
                            solution = _aux2;
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

#endif /* sfjs_deleteConditionalLocalSearch_h */
