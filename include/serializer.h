#pragma once

#include <iostream>
#include <stdexcept>

class OutputSerializer
{
public:
    template <typename T>
    std::ostream &operator&(T &t)
    {
        return os.write(reinterpret_cast<char*>(&t), sizeof(t));
    }

    inline OutputSerializer(std::ostream &os) : os(os) {}

private:
    std::ostream &os;

    template <typename T>
    typename std::enable_if<is_iterable<T>::value>::type
    serialize(T& t)
    {
        for (auto it = begin(t); it != end(t); ++it)
        {
            serialize(*it);
        }
    }

    template <typename T>
    typename std::enable_if<!is_iterable<T>::value>::type
    serialize(T& t)
    {
        os.write(reinterpret_cast<char*>(&t), sizeof(t));
    }
};

class InputSerializer
{
public:
    template <typename T>
    std::istream &operator&(T &t)
    {
        return is.read(reinterpret_cast<char*>(&t), sizeof(t));
    }

    inline InputSerializer(std::istream &is) : is(is) {}

private:
    std::istream &is;

    template <typename T>
    typename std::enable_if<is_iterable<T>::value>::type
    serialize(T& t)
    {
        for (auto it = begin(t); it != end(t); ++it)
        {
            serialize(*it);
        }
    }

    template <typename T>
    typename std::enable_if<!is_iterable<T>::value>::type
    serialize(T& t)
    {
        is.read(reinterpret_cast<char*>(&t), sizeof(t));
    }
};