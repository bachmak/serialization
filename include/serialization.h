/*  Шаблон класса Archive является "входной точкой" процесса сериализации,
    в зависимости от шаблонного параметра Stream (входной или выходной поток)
    инициирует сериализацию либо десериализацию объекта.
    Содержит удобные в использовании перегрузки операторов.
*/

#pragma once

#include "serializer.h"
#include "traits.h"

#include <iostream>

template <typename Stream>
class Archive
{
public:
    static_assert(is_ostream<Stream>::value ||                                  // Проверка на этапе компиляции:
                  is_istream<Stream>::value,                                    // инстанцирование может производиться только от стандартных
                  "template argument must be a stream");                        // потоков.

    Archive(Stream &stream) : stream(stream) {}                                 // Конструктор с передачей потока по ссылке

    template <typename T, bool Enable=true>                                     // Оператор вывода в поток (сериализации) для lvalue-ссылок:
    typename std::enable_if<is_ostream<Stream>::value && Enable>::type          // срабатывает только если шаблонный параметр является выходным
    operator<<(T& t)                                                            // потоком.
    {
        Serializer<std::ostream> s(stream);                                     // Создаем сериализатор,
        s & t;                                                                  // вызываем сериализацию шаблонного аргумента.
    }

    template <typename T, bool Enable=true>                                     // Оператор вывода в поток для rvalue-ссылок
    typename std::enable_if<is_ostream<Stream>::value && Enable>::type          // (временных объектов)
    operator<<(T&& t)
    {
        Serializer<std::ostream> s(stream);
        s & t;
    }

    template <typename T, bool Enable=true>
    typename std::enable_if<is_istream<Stream>::value && Enable>::type
    operator>>(T& t)
    {
        Serializer<std::istream> s(stream);
        s & t;
    }

    template <typename T, bool Enable=true>
    typename std::enable_if<is_ostream<Stream>::value && Enable>::type
    operator&(T& t)
    {
        *this << t;
    }

    template <typename T, bool Enable=true>
    typename std::enable_if<is_istream<Stream>::value && Enable>::type
    operator&(T& t)
    {
        *this >> t;
    }

private:
    Stream &stream;
};