#pragma once

#include "access.h"
#include "serializer.h"

#include <iostream>

template <typename Stream>
class Archive
{
public:
    static_assert(is_ostream<Stream>::value ||
                  is_istream<Stream>::value,
                  "template argument must be a stream");

    Archive(Stream &stream = std::cout) : stream(stream) {}

    template <typename T, bool Enable=true>
    typename std::enable_if<is_ostream<Stream>::value && Enable>::type
    operator<<(T &t)
    {
        Serializer<std::ostream> s(stream);
        Access::serialize(s, t);
    }

    template <typename T, bool Enable=true>
    typename std::enable_if<is_istream<Stream>::value && Enable>::type
    operator>>(T &t)
    {
        Serializer<std::istream> s(stream);
        Access::serialize(s, t);
    }

private:
    Stream &stream;
};