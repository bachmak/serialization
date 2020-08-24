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
        static int detect(...);

        template <typename U>
        decltype(std::declval<U>().serialize(OutputSerializer())) detect(const U &);

    public:
        static constexpr bool value = std::is_same<void, decltype(detect(std::declval<T>()))>::value;
    };
};