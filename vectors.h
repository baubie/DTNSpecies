#include <map>
#include <string>
#include <iostream>

template <class T>
class vectors {

    public:
        long size();
        void add(std::string name, std::vector<T> v);
        T val(std::string name);
        vectors<T>& operator++();
        vectors<T> operator++(int);
        void print();
        bool done();
        void reset();

    private:
       typename std::map<std::string, std::vector<T> > m_Vectors;
       std::map<std::string, unsigned int> m_Count;
       bool m_Done;
};






template <class T>
void vectors<T>::print() {
    std:: cout << "[";
    typename std::map<std::string, unsigned int>::iterator i;
    for (i = this->m_Count.begin(); i != this->m_Count.end(); i++) {
        std::cout << i->first << ": " << i->second << "\t";
    }
    std::cout << "]" << std::endl;
}


template <class T>
long vectors<T>::size() {

    unsigned int r = 1;
    typename std::map<std::string, std::vector<T> >::iterator i;
    for (i = this->m_Vectors.begin(); i != this->m_Vectors.end(); i++) {
        r *= i->second.size();
    }
    return r;

}

template <class T>
void vectors<T>::reset() {

    typename std::map<std::string, unsigned int >::iterator i;
    for (i = this->m_Count.begin(); i != this->m_Count.end(); i++) {
        i->second = 0;
    }
    this->m_Done = false;
}

template <class T>
bool vectors<T>::done() {
    return this->m_Done;
}

template <class T>
vectors<T>& vectors<T>::operator++() {

    typename std::map<std::string, unsigned int>::iterator i;
    for (i = this->m_Count.begin(); i != this->m_Count.end(); i++) {
        i->second++;
        if (i->second == this->m_Vectors[i->first].size()) {
            i->second = 0;
        } else {
            return *this;
        }
    }
    // We made it all the way through
    this->m_Done = true; 
    return *this;
}

template <class T>
vectors<T> vectors<T>::operator++(int) {
    vectors<T> a = *this;
    ++(*this);
    return a;
}

template <class T>
T vectors<T>::val(std::string name) {
    return this->m_Vectors[name][this->m_Count[name]];
}

template <class T>
void vectors<T>::add(std::string name, std::vector<T> v) {
    this->m_Vectors[name] = v;
    this->m_Count[name] = 0;
    this->reset();
}
