#pragma once

#include "access.h"

#include <iostream>
#include <stdexcept>
#include <type_traits>

class OutputSerializer
{
public:
    template <typename T>
    void operator&(T &t)
    {
        serialize(t);
    }

    OutputSerializer(std::ostream &os = std::cout) : os(os) {}

private:
    std::ostream &os;

    template <typename T, typename std::enable_if<Access::has_serialize<T>::value>::type>
    void serialize(T& t)
    {
        Access::serialize(*this, t);
    }

    template <typename T>
    void serialize(T& t)
    {
        os.write(reinterpret_cast<char*>(&t), sizeof(t));
    }
};

class InputSerializer
{
public:
    template <typename T>
    void operator&(T &t)
    {
        serialize(t);
    }

    InputSerializer(std::istream &is = std::cin) : is(is) {}

private:
    std::istream &is;

    template <typename T, typename std::enable_if<Access::has_serialize<T>::value>::type>
    void serialize(T& t)
    {
        Access::serialize(*this, t);
    }

    template <typename T>
    void serialize(T& t)
    {
        is.read(reinterpret_cast<char*>(&t), sizeof(t));
    }
};