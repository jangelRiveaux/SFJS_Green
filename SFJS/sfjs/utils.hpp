
#ifndef Utils_hpp
#define Utils_hpp

#include <utility>
#include <vector>
#include <math.h>

class load_file_exception : public std::exception{
    public:
        load_file_exception(const char* _message) : message(_message){}
        const char* message;
};


int compare (const void * a, const void * b)
{
    std::pair<int,double>* first = (std::pair<int,double>*)a;
    std::pair<int,double>* second = (std::pair<int,double>*)b;
    if(first->second  < second->second) return -1;
    if(first->second  > second->second) return 1;
    return 0;
}

int compare_int (const void * a, const void * b)
{
    std::pair<int,int>* first = (std::pair<int,int>*)a;
    std::pair<int,int>* second = (std::pair<int,int>*)b;
    if(first->second  < second->second) return -1;
    if(first->second  > second->second) return 1;
    return 0;
}

double eclidian(std::vector<int> v1, std::vector<int> v2) {
    if (v1.size() != v2.size()) return -1;
    double sum = 0;
    for (int i = 0; i < v1.size(); i++)
    {
        sum += pow(v1[i] - v2[i], 2);
    }
    return sqrt(sum);
}

inline float mean(const std::vector<float>& data) {
    int n =  (int)data.size();
    float sum = 0.0f;
    for(int i = 0; i < n; ++i) {
      sum += data[i];
    }

    return sum / n;
}

inline float calculateSD(const std::vector<float>& data) {
  float _mean, standardDeviation = 0.0;
  int n =  (int)data.size();
  _mean = mean(data);

  for(int i = 0; i < n; ++i) {
    standardDeviation += pow(data[i] - _mean, 2);
  }

  return sqrt(standardDeviation / n);
}

inline int getInsertSortedPosition(const std::vector<int>& degrees, int val){
    if(degrees.size() == 0) return 0;
    int m = (int)degrees.size()/2;
    int max = (int)degrees.size();
    int min = 0;
    while(max != min){
        if(val  > degrees[m]){
            max = m;
            m = (m+min)/2;
        }
        else if(val <= degrees[m]){
            min = m+1;
            m = (min+max)/2;
        }
    }
    return m;
}

inline int getInsertSortedPosition(const std::vector< long>& degrees,  long val){
    if(degrees.size() == 0) return 0;
    int m = (int)degrees.size()/2;
    int max = (int)degrees.size();
    int min = 0;
    while(max != min){
        if(val  > degrees[m]){
            max = m;
            m = (m+min)/2;
        }
        else if(val <= degrees[m]){
            min = m+1;
            m = (min+max)/2;
        }
    }
    return m;
}

inline int getInsertSortedPosition(const std::vector<double>& degrees, double val){
    if(degrees.size() == 0) return 0;
    int m = (int)degrees.size()/2;
    int max = (int)degrees.size();
    int min = 0;
    while(max != min){
        if(val  > degrees[m]){
            max = m;
            m = (m+min)/2;
        }
        else if(val <= degrees[m]){
            min = m+1;
            m = (min+max)/2;
        }
    }
    return m;
}
#endif
