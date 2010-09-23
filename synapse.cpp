#include "synapse.h"


Synapse::Synapse()
{
	this->E = 0;
	this->tau1 = 0.5;
	this->tau2 = 0.5;
	this->gMax = 30;
	this->del = 0;
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
	if (t < 0) return 0;
	double tt = t/tau1;
	if (tt > 10) return 0; 

    if (tau1 == tau2) 
    {
        return gMax*(tt)*exp(1-tt);
    } else {
        return gMax*(exp(-t/tau1)-exp(-t/tau2));
    }
}

