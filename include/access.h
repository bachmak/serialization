#pragma once

#include <utility>
#include <type_traits>

class OutputSerializer;

struct Access
{
    template <typename Serializer, typename T>
    static void serialize(Serializer &s, T &t)
    {
        t.serialize(s);
    }

    template <typename T>
    struct has_serialize
    {
    private:
        static std::false_type detect(...);

        template <typename U>
        static decltype(std::declval<U>().serialize(OutputSerializer())) detect(const U &);

    public:
        static constexpr bool value = decltype(detect(std::declval<T>()))::value;
    };

    struct Serializable
    {
    };

    struct Iterable
    {
        template <typename T>
        static decltype(
            std::begin(std::declval<T &>()) != std::end(std::declval<T &>()),
            ++std::declval<decltype(std::begin(std::declval<T &>())) &>(),
            *std::begin(std::declval<T &>()),
            std::true_type{})
        is_iterable(int);

        template <typename T>
        static std::false_type is_iterable(...);
    };
};

template <typename T>
using is_iterable = decltype(Access::Iterable::is_iterable<T>(0));
