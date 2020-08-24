#include <ostream>
#include <iostream>

class OutputSerializer;
class InputSerializer;

class Access
{
public:
    template <typename Serializer, typename T>
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
    OutputArchive &operator<<(T &t)
    {
        OutputSerializer s(os);
        Access a;
        a.Serialize(s, t);
        return *this;
    }

    OutputArchive(std::ostream &os = std::cout) : os(os) {}

private:
    std::ostream &os;
};

class InputArchive
{
public:
    template <typename T>
    InputArchive &operator>>(T &t)
    {
        InputSerializer s(is);
        Access a;
        a.Serialize(s, t);
        return *this;
    }

    InputArchive(std::istream &is = std::cin) : is(is) {}

private:
    std::istream &is;
};

class OutputSerializer
{
public:
    template <typename T>
    std::ostream &operator&(T &t)
    {
        return os << t << ' ';
    }

    OutputSerializer(std::ostream &os) : os(os) {}

private:
    std::ostream &os;
};

class InputSerializer
{
public:
    template <typename T>
    std::istream &operator&(T &t)
    {
        return is >> t;
    }

    InputSerializer(std::istream &is) : is(is) {}

private:
    std::istream &is;
};