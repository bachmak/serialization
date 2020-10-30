/*  Структура Access при объявлении её как дружественной в теле
    сериализуемого класса обеспечивает доступ к методу serialize
    для возможности проверки наличия этого метода и его вызова
    независимо от модификатора доступа */

#pragma once

#include <type_traits>
#include <iostream>

template <typename Stream>                                                      // Объявляем класс Seializer для проверки наличия метода 
class Serializer;                                                               // serialize(Serializer<ostream>) у сериализуемого объекта

struct Access
{
    template <typename Stream, typename T>
    static void serialize(Serializer<Stream> s, T& t)                           // Функция для вызова метода serialize у сериализуемого
    {                                                                           // объекта.
        t.serialize(s);                                                         // Если метод serialize приватный, доступ к нему можно получить
    }                                                                           // только из структуры Access.

    struct Serializable                                                         // Структура для проверки наличия у объекта метода serialize
    {                                                                           // с помощью идиомы SFINAE.
        template <typename T>                                                   // Сначала попытка подстановки функции is_serializable(int),
        static decltype(                                                        // т.к. перегрузка с int имеет больший приоритет, чем перегрузка
            std::declval<T&>().serialize(Serializer<std::ostream>(std::cout)),  // с эллипсисом: для этого нужно вывести тип возвращаемого
            std::true_type{})                                                   // значения функции decltype(..., std::true_type{}), но сначала
        is_serializable(int);                                                   // должна произойти компиляция выражения до запятой – 
                                                                                // если это получается, у объекта есть метод serialize.
        template <typename T>                                                   // Функция is_serializable(...) имеет более низкий приоритет 
        static std::false_type is_serializable(...);                            // подстановки и возвращает std::false_type (если первая
    };                                                                          // подстановка не сработала).
};

enum class AllocType                                                            // Перечисление для указания типа выделенной для указателя памяти:
{
    Empty,                                                                      // - пустой указатель;
    Static,                                                                     // - память выделена статически (данные на стеке или в глобальной области);
    DynamicSingle,                                                              // - память выделена в куче с помощью оператора new;
    DynamicMultiple                                                             // - память выделена в куче с помощью оператора new[].
};

template <typename T>                                                           // Структура-обертка над указателем для сериализации
struct Pointer
{
    Pointer(T* ptr, AllocType alloc_type, size_t size = 1)                      // Параметрический конструктор
        : ptr(ptr), alloc_type(alloc_type), size(size) {}

    Pointer() = default;                                                        // Конструктор умолчания

    T& operator*()                                                              // Операторы разыменования
    {
        return *ptr;
    }

    const T& operator*() const                                                  //
    {
        return *ptr;
    }

    T& operator[](size_t index)                                                 // Операторы обращения к элементу
    {
        return ptr[index];
    }


    const T& operator[](size_t index) const                                     //
    {
        return ptr[index];
    }
                                                                                // Поля:
    T* ptr = nullptr;                                                           // - сырой указатель;
    AllocType alloc_type = AllocType::Empty;                                    // - тип выделенной под указатель памяти;
    size_t size = 0;                                                            // - размер массива данных указателя.
};

template <typename T>                                                           // Непосредственно проверка на наличие метода serialize:
using is_serializable = decltype(Access::Serializable::is_serializable<T>(0));  // 0 в параметре – для попытки подстановки функции с более
                                                                                // высоким приоритетом – is_serializable(int).
                                                                                // Выражение возвращает std::true_type или std::false_type.