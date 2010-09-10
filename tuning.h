#ifndef TUNING_H
#define TUNING_H

#include <map>
#include <sstream>
#include <string>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <numeric>
#include <algorithm>

class Tuning {

    public:
        Tuning(double start, double end, double interval): start(start),end(end),interval(interval) {}
        double score(std::map<double,double> input);
        bool possiblematch(std::map<double,double> input);
        bool useinsearch(double x);
        int numsearchedfor();
        void define(double x, double y);
        void define(double x, double y, double extra);
        void smooth();
        std::string print();
        void test();

    private:
        std::map<double,double> counts;
        std::map<double,double> extraPenalty;
        double start;
        double end;
        double interval;
};

#endif
