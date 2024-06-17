//
//  sfjs_deleteLocalSearch.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 13/12/23.
//

#ifndef sfjs_deleteNLocalSearch_h
#define sfjs_deleteNLocalSearch_h
#include "sfjs_localSearchs.h"
#include "sfjs_builder.h"
//TODO:stop time
class SFJS_deleteNLS: public  SFJS_LocalSearch{
private:
    const SFJS_Instance & _instance;
    SFJSBuilder &_builder;
    int _numberOPerations;
    bool _firstImprovment;
    
public:
    SFJS_deleteNLS(const SFJS_Instance &instance, SFJSBuilder& builder, int numberOPerations = 1, bool firstImprovment = false):   SFJS_LocalSearch(false, __DBL_MAX__), _instance(instance),
        _builder(builder), _numberOPerations(numberOPerations), _firstImprovment(firstImprovment){};
    
    void shake(SFJS_Solution& solution,  std::mt19937& rnd,  float epsilon=0) const override {
        int  n = _instance.getProcessCount();
        std::vector<bool> visited(n,true);
        std::vector<std::set<int> > succesorsLst;
        std::vector<int>  invertedTopologyList;
        solution.BuildSucessorsDeleteLists(succesorsLst,invertedTopologyList);
        for(int i =0; i < _numberOPerations; i++)
        {
            std::vector<int> candidates;
            for(int j = 0; j < visited.size(); j++){ if(visited[j]) candidates.push_back(j);}
            if(candidates.size() == 0) break;
            int op = candidates[rnd() % candidates.size()];
            solution.deleteProcess(op,invertedTopologyList, succesorsLst, visited);
            //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
        }
        
        _builder.partialConstructSolution(solution, visited, rnd);
       // assert(_solution.isFeasible());
        //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
      
    };
    
    void search( SFJS_Solution& solution, std::mt19937& rnd, double startTime=0, bool oneIteration = false) const override {

        int n =  _instance.getProcessCount();
        int neigs = 0;
        int iter = 0;
        int bestObj =0;
        //int gamma_extra = _instance.getExtraEnergyConsumtion();
        
        do{
            SFJS_Solution _solution = solution;
            bestObj =solution.value();
           // assert(abs(_solution.fo() - _solution.value()) <= 0.001);
            bool notImproved = true;
            int deleted = 0;

            std::vector<std::set<int> > succesorsLst;
            std::vector<int>  invertedTopologyList;
            solution.BuildSucessorsDeleteLists(succesorsLst,invertedTopologyList);
            for(int i =0; i < n; i++)
            {
                neigs++;
                std::vector<bool> _visited(n,true);
                SFJS_Solution _aux1 =_solution;
                std::vector<int> invLst =invertedTopologyList;
                _aux1.deleteProcess(i,invLst,succesorsLst, _visited);
                 if(_numberOPerations > 1)
                {
                     for(int j = 0; j < n && notImproved; j++){
                        if(_visited[j])
                        {
                            //std::cout  << i << " ; "<< j << "\n";
                            SFJS_Solution _aux2 = _aux1;
                            std::vector<bool> visited_aux(_visited.begin(), _visited.end());
                            std::vector<int> invLst2 =invLst;
                            _aux2.deleteProcess(j,invLst2,succesorsLst,visited_aux);
                           _builder.partialConstructSolution(_aux2, visited_aux, rnd);
                           if (_aux2.value() < solution.value())
                           {
                               solution = _aux2;
                               notImproved = !_firstImprovment;
                               
                           }
                           //assert(abs(_solution.fo() - _solution.value()) <= 0.001);
                        }
                    }
                    
                }
                else{
                    _builder.partialConstructSolution(_aux1, _visited, rnd);
                    if (_aux1.value() < solution.value())
                    {
                        solution = _aux1;
                        notImproved =  !_firstImprovment;
                        break;
                    }
                }
               
                deleted++;
            }
            
            //assert(solution_best.isFeasible());
            //assert(abs(solution_best.fo() - _solution.value()) <= 0.001);
            iter++;
         }while(!oneIteration && solution.value() < bestObj && !exceedsTime(startTime));
        solution.ls_neigborhoodSize += neigs;
        solution.best_iteration = solution.iterations += iter;
    };
};

#endif /* sfjs_deleteLocalSearch_h */
