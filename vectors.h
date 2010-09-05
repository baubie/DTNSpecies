#include <map>
#include <vector>
#include <string>

template <class T>
class Vectors {

    public:
        unsigned int size();
        typename void add(std::string name, std::vector<T> v);

    private:
        typename std::map<std::string, std::vector<T> > m_Vectors;

};
