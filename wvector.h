#ifndef WVECTOR_H
#define WVECTOR_H

#include <vector>

template <class T>
class wvector {

    public:
        T random();
        void reserve(int n);
        void push_back(T item);
        void push_back(T item, float weight);
        void clear();

        std::vector<T>::iterator begin();
        std::vector<T>::iterator end();
        std::vector<T>::iterator erase( std::vector<T>::iterator position );
        std::vector<T>::iterator erase( std::vector<T>::iterator first, std::vector<T>::iterator last );

    private:
        std::vector<T> items;
        std::vector<float> weights;
};

#endif
