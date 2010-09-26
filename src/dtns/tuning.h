#ifndef TUNING_H
#define TUNING_H

#include <map>
#include <vector>
#include <string>


class Tuning {

    public:
        bool possiblematch(std::map<double,double> input);
        bool useinsearch(double x);
        int numsearchedfor();
        void define(std::string name, double BD, double min, double max);
        void check(double dur);

    private:
        struct DTN {
            double BD;
            double min;
            double max;
        };
        std::vector<double> checks;
        std::map<std::string, DTN> tunings;
};

#endif
