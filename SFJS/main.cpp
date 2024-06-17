//
//  main.cpp
//  LRKGA
//
//  Created by Jose Angel Riveaux on 10/08/22.
//
#include <iostream>
//#include "stt_kovacs/STT_Decoder_v2.hpp"
//#include "stt_kovacs/KovacInstancePool.hpp"
//#include "tests/TestLogManager.h"
//#include "stt_kovacs/KovacInstance.hpp"
//#include "qclique_partition/DecoderHCB.hpp"
#include <math.h>
#include <vector>
//#include "tests/qclique/DecoderHCBTest.hpp"
//#include "tests/LRKGA/TestUtils.hpp"
//#include "qclique_partition/GeneraPDecoder.hpp"
//#include "tests/strsp/STRSP_Test.hpp"
//#include "tests/strsp/Soulution_Test.h"
//#include "qclique_partition/QcliquePartitionHeuristics.h"
//#include "tests/qclique/TestQCliquePartitionHeuristics.hpp"
#include "sfjs/sfjs_solution.hpp"
#include "sfjs/sfjs_instance.hpp"
#include "sfjs/sfjs_instance_pool.h"
#include "sfjs/sfjs_localSearchs.h"
#include "sfjs/sfjs_deleteLocalSearch.h"
#include "sfjs/sfjs_builder.h"
#include "Heuristics/VNS.h"
#include "Heuristics/GRASP.h"
#include "sfjs/sfjs_builder.h"
#include "tests/test_utils.h"
#include "tests/sfjs/sfjs_methaheuristictest.h"
#include "tests/sfjs/sfjs_testexecutors.h"
using namespace std;


int main(int argc, const char * argv[])
{
    //nohup ./Metaheuristics -main 1 -src /home/jose/LRKGA/remote_results/sa1_all/ -alg 23 -lst 4 -tests 10 -t 600 -start 0  -end 100 -tm 0.5 -tp 0.3 -tf 0.1 -gamma 0.95 -maxExcs 10 >> 1_sa1 &
    
    //run_test_algs();
   // return run_metaheuristic(argc,argv);
    const char* argv_t[27];
     argc = 27;
    argv_t[0] = "/home/jose/LRKGA/irace/vns-irace/Metaheuristics";
    argv_t[1] = "-maxExcs";
    argv_t[2] = "10";
    argv_t[3] = "-list";
    argv_t[4] = "4";
    argv_t[5] = "-main";
    argv_t[6] = "1";
    argv_t[7] = "-alg";
    argv_t[8] = "23";
    argv_t[9] = "-tm";
    argv_t[10] = "0.5";
    argv_t[11] = "-tp";
    argv_t[12] = "0.3";
//argv_t[11] = "-p";
    //argv_t[12] = "100";
    argv_t[13] = "-t";
    argv_t[14] ="10";
    argv_t[15] ="-tests";
    argv_t[16] ="10";
    argv_t[17] ="-start";
    argv_t[18] ="0";
    argv_t[19]="-end";
    argv_t[20]="1800";
    argv_t[21]="-tf";
    argv_t[22]="0.1";
    argv_t[23]="-gamma";
    argv_t[24]="0.95";
    

    if(argc > 2)
    {
        const char* chr = get_arg_val("-main", argc, argv_t);
        int i = -1;
        if(chr != 0) i = atoi(chr);
        if(i == 0)
        {
            return run_heuristic(argc, argv);
        }
        else if(i == 1)
        {
            return run_metaheuristic(argc,argv);
        }
        
        return run_irace(argc, argv);
    }
    std::cout << "Not params selected\n";
    return  run_heuristic(argc, argv);
}

