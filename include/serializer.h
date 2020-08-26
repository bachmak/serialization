#pragma once

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
        serialize(t);
    }

    Serializer(Stream &stream) : stream(stream) {}

private:
    Stream &stream;

    template <typename T>
    typename std::enable_if<is_serializable<T>::value>::type
    serialize(T &t)
    {
        Access::serialize(*this, t);
    }

    template <typename T>
    typename std::enable_if<(is_std_vector<T>::value ||
                             is_std_string<T>::value ||
                             is_std_list<T>::value ||
                             is_std_deque<T>::value) &&
                            is_ostream<Stream>::value>::type
    serialize(T &t)
    {
        auto size = static_cast<uint32_t>(t.size());
        serialize(size);
        serialize_container(t);
    }

    template <typename T>
    typename std::enable_if<is_std_forward_list<T>::value &&
                            is_ostream<Stream>::value>::type
    serialize(T &t)
    {
        auto size = static_cast<uint32_t>(std::distance(t.begin(), t.end()));
        serialize(size);
        serialize_container(t);
    }

    template <typename T>
    typename std::enable_if<(is_std_vector<T>::value ||
                             is_std_string<T>::value) &&
                            is_istream<Stream>::value>::type
    serialize(T &t)
    {
        uint32_t size = 0;
        serialize(size);
        t.resize(size);

        for (uint32_t i = 0; i < size; i++)
        {
            serialize(t[i]);
        }
    }

    template <typename T>
    typename std::enable_if<(is_std_list<T>::value ||
                             is_std_deque<T>::value) &&
                            is_istream<Stream>::value>::type
    serialize(T &t)
    {
        uint32_t size = 0;
        serialize(size);
        t.clear();

        for (uint32_t i = 0; i < size; i++)
        {
            typename T::value_type item;
            serialize(item);
            t.push_back(std::move(item));
        }
    }

    template <typename T>
    typename std::enable_if<is_std_forward_list<T>::value &&
                            is_istream<Stream>::value>::type
    serialize(T &t)
    {
        uint32_t size = 0;
        serialize(size);
        t.clear();

        for (uint32_t i = 0; i < size; i++)
        {
            typename T::value_type item;
            serialize(item);
            t.push_front(std::move(item));
        }

        t.reverse();
    }

    template <typename T>
    void serialize_container(T &t)
    {
        for (auto iter = begin(t); iter != end(t); ++iter)
        {
            serialize(*iter);
        }
    }

    template <typename T>
    typename std::enable_if<!is_serializable<T>::value &&
                            std::is_pod<T>::value &&
                            !std::is_pointer<T>::value &&
                            !std::is_reference<T>::value &&
                            is_ostream<Stream>::value>::type
    serialize(T &t)
    {
        stream.write(reinterpret_cast<char *>(&t), sizeof(t));
    }

    template <typename T>
    typename std::enable_if<!is_serializable<T>::value &&
                            std::is_pod<T>::value &&
                            !std::is_pointer<T>::value &&
                            !std::is_reference<T>::value &&
                            is_istream<Stream>::value>::type
    serialize(T &t)
    {
        stream.read(reinterpret_cast<char *>(&t), sizeof(t));
    }
};
