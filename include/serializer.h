#pragma once

#include <iostream>
#include <stdexcept>

#include "traits.h"

struct Access;

template <typename Stream>
class Serializer
{
public:
    static_assert(is_ostream<Stream>::value ||
                  is_istream<Stream>::value,
                  "template argument must be a stream");
    template <typename T>
    void operator&(T &t)
    {
        serialize(t);
    }

    Serializer(Stream &stream = std::cout) : stream(stream) {}

private:
    Stream &stream;

    template <typename T>
    typename std::enable_if<is_serializable<T>::value>::type
    serialize(T &t)
    {
        Access::serialize(*this, t);
    }

    template <typename T>
    typename std::enable_if<is_iterable<T>::value>::type
    serialize(T &t)
    {
        for (auto iter = begin(t); iter != end(t); ++iter)
        {
            serialize(*iter);
        }
    }

    template <typename T>
    typename std::enable_if<!is_iterable<T>::value && is_ostream<Stream>::value>::type
    serialize(T &t)
    {
        stream.write(reinterpret_cast<char *>(&t), sizeof(t));
    }

    template <typename T>
    typename std::enable_if<!is_iterable<T>::value && is_istream<Stream>::value>::type
    serialize(T &t)
    {
        stream.read(reinterpret_cast<char *>(&t), sizeof(t));
    }
};
