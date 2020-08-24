struct Access
{
    template <typename Serializer, typename T>
    static void serialize(Serializer &s, T &t)
    {
        t.serialize(s);
    }

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

// template <typename T>
// using is_serializable = decltype(Access::Serializable::is_serializable<T>(0));
