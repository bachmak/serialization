#include <iostream>
#include <fstream>
#include "serialization.h"

class Simple
{
public:
    explicit Simple(int a, double b, float c, char d) : a(a), b(b), c(c), d(d) {}
    explicit Simple() = default;

private:
    friend struct Access;

    template <typename Serializer>
    void serialize(Serializer &s)
    {
        s &a;
        s &b;
        s &c;
        s &d;
    }

    int a = 0;
    double b = 0.0;
    float c = 0.0;
    char d = 'a';
};

int main(int argc, char const *argv[])
{
    {
        std::ofstream output("data.txt", std::ios_base::binary);
        OutputArchive oa(output);
        Simple simple_1(23, 2.5, 3.14, 'd');
        oa << simple_1;
    }

    {
        std::ifstream input("data.txt", std::ios_base::binary);
        InputArchive ia(input);
        Simple simple_2;
        ia >> simple_2;
    }
    return 0;
}
