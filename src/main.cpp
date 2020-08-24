#include <iostream>
#include "serializer.h"

class Simple
{
private:
    friend class Access;
    template <typename Serializer>
    void serialize(Serializer& s)
    {
        s & a;
        s & b;
        s & c;
        s & d;
    }

    int a = 23;
    double b = 2.5;
    float c = 3.14;
    char d = 'd';
};

int main(int argc, char const *argv[])
{

    OutputArchive oa(std::cout);
    Simple simple = Simple();
    oa << simple;
    
    return 0;
}
