#pragma once

#define NAME(x) #x

#include <iostream>
#include <stdexcept>

#include "traits.h"
#include "access.h"

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
        serialize(t, 0);
    }

    Serializer(Stream &stream) : stream(stream) {}

private:
    Stream &stream;

    template <typename T>
    typename std::enable_if<is_serializable<T>::value>::type
    serialize(T &t, int)
    {
        Access::serialize(*this, t);
    }

    template <typename T>
    typename std::enable_if<(is_std_vector<T>::value ||
                             is_std_string<T>::value ||
                             is_std_list<T>::value ||
                             is_std_deque<T>::value) &&
                            is_ostream<Stream>::value>::type
    serialize(T &t, int)
    {
        auto size = static_cast<uint32_t>(t.size());
        serialize(size, 0);
        serialize_container(t);
    }

    template <typename T>
    typename std::enable_if<is_std_forward_list<T>::value &&
                            is_ostream<Stream>::value>::type
    serialize(T &t, int)
    {
        auto size = static_cast<uint32_t>(std::distance(t.begin(), t.end()));
        serialize(size, 0);
        serialize_container(t);
    }

    template <typename T>
    typename std::enable_if<(is_std_vector<T>::value ||
                             is_std_string<T>::value) &&
                            is_istream<Stream>::value>::type
    serialize(T &t, int)
    {
        uint32_t size = 0;
        serialize(size, 0);
        t.resize(size);

        for (uint32_t i = 0; i < size; i++)
        {
            serialize(t[i], 0);
        }
    }

    template <typename T>
    typename std::enable_if<(is_std_list<T>::value ||
                             is_std_deque<T>::value) &&
                            is_istream<Stream>::value>::type
    serialize(T &t, int)
    {
        uint32_t size = 0;
        serialize(size, 0);
        t.clear();

        for (uint32_t i = 0; i < size; i++)
        {
            typename T::value_type item;
            serialize(item, 0);
            t.push_back(std::move(item));
        }
    }

    template <typename T>
    typename std::enable_if<is_std_forward_list<T>::value &&
                            is_istream<Stream>::value>::type
    serialize(T &t, int)
    {
        uint32_t size = 0;
        serialize(size, 0);
        t.clear();

        for (uint32_t i = 0; i < size; i++)
        {
            typename T::value_type item;
            serialize(item, 0);
            t.push_front(std::move(item));
        }

        t.reverse();
    }

    template <typename T>
    void serialize_container(T &t)
    {
        for (auto iter = begin(t); iter != end(t); ++iter)
        {
            serialize(*iter, 0);
        }
    }

    template <typename T>
    typename std::enable_if<!is_serializable<T>::value &&
                            std::is_fundamental<T>::value &&
                            !std::is_reference<T>::value &&
                            is_ostream<Stream>::value>::type
    serialize(T &t, int)
    {
        stream.write(reinterpret_cast<const char*>(&t), sizeof(t));
    }

    template <typename T>
    typename std::enable_if<!is_serializable<T>::value &&
                            std::is_fundamental<T>::value &&
                            !std::is_reference<T>::value &&
                            is_istream<Stream>::value>::type
    serialize(T &t, int)
    {
        stream.read(const_cast<char *>(reinterpret_cast<const char *>(&t)), sizeof(t));
    }

    template <typename T>
    void serialize(T &t, ...)
    {
        throw std::invalid_argument("Unsupported type. Serialization failed.");
    }
};
