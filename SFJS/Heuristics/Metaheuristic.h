//
//  Metaheuristic.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 06/02/24.
//

#ifndef Metaheuristic_h
#define Metaheuristic_h


enum StopRule {TIME, GENERATIONS, TARGET , IMPROVEMENT};

typedef struct _Stats{
    double solution = 0;
    double time = 0;
    double last_improvment_time = 0;
    int generations = 0;
    int last_improvmentt_iteration= 0;
    int restarts = 0;
    int grater_inprovment_distnace=0;
    int sum = 0;
    std::vector<std::pair<double, double>> _history;
    
#ifdef  _WIN32 //|| _WIN64
        double bestSolution = DBL_MAX;
#else
        double bestSolution = __DBL_MAX__;
#endif
    
    void sumHistory(const _Stats& rightStats){
        int max= (int)_history.size();
        if(rightStats._history.size() > max) max = (int)rightStats._history.size();
        int l1 = 0; int l2 = 0;
        int val1 = 0;  int val2 = 0;

        std::vector<std::pair<double, double>> newHistory;
        
        for(int i = 0; i < max; i++)
        {
            double __time = 0;
            if(l1 >_history.size())
            {
                val2 = rightStats._history[l2].first;
                __time = rightStats._history[l2].second;
                l2++;
            }
            else if(l2 >_history.size())
            {
                 val1 = _history[l1].first;
                __time = _history[l1].second;
                l1++;
            }
            else if(rightStats._history[l2].second >_history[l1].second)
            {
                 val1 = _history[l1].first;
                __time = _history[l1].second;
                l1++;
            }
            else if(rightStats._history[l2].second <_history[l1].second)
            {
                 val2 =rightStats. _history[l2].first;
                __time = rightStats._history[l2].second;
                l2++;
            }
            else
            {
                val1 = _history[l1].first;
                val2 =  rightStats._history[l2].second;
                __time = _history[l1].second;
                l1++; l2++;
            }
            
            double val = val1 + val2;
            newHistory.push_back(std::pair<double, double>(val,__time));
        }
        _history=newHistory;
    }
    
    inline _Stats& operator+=( const _Stats& rightStats)
    {
        sum += rightStats.sum;
        solution += rightStats.solution;
        generations += rightStats.generations;
        restarts += rightStats.restarts;
        time += rightStats.time;
        last_improvment_time += rightStats.last_improvment_time;
        last_improvmentt_iteration += rightStats.last_improvmentt_iteration;
        grater_inprovment_distnace += rightStats.grater_inprovment_distnace;
        bestSolution = fminf(rightStats.bestSolution, bestSolution);
        sumHistory(rightStats);
        return *this;
    }
    
    void printHistory(FILE &file) const{
        for(auto item : _history)
        {
#ifdef  _WIN32 //|| _WIN64
            fprintf_s(&file,"%.4f\t%.4f\n",item.firt,item.second);
#else
            fprintf(&file,"%.4f\t%.4f\n",item.first,item.second*1000);
#endif //
           // printf("%.4f\t%.4f\n",item.first,item.second*1000);
        }
    };
    
    void printStats() const {
        printf("%.4f\t%.4f\t%.4f\t%d\t%d\t%d\t%d\t%d\t",
                solution, time,last_improvment_time,generations, last_improvmentt_iteration,restarts, grater_inprovment_distnace,sum);
    };
    
    void fprintStats(FILE &file) const{
#ifdef  _WIN32 //|| _WIN64
        fprintf_s(&file,"%.4f\t%.4f\t%.4f\t%d\t%d\t%d\t%d\t%d\t",
                  solution, time,last_improvment_time,generations, last_improvmentt_iteration,restarts, grater_inprovment_distnace,sum);
#else
        fprintf(&file,"%.4f\t%.4f\t%.4f\t%d\t%d\t%d\t%d\t%d\t",
                solution, time,last_improvment_time,generations, last_improvmentt_iteration,restarts, grater_inprovment_distnace,sum);
#endif //
        
    };

    
  }Stats;

struct Setting{
    virtual ~Setting() = default;
    virtual void setFromMap(const std::map<std::string,std::string>& setting) = 0;
    
    virtual std::string toString() = 0;
};

template<class Solution>
class Metaheuristic{
protected:
    StopRule _rule; 
    double _stop_argument;
    Stats _stats;
    Solution lastBestSolution;
    bool isNotInitialized;
    std::string _name;
    
public:
    virtual void run(int seed, void *setting) = 0;
    
    Metaheuristic( StopRule rule, double stop_argument,  std::string name = ""):
    _rule(rule),_stop_argument(stop_argument), _name(name){
        isNotInitialized = true;
    };
    virtual ~Metaheuristic()= default;
    
    void initSolution(const Solution& solution){ 
        isNotInitialized = false;
        lastBestSolution = solution;
    }
    
    Stats getStats()const {return _stats; };
    
    std::string getName() const {return _name; };
    
    const Solution& getBestSolution()const {return lastBestSolution;};
};

template<class Solution>
class MetaheuristicTest{
private:
    std::string _testResultPath, _logFilePath;
    bool _terminalPrint;
public:
    MetaheuristicTest(std::string testResultPath, std::string logFilePath, bool terminalPrint = false):
    _testResultPath(testResultPath), _logFilePath(logFilePath), _terminalPrint(terminalPrint){
        
    }
    
    void printResult(Metaheuristic<Solution>& metaHeuristic , Setting& setting, int seed, const Stats& stats, int testId) {
        FILE* fileTotal;
#ifdef  _WIN32 //|| _WIN64
        errno_t err= fopen_s(&fileTotal,_testResultPath.c_str(), "a");
        fprintf_s(fileTotal,metaHeuristic.getName());
        fprintf_s(fileTotal,"\t");
#else
        fileTotal= fopen(_testResultPath.c_str(), "a");
        fprintf(fileTotal, "%d\t%s\t", testId, metaHeuristic.getName().c_str());
        stats.fprintStats(*fileTotal);
        fprintf(fileTotal,"%d\t",seed);
        fprintf(fileTotal,"%s\t",setting.toString().c_str());
        fprintf(fileTotal,"\n");
#endif //
        
        fclose(fileTotal);
        if(_terminalPrint)
        {
            printf("%s\t", metaHeuristic.getName().c_str());
            stats.printStats();
            printf("%d\t",seed);
            printf("%s\t",setting.toString().c_str());
            printf("\n");
        }
    }
    
    Stats runTest( Metaheuristic<Solution>& metaHeuristic , Setting& setting, int seed, int testId=0)
    {
        metaHeuristic.run(seed, &setting);
        const Stats& stats = metaHeuristic.getStats();
        printResult(metaHeuristic,setting,seed,stats,testId);
        FILE *logFile;
#ifdef  _WIN32 //|| _WIN64
        errno_t err= fopen_s(&logFile,_logFilePath.c_str(), "a");
#else
        logFile = fopen(_logFilePath.c_str(), "a");
#endif //
        if(logFile)
        {
            stats.printHistory(*logFile);
            fclose(logFile);
        }
        return stats;
    }
};

#endif /* Metaheuristic_h */
