#pragma once

#include <iostream>
#include <stdexcept>
#include <sstream>

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
    enable_if_t<is_serializable<T>::value>
    serialize(T &t)
    {
        Access::serialize(*this, t);
    }

    template <typename T>
    enable_if_t<(is_std_array<T>::value  ||
                 is_std_vector<T>::value ||
                 is_std_string<T>::value ||
                 is_std_list<T>::value   ||
                 is_std_deque<T>::value) &&
                 is_ostream<Stream>::value>
    serialize(T &t)
    {
        auto size = static_cast<uint32_t>(t.size());
        serialize(size);
        serialize_container(t);
    }

    template <typename T>
    enable_if_t<is_std_forward_list<T>::value &&
                is_ostream<Stream>::value>
    serialize(T &t)
    {
        auto size = static_cast<uint32_t>(std::distance(t.begin(), t.end()));
        serialize(size);
        serialize_container(t);
    }

    template <typename T>
    enable_if_t<is_std_array<T>::value &&
                is_istream<Stream>::value>
    serialize(T& t)
    {
        uint32_t size = 0;
        serialize(size);
        
        if (size != t.size())
        {
            std::ostringstream os;
            os << "Different sizes of static std arrays. Serialized array size: " << size <<
               ". Deserializable array size: " << t.size() << ". Deserialization failed.";
            throw std::invalid_argument(os.str());
        }

        for (auto &item : t)
        {
            serialize(item);
        }
    }

    template <typename T>
    enable_if_t<(is_std_vector<T>::value  ||
                 is_std_string<T>::value) &&
                 is_istream<Stream>::value>
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
    enable_if_t<(is_std_list<T>::value   ||
                 is_std_deque<T>::value) &&
                 is_istream<Stream>::value>
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
    enable_if_t<is_std_forward_list<T>::value &&
                is_istream<Stream>::value>
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
    enable_if_t<!is_serializable<T>::value    &&
                std::is_fundamental<T>::value &&
                !std::is_reference<T>::value  &&
                is_ostream<Stream>::value>
    serialize(T &t)
    {
        stream.write(reinterpret_cast<const char*>(&t), sizeof(t));
    }

    template <typename T>
    enable_if_t<!is_serializable<T>::value    &&
                std::is_fundamental<T>::value &&
                !std::is_reference<T>::value  &&
                is_istream<Stream>::value>
    serialize(T &t)
    {
        stream.read(const_cast<char *>(reinterpret_cast<const char *>(&t)), sizeof(t));
    }

    void serialize(...)
    {
        throw std::invalid_argument("Unsupported type. Serialization failed.");
    }
};
