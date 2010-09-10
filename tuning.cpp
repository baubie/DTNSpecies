#include "tuning.h"

void Tuning::define(double x, double y)
{
    this->counts[x] = y;
}

void Tuning::define(double x, double y, double extra)
{
    this->define(x,y);
    this->extraPenalty[x] = extra;
}

bool Tuning::useinsearch(double x)
{
    return this->extraPenalty.find(x) != this->extraPenalty.end();
}

int Tuning::numsearchedfor()
{
    return (int)this->extraPenalty.size();
}

bool Tuning::possiblematch(std::map<double,double> input) 
{
    bool r = true;
    std::map<double,double>::iterator it_count, it_input;
    for (it_count = this->extraPenalty.begin(); it_count != this->extraPenalty.end(); it_count++)
    {
        it_input = input.find(it_count->first);
        if (it_input != input.end() && this->useinsearch(it_count->first))
        {
           if (it_input->second == 0 && this->counts.find(it_count->first)->second > 0) r = false;
           if (it_input->second > 0 && this->counts.find(it_count->first)->second == 0) r = false;
        }
    }
    return r;
}

void Tuning::smooth()
{
    std::map<double,double> c;
    std::map<double,double>::iterator it, it_count;
    double a, b, dy, val; // The points we are interpolating between
    int posA, posB;
    a = b = dy = val = 0;

    for (double x = this->start; x <= this->end; x += this->interval)
    {
        it = this->counts.find(x);
        if (it != this->counts.end())
        {
            val = it->second;
            posA = (it->first-this->start)/this->interval+1;
            a = val;
            it++;
            posB = (it->first-this->start)/this->interval+1;
            if (it == this->counts.end()) {posB = (this->end-this->start)/this->interval+1; b = 0;}
            else b = it->second;
            dy = (b-a) / (posB-posA);
        }
        else
        {
            val += dy;
        }
        c[x] = val;
    }
    this->counts = c;
}

std::string Tuning::print()
{
    std::stringstream ss;

    ss << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    for (std::map<double,double>::iterator i = this->counts.begin(); i != this->counts.end(); i++)
    {
        ss << "," << i->second;
    }

    return ss.str();
}

void Tuning::test()
{
    std::cout << "=========================" << std::endl;
    std::cout << "Running Tuning Score Test" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "Expect: 1   Got: " << this->score(this->counts) << std::endl;
    std::cout << "=========================" << std::endl;
}

double Tuning::score(std::map<double,double> input) 
{
    std::map<double,double>::iterator it_count, it_input;


    if (std::min_element(input.begin(), input.end())->second == std::max_element(input.begin(), input.end())->second) return 0;

    double sum = 0;
    double tmp;
    for(it_count = this->counts.begin(); it_count != this->counts.end(); ++it_count)
    {
        it_input = input.find(it_count->first);
        if (it_input != input.end()) 
        {
            tmp = (it_input->second-it_count->second);
            sum += tmp*tmp;
        }
    }


    double dist = sqrt(sum);

    // Since results are all >=0, only half of our ND sphere will be used.
    double score = pow(1/(0.1+dist),0.5)+0.75;

    // Assign the extra penalty
    for (it_count = this->extraPenalty.begin(); it_count != this->extraPenalty.end(); it_count++)
    {
        it_input = input.find(it_count->first);
        if (it_input != input.end())
        {
           if (it_input->second !=  this->counts.find(it_count->first)->second) score *= it_count->second;
        }
    }

    return score;
}
