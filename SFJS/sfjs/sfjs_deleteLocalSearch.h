//
//  sfjs_deleteLocalSearch.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 13/12/23.
//

#ifndef sfjs_deleteLocalSearch_h
#define sfjs_deleteLocalSearch_h
#include "sfjs_localSearchs.h"
#include "sfjs_builder.h"

template< class Builder >
class SFJS_deleteLS: public  SFJS_LocalSearch{
private:
    const SFJS_Instance & _instance;
    Builder &_builder;
    bool _firstImrpovment;
    
public:
    
#ifdef  _WIN32 //|| _WIN64
    SFJS_deleteLS(const SFJS_Instance &instance, Builder& builder, bool firstImrpovment = false, bool useCritical = false, double timeToStop = DBL_MAX) :
        SFJS_LocalSearch(useCritical, timeToStop),
        _instance(instance),
        _random(random),
        _builder(builder),
        _firstImrpovment(firstImrpovment){};
#else
    SFJS_deleteLS(const SFJS_Instance &instance, Builder& builder, bool firstImrpovment = false, bool useCritical = false, double timeToStop = __DBL_MAX__) :
        SFJS_LocalSearch(useCritical, timeToStop),
        _instance(instance),
        _builder(builder),
        _firstImrpovment(firstImrpovment){};
#endif // _WIN32 || _WIN64
          
    
   
    
    void shake(SFJS_Solution& solution, std::mt19937& rnd, float epsilon) const override
    {

       int n =  _instance.getProcessCount();
       int neigs = 0;
       int iter = 0;
       int bestObj =0;
       int gamma_extra = _instance.getExtraEnergyConsumtion();
       
       do{
           SFJS_Solution _solution = solution;
           bestObj =solution.value();
           
           std::vector<std::set<int> > predecessorsLst, succesorsLst;
           std::vector<int> topologyList, invertedTopologyList;
           _solution.BuildPredecesorsMoveLists(predecessorsLst,topologyList);
           //_solution.BuildPredecesorsLists();

           _solution.BuildSucessorsDeleteLists(succesorsLst,invertedTopologyList);
           //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
           bool notImproved = true;
           //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
           //bool isFeasible = _solution.isFeasible();
           //assert(isFeasible && abs(_solution.fo() - solution.value()) <= 0.001);
           std::vector<int> operations;
           if (_useCritical)
           {
               operations = solution.getCriticalPathOperations();
               n = (int)operations.size();
           }

           for(int op =0; op < n && notImproved ; op++)
           {
               int _rnd = rnd()%n;
               int i = (_useCritical) ? operations[_rnd] : _rnd;
               //int procMachine = _solution.getProcessMachineScheduled(i);
               //int procMachinePos  = _solution.getProcessMachineScheduledPosition(i);
               
               std::vector<int> _topology = invertedTopologyList;
               std::vector<bool> visited(_instance.getProcessCount(), true);
               SFJS_Solution _aux1 = _solution;
               //std::cout << iter << ": " << i << "\n";
               _aux1.deleteProcess(i,_topology,succesorsLst,visited);
               //assert(abs(_aux1.fo() - _aux1.value()) <= 0.001);
               auto predecessors= predecessorsLst[i];
               auto machines = _instance.getOperationMachineInfo(i);
               int k = rnd()%machines.size();
               std::vector<std::pair<int, int>> __aux(machines.begin(), machines.end());
               auto kInfo = __aux[k];
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
               if(end - begin > 0)
               {
                   //std::cout << begin<< "  :begin\n";
                   //std::cout << end << "  :end\n";
                   int r = begin + rnd()%(end-begin);
                   //std::cout << r << "  :r\n";
                   neigs++;
                   //std::cout << neigs << "\n";
                   SFJS_Solution _aux2 = _aux1;
                   std::vector<bool> visited2 = visited;

                   
                   std::vector<int> _topology2 = _topology;
                   if(sch.size() > r)_aux2.deleteProcess(sch[r],_topology2,succesorsLst,visited2);
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
                   _builder.partialConstructSolution(_aux2, visited2,rnd);
                   //bool isFeasible = _aux2.isFeasible();
                   //assert(isFeasible && abs(_aux2.fo() - _aux2.value()) <= 0.001);
                   if((_aux2.value()  - solution.value())/solution.value() <= epsilon)
                   {
                       solution = _aux2;
                       return;
                   }
                       
               }//for maq
           }//for op
           iter++;
       }while(solution.value() < bestObj);
       solution.ls_neigborhoodSize += neigs;
       solution.best_iteration = solution.iterations += iter;
   };
    

    void search(SFJS_Solution& solution, std::mt19937& rnd, double startTime= 0, bool oneIteration =  false) const override {

        int n =  _instance.getProcessCount();
        int neigs = 0;
        int iter = 0;
        int bestObj =0;
        int gamma_extra = _instance.getExtraEnergyConsumtion();
        
        do{
            SFJS_Solution _solution = solution;
            bestObj =solution.value();
            
            std::vector<std::set<int> > predecessorsLst, succesorsLst;
            std::vector<int> topologyList, invertedTopologyList;
            _solution.BuildPredecesorsMoveLists(predecessorsLst,topologyList);
            //_solution.BuildPredecesorsLists();

            _solution.BuildSucessorsDeleteLists(succesorsLst,invertedTopologyList);
            //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
            bool notImproved = true;
            //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
            //bool isFeasible = _solution.isFeasible();
            //assert(isFeasible && abs(_solution.fo() - solution.value()) <= 0.001);
            std::vector<int> operations;
            if (_useCritical)
            {
                operations = solution.getCriticalPathOperations();
                n = (int)operations.size();
            }

            for(int op =0; op < n && notImproved; op++)
            {
                int i = (_useCritical)? operations[op]: op;
                //int procMachine = _solution.getProcessMachineScheduled(i);
                //int procMachinePos  = _solution.getProcessMachineScheduledPosition(i);
                
                std::vector<int> _topology = invertedTopologyList;
                std::vector<bool> visited(_instance.getProcessCount(), true);
                SFJS_Solution _aux1 = _solution;
                //std::cout << iter << ": " << i << "\n";
                _aux1.deleteProcess(i,_topology,succesorsLst,visited);
                //assert(abs(_aux1.fo() - _aux1.value()) <= 0.001);
                auto predecessors= predecessorsLst[i];
                
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

                        
                        std::vector<int> _topology2 = _topology;
                        if(sch.size() > r)_aux2.deleteProcess(sch[r],_topology2,succesorsLst,visited2);
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
                       // assert(abs(_aux2.fo() - _aux2.value()) <= 0.001);
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
        }while(!oneIteration &&  solution.value() < bestObj && !exceedsTime(startTime));
        solution.ls_neigborhoodSize += neigs;
        solution.best_iteration = solution.iterations += iter;
    };
};

#endif /* sfjs_deleteLocalSearch_h */
