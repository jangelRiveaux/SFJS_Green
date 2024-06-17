//
//  sfjs_solution.hpp
//  Models
//
//  Created by Jose Angel Riveaux on 29/06/23.
//

#ifndef sfjs_solution_h
#define sfjs_solution_h

#include <vector>
#include <map>
#include <tuple>
#include "../graph/DGraph.h"
#include "sfjs_instance.hpp"
#include <cassert>

///<process, start, real processing time>

class SFJS_Solution
{
private:
    const SFJS_Instance* _instance; //instancia
    int totalTime,  machineMaxTime; //Cmax, machine with maximum time
    std::vector<std::vector<int>> machineSchedule; //machine schedulig
    double energy; //final energy
    std::vector<int> processStart;
    //std::vector<int> realTime;
    std::vector<int> processMachinePosition;
    std::vector<int> processMachine;
    //std::vector<std::vector<bool>> _turnOnOff;
    //std::vector<std::set<int>> predecessors, sucessors;
    //std::vector<int> topologyList, invertedTopologyList;
public:
    int iterations, best_iteration = 0, ls_neigborhoodSize;
    float time = 0.0, best_time = 0.0;
    double sum=0;
    SFJS_Solution(){};
    
    SFJS_Solution(const SFJS_Solution& solution): _instance(solution._instance)
    {
        // int n =disjunctiveGraph->getNodesCount();
        //int m = _instance->getMachinesCount();
       // machine_consume =solution.machine_consume;
        energy = solution.energy;
        machineSchedule = solution.machineSchedule;
        processStart = solution.processStart;
        processMachinePosition = solution.processMachinePosition;
        processMachine = solution.processMachine;
        //realTime = solution.realTime;
        //_turnOnOff = solution._turnOnOff;
        totalTime = solution.totalTime;
        machineMaxTime = solution.machineMaxTime;
        //Adding source and destiny to Dgraph
        time = solution.time;
        iterations = solution.iterations;
        ls_neigborhoodSize = solution.ls_neigborhoodSize;
        sum = solution.sum;
        best_iteration = solution.best_iteration;
        best_time = solution.best_time;
        //predecessors = solution.predecessors;
        //sucessors = solution.sucessors;
       // topologyList = solution.topologyList;
       // invertedTopologyList = solution.invertedTopologyList;
    }
    
    SFJS_Solution(const SFJS_Instance& instance):_instance(&instance){
         int n =instance.getDGraph().getNodesCount();
        int m = _instance->getMachinesCount();
        energy = 0;
        //machine_consume = std::vector<unsigned long>(m,0);
        machineSchedule =  std::vector<std::vector<int>>(m, std::vector<int>());
        processStart = std::vector<int>(n,-1);
        //realTime = std::vector<int>(n,-1);
        processMachinePosition = std::vector<int>(n,-1);
        processMachine = std::vector<int>(n,-1);
        //for(int i = 0; i < m; i++)
            //_turnOnOff.push_back(std::vector<bool>(_instance->getMachineProcesses(i).size(),false));
        ls_neigborhoodSize = iterations = 0;
        totalTime = 0; machineMaxTime = 0;
        //Adding source and destiny to Dgraph
    };
    
    ~SFJS_Solution(){
    }
    
    SFJS_Solution& operator= ( const SFJS_Solution & other){
        if (this != &other) // not a self-assignment
               {
                  // int n =disjunctiveGraph->getNodesCount();
                   //int m = _instance->getMachinesCount();
                   _instance=  other._instance;
                   energy =other.energy;
                   machineSchedule = other.machineSchedule;
                   processMachinePosition = other.processMachinePosition;
                   processMachine = other.processMachine;
                   processStart = other.processStart;
                   //realTime = other.realTime;
                   //_turnOnOff = other._turnOnOff;
                   totalTime = other.totalTime;
                   machineMaxTime = other.machineMaxTime;
                   //Adding source and destiny to Dgraph
                   iterations = other.iterations;
                   ls_neigborhoodSize = other.ls_neigborhoodSize;
                  // predecessors = other.predecessors;
                  // sucessors = other.sucessors;
                  // topologyList = other.topologyList;
                  // invertedTopologyList = other.invertedTopologyList;
               }
               return *this;
    }
    
