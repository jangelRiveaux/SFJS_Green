//
//  sfjs_methaheuristictest.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 06/02/24.
//

#ifndef sfjs_methaheuristictest_h
#define sfjs_methaheuristictest_h


#include "../../sfjs/sfjs_builder.h"
#include "../../sfjs/sfjs_solution.hpp"
#include "../../sfjs/sfjs_instance.hpp"
#include "../../sfjs/sfjs_instance_pool.h"
#include "../../sfjs/sfjs_localSearchs.h"
#include "../../sfjs/sfjs_localSearchsConditional.h"
#include "../../sfjs/sfjs_deleteLocalSearch.h"
#include "../../sfjs/sfj_deleteConditionalLocalSearch.h"
#include "../../sfjs/sfjs_deleteNLocalSearch.h"
#include "../test_utils.h"
#include <cassert>
#include "../../Heuristics/SimulatedAnealing.h"
#include "../../Heuristics/VNS.h"
#include "../../Heuristics/VNS_0.h"
#include "../../Heuristics/VNS_1.h"
#include "../../Heuristics/Metaheuristic.h"

enum MethaheuristicID{
    MultiStart = 2,
    GraspLS = 3,
    GraspIG = 5,
    VNS_0 = 7,
    VNS_1 = 11,
    VNS_2 =  13,
    VNS_3 =  17,
    VNS_4 = 19,
    SimulatedAnniling1 = 23,
    SimulatedAnniling2 = 29,
};


class SFJSTest{
protected:
    StopRule _rule;
    double _stop_argument;
    const SFJS_Instance& _instance;
    std::map<int, Metaheuristic<SFJS_Solution>*> methaheuristics;
    
    SFJSBuilder* builder;
    SFJS_LocalSearch* voidLs;
    SFJS_simpleInsertionLS* simpleInsertionLS;
    SFJS_deleteLS<SFJSBuilder>* deleteSimpleInsertLS;
    SFJS_deleteNLS* delete1LS;  SFJS_deleteNLS* delete2LS;
public:
    
    
    /** Initialise a Test
     * @param rule stop criteria used for tests
     * @param stop_argument stop criteria value used based on the stop criteria
     */
    SFJSTest(const SFJS_Instance& instance, StopRule rule, double stop_argument): _instance(instance),_rule(rule), _stop_argument(stop_argument){
        createAlgorithm();
    };
    
    ~SFJSTest()
    {
        for(auto elem : methaheuristics)
        {
            if(elem.second) delete elem.second;
        }
        
        if(builder) delete builder;
       if(voidLs) delete voidLs;
        if(simpleInsertionLS) delete simpleInsertionLS;
        if(deleteSimpleInsertLS) delete deleteSimpleInsertLS;
        if(delete1LS) delete delete1LS;
        if(delete2LS) delete delete2LS;
    }
    
    void createAlgorithm(){
        builder = new SFJSBuilder(_instance);
        std::mt19937 _rand;
        SFJS_Solution* sol = builder->constructSolution(_rand);
        std::vector<double> initialChromosome = builder->buildChromosome();
        voidLs = new SFJS_LocalSearch();
        simpleInsertionLS = new SFJS_simpleInsertionLS(_instance,true);
        deleteSimpleInsertLS = new SFJS_deleteLS<SFJSBuilder>(_instance,*builder, true);
        delete1LS = new SFJS_deleteNLS(_instance,*builder,true);
        delete2LS = new SFJS_deleteNLS(_instance,*builder,2,true);
        std::vector<SFJS_LocalSearch*> localSearchsVNS_0 = {simpleInsertionLS, deleteSimpleInsertLS};
        std::vector<SFJS_LocalSearch*> localSearchsVNS_1 = {deleteSimpleInsertLS, simpleInsertionLS};
        std::vector<SFJS_LocalSearch*> localSearchsVNS_4 =  {simpleInsertionLS, deleteSimpleInsertLS, delete1LS, delete2LS};
        
        methaheuristics[MethaheuristicID::MultiStart] =  new GRASP<SFJS_Solution,SFJSBuilder ,SFJS_LocalSearch>(builder,voidLs,_rule,_stop_argument,"MS");
        methaheuristics[MethaheuristicID::GraspLS] =  new GRASP<SFJS_Solution,SFJSBuilder ,SFJS_LocalSearch>(builder,simpleInsertionLS,_rule,_stop_argument,"GRASP");
        methaheuristics[MethaheuristicID::GraspIG] =  new GRASP<SFJS_Solution,SFJSBuilder ,SFJS_LocalSearch>(builder,deleteSimpleInsertLS,_rule,_stop_argument,"GRASP-IG");
        methaheuristics[MethaheuristicID::VNS_0] = new VNS0<SFJS_Solution,SFJSBuilder , SFJS_LocalSearch>(builder,localSearchsVNS_0,_rule,_stop_argument, "VNS_0");
        methaheuristics[MethaheuristicID::VNS_1] = new VNS0<SFJS_Solution,SFJSBuilder , SFJS_LocalSearch>(builder,localSearchsVNS_1,_rule,_stop_argument, "VNS_1");
        methaheuristics[MethaheuristicID::VNS_2] = new VNS1<SFJS_Solution,SFJSBuilder , SFJS_LocalSearch>(builder,localSearchsVNS_0,_rule,_stop_argument, "VNS_2");
        methaheuristics[MethaheuristicID::VNS_3] = new VNS<SFJS_Solution,SFJSBuilder , SFJS_LocalSearch>(builder,localSearchsVNS_0,_rule,_stop_argument, "VNS_3");
        methaheuristics[MethaheuristicID::VNS_4] = new VNS<SFJS_Solution,SFJSBuilder , SFJS_LocalSearch>(builder,localSearchsVNS_0,_rule,_stop_argument, "VNS_4");
        methaheuristics[MethaheuristicID::SimulatedAnniling1] = new SimulatedAnnealing<SFJS_Solution, SFJSBuilder, SFJS_simpleInsertionLS> (builder, simpleInsertionLS,_rule,_stop_argument, "Simulated1");
        methaheuristics[MethaheuristicID::SimulatedAnniling2] = new SimulatedAnnealing<SFJS_Solution, SFJSBuilder,  SFJS_deleteLS<SFJSBuilder> > (builder, deleteSimpleInsertLS,_rule,_stop_argument, "Simulated2");
        
        for(auto meta : methaheuristics)
        {
            meta.second->initSolution(*sol);
        }
        delete sol;
    }
    
