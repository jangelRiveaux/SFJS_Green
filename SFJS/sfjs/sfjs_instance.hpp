//
//  sfjs_instance.hpp
//  Models
//
//  Created by Jose Angel Riveaux on 29/06/23.
//

#ifndef sfjs_instance_h
#define sfjs_instance_h


#include <vector>
#include <map>
#include <algorithm>
#include "../graph/DGraph.h"
#include <exception>
#include <fstream>
#include "utils.hpp"

class SFJS_Instance
{
private:
    std::string _instanceFile;
    DGraph *_dgraph;
    int process, relations, machines;
    std::vector<std::vector<int>> machinesInfo;
    std::vector<std::map<int,int >> process_machinesInfo;
    std::vector<std::set<int>>   machines_process;
    int extraEnergy;
    const float _alpha;
    void loadInfo();
    std::string _name;
    std::vector<std::vector<std::set<int>>> mac_relations;
    std::map<std::tuple<int,int,int>, std::set<int>> mac_relations3;
    std::map<int,std::tuple<int, int,int>> mac_tuple_order_all;
    std::vector<int> values_all;
    
    std::map<int,std::tuple<int, int,int>> mac_tuple_order_pairs;
    std::vector<int> values_pairs;
    
    std::map<int,std::tuple<int, int,int>> mac_tuple_order_tuples;
    std::vector<int> values_tuples;
    int T2, T3;
    void initialize();
    
public:
    SFJS_Instance(std::string instanceFile, std::string name = "", float alpha=0.3): _instanceFile(instanceFile), _name(name), _alpha(alpha){
        extraEnergy = 1500;
        loadInfo();
        initialize();
    };
    
    ~SFJS_Instance(){
        if(_dgraph) delete _dgraph;
    } ;
    
    int getProcessCount() const{return process; };
    
    int getMachineOperationProcessingTime(int operation, int machine) const
    {
        return  process_machinesInfo[operation].at(machine);
    };
    
    const std::map<int,int>& getOperationMachineInfo(int operation) const
    {
        return  process_machinesInfo[operation];
    };
    
    int getRValue() const {return T2+T3; };
    
    float getLearning() const {return  _alpha; };
    
    //#γ^proc_k γ^on_k γ^off_k γ^idle_k τ^on_k τ^off_k
    int getProcessEnergyConsumtion(int machine)const {return machinesInfo[machine][0]; };
    
    int getTurnOnEnergyConsumtion(int machine)const { return machinesInfo[machine][1]; };
    
    int getTurnOffEnergyConsumtion(int machine)const {return machinesInfo[machine][2]; };
    
    int getIdleEnergyConsumtion(int machine)const {return machinesInfo[machine][3]; };
    
    int getExtraEnergyConsumtion()const {return extraEnergy; };
    
    int getmMachineTurnOnTime(int machine) const {return machinesInfo[machine][4];  };
    
    int getmMachineTurnOffTime(int machine) const {
        return machinesInfo[machine][5];  };
    
    int f(int process, int machine, int position) const
    {
        //TODO: mudar 1/2 por 0.5f
        int result = int(getMachineOperationProcessingTime(process, machine)/powf(position+1,_alpha) +0.5);
        //assert(result >= 0);
        return result;
    };
    
    int getProcessRelationsCount() const{return relations; };
    
    int getMachinesCount() const{return machines; };
    
    const std::set<int>& getMachineProcesses(int machine) const {return machines_process[machine]; };
    
    const DGraph& getDGraph() const {return *_dgraph; };
    
    std::string getInstanceName() const { 
        return _name;
    };
    
    std::tuple<int,int,int> getMacTuple(int randProbability) const;
    
    std::tuple<int,int,int> getMacTupleAll(int randProbability) const;
    
    std::tuple<int,int,int> getMacPair(int randProbability) const;
    
    const std::vector<std::vector<std::set<int>>>& getMacPairRelations() const {return mac_relations; }
    
    const std::map<std::tuple<int,int,int>, std::set<int>>& getMacRelations3() const {
        return mac_relations3;
    }
};

