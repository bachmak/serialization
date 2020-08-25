#pragma once

#include "access.h"
#include "serializer.h"

#include <iostream>

class OutputArchive
{
public:
    template <typename T>
    void operator<<(T &t)
    {
        Serializer<std::ostream> s(os);
        Access::serialize(s, t);
    }

    OutputArchive(std::ostream &os) : os(os) {}

private:
    std::ostream &os;
};

class InputArchive
{
public:
    template <typename T>
    void operator>>(T &t)
    {
        Serializer<std::istream> s(is);
        Access::serialize(s, t);
    }

    InputArchive(std::istream &is) : is(is) {}

private:
    std::istream &is;
};