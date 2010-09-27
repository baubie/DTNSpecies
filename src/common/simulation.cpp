#include "simulation.h"

std::vector<double> Simulation::timesteps()
{
	int steps = T/dt;
	std::vector<double> r;
	r.resize(steps,0);

	for (int i = 0; i < steps; ++i)
		r[i] = i*dt-del; 

	return r;
}
std::vector<double> Simulation::voltagetrace()
{
	return this->m_Vstored;
}
std::vector<double> Simulation::spikes()
{
	return this->m_Spikes;
}

double Simulation::dV(double V, double I)
{
	double r = (-this->gL*(V-this->EL)+this->gL*this->dT*exp((V-this->VT)/this->dT)+I-this->w)/this->C;
	if (r > 10000) return 10000;
	return r;
}

double Simulation::dw(double w)
{
	double r = (this->a*(this->V-this->EL)-w)/this->tau_w;
	if (r > 10000) return 10000;
	return r;
}

void Simulation::runSim()
{
	int steps = T/dt;
	if (this->useVoltage) this->m_Vstored.reserve(steps);
	double k1,k2,k3,k4;
	double I;
	std::vector<Synapse>::iterator synit;
	for (int i = 0; i < steps; ++i)
	{
            I = 0;
            for(synit = this->synapses.begin(); synit < this->synapses.end(); synit++)
            {
                    I += synit->I(i*dt-del,V);
            }

            k1 = this->dV(V,I);
            k2 = this->dV(V+0.5*dt*k1,I);
            k3 = this->dV(V+0.5*dt*k2,I);
            k4 = this->dV(V+dt*k3,I);
            V = V + dt*(k1+2*k2+2*k3+k4)*0.16666666666;

            k1 = this->dw(w);
            k2 = this->dw(w+0.5*dt*k1);
            k3 = this->dw(w+0.5*dt*k2);
            k4 = this->dw(w+dt*k3);
            w = w + dt*(k1+2*k2+2*k3+k4)*0.166666666666;

            if (V > 0)
            {
                    if (this->useVoltage) this->m_Vstored.push_back(40);
                    this->m_Spikes.push_back(i*dt);
                    V = Vr;
                    w = w + b;
            } else {
                    if (this->useVoltage) this->m_Vstored.push_back(V);
            }

	}
}

void Simulation::defaultparams()
{
	// Simulation Parameters
        this->dt = 0.05;
	this->T = 333;
	this->del = 10;

	this->C = 200;
	this->gL = 10;
	this->EL = -58;
	this->VT = -50;
	this->dT = 2;
	this->a = 2;
	this->tau_w = 120;
	this->b = 10;
	this->Vr = -46;

	this->V = this->EL;

	this->useVoltage = false;
}