    Setting* getSetting(int algs)
    {
        
        if(algs%MethaheuristicID::MultiStart == 0 || algs%MethaheuristicID::GraspLS == 0 || algs%MethaheuristicID::GraspIG == 0 )
        {
            return  new GRASPSetting();
        }
        if(algs%MethaheuristicID::VNS_0 == 0 || algs%MethaheuristicID::VNS_1 == 0 || algs%MethaheuristicID::VNS_2 == 0 || algs%MethaheuristicID::VNS_3 == 0 || algs%MethaheuristicID::VNS_4 == 0 )
        {
            return new  VNSSetting();
        }
        
        return new  SimulatedAnnealingSetting();

    }
    
    void printInstance(std::string totalFileName, int testId, bool consolePrint=false) {
        
        FILE* fileTotal;
#ifdef  _WIN32 //|| _WIN64
        errno_t err= fopen_s(&fileTotal,_testResultPath.c_str(), "a");
        fprintf(fileTotal, "%d\t%s\t%.2f\t", testId, _instance.getName().c_str(), _instance.getLearning());
#else
        fileTotal= fopen(totalFileName.c_str(), "a");
        fprintf(fileTotal, "%d\t%s\t%.2f\t", testId, _instance.getInstanceName().c_str(), _instance.getLearning());
      
#endif //
        if(consolePrint)
        {
            printf("%d\t%s\t%.2f\t", testId, _instance.getInstanceName().c_str(), _instance.getLearning());
        }
        fclose(fileTotal);
    }
    
    std::pair<double,double> runOneMetaheuristic(const std::map<std::string,std::string>& setting,
                          int alg, int seed, bool consolePrint = false, int testId=0){
       std::string logFile_src = "./";
        std::string _id = "0";
        if (setting.find("-logs") != setting.end())
            logFile_src =setting.at("-logs");
        if(setting.find("-start") != setting.end())
            _id  = setting.at("-start");
         Metaheuristic<SFJS_Solution>*  meta = methaheuristics.at(alg);
        //std::string testName  = logFile_src+"test_name.txt";
        std::string totaleFileName = logFile_src+_id+ "_final_restuls.txt";
        std::string learn = std::to_string(_instance.getLearning());
        std::string runName =  logFile_src+ meta->getName() + "_"+ _instance.getInstanceName() + "_"+ learn + "_" + std::to_string(seed);
        MetaheuristicTest<SFJS_Solution> tester(totaleFileName,runName ,consolePrint);
        Setting* sett = getSetting(alg);
        sett->setFromMap(setting);
        printInstance(totaleFileName, testId, consolePrint);
       Stats stat = tester.runTest(*meta, *sett, seed, testId);
        delete sett;
        return  std::pair<double,double>(stat.solution,stat.last_improvment_time);
    }
    
};



#endif /* sfjs_methaheuristictest_h */
