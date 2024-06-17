//
//  sfjs_builder.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 05/12/23.
//

#ifndef sfjs_builder_h
#define sfjs_builder_h

#include "sfjs_solution.hpp"
#include "utils.hpp"
#include <random>

/**
 * Implementation of a selection for the Select algorithm
 */
typedef struct  _BestSelecion{
    int operation;
    int machine;
    int start;
    int realTime;
    int energy;
    bool previusTurnOffOn = false;
    int position = -1;
}BestSelecion;

class SFJSBuilder{
    
protected:
    const SFJS_Instance& _instance;
    float _alpha;
    
    
    inline void scheduleOperation(std::set<int> &avaible_op, std::map<int,int> & dependenciesMaxEndTime, const BestSelecion &selection, SFJS_Solution *solution, std::set<int> &visited) {
        solution->schedule(selection.operation, selection.machine, selection.start, selection.realTime, selection.energy);
        visited.insert(selection.operation);//TODO: improve with boolean vector
        avaible_op.erase(selection.operation);
        //get adjacent vertices (operations) to the selected vertex v in DG
        const DGraph& graph = _instance.getDGraph();
        const std::set<int> & adjs = graph.NeighbordsOut(selection.operation);
        for(int v : adjs)
        {
            if(visited.find(v) == visited.end())
            {
                //Verify is all dependencies are already visited
                const std::set<int> & in_adjs = graph.NeighbordsIn(v);
                bool allDependenciesIn = true;
                int time = 0; //tempo maximo de acabar tudas as dependencias
                for(int w : in_adjs )
                {
                    allDependenciesIn = visited.find(w) != visited.end();
                    if(!allDependenciesIn)  break;
                    //const std::vector<int>& pSchedule =  solution->getProcessScheduled(w);
                    if(solution->getProcessStart(w) + solution->getProcessRealTime(w) > time)
                        time = solution->getProcessStart(w) + solution->getProcessRealTime(w);
                
                }
                if(allDependenciesIn)
                {
                    avaible_op.insert(v);
                    assert(time > 0);
                   dependenciesMaxEndTime[v] = time; //s(v,k) sem o w
                }
            }
        }
    }
    
    /**Selection based in the objetive funtion value*/
   inline BestSelecion select(const SFJS_Solution& solution, const std::set<int>& process, std::map<int,int> & dependenciesMaxEndTime, std::mt19937& _rand) const
    {
        std::vector<BestSelecion> restricted;
        std::vector<double> best_value;
         
       int gamma_extra = _instance.getExtraEnergyConsumtion();
       
        for(int v : process )
        {
            const std::map<int, int>& pmacs = _instance.getOperationMachineInfo(v);
            int miu = dependenciesMaxEndTime[v];
            for(auto k : pmacs)
            {
                //multiply energy
                const std::vector<int>& schedule_k =solution.getCstSchedule(k.first);
                int rho = _instance.f(v, k.first,(int) schedule_k.size());
                int gamma_k_on = _instance.getTurnOnEnergyConsumtion(k.first);
                int gamma_k_off = _instance.getTurnOffEnergyConsumtion(k.first);
                int gamma_k_idle = _instance.getIdleEnergyConsumtion(k.first);
                int gamma_k_proc = _instance.getProcessEnergyConsumtion(k.first);
                int time_k_off = _instance.getmMachineTurnOffTime(k.first);
                int time_k_on = _instance.getmMachineTurnOnTime(k.first);
                int Gamma, Zeta;
                bool turnOnOff = false;
                if(schedule_k.size() == 0)
                {
                    Zeta = int(std::fmaxf(_instance.getmMachineTurnOnTime(k.first),miu));
                    int ext = Zeta + rho + time_k_off - solution.getTotalTime();
                    Gamma =gamma_k_proc*rho +gamma_extra * (ext > 0? ext : 0) +gamma_k_on + gamma_k_off;
                }
                else{
                    int w = schedule_k[schedule_k.size()-1];
                    int r_kmac =   solution.getProcessStart(w)+ solution.getProcessRealTime(w);
                    int Zeta1 = r_kmac > miu? r_kmac: miu;
                    int ext = Zeta1 + rho + time_k_off - solution.getTotalTime();
                    int Gamma1 =gamma_extra * (ext > 0? ext : 0) +gamma_k_idle*(Zeta1 - r_kmac);
                    int Zeta2 = r_kmac + time_k_off + time_k_on > miu ? r_kmac + time_k_off + time_k_on : miu;
                    ext = Zeta2 + rho + time_k_off - solution.getTotalTime();
                    int Gamma2 =gamma_extra * (ext > 0? ext : 0) +gamma_k_on + gamma_k_off;
                    turnOnOff = Gamma1 > Gamma2 || (Gamma1 == Gamma2 && Zeta1 > Zeta2  );
                    if(!turnOnOff)
                    {
                        Gamma = gamma_k_proc*rho + Gamma1;
                        Zeta = Zeta1;
                    }
                    else{
                        Gamma = gamma_k_proc*rho + Gamma2;
                        Zeta = Zeta2;
                    }
                }
                
                double value= double(Gamma) + double(Zeta)/ powf(10.0, 5);
                int pos  = getInsertSortedPosition(best_value, -value);
                
                BestSelecion result;
                result.operation = v;
                result.machine = k.first;
               //TODO: REMOVE ASSERTS
               /* if(solution.getCstSchedule(k.first).size() > 0)
                {
                    auto w = *solution.getCstSchedule(k.first).rbegin();
                    assert(Zeta >= solution.getProcessStart(w)+ solution.getProcessRealTime(w));
                }
                
                for(int w : solution.getDisjunctiveGraph().NeighbordsIn(v))
                {
                    assert(w == -1 ||  Zeta >=  solution.getProcessStart(w) + solution.getProcessRealTime(w));
                  
                }*/
                result.start = Zeta;
                result.realTime = rho;
                result.previusTurnOffOn = turnOnOff;
                result.energy = Gamma;
                restricted.insert(restricted.begin() + pos, result);
                best_value.insert(best_value.begin() + pos, -value);
            }
        }
       int restricted_size = 1;
         int select = 0;
         BestSelecion& selecion = restricted[0];
       if(_alpha > 0.0f)
       {
           restricted_size =  _alpha * restricted.size();
           if (restricted_size > 1)
           {
               select =_rand() % restricted_size;
               //std::cout << restricted_size;
               selecion = restricted[select];
           }
       }
        return selecion;
    }
    
public:
    SFJSBuilder(const SFJS_Instance& instance, float alpha = 0) :
    _instance(instance) , _alpha(alpha){
        
    }
    
    
    void partialConstructSolution(SFJS_Solution& solution, std::vector<bool>& visited,
                                  std::mt19937& _rand)
    {
        std::vector<int> order;
        partialConstructSolution( solution,visited,order,_rand);
    }
    
