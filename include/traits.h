#pragma once

#include <type_traits>
#include <array>
#include <vector>
#include <string>
#include <deque>
#include <list>
#include <forward_list>
#include <set>
#include <map>

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

// Проверка на возможность поддержки цикла range-based for

struct Iterable
{
    template <typename T>
    static decltype(
        std::begin(std::declval<T&>()) != std::end(std::declval<T&>()),
        ++std::declval<decltype(std::begin(std::declval<T&>()))&>(),
        *std::begin(std::declval<T&>()),
        std::true_type{})
    is_iterable(int);

    template <typename T>
    static std::false_type is_iterable(...);
};

template <typename T>
using is_iterable = decltype(Iterable::is_iterable<T>(0));

// Проверка на наличие метода size()

struct HasSize
{
    template <typename T>
    static decltype(
        std::declval<size_t>() == std::declval<T&>().size(),
        std::true_type{})
    has_size(int);

    template <typename T>
    static std::false_type has_size(...);
};

template <typename T>
using has_size = decltype(HasSize::has_size<T>(0));

// Проверка на наличие метода insert()

struct HasInsert
{
    template <typename T>
    static decltype(
        std::declval<T&>().insert(std::declval<typename T::value_type>()),
        std::true_type{})
    has_insert(int);

    template <typename T>
    static std::false_type has_insert(...);
};

template <typename T>
using has_insert = decltype(HasInsert::has_insert<T>(0));

// Проверки потоков

template <typename T>
using is_ostream = std::is_base_of<std::ostream, T>;

template <typename T>
using is_istream = std::is_base_of<std::istream, T>;

// Проверки стандартных линейных (последовательных) контейнеров

template <typename>
struct is_std_array : public std::false_type {};

template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : public std::true_type {};

//

template <typename>
struct is_std_vector : public std::false_type {};

template <typename T>
struct is_std_vector<std::vector<T>> : public std::true_type {};

//

template <typename>
struct is_std_string : public std::false_type {};

template <typename T>
struct is_std_string<std::basic_string<T>> : public std::true_type {};

//

template <typename>
struct is_std_deque : public std::false_type {};

template <typename T>
struct is_std_deque<std::deque<T>> : public std::true_type {};

//

template <typename>
struct is_std_list : public std::false_type {};

template <typename T>
struct is_std_list<std::list<T>> : public std::true_type {};

//

template <typename>
struct is_std_forward_list : public std::false_type {};

template <typename T>
struct is_std_forward_list<std::forward_list<T>> : public std::true_type {};
