#pragma once 
#include <type_traits>

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

template <typename T>
using is_iterable = decltype(Iterable::is_iterable<T>(0));

template <typename T>
using is_ostream = std::is_base_of<std::ostream, T>;

template <typename T>
using is_istream = std::is_base_of<std::istream, T>;