    std::vector<double> buildChromosome()
    {
        _alpha = 0;
        SFJS_Solution solution(_instance);
        std::vector<bool> visited;
        std::vector<int> order;
        std::mt19937 _rand(7);
        partialConstructSolution(solution,visited,order,_rand);
        std::vector<double>  chromosome(2* order.size(),0);
        int n = (int)order.size();
        
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        for(int i = 0; i < order.size(); i++ )
        {
            chromosome[order[i]] = 1.0f/(i+2.0f);
            chromosome[n+i] =  dist(_rand);
        }
        return chromosome;
    }
    
    void partialConstructSolution(SFJS_Solution& solution, std::vector<bool>& visited, std::vector<int>& order,
                                  std::mt19937& _rand)
    {
        auto graph = _instance.getDGraph();
        std::vector<int> degs(graph.getNodesCount(),0);
        std::set<int> avaible_op;
        std::map<int,int>  dependenciesMaxEndTime;
        
        for(auto v: graph.Vertices())
        {
            dependenciesMaxEndTime[v] = 0;
            degs[v] = graph.DegreeIn(v);
            
            if(visited.size() > 0)
            {
                for(int w : graph.NeighbordsIn(v))
                {
                    if(visited[w])
                    {
                        degs[v]--;
                       // auto sch = solution.getProcessScheduled(v);
                        //TODO: remove assert
                        assert(solution.getProcessStart(w) >= 0 && solution.getProcessRealTime(w)>= 0 );
                        if(solution.getProcessStart(w)+ solution.getProcessRealTime(w)  > dependenciesMaxEndTime[v])
                            dependenciesMaxEndTime[v] =solution.getProcessStart(w)+ solution.getProcessRealTime(w) ;
                    }
                }
            }
            
            if(degs[v] == 0 && (visited.size() ==0 || !visited[v]))
            {
                avaible_op.insert(v);
            }
        }
        
        if(visited.size() ==  0) visited = std::vector<bool> (graph.getNodesCount(),false);
        
        //operation are added to avaible_op as in bfs
        while (!avaible_op.empty()) {
            //select a vertices
            BestSelecion selection = select(solution, avaible_op, dependenciesMaxEndTime, _rand); //selected vertex,
            order.push_back(selection.operation);
            solution.schedule(selection.operation, selection.machine, selection.start, selection.realTime, selection.energy);
         
            visited[selection.operation] = true;
            avaible_op.erase(selection.operation);
           for(auto j :  graph.NeighbordsOut(selection.operation))
           {
               dependenciesMaxEndTime[j] = int(std::fmax(selection.start + selection.realTime, dependenciesMaxEndTime[j]));
               degs[j]--;
               if (degs[j] == 0) {
                   avaible_op.insert(j);
               }
           }
        }
    }
    

    
    SFJS_Solution* constructSolution(std::mt19937& _rand, float alpha=0)
    {
        _alpha = alpha;
         SFJS_Solution* solution = new SFJS_Solution(_instance);
        std::vector<bool> visited;
        partialConstructSolution(*solution,visited, _rand);
        return solution;
    }

    SFJS_Solution buildSolution(std::mt19937& _rand, float alpha = 0)
    {
        _alpha = alpha;
        SFJS_Solution solution(_instance);
        std::vector<bool> visited;
        partialConstructSolution(solution, visited, _rand);
        return solution;
    }
    

    
};
#endif /* sfjs_builder_h */
