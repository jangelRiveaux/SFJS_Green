//
//  sfjs_testexecutors.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 06/02/24.
//

#ifndef sfjs_testexecutors_h
#define sfjs_testexecutors_h

#include "../test_utils.h"
#include "sfjs_methaheuristictest.h"
#include "../../sfjs/sfjs_builder.h"
#include "../../sfjs/sfjs_localSearchs.h"
#include "../../sfjs/sfjs_deleteLocalSearch.h"


int run_irace(int argc, const char * argv[])
{
    std::string logFile_src = "/Users/joseangelriveaux/Desktop/PostDoc/LRKGA/LRKGA/results/sfjs/";
    
    const char* inst_arg =  argv[4];
    int seed = atoi(argv[3]);
    
    if(inst_arg == 0) {
        printf("not instance specifies\n");
        return 1;
    }
    std::string name(inst_arg);
    auto setting = get_parameters_map(argc,argv);
    float  learn = std::stof(setting["-learn"]);
    
    int alg = 2;
    if(setting.find("-alg") != setting.end())
        alg = std::atoi(setting["-alg"].c_str());
    int time = 300;
    if (setting.find("-time") != setting.end())
        time = std::atoi(setting["-time"].c_str());

    SFJS_Instance instance(name, name, learn);
    SFJSTest tester(instance,StopRule::TIME,time);
     auto result = tester.runOneMetaheuristic(setting, alg,seed);
    //std::cout << result.first << "\t" << result.second ;
    printf("%.4f\t%.4f",(float)result.first,(float)result.second);
    return  0;
}


std::vector<std::string> selectList(int instancesLst)
{
    if (instancesLst == 1)
    {
        return SFJSInstancePool::getDAFJSInstances();
    }
    if (instancesLst == 2)
    {
        return SFJSInstancePool::getYFJSInstances();
    }
   if (instancesLst == 3)
    {
        return SFJSInstancePool::getSubSetInstances();
    }
    if(instancesLst == 4)
    {
        auto lst =  SFJSInstancePool::getDAFJSInstances();
        for(auto elm : SFJSInstancePool::getYFJSInstances())
        {
            lst.push_back(elm);
        }
        return lst;
    }
    return SFJSInstancePool::getSmall();
}

int run_heuristic(int argc, const char * argv[])
{
    
    int alg = 2; int list = 4; int test = 5;
    auto setting = get_parameters_map(argc,argv);
    int stop_time = 600;
    int start = 0;
    int end = INT_MAX;
    std::string instance_src = "/Users/joseangelriveaux/Desktop/PostDoc/LRKGA/LRKGA/LRKGA/test_instances/flow-shop/all/";
    std::string resultFile = "/Users/joseangelriveaux/Desktop/PostDoc/LRKGA/LRKGA/constuctive_result/result.txt";
  if (setting.find("-src") != setting.end())
        instance_src = setting.at("-src");
    if (setting.find("-r") != setting.end())
        resultFile = setting.at("-r");
    if(setting.find("-alg") != setting.end())
        alg = std::atoi(setting["-alg"].c_str());
    if(setting.find("-lst") != setting.end())
        list = std::atoi(setting["-lst"].c_str());
    if(setting.find("-t") != setting.end())
        stop_time  = std::atoi(setting["-t"].c_str());
    if(setting.find("-start") != setting.end())
        start  = std::atoi(setting["-start"].c_str());
    if(setting.find("-end") != setting.end())
        end  = std::atoi(setting["-end"].c_str());
    std::vector<std::string> instances = selectList(list);
    
    int inst = 0;
    for(float learn : {0.30f, 0.20f, 0.10f})
    {
        for(std::string name : instances){
            SFJS_Instance instance(instance_src + name, name, learn);
            
            if(inst >= start && inst < end)
            {
                SFJSBuilder builder(instance);
                std::vector<SFJS_LocalSearch*> localSearch;
                SFJS_simpleInsertionLS ls1(instance);
                SFJS_deleteLS<SFJSBuilder> ls2(instance, builder);
                localSearch.push_back(&ls1);
                localSearch.push_back(&ls2);
                std::mt19937 _rand;
                double sol_time = get_cpu_time();
                SFJS_Solution* sol = builder.constructSolution(_rand);
                sol_time = 1000*(get_cpu_time() - sol_time);
                std::vector<double> solsData;
                std::vector<int> iters;
                for(auto ls : localSearch)
                {
                    double tm = get_cpu_time();
                    SFJS_Solution crrSol = *sol;
                    ls->search(crrSol, _rand);
                    tm = 1000*(get_cpu_time() - tm);
                    solsData.push_back(crrSol.value());
                    iters.push_back(crrSol.iterations);
                    solsData.push_back(tm);
                }
                
                FILE* fileTotal;
#ifdef  _WIN32 //|| _WIN64
                errno_t err= fopen_s(&fileTotal,resultFile.c_str(), "a");
                fprintf_s(fileTotal,metaHeuristic.getName());
                fprintf_s(fileTotal,"\t");
#else
                fileTotal= fopen(resultFile.c_str(), "a");
                fprintf(fileTotal, "%d\t%s\t%s\t%.4f\t%.4f\t%.4f\t", inst, "builder",name.c_str(), learn, float(sol->value()), float(sol_time));
                for(auto val : solsData)
                {
                    fprintf(fileTotal, "%.4f\t",float(val));
                }
                for(int it : iters)
                {
                    fprintf(fileTotal, "%d\t",it);
                }
                fprintf(fileTotal, "\n");
            
#endif //
                
                fclose(fileTotal);
                printf( "%d\t%s\t%s\t%.4f\t%.4f\t", inst, "builder",name.c_str(), learn, float(sol->value()));
                for(float val : solsData)
                {printf("%.4f\t",val);}
                for(int it : iters)
                {
                    printf("%d\t",it);
                }
                printf("\n");
            
                if(sol) delete sol;
            }
        }
    }

    return  0;
}

