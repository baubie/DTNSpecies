#ifndef TUNING_H
#define TUNING_H

#include <map>
#include <vector>
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
        bool possiblematch(std::map<double,double> input);
        bool useinsearch(double x);
        int numsearchedfor();
        void define(double x, double y);
        void define(double x, double y, double extra);
        void reldefine(double x1, unsigned int rel, double x2);
        void smooth();
        std::string print();

        static const unsigned int LTEQ = 100;
        static const unsigned int LTEQNZ = 110;
        static const unsigned int LT = 200
        static const unsigned int LTNZ = 210
        static const unsigned int GTEQ = 300;
        static const unsigned int GT = 400;
        static const unsigned int EQ = 500;

    private:
        struct Relation {
            double x1;
            double x2;
            unsigned int rel;
        };

        std::map<double,double> counts;
        std::map<double,double> extraPenalty;
        std::vector<Relation> relations;
        double start;
        double end;
        double interval;
};

#endif
