/*  Шаблон класса Archive является "входной точкой" процесса сериализации,
    в зависимости от шаблонного параметра Stream (входной или выходной поток)
    инициирует сериализацию либо десериализацию объекта.
    Содержит удобные в использовании перегрузки операторов.
*/

#pragma once

#include "serializer.h"
#include "traits.h"

template <typename Stream>
class Archive
{
public:
    static_assert(is_ostream<Stream>::value ||                                  // Проверяем на этапе компиляции:
                  is_istream<Stream>::value,                                    // инстанцирование может производиться только от стандартных
                  "Template argument must be a stream.");                       // потоков.

    Archive(Stream& stream) : serializer(Serializer<Stream>(stream)) {}         // Конструктор с передачей потока для сериализации по ссылке
                                                                                // и созданием сериализатора для этого потока.
    template <typename T, bool Enable=true>                                     // Оператор вывода в поток (сериализации) для lvalue-ссылок:
    typename std::enable_if<is_ostream<Stream>::value && Enable>::type          // доступен, только если шаблонный параметр является выходным
    operator<<(T& t)                                                            // потоком.
    {
        serializer.serialize(t);                                                // Вызываем сериализацию шаблонного аргумента.
    }

    template <typename T, bool Enable=true>                                     // Оператор вывода в поток для rvalue-ссылок
    typename std::enable_if<is_ostream<Stream>::value && Enable>::type          // (временных объектов).
    operator<<(T&& t)
    {
        serializer.serialize(t);
    }

    template <typename T, bool Enable=true>                                     // Оператор ввода из потока (десериализации):
    typename std::enable_if<is_istream<Stream>::value && Enable>::type          // доступен, только если шаблонный параметр является входным
    operator>>(T& t)                                                            // потоком.
    {
        serializer.serialize(t);
    }

    template <typename T, bool Enable=true>                                     // Универсальный оператор для сериализации/десериализации
    void operator&(T& t)                                                        // (для использования при необходимости).
    {
        serializer.serialize(t);
    }

private:
    Serializer<Stream> serializer;                                              // сериализатор, выполняющий, непосредственно, сериализацию.
};