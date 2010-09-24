#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

template <class T>
class vectors {

    public:
        long size();
        void add(std::string name, std::vector<T> v);
        T val(std::string name);
        T val(std::string name, unsigned int pos);
        vectors<T>& operator++();
        vectors<T> operator++(int);
        void print();
        bool done();
        void reset();
        std::string toString();

    private:
       typename std::map<std::string, std::vector<T> > m_Vectors;
       std::map<std::string, unsigned int> m_Count;
       bool m_Done;
       bool m_Last;
};




template <class T>
std::string vectors<T>::toString() {
    std::stringstream ss;
    typename std::map<std::string, std::vector<T> >::iterator i;
    typename std::vector<T>::iterator v;
    for (i = this->m_Vectors.begin(); i != this->m_Vectors.end(); i++) {
        ss << i->first << " = [";
        for (v = i->second.begin(); v != i->second.end(); v++) {
            ss << *v;
            if ((v+1) != i->second.end()) ss << ", ";
        }
        ss << "]" << std::endl;
    }
    return ss.str();
}

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
    this->m_Last = false;
}

template <class T>
bool vectors<T>::done() {
    return this->m_Done;
}

template <class T>
vectors<T>& vectors<T>::operator++() {

    typename std::map<std::string, unsigned int>::iterator i;
    int count = 0;
    for (i = this->m_Count.begin(); i != this->m_Count.end(); i++) {
        i->second++;
        ++count;
        if (i->second == this->m_Vectors[i->first].size()) {
            i->second = 0;
        } else {
            if (count == this->size()) this->m_Last = true;
            return *this;
        }
    }
    // We have just rolled over
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
    if (this->m_Vectors.find(name) != this->m_Vectors.end())
        return this->m_Vectors[name][this->m_Count[name]];
    else
        std::cout << "Error, " << name << " is not in this vectors object." << std::endl;
        return T();
}

template <class T>
void vectors<T>::add(std::string name, std::vector<T> v) {
    this->m_Vectors[name] = v;
    this->m_Count[name] = 0;
    this->reset();
}



template <class T>
T vectors<T>::val(std::string name, unsigned int pos) {
    // A silly little hack to get the arbitrary position
    // Make a copy of the current object, then iterate up to pos
    // Then use that object's val(string) function to get the value
    // Hey.  it works.

    vectors<T> tmp = *this;
    tmp.reset();
    for (unsigned int i = 0; i < pos; ++i)
        tmp++;
    return tmp.val(name);
}
