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

void Tuning::reldefine(double x1, unsigned int rel, double x2)
{
    // Define a relative definition
    // We assume that x2 is already defined
    // and we are saying what x1 should be relative to it
    Relation r;
    r.x1 = x1;
    r.x2 = x2;
    r.rel = rel;
    this->relations.push_back(r);
}

bool Tuning::useinsearch(double x)
{
    bool r = false;
    std::vector<Relation>::iterator it_rel;
    for (it_rel = this->relations.begin(); it_rel != this->relations.end(); it_rel++)
        if (it_rel->x1 == x || it_rel->x2 == x) r = true;

    return this->extraPenalty.find(x) != this->extraPenalty.end() || r;
}

int Tuning::numsearchedfor()
{
    return (int)this->extraPenalty.size()+(int)this->relations.size();
}

bool Tuning::possiblematch(std::map<double,double> input) 
{
    bool r = true;
    std::map<double,double>::iterator it_count, it_input;
    std::vector<Relation>::iterator it_rel;
    for (it_count = this->extraPenalty.begin(); it_count != this->extraPenalty.end(); it_count++)
    {
        it_input = input.find(it_count->first);
        if (it_input != input.end() && this->useinsearch(it_count->first))
        {
           if (it_input->second == 0 && this->counts.find(it_count->first)->second > 0) r = false;
           if (it_input->second > 0 && this->counts.find(it_count->first)->second == 0) r = false;
        }
    }

    // We pass the basic test, now check relations
    if (r)
    {
        for (it_rel = this->relations.begin(); it_rel != this->relations.end(); it_rel++)
        {
            switch (it_rel->rel) 
            {
                case Tuning::LTEQ:
                    if (input[it_rel->x1] > input[it_rel->x2]) r = false; 
                    break;
                case Tuning::LTEQNZ:
                    if (input[it_rel->x1] > input[it_rel->x2] || input[it_rel->x1] == 0) r = false; 
                    break;
                case Tuning::LT:
                    if (input[it_rel->x1] >= input[it_rel->x2]) r = false; 
                    break;
                case Tuning::LTNZ:
                    if (input[it_rel->x1] >= input[it_rel->x2] || input[it_rel->x1] == 0) r = false; 
                    break;
                case Tuning::GTEQ:
                    if (input[it_rel->x1] < input[it_rel->x2]) r = false; 
                    break;
                case Tuning::GT:
                    if (input[it_rel->x1] <= input[it_rel->x2]) r = false; 
                    break;
            }
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

