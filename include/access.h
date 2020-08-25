#pragma once

template <typename Stream>
class Serializer;

struct Access
{
    template <typename Stream, typename T>
    static void serialize(Serializer<Stream> s, T &t)
    {
        t.serialize(s);
    }

    struct Serializable
    {
        template <typename T>
        static decltype(
            std::declval<T &>().serialize(Serializer<std::ostream>()),
            std::true_type{})
        is_serializable(int);

        template <typename T>
        static std::false_type is_serializable(...);
    };
};

template <typename T>
using is_serializable = decltype(Access::Serializable::is_serializable<T>(0));