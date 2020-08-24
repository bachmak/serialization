#include <iostream>
#include <fstream>
#include "serialization.h"

struct SimpleStruct
{
    template <typename Serializer>
    void serialize(Serializer &s)
    {
        s &a;
        s &b;
        s &c;
        s &d;
    }

    int a = 1;
    double b = 1.0;
    float c = 1.0;
    char d = 'b';
};

class SimpleClass
{
public:
    explicit SimpleClass(int a, double b, float c, char d, SimpleStruct ss)
        : a(a), b(b), c(c), d(d), e(ss) {}
    explicit SimpleClass() = default;

private:
    friend struct Access;

    template <typename Serializer>
    void serialize(Serializer &s)
    {
        s &a;
        s &b;
        s &c;
        s &d;
        s &e;
    }

    int a = 0;
    double b = 0.0;
    float c = 0.0;
    char d = 'a';
    SimpleStruct e;
};

int main(int argc, char const *argv[])
{
    {
        std::ofstream output("data.txt", std::ios_base::binary);
        OutputArchive oa(output);
        SimpleClass simple_1(23, 2.5, 3.14, 'd', {24, 2.6, 3.15, 'e'});
        oa << simple_1;
    }

    {
        std::ifstream input("data.txt", std::ios_base::binary);
        InputArchive ia(input);
        SimpleClass simple_2;
        ia >> simple_2;
    }
    return 0;
}
