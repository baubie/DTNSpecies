#include "tuning.h"

using namespace std;

bool Tuning::useinsearch(double x)
{
    bool r = false;
    for (map<string,DTN>::iterator i = this->tunings.begin(); i != this->tunings.end(); i++)
        r = r || i->second.BD == x || i->second.min == x || i->second.max == x;
    for (vector<double>::iterator j = this->checks.begin(); j != this->checks.end(); j++)
        r = r || *j == x;
    return r;
}

bool Tuning::possiblematch(map<double,double> input) 
{
    bool r = false;
    map<double,double>::iterator it_input;
    map<string, DTN>::iterator it_tunings;

    for (it_tunings = this->tunings.begin(); it_tunings != this->tunings.end(); it_tunings++)
    {
        bool possible = true;
        if (input[it_tunings->second.BD] <= 0.5) possible = false;
        if (possible) {
            for (it_input = input.begin(); it_input != input.end(); it_input++)
            {

                // Could this be the one?
                if (it_input->second > input[it_tunings->second.BD])
                {
                    possible = false;
                }
                if (it_input->first <= it_tunings->second.min && it_input->second > 0.2)
                {
                    possible = false;
                }
                if (it_input->first > it_tunings->second.min && 
                    it_input->first < it_tunings->second.max &&
                    it_input->second < 0.2)
                {
                    possible = false;
                }
                if (it_input->first >= it_tunings->second.max && it_input->second > 0.2)
                {
                    possible = false;
                }

            }
        }
        r = r || possible;
    }

    return r;
}

void Tuning::define(string name, double BD, double min, double max)
{
    DTN t;
    t.BD = BD;
    t.min = min;
    t.max = max;
    this->tunings[name] = t;
}

void Tuning::check(double check)
{
    this->checks.push_back(check);
}
