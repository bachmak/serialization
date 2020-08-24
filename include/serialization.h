#pragma once

#include "access.h"
#include "serializer.h"

#include <iostream>

class OutputArchive
{
public:
    template <typename T>
    OutputArchive& operator<<(T &t)
    {
        OutputSerializer s(os);
        Access::serialize(s, t);
        return *this;
    }

    inline OutputArchive(std::ostream &os) : os(os) {}

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
        Access::serialize(s, t);
        return *this;
    }

    inline InputArchive(std::istream &is) : is(is) {}

private:
    std::istream &is;
};