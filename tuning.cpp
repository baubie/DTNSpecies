#include "tuning.h"

float Tuning::score(std::map<double,double> input) 
{
    std::map<double,double>::iterator it_count, it_input;

    float penalty = 0;
    int valid = 0;

    for(it_count = this->counts.begin(); it_count != this->counts.end(); ++it_count)
    {
        it_input = input.find(it_count->first);
        if (it_input != input.end()) 
        {
            ++valid;     
            if (it_input->second + it_count->second > 0) {
                penalty += fabs(it_input->second - it_count->second)/(it_input->second + it_count->second);
            }
        }
    }

    // Return a score in [0,1] with 0 being poor and 1 being perfect.
    if (valid == 0) return 0; // Unable to produce score so fail.
    return 1- penalty / valid;
}
