#ifndef TUNING_H
#define TUNING_H

#include <map>
#include <math.h>

class Tuning {

    public:
        std::map<double,double> counts;
        float score(std::map<double,double> input);

};

#endif