int run_metaheuristic(int argc, const char * argv[])
{
    
    int alg = 2; int list = 0; int test = 5;
    auto setting = get_parameters_map(argc,argv);
    int stop_time = 600;
    int start = 0;
    int end = INT_MAX;
    std::string instance_src = "/Users/joseangelriveaux/Desktop/PostDoc/LRKGA/LRKGA/LRKGA/test_instances/flow-shop/newYsmall/";
  if (setting.find("-src") != setting.end())
  {
      instance_src = setting.at("-src");
  }
 std::cout << "Reading instance from: " << instance_src << "\n";
    
    if(setting.find("-alg") != setting.end())
        alg = std::atoi(setting["-alg"].c_str());
    std::cout << "Running algorithm: " << alg << "\n";
    
    if(setting.find("-lst") != setting.end())
        list = std::atoi(setting["-lst"].c_str());
    std::cout << "Running instance list: " << list << "\n";
    
    if(setting.find("-tests") != setting.end())
        test = std::atoi(setting["-tests"].c_str());
    
    std::cout << "Number of test: " << test << "\n";
    if(setting.find("-t") != setting.end())
        stop_time  = std::atoi(setting["-t"].c_str());
    
    std::cout << "Target time (sec): " << stop_time << "\n";
    if(setting.find("-start") != setting.end())
        start  = std::atoi(setting["-start"].c_str());
    
    std::cout << "Start from instance: " << start << "\n";
    if(setting.find("-end") != setting.end())
        end  = std::atoi(setting["-end"].c_str());
    
    std::cout << "Final instance: " << end << "\n";
    std::vector<std::string> instances = selectList(list);
    
    int inst = 0;
    for(float learn : {0.30f, 0.20f, 0.10f})
    {
        for(std::string name : instances){
            SFJS_Instance instance(instance_src + name, name, learn);
            for(int i = 0; i < test; i++, inst++)
            {
                if(inst >= start && inst < end)
                {
                    std::cout << inst << "\t"<< name << "\t" << learn << "\n";
                    SFJSTest tester(instance,StopRule::TIME,stop_time);
                    tester.runOneMetaheuristic(setting, alg,i*7,true,inst);
                }
            }
        }
    }

    return  0;
}

int list_instances(int argc, const char * argv[])
{
    
    int alg = 2; int list = 0; int test = 5;
    auto setting = get_parameters_map(argc,argv);
    int stop_time = 600;
    int start = 0;
    int end = INT_MAX;
    std::string instance_src = "/Users/joseangelriveaux/Desktop/PostDoc/LRKGA/LRKGA/LRKGA/test_instances/flow-shop/newYsmall/";
  if (setting.find("-src") != setting.end())
        instance_src = setting.at("-src");
    if(setting.find("-lst") != setting.end())
        list = std::atoi(setting["-lst"].c_str());
    if(setting.find("-tests") != setting.end())
        test = std::atoi(setting["-tests"].c_str());
    if(setting.find("-start") != setting.end())
        start  = std::atoi(setting["-start"].c_str());
    if(setting.find("-end") != setting.end())
        end  = std::atoi(setting["-end"].c_str());
    std::vector<std::string> instances = selectList(list);
    
    int inst = 0;
    for(float learn : {0.30f, 0.20f, 0.10f})
    {
        for(std::string name : instances){
            SFJS_Instance instance(instance_src + name, name, learn);
            for(int i = 0; i < test; i++, inst++)
            {
                if(inst >= start && inst < end)
                {
                   // std::cout <<  inst << "\t" << instance.getInstanceName() << "\t" << learn << "\n";
                    SFJSTest tester(instance,StopRule::TIME,stop_time);
                    tester.printInstance("/Users/joseangelriveaux/Desktop/PostDoc/LRKGA/LRKGA/remote_results/instances.txt", inst,true);
                }
            }
        }
    }

    return  0;
}

void run_test_algs(){
    //LRKGA -src <src>  -logs <log> -alg 2 -lst 0 -tests 5 -t 60 -alpha 0.25 -start 5 -end 15
    //-src "/home/jangel/Desktop/LRKGA/LRKGA/test_instances/flow-shop/newYsmall/" -logs "/home/jangel/Desktop/LRKGA/results/sfjs/" -alg 29 -lst 0 -tests 2 -t 10 -alpha 0.25 -start 24 -end 30 -epsilon 0
    const char* argv_t[20];
      
    argv_t[0] = "-src";
    argv_t[1] = "/home/jangel/Desktop/LRKGA/LRKGA/test_instances/flow-shop/newYsmall/";
    argv_t[2] = "-logs";
    argv_t[3] = "/home/jangel/Desktop/LRKGA/results/sfjs/";
    argv_t[4] = "-alg";
    argv_t[5] = "29";
    argv_t[6] = "-lst";
    argv_t[7] = "0";
    argv_t[8] = "-tests";
    argv_t[9] = "2";
    argv_t[10] = "-t";
    argv_t[11] = "10";
    argv_t[12] = "-alpha";
    argv_t[13] =  "0.25";
    argv_t[14] = "-start";
    argv_t[15] = "24";
    argv_t[16] = "-end";
    argv_t[17] = "30";
    argv_t[18] = "-epsilon";
    argv_t[19] = "0";
    run_metaheuristic(20, argv_t);
}
#endif /* sfjs_testexecutors_h */
