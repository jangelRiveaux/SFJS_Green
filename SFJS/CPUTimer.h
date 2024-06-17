#pragma once
#ifndef  CPUTimer_H
#define CPUTimer_H

#include <time.h>

#ifdef  _WIN32 //|| _WIN64

#include <Windows.h>
double get_cpu_time() {
    FILETIME a, b, c, d;
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
                ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }
    else {
        //  Handle error
        return 0;
    }
}
#else
double get_cpu_time(){
    double d = (double)(clock()) /  CLOCKS_PER_SEC;

    return d;
}
#endif // _WIN32 || _WIN64
#endif // ! CPUTimer_H