    void schedule(int operation, int machine, int start, int processTime, int delta_energy)
    {
        //O(1) :Checked
        int pos = (int)machineSchedule[machine].size();
        //TODO: delete assert
        /*if(pos-1 > 0)
        {
            int proc = machineSchedule[machine][pos-1];
            assert(start >= processStart[proc] + realTime[proc]);
        }*/
        processMachinePosition[operation] = pos;
        processMachine[operation]= machine;
        processStart[operation] = start;
        //realTime[operation] = processTime;
        machineSchedule[machine].push_back(operation);
        energy += delta_energy;
        int endTime = start + processTime + _instance->getmMachineTurnOffTime(machine);
        //if s_v + p'_v + t^off_k > Cmax then line 18
        //    Cmax ← s_v + p'_v + t^off_k    line 19
        if(endTime > totalTime) {
            totalTime = endTime;
            machineMaxTime = machine;
        }
        
        //if(pos > 0)
           // _turnOnOff[machine][pos-1] = turnOnOff;
    };
    
 
    int getTotalTime()const {return totalTime; };
    
    /**
     * Atualiza a informação das operações que sofrem mudanza depois de um movimento em ordem topológica
     */
    void _topologyUpdate()  {
        std::vector<int> vertices;
        std::map<int,int> degrees = _instance->getDGraph().AllDegreeIn();
        std::map<int,int>  dependenciesTime;
        
        for(auto v : degrees){
            dependenciesTime[v.first]= 0;
            if(processMachinePosition[v.first] > 0) degrees[v.first]++;
            if(degrees[v.first] == 0) vertices.push_back(v.first);
        }
        energy = 0; totalTime = 0;
        int gamma_extra = _instance->getExtraEnergyConsumtion();
        
        while(!vertices.empty())
        {
            int v = vertices[0];
            vertices.erase(vertices.begin());
            int vMac = processMachine[v];
            int vPos = processMachinePosition[v];
            int _realTime = _instance->f(v,vMac,vPos);
            std::set<int> next =  _instance->getDGraph().NeighbordsOut(v);
            //assert(realTime[v] == _instance->f(v,vMac,vPos));
             if(vPos == 0)
            {
                processStart[v] = (int)std::fmax(_instance->getmMachineTurnOnTime(vMac), dependenciesTime[v]);
                int ext =  processStart[v]  + _realTime+ _instance->getmMachineTurnOffTime(vMac) - totalTime;
                energy += _instance->getProcessEnergyConsumtion(vMac)*_realTime + _instance->getTurnOnEnergyConsumtion(vMac) + _instance->getTurnOffEnergyConsumtion(vMac) +gamma_extra * (ext > 0? ext : 0) ;
            }
            else
            {
                int w = machineSchedule[vMac][vPos-1];
                int w_realTime = _instance->f(w,vMac,vPos-1);
                int r_kmac = processStart[w] + w_realTime;
                int Zeta1 = (int)fmaxf(r_kmac, dependenciesTime[v]);
                int ext = Zeta1 + _realTime + _instance->getmMachineTurnOffTime(vMac) - totalTime;
                int Gamma1 = gamma_extra* (ext > 0? ext : 0) +_instance->getIdleEnergyConsumtion(vMac)*(Zeta1 - r_kmac);
                int Zeta2 =(int)fmaxf(processStart[w] + w_realTime + _instance->getmMachineTurnOnTime(vMac)+_instance->getmMachineTurnOffTime(vMac), dependenciesTime[v]);
                ext = Zeta2 + _realTime + _instance->getmMachineTurnOffTime(vMac) - totalTime;
                int Gamma2 =gamma_extra * (ext > 0? ext : 0) + _instance->getTurnOnEnergyConsumtion(vMac) + _instance->getTurnOffEnergyConsumtion(vMac);
                bool _turnOnOff = Gamma1 > Gamma2 || (Gamma1 == Gamma2 && Zeta1 > Zeta2  );
                if(_turnOnOff)  {
                    processStart[v] = Zeta2; energy += Gamma2 + _instance->getProcessEnergyConsumtion(vMac)*_realTime ;
                }
                else{  processStart[v] = Zeta1;  energy += Gamma1 + _instance->getProcessEnergyConsumtion(vMac)*_realTime ;  }
            }
            if(processStart[v] + _realTime + _instance->getmMachineTurnOffTime(vMac) > totalTime)
            {
                totalTime = processStart[v] + _realTime + _instance->getmMachineTurnOffTime(vMac);
                
            }
            for(int w :  _instance->getDGraph().NeighbordsOut(v))
            {
                degrees[w]--;
                //Update dependencies time
                if(processStart[v] + _realTime  >  dependenciesTime[w]) dependenciesTime[w] = processStart[v] + _realTime;
                if(degrees[w] == 0)
                    vertices.push_back(w);
            }
            if(vPos+1 < machineSchedule[vMac].size())
            {
                int w = machineSchedule[vMac][vPos+1];
                    degrees[w]--;
                    if(degrees[w] == 0)
                        vertices.push_back(w);
            }
        }
        
       // assert(energy == fo());
    }
    
