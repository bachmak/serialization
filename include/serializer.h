#include <ostream>
#include <iostream>

class Serializer;

class Access
{
public:
    template <typename T>
    void Serialize(Serializer &s, T &t)
    {
        t.serialize(s);
    }

private:
};

class OutputArchive
{
public:
    template <typename T>
    OutputArchive& operator<<(T &t)
    {
        Serializer s();
        Access a;
        a.Serialize(s, t);
        return *this;
    }

    OutputArchive(std::ostream& os = std::cout) : os(os) {}

private:
    std::ostream& os;
};

class Serializer
{
public:
    template <typename T>
    std::ostream &operator&(T &t)
    {
        return os << t << ' ';
    }

    Serializer(std::ostream& os) : os(os) {}

private:
    std::ostream& os;
};