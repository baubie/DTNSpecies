#include "synapse.h"





double Synapse::dt;
boost::unordered_map<double,double> Synapse::cache;




Synapse::Synapse()
{
	this->E = 0;
	this->tau1 = 0.5;
	this->tau2 = 0.5;
	this->gMax = 30;
	this->del = 0;
}

void Synapse::reset()
{
    Synapse::cache.clear();
}

void Synapse::prepare(double tau1, double tau2)
{
    std::pair<double,double> tau(tau1,tau2);
    for (double t = 0; t < 11*tau2; t+=Synapse::dt)
    {
        double key = tau1+1000*tau2+1000000*t;
        if (tau1 == tau2) 
        {
            double tt = t/tau1;
            Synapse::cache[key] = (tt)*exp(1-tt);
        } else {
            Synapse::cache[key] = (exp(-t/tau1)-exp(-t/tau2));
        }
    }
}

double Synapse::I(double t, double V)
{
	return -1*this->g(t)*(V-this->E);
}

double Synapse::g(double t)
{
	double g=0;
	std::vector<double>::iterator it;
	for (it = this->spikes.begin(); it < this->spikes.end(); it++)
	{
		g += this->alpha((t-del)-*it);
	}
	return g;
}

double Synapse::alpha(double t)
{
    if (t < 0 || t > 10*tau2) return 0;

    if (tau1 == tau2) 
    {
        double tt = t/tau1;
        return gMax*(tt)*exp(1-tt);
    } else {
        return gMax*(exp(-t/tau2)-exp(-t/tau1));
    }
}