    int recalculateMachineTime(int machine) const {
        if(machineSchedule[machine].size() > 0)
        {
            auto last = *machineSchedule[machine].rbegin();
            int _realtime = _instance->f(last,machine,(int)machineSchedule[machine].size()-1);
            int time_aux = processStart[last] +_realtime + _instance->getmMachineTurnOffTime(machine);
            return time_aux;
        }
        return 0;
    }
    
    
    void BuildSucessorsDeleteLists(std::vector<std::set<int> > &sucessors, std::vector<int>& invertedTopologyList){
        int n = _instance->getProcessCount();
        std::vector<int> degs(n);
        sucessors =  std::vector<std::set<int> > (n,std::set<int>());
        std::vector<std::set<int> > aux_sucessors =  std::vector<std::set<int> > (n,std::set<int>());
        std::vector<int> C;        for(int v : _instance->getDGraph().Vertices())
        {
            degs[v] =_instance->getDGraph().DegreeOut(v);
            if(processMachinePosition[v] < machineSchedule[processMachine[v]].size()-1) degs[v]++;
            if(degs[v] == 0)
            {
                C.push_back(v);
                invertedTopologyList.insert(invertedTopologyList.begin(),v);
            }
        }
            
        while(!C.empty())
        {
            int v = C[0];
            C.erase(C.begin());
            int pos = processMachinePosition[v];
            if(pos > 0)
            {
                int w = machineSchedule[processMachine[v]][pos-1];
                degs[w]--;
                sucessors[w].insert(sucessors[v].begin(), sucessors[v].end());
                sucessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    invertedTopologyList.insert(invertedTopologyList.begin(),w);
                }
            }
            for(int w : _instance->getDGraph().NeighbordsIn(v))
            {
                degs[w]--;
                sucessors[w].insert(sucessors[v].begin(), sucessors[v].end());
                sucessors[w].insert(aux_sucessors[v].begin(), aux_sucessors[v].end());
                sucessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    invertedTopologyList.insert(invertedTopologyList.begin(),w);
                }
            }
        }
    };
    
    void BuildPredecesorsMoveLists(std::vector<std::set<int> > &predecessors, std::vector<int>& topologyList ){
        int n = _instance->getProcessCount();
        std::vector<int> degs(n);
        predecessors =  std::vector<std::set<int> > (n,std::set<int>());
        std::vector<std::set<int> > aux_predecessors =  std::vector<std::set<int> > (n,std::set<int>());
        std::vector<int> C;
        topologyList.clear();
        for(int v : _instance->getDGraph().Vertices())
        {
            degs[v] =_instance->getDGraph().DegreeIn(v);
            if(processMachinePosition[v] > 0) degs[v]++;
            if(degs[v] == 0)
            {
                C.push_back(v);
                topologyList.push_back(v);
    
            }
        }
        
        while(!C.empty())
        {
            int v = C[0];
            C.erase(C.begin());
            int pos = processMachinePosition[v];
            if(pos < machineSchedule[processMachine[v]].size()-1)
            {
                int w = machineSchedule[processMachine[v]][pos+1];
                degs[w]--;
                aux_predecessors[w].insert(predecessors[v].begin(), predecessors[v].end());
                aux_predecessors[w].insert(aux_predecessors[v].begin(), aux_predecessors[v].end());
                aux_predecessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    topologyList.push_back(w);
                }
            }
            for(int w : _instance->getDGraph().NeighbordsOut(v))
            {
                degs[w]--;
                predecessors[w].insert(predecessors[v].begin(), predecessors[v].end());
                predecessors[w].insert(aux_predecessors[v].begin(), aux_predecessors[v].end());
                predecessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    topologyList.push_back(w);
                }
            }
        }
    };
    
    void BuildSucessorsMoveLists(std::vector<std::set<int> > &sucessors, std::vector<int>& invertedTopologyList){
        int n = _instance->getProcessCount();
        std::vector<int> degs(n);
        sucessors =  std::vector<std::set<int> > (n,std::set<int>());
        std::vector<std::set<int> > aux_sucessors =  std::vector<std::set<int> > (n,std::set<int>());
        std::vector<int> C;
        for(int v : _instance->getDGraph().Vertices())
        {
            degs[v] =_instance->getDGraph().DegreeOut(v);
            if(processMachinePosition[v] < machineSchedule[processMachine[v]].size()-1) degs[v]++;
            if(degs[v] == 0)
            {
                C.push_back(v);
                invertedTopologyList.insert(invertedTopologyList.begin(),v);
            }
        }
            
        while(!C.empty())
        {
            int v = C[0];
            C.erase(C.begin());
            int pos = processMachinePosition[v];
            if(pos > 0)
            {
                int w = machineSchedule[processMachine[v]][pos-1];
                degs[w]--;
                aux_sucessors[w].insert(sucessors[v].begin(), sucessors[v].end());
                aux_sucessors[w].insert(aux_sucessors[v].begin(), aux_sucessors[v].end());
                aux_sucessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    invertedTopologyList.insert(invertedTopologyList.begin(),w);
                }
            }
            for(int w : _instance->getDGraph().NeighbordsIn(v))
            {
                degs[w]--;
                sucessors[w].insert(sucessors[v].begin(), sucessors[v].end());
                sucessors[w].insert(aux_sucessors[v].begin(), aux_sucessors[v].end());
                sucessors[w].insert(v);
                if(degs[w] == 0)
                {
                    C.push_back(w);
                    invertedTopologyList.insert(invertedTopologyList.begin(),w);
                }
            }
        }
    };
    
  
    bool turnOnOffAfter(int mac, int pos) const{
        if(pos < machineSchedule[mac].size())
        {
            int gamma = _instance->getIdleEnergyConsumtion(mac);
            int gammaOnOff = _instance->getTurnOnEnergyConsumtion(mac) + _instance->getTurnOffEnergyConsumtion(mac);
            int w = machineSchedule[mac][pos];
            int v = machineSchedule[mac][pos+1];
            
            double time = processStart[v] - processStart[w] - _instance->f(w,mac,pos);
            return time*gamma > gammaOnOff;
        }
        return false;
    }
    
    unsigned long recalculateMachineEnergyAndTime(int machine) const
    {
        unsigned long machine_energy = 0;
        unsigned long mproc_energy = 0;
        unsigned long midle_ener = 0;
        unsigned long on_off = _instance->getTurnOnEnergyConsumtion(machine) + _instance->getTurnOffEnergyConsumtion(machine);
        
        if(machineSchedule[machine].size() > 0)
        {
            machine_energy += on_off;
            int tm = 0;
            for(int j = 0; j <machineSchedule[machine].size(); j++)
            {
                int proc = machineSchedule[machine][j];
                tm = _instance->f(proc,machine, j);
                int procCons =  tm *_instance->getProcessEnergyConsumtion(machine);
                mproc_energy += procCons;
                machine_energy += procCons;
                assert(machine_energy > 0);
                if(j> 0 &&  turnOnOffAfter(machine,j-1))
                {
                    //on_off += _instance->getTurnOnEnergyConsumtion(machine) + _instance->getTurnOffEnergyConsumtion(machine);
                    machine_energy += on_off;
                }
                else if(j > 0){
                    const int proc2 = machineSchedule[machine][j-1];
                    int tm2 = _instance->f(proc2,machine, j-1);
                    int delta_t =   processStart[proc] - processStart[proc2] - tm2;
                    assert(delta_t >= 0);
                    midle_ener += _instance->getIdleEnergyConsumtion(machine)*delta_t;
                    machine_energy += _instance->getIdleEnergyConsumtion(machine)*delta_t;
                }
            }
        }//machine_energy    unsigned long    184467440 6  9392797376
        return machine_energy;
    }
    
    /**
     * move an opracao <operation> to the machine <machine> at position <position> and update the dynamic <dependenciesTime> matrix
     */
    void move(int operation, int machine, int position) {
       
      //  std::cout << operation << "\t" << machine <<"\t" << position << "\n";
        int procMachine = getProcessMachineScheduled(operation);
        int procMachinePos  =getProcessMachineScheduledPosition(operation);
        if(machine == procMachine && (procMachinePos == position ||procMachinePos == position+1 )) return;
        std::vector<int>& origin = getSchedule(procMachine);
        std::vector<int>& destinyMac = getSchedule(machine);
        //elimino do origem
        origin.erase(origin.begin() + procMachinePos);
        if(machine == procMachine && procMachinePos < position)
            position--;
        //inserto no destino
        destinyMac.insert(destinyMac.begin() + position, operation); //O(|Q_k|)
       
        
        std::vector<bool> toUpdate(_instance->getProcessCount(),false);
        //adiciono a opecacao a lista de candidados a atualizar
        toUpdate[operation] = true;
        //adiciono o proximo da maquina origem na lista de candidados a atualizar
        if(procMachinePos < origin.size())
        {
            int v = origin[procMachinePos];
            toUpdate[v] = true;
        }
        //atualizo as posicoes da operacaoes qreceden a opreation na maquina origem
        for(int i = procMachinePos; i < origin.size(); i++){
            int proc =origin[i];
            processMachinePosition[proc] = i;
        }
        //atualizo as posicoes da operacaoes qreceden a opreation na maquina destino
        for(int i = position; i < destinyMac.size(); i++){
            int proc =destinyMac[i];
            processMachinePosition[proc] = i;
        }
        processMachine[operation] = machine;
        //TODO: reimplement topologyUpdate
        _topologyUpdate();
    }
    
    
    void deleteProcess(int proc,  std::vector<int>&invertedTopologyList, const  std::vector<std::set<int>>& sucessors, std::vector<bool>& visited)
    {
        int wMac = processMachine[proc];
        int wPos = processMachinePosition[proc];
        int w = -3;
        if(machineSchedule[wMac].size() > wPos+1)
            w =machineSchedule[wMac][wPos+1];
        for(int i = (int)invertedTopologyList.size() - 1; i >= 0 ; i--)
        {
            int v = invertedTopologyList[i];
            
            if(visited[v] &&
               (v == proc || sucessors[proc].find(v) != sucessors[proc].end() || v == w)  )
            {
                int vMac = processMachine[v];
                int vPos = processMachinePosition[v];
                assert(*(machineSchedule[vMac].rbegin()) == v);
                machineSchedule[vMac].erase(machineSchedule[vMac].begin() + vPos);
                invertedTopologyList.erase(invertedTopologyList.begin() + i);
                visited[v]=false;
                int extraE = 0; int  auxTotalTime = 0; int eInterval = 0;
                int _realTime = _instance->f(v, vMac, vPos);
                if(processStart[v] + _realTime+ _instance->getmMachineTurnOffTime(vMac) == totalTime)
                {
                    for(int k = 0; k < machineSchedule.size(); k++)
                    {
                        if(machineSchedule[k].size() > 0)
                        {
                            int w = *(machineSchedule[k].rbegin());
                            int w_realTime = _instance->f(w, k, (int)machineSchedule[k].size()-1);
                            if(processStart[w] + w_realTime + _instance->getmMachineTurnOffTime(k) > auxTotalTime)
                            {
                                auxTotalTime =processStart[w] + w_realTime + _instance->getmMachineTurnOffTime(k);
                            }
                        }
                    }
                    extraE = _instance->getExtraEnergyConsumtion()* (totalTime - auxTotalTime);
                    totalTime = auxTotalTime;
                }
                
                eInterval = _instance->getTurnOnEnergyConsumtion(vMac) + _instance->getTurnOffEnergyConsumtion(vMac);
                if(machineSchedule[vMac].size()>0)
                {
                    int w = machineSchedule[vMac][vPos-1];
                    int w_realTime = _instance->f(w, vMac, vPos-1);
                    if(_instance->getIdleEnergyConsumtion(vMac)*(processStart[v]-processStart[w] -  w_realTime) < eInterval ) {
                    eInterval = _instance->getIdleEnergyConsumtion(vMac)*(processStart[v]-processStart[w] -  w_realTime) ;
                    }
                }
                
                energy -= (eInterval + extraE + _instance->getProcessEnergyConsumtion(vMac)*_realTime);
            }
            if(v == proc) break;
        }
    }
    
    std::vector<bool> getCriticalPathClasification()
    {
        std::vector<int> elms;
        int n = _instance->getProcessCount();
        std::vector<bool> _set(n, false);
        for (int k = 0; k < machineSchedule.size(); k++)
        {
            const std::vector<int>& sch = machineSchedule[k];
            if (sch.size() > 0)
            {
                int v = *sch.rbegin();
                if (processStart[v] + _instance->f(v, k,(int)sch.size() - 1) + _instance->getmMachineTurnOffTime(k) == totalTime)
                {
                    _set[v] =  true;
                    elms.push_back(v);
                }
            }
        }


        for (int i = 0; i < elms.size(); i++) {
            int v = elms[i];
            int start = processStart[v];
            int pos = processMachinePosition[v];
            int k = processMachine[v];
            const std::set<int>& neigbord = _instance->getDGraph().NeighbordsIn(v);

            for (int w : neigbord)
            {
                if (!_set[w]) {
                    int w_realTime = _instance->f(w, processMachine[w], processMachinePosition[w]);
                    if (processStart[w] + w_realTime == start)
                    {
                        _set[w] = true;
                        elms.push_back(w);
                    }
                }
            }//for neigh
            if (pos > 0)
            {
                int w = machineSchedule[k][pos - 1];
                if (!_set[w]) {
                    int w_realTime = _instance->f(w, processMachine[w], processMachinePosition[w]);
                    if (processStart[w] + w_realTime == start)
                    {
                        _set[w] = true;
                        elms.push_back(w);
                    }
                }
            }
        }//while
        return  _set;
    }
    
    std::vector<int> getCriticalPathOperations() const
    {

        std::vector<int> _set;
        int n = _instance->getProcessCount();
        std::vector<bool> visited(n, false);
        for (int k = 0; k < machineSchedule.size(); k++)
        {
            const std::vector<int>& sch = machineSchedule[k];
            if(sch.size() > 0)
            {
                int v = *sch.rbegin();
                if (processStart[v] + _instance->f(v, k,(int)sch.size() - 1) + _instance->getmMachineTurnOffTime(k) == totalTime)
                {
                    _set.push_back(v);
                    visited[v] = true;
                }
            }
        }

        
        for (int i = 0; i < _set.size(); i++) {
            int v = _set[i];
            int start = processStart[v];
            int pos = processMachinePosition[v];
            int k = processMachine[v];
            const std::set<int>& neigbord = _instance->getDGraph().NeighbordsIn(v);
           
            for (int w : neigbord)
            {
                if (!visited[w]) {
                    int w_realTime = _instance->f(w, processMachine[w], processMachinePosition[w]);
                    if (processStart[w] + w_realTime == start)
                    {
                        _set.push_back(w);
                        visited[v] = true;
                    }
                }
            }//for neigh
            if (pos > 0)
            {
                int w = machineSchedule[k][pos - 1];
                if(!visited[w]) {
                    int w_realTime = _instance->f(w, processMachine[w], processMachinePosition[w]);
                    if (processStart[w] + w_realTime == start)
                    {
                        _set.push_back(w);
                        visited[v] = true;
                    }
                }
            }
        }//while
        return  _set;
    }
    
    const SFJS_Instance& getInstance() const {return *_instance; };
    
    const std::vector<int>& getCstSchedule(int machine) const { return  machineSchedule[machine];} ;
    
    std::vector<int>& getSchedule(int machine)  { return  machineSchedule[machine];} ;
    
    int getMacTime(int machine) const{
        if( machineSchedule[machine].size() == 0) return 0;
        int w =machineSchedule[machine][machineSchedule[machine].size()  - 1];
        return processStart[w] +_instance->f(w, machine, (int)machineSchedule[machine].size()  - 1)+ _instance->getmMachineTurnOffTime(machine);
    }
    
    int getProcessMachineScheduled(int process) const {return processMachine[process];};
    
    int getProcessMachineScheduledPosition(int process) const {return processMachinePosition[process];};
    
    int getProcessStart(int process) const{ return processStart[process]; }
    
    int getProcessRealTime(int process) const{ return _instance->f(process, processMachine[process], processMachinePosition[process]); }
    
    int getScheduleTime() const {return totalTime; };
    
    int getScheduleExtraEnergyConsume() const {return _instance->getExtraEnergyConsumtion()*totalTime; };
  
    double value() const {
        return energy;
    };
    
   
    
    double fo() const{
        double time = 0;
        unsigned long _energy = 0;
        for(int i =0; i <  _instance->getMachinesCount(); i++)
        {
            unsigned long machine_energy = recalculateMachineEnergyAndTime(i);
            
            _energy += machine_energy;
            int time_aux =   recalculateMachineTime(i);
            if(time_aux > time)
                time = time_aux;
           
        }
        _energy += time*_instance->getExtraEnergyConsumtion();
        assert(time == totalTime);
        assert(_energy == energy);
        return _energy;
    }
    

    bool isFeasible() const{
        
        std::vector<bool> opScheduled( _instance->getProcessCount(), false);
        int mac = -1;
        std::vector<bool> visited(_instance->getProcessCount(), false);
        for(auto sch : machineSchedule)
        {
            mac++;
            for(int i = 0; i < sch.size(); i++)
            {
                int proc =  sch[i];
                if(visited[proc])
                {
                    std::cout <<"process scheduled more than once: " << proc << "\n";
                    return false;
                }
                visited[proc] = true;
                
             
                if(opScheduled[proc])
                {
                    std::cout <<"process sceduled twice: " << proc << "\n";
                    return false;
                    
                }
                if(i > 0 && processStart[proc] < processStart[sch[i-1]] + getProcessRealTime(sch[i-1]))
                {
                    
                    std::cout <<"process scedule start error: " << proc << "\t" <<processStart[proc] << "<" <<processStart[sch[i-1]] <<"+"<<  getProcessRealTime(sch[i-1]) <<  "\n";
                    return false;
                }
                for(int w : _instance->getDGraph().NeighbordsIn(proc))
                {
                    if(processStart[proc] <  processStart[w]+ getProcessRealTime(w))
                    {
                        std::cout <<"process scedule start error from adj: " << proc << "\t" << processStart[proc] << "<" <<processStart[w] <<"+"<< getProcessRealTime(w) <<  "\n";
                        return false;
                    }
                }
                opScheduled[proc] = true;
            }
        }
        for(int i =0; i < _instance->getProcessCount(); i++)
        {
            if(!visited[i])
            {
                std::cout <<"process not scheculed: " << i << "\n";
                return false;
            }
        }
        return abs(value() - fo()) < 0.001; 
    }
    
    void printSolution(FILE *f){
        int n = _instance->getProcessCount();
        int m = _instance->getMachinesCount();
        float f1 = fo();
        float f2  = value();
        assert(abs(f1 - f2) < 0.00001);
         if(isFeasible() && abs(f1-f2) < 0.0000 )
             fprintf(f, "%s", "\n#Problem feasible.\n");
        else
            fprintf(f, "%s", "\n#Problem unfeasible.\n");
       
        char text[500];
        snprintf(text,500,"#(cost): %.6f\n", (float)value());
        std::cout << text << std::endl;
        fprintf(f, "%s", text);
        
        int process = 0;
        //Create x_ikr
        for(int k = 0; k < m; k++){
            for(int r = 0; r < machineSchedule[k].size(); r++)
            {
                process += 1;
                fprintf(f, "x\t%d\t%d\t%d\t%.6f\n",machineSchedule[k][r],k,r, 1.0f);
            }
            
        }
        assert(process == _instance->getProcessCount());
        //Create s_i and p'_i
        for(int i = 0; i < n; i++){
            float val = processStart[i];
            fprintf(f, "s\t%d\t%.6f\n", i, val);
            val = getProcessRealTime(i);
            fprintf(f, "p\t%d\t%.6f\n",i,val);
        }
        
        //Create h_kr
        for(int k = 0; k< m; k++)
        {
            for(int r = 0; r < machineSchedule[k].size(); r++)
            {
                float val = processStart[machineSchedule[k][r]];
                fprintf(f, "h\t%d\t%d\t%.6f\n",k,r,val);
            }
        }
        //Create y_kr and t^idle_kr
        for(int k = 0; k < m; k++)
        {
            for(int r = 0; r < (int)machineSchedule[k].size() - 1; r++)
            {
                bool _turnOnOff =  turnOnOffAfter(k,r);
                float val = _turnOnOff? 1.0f: 0.0f;
                
                fprintf(f, "y\t%d\t%d\t%.6f\n",k,r,val);
                val = (float)_turnOnOff? 0.0f: processStart[machineSchedule[k][r+1]] - processStart[machineSchedule[k][r]] - getProcessRealTime(machineSchedule[k][r]);
                fprintf(f, "t\t%d\t%d\t%.6f\n",k,r,val);
            }
        }
        
        
        fprintf(f, "C\t%.6f\n",(float)totalTime);
    };
    
};

#endif /* sfjs_solution_h */
