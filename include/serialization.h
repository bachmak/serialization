#pragma once

#include "serializer.h"

#include <iostream>

class OutputArchive
{
public:
    template <typename T>
    void operator<<(T &t)
    {
        OutputSerializer s(os);
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
        InputSerializer s(is);
        Access::serialize(s, t);
    }

    InputArchive(std::istream &is) : is(is) {}

private:
    std::istream &is;
};