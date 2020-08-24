#include <iostream>
#include <fstream>
#include <vector>
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

    SimpleStruct(int a = 1, double b = 1.0, float c = 1.0, char d = 'b') 
        : a(a), b(b), c(c), d(d) {}

    int a = 1;
    double b = 1.0;
    float c = 1.0;
    char d = 'b';
};

class SimpleClass
{
public:
    explicit SimpleClass(int a, double b, float c, char d, SimpleStruct e, std::vector<int> f)
        : a(a), b(b), c(c), d(d), e(e), f(f) {}
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
        s &f;
    }

    int a = 0;
    double b = 0.0;
    float c = 0.0;
    char d = 'a';
    SimpleStruct e;
    std::vector<int> f = {0, 0, 0, 0};
};

int main(int argc, char const *argv[])
{
    {
        std::ofstream output("data.txt", std::ios_base::binary);
        OutputArchive oa(output);
        SimpleClass simple_1(23, 2.5, 3.14, 'd', SimpleStruct(24, 2.6, 3.15, 'e'), std::vector<int>({1, 2, 3, 4}));
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
