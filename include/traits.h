#pragma once

#include <type_traits>
#include <vector>
#include <string>
#include <deque>
#include <list>
#include <forward_list>

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

template <typename>
struct is_std_vector : std::false_type {};

template <typename T>
struct is_std_vector<std::vector<T>> : std::true_type {};

template <typename>
struct is_std_string : std::false_type {};

template <typename T>
struct is_std_string<std::basic_string<T>> : std::true_type {};

template <typename>
struct is_std_deque : std::false_type {};

template <typename T>
struct is_std_deque<std::deque<T>> : std::true_type {};

template <typename>
struct is_std_list : std::false_type {};

template <typename T>
struct is_std_list<std::list<T>> : std::true_type {};

template <typename>
struct is_std_forward_list : std::false_type {};

template <typename T>
struct is_std_forward_list<std::forward_list<T>> : std::true_type {};