void SFJS_Instance::loadInfo(){
    
#ifdef  _WIN32 //|| _WIN64
    std::ifstream ifs(_instanceFile.c_str());
    if(!ifs) throw load_file_exception("Unavaible to open task file");
    
     char line[600];
     while(ifs.getline(line, 600))
     {
         if(line[0] != '#')
             break;
         
     }
    
    sscanf_s(line,"%d %d %d\n",&process,&relations,&machines);
#else
    FILE *file  = fopen(_instanceFile.c_str(), "r");
    if(file == 0) throw load_file_exception("Unavaible to open task file");
    fscanf(file,"%d %d %d",&process,&relations,&machines);
#endif
  
    
    for(int i = 0; i < machines; i++) machines_process.push_back(std::set<int>());
    _dgraph = new DGraph();
    
    //create the dgraph
    for(int i = 0; i < relations; i++)
    {
        
         int x,y;
#ifdef  _WIN32// || _WIN64
        if(!ifs.getline(line,600)) throw file_wrong_format_exception("erro lndo instancia");
        sscanf_s(line,"%d %d",&x,&y);
#else
    fscanf(file,"%d %d",&x,&y);
#endif
    
      _dgraph->AddArc(x,y);
    }
    _dgraph->BuildPredecesorsLists();
    _dgraph->BuildSucessorsLists();
    //read and create the process-machine relation informations
    for(int i = 0; i < process; i++)
    {
        std::map<int,int> proc_mach;
        int m = 0;
        //if(!ifs.getline(line,600)) throw file_wrong_format_exception("erro lndo instancia");
        
#ifdef  _WIN32 //|| _WIN64
        ifs >> m;
#else
        fscanf(file,"%d",&m);
#endif
        
        for(int j = 0; j < m; j++)
        {
            int mac, tmac;
            //if(!ifs.getline(line,600)) throw file_wrong_format_exception("erro lndo instancia");
            //fscanf(file," %d %d",&mac, &tmac);
#ifdef  _WIN32 //|| _WIN64
            ifs >> mac >> tmac;
#else
            fscanf(file," %d %d",&mac, &tmac);
#endif
            proc_mach[mac] = tmac;
            machines_process[mac].insert(i);
        }
        process_machinesInfo.push_back(proc_mach);
    }
#ifdef  _WIN32 //|| _WIN64
     ifs.getline(line,600);
#endif
    //read all machines information
    for(int i = 0; i < machines; i++)
    {
        //#γ^proc_k γ^on_k γ^off_k γ^idle_k τ^on_k τ^off_k
        int eproc, eon, eoff,eidle,ton,toff;
#ifdef  _WIN32 //|| _WIN64
        if(!ifs.getline(line,600)) throw file_wrong_format_exception("erro lndo instancia");
        sscanf_s(line,"%d %d %d %d %d %d",&eproc, &eon, &eoff, &eidle, &ton, &toff);
#else
        fscanf(file,"%d %d %d %d %d %d",&eproc, &eon, &eoff, &eidle, &ton, &toff);
#endif
        
        //fscanf(file,"%d %d %d %d %d %d",&eproc, &eon, &eoff, &eidle, &ton, &toff);
        std::vector<int> mInfo = {eproc, eon, eoff, eidle, ton, toff};
        machinesInfo.push_back(mInfo);
    }
    
#ifdef  _WIN32 //|| _WIN64
    if(!ifs.getline(line,600)) throw file_wrong_format_exception("erro lndo instancia");
    sscanf_s(line,"%d",&extraEnergy);
    ifs.close();
#else
    fscanf(file,"%d",&extraEnergy);
    fclose(file);
#endif

   //fclose(file);
}



void SFJS_Instance::initialize(){
    mac_relations = std::vector<std::vector<std::set<int>>>(machines, std::vector<std::set<int>>(machines,std::set<int>()));
    std::vector<std::tuple<int,int>> mac_pairs;
    std::vector<std::tuple<int,int,int>> macs_tuples3;
    T2 = T3 = 0;
    for(int i = 0; i < machines; i++)
    {
        for(int j = i+1; j < machines; j++)
        {
            for(int elm : machines_process[i])
            {
                if(machines_process[j].find(elm) != machines_process[j].end())
                {
                    if(mac_relations[i].size() == 0) mac_pairs.push_back(std::tuple<int,int>(i,j));
                    mac_relations[i][j].insert(elm);
                    mac_relations[j][i].insert(elm);
                    T2++;
                    
                    for(int k = j+1; k < machines; k++)
                    {
                        if(machines_process[k].find(elm) != machines_process[k].end())
                        {
                            std::tuple<int,int,int> tpl(i,j,k);
                            mac_relations3[tpl].insert(elm);
                            if(mac_relations3[tpl].size() == 1) macs_tuples3.push_back(tpl);
                            T3++;
                        }
                    }
                }
            }
        }
    }
    
    int val = 0;
    for(int i = 0; i < machines; i++)
    {
        for(int j = i+1; j < machines; j++)
        {
            if( mac_relations[i][j].size() > 0)
            {
                val += mac_relations[i][j].size();
                mac_tuple_order_all[val] = std::tuple<int,int,int>(i,j,-1);
                mac_tuple_order_pairs[val] = std::tuple<int,int,int>(i,j,-1);
                values_all.push_back(-val);
                values_pairs.push_back(-val);
            }
            for(int k = j+1; k < machines; k++)
            {
                std::tuple<int,int,int> tpl(i,j,k);
                if(mac_relations3.find(tpl) != mac_relations3.end())
                {
                    val += mac_relations3[tpl].size();
                    mac_tuple_order_tuples[val] = tpl;
                    mac_tuple_order_all[val] = tpl;
                    values_tuples.push_back(-val);
                    mac_tuple_order_all[val] = tpl;
                }
            }
        }
    }
}


std::tuple<int,int,int> SFJS_Instance::getMacTuple(int randProbability) const
{
    int pos = getInsertSortedPosition(values_tuples,-randProbability);
    int val = values_tuples[pos];
    return  mac_tuple_order_tuples.at(-val);
}

std::tuple<int,int,int> SFJS_Instance::getMacTupleAll(int randProbability) const
{
    int pos = getInsertSortedPosition(values_all,-randProbability);
    int val = values_all[pos];
    return  mac_tuple_order_all.at(-val);
}
std::tuple<int,int,int> SFJS_Instance::getMacPair(int randProbability) const
{
    int pos = getInsertSortedPosition(values_pairs,-randProbability);
    int val = values_pairs[pos];
    return  mac_tuple_order_pairs.at(-val);
}

#endif /* sfjs_instance_h */
