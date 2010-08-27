#ifndef WVECTOR_H
#define WVECTOR_H

#include <vector>
#include <numeric>
#include <time.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

template <class T>
class wvector {

    public:
        wvector() : gen(static_cast<unsigned int>(std::time(0))), range(0,1), getrand(gen,range) {}
        void reserve(int n);
        void push_back(T item);
        void push_back(T item, float weight);
        void clear();
        float weight( typename std::vector<T>::iterator position );
        float weight( typename std::vector<T>::iterator position, float weight);

        typename std::vector<T>::iterator random();
        typename std::vector<T>::iterator begin();
        typename std::vector<T>::iterator end();
        typename std::vector<T>::iterator erase( typename std::vector<T>::iterator position );
        typename std::vector<T>::iterator erase( typename std::vector<T>::iterator first, typename std::vector<T>::iterator last );

    private:
        std::vector<T> items;
        std::vector<float> weights;

        boost::mt19937 gen;
        boost::uniform_real<long double> range;
        boost::variate_generator<boost::mt19937&, boost::uniform_real<long double> > getrand;
};

template <class T>
float wvector<T>::weight( typename std::vector<T>::iterator position )
{
    int pos = std::distance(this->items.begin(), position);
    if (this->weights.size() > pos) return this->weights[pos];
}

template <class T>
float wvector<T>::weight( typename std::vector<T>::iterator position, float weight)
{
    int pos = std::distance(this->items.begin(), position);
    if (this->weights.size() > pos) this->weights[pos]=weight;
    return weight;
}


template <class T> 
typename std::vector<T>::iterator wvector<T>::random()
{
    if (!this->items.empty())
    {
        long double rnd = getrand() * (std::accumulate(this->weights.begin(), this->weights.end(),0));
        long double sum = 0;
        for (std::vector<float>::iterator i = this->weights.begin(); i != this->weights.end(); i++) 
        {
            sum += *i; 
            if (sum >= rnd)
            {
                typename std::vector<T>::iterator it = this->items.begin()+std::distance(this->weights.begin(), i);
                return it;
            }
        }
    }
    return this->items.end();
}

template <class T> 
void wvector<T>::reserve(int n)
{
    this->items.reserve(n);
    this->weights.reserve(n);
}

template <class T> 
void wvector<T>::push_back(T item)
{
    this->push_back(item, 1);
}

template <class T> 
void wvector<T>::push_back(T item, float weight)
{
    this->items.push_back(item);
    this->weights.push_back(weight);
}

template <class T> 
void wvector<T>::clear()
{
    this->items.clear();
    this->weights.clear();
}

template <class T> 
typename std::vector<T>::iterator wvector<T>::begin()
{
    return this->items.begin();
}

template <class T> 
typename std::vector<T>::iterator wvector<T>::end()
{
    return this->items.end();
}

template <class T> 
typename std::vector<T>::iterator wvector<T>::erase( typename std::vector<T>::iterator position )
{
    return this->items.erase(position);
}

template <class T> 
typename std::vector<T>::iterator wvector<T>::erase( typename std::vector<T>::iterator first, typename std::vector<T>::iterator last )
{
    return this->items.erase(first,last);
}

#endif
