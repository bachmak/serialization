/*  Заголовочный файл с объявлениями тестовых классов и функций,
    используемых для юнит-тестирования.
    Функция TestAll() должна вызываться до начала работы
    основного кода программы в функции main().
*/

#pragma once

#include "serializer.h"
#include "test_runner.h"

#include <array>
#include <vector>
#include <string>
#include <list>
#include <deque>
#include <forward_list>
#include <iostream>

/*  Тестовые классы для проверки корректности сериализации.
    Для каждого такого класса реализованы:
    – оператор сравнения для проверки действительного и
      ожидаемого результатов сериализации в tests.cpp;
    – оператор вывода в поток для получения отладочной
      информации о неудачных тестах;
    – параметризованный конструктор и конструктор умолчания
      для более удобного создания различных объектов класса. 
*/

class NotSerializableWithFriendAccess                                           // (1) нет serialize(), есть Access
{
public:
    NotSerializableWithFriendAccess(int a, int b) : a(a), b(b) {}
    NotSerializableWithFriendAccess() = default;

    bool operator==(const NotSerializableWithFriendAccess& x) const
    {
        return a == x.a && b == x.b;
    }

    friend std::ostream& operator<<(std::ostream& os,
        const NotSerializableWithFriendAccess& x)
    {
        return os << '{' << x.a << ", " << x.b << '}';
    }

private:
    friend struct Access;
    int a = 1;
    int b = 2;
};

class NotSerializableWithoutFriendAccess                                        // (2) нет serialize(), нет Access
{
public:
    NotSerializableWithoutFriendAccess(int a, int b) : a(a), b(b) {}
    NotSerializableWithoutFriendAccess() = default;

    bool operator==(const NotSerializableWithoutFriendAccess& x) const
    {
        return a == x.a && b == x.b;
    }

    friend std::ostream& operator<<(std::ostream& os,
        const NotSerializableWithoutFriendAccess& x)
    {
        return os << '{' << x.a << ", " << x.b << '}';
    }

private:
    int a = 1;
    int b = 2;
};

class SerializableWithoutFriendAccess                                           // (3) есть serialize(), нет Access
{
public:
    SerializableWithoutFriendAccess(int a, int b) : a(a), b(b) {}
    SerializableWithoutFriendAccess() = default;

    bool operator==(const SerializableWithoutFriendAccess& x) const
    {
        return a == x.a && b == x.b;
    }

    friend std::ostream& operator<<(std::ostream& os,
        const SerializableWithoutFriendAccess& x)
    {
        return os << '{' << x.a << ", " << x.b << '}';
    }

private:
    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        s & a;
        s & b;
    }

    int a = 1;
    int b = 2;
};

class SerializableWithFriendAccess                                              // (4) есть приватный serialize() и Access
{
public:
    SerializableWithFriendAccess(int a, int b) : a(a), b(b) {}
    SerializableWithFriendAccess() = default;

    bool operator==(const SerializableWithFriendAccess& x) const
    {
        return a == x.a && b == x.b;
    }

    friend std::ostream& operator<<(std::ostream& os,
        const SerializableWithFriendAccess& x)
    {
        return os << '{' << x.a << ", " << x.b << '}';
    }

private:
    friend struct Access;

    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        s & a;
        s & b;
    }

    int a = 1;
    int b = 2;
};

class PublicSerializableWithoutFriendAccess                                     // (5) есть публичный serialize() и Access
{
public:
    PublicSerializableWithoutFriendAccess(int a, int b) : a(a), b(b) {}
    PublicSerializableWithoutFriendAccess() = default;

    bool operator==(const PublicSerializableWithoutFriendAccess& x) const
    {
        return a == x.a && b == x.b;
    }

    friend std::ostream& operator<<(std::ostream& os,
        const PublicSerializableWithoutFriendAccess& x)
    {
        return os << '{' << x.a << ", " << x.b << '}';
    }

    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        s & a;
        s & b;
    }

private:
    int a = 1;
    int b = 2;
};

struct StructWithBasicTypesAndContainers                                        // тестовая структура с полями базовых и контейнерных типов
{
    StructWithBasicTypesAndContainers(int a = 1, double b = 1.0,
                                      float c = 1.0, char d = 'b',
                                      std::deque<int> e = {},
                                      std::forward_list<double> f = {})
        : a(a), b(b), c(c), d(d), e(e), f(f) {}

    bool operator==(const StructWithBasicTypesAndContainers& x) const
    {
        return a == x.a && b == x.b && c == x.c && 
               d == x.d && e == x.e && f == x.f;
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const StructWithBasicTypesAndContainers& x)
    {
        return os << '{' << x.a << ", " << x.b << ", " << x.c << ", "
                         << x.d << ", " << x.e << ", " << x.f << '}';
    }

private:
    friend struct Access;

    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        s & a;
        s & b;
        s & c;
        s & d;
        s & e;
        s & f;
    }

    int a = 1;
    double b = 1.0;
    float c = 1.0;
    char d = 'b';
    std::deque<int> e = { 0, 0 };
    std::forward_list<double> f = { 0.0, 0.0 };
};

class ClassWithNestedStruct                                                     // тестовый класс с вложенной пользовательской структурой
{
public:
    ClassWithNestedStruct(int a, std::vector<int> b, float c, std::string d,
                          StructWithBasicTypesAndContainers e,
                          std::list<std::string> f)
        : a(a), b(b), c(c), d(d), e(e), f(f) {}

    ClassWithNestedStruct() = default;

    bool operator==(const ClassWithNestedStruct& x) const
    {
        return a == x.a && b == x.b && c == x.c && 
               d == x.d && e == x.e && f == x.f;
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const ClassWithNestedStruct& x)
    {
        return os << '{' << x.a << ", " << x.b << ", " << x.c << ", "
                         << x.d << ", " << x.e << ", " << x.f << '}';
    }

private:
    friend struct Access;

    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        s & a;
        s & b;
        s & c;
        s & d;
        s & e;
        s & f;
    }

    int a = 0;
    std::vector<int> b = {0, 0};
    float c = 0.0;
    std::string d = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";
    StructWithBasicTypesAndContainers e;
    std::list<std::string> f = {"one", "two"};
};

class PodClass                                                                  // тестовый класс с полями базовых типов
{
public:
    PodClass(int a, char b, uint32_t c, int64_t d)
        : a(a), b(b), c(c), d(d) {}

    PodClass() = default;

    bool operator==(const PodClass& pod) const
    {
        return (a == pod.a && b == pod.b && c == pod.c && d == pod.d);
    }

    friend std::ostream& operator<< (std::ostream& os, const PodClass& pod)
    {
        return os << '{' << pod.a << ", " << pod.b << ", " 
                  << pod.c << ", " << pod.d << '}';
    }

private:
    int a = 0;
    char b = 'a';
    uint32_t c = 0;
    int64_t d = 0;

    friend struct Access;

    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        s & a;
        s & b;
        s & c;
        s & d;
    }
};

class BaseClass
{
public:
    BaseClass(int a, double b, char c)
        : a(a), b(b), c(c) {}

    BaseClass() = default;

    bool operator==(const BaseClass& x) const
    {
        return (a == x.a && b == x.b && c == x.c);
    }

    friend std::ostream& operator<<(std::ostream& os, const BaseClass& x)
    {
        return os << '{' << x.a << ", " << x.b << ", " << x.c << '}';
    }

protected:
    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        s & a;
        s & b;
        s & c;
    }

private:
    friend struct Access;

    int a = 0;
    double b = 0.0;
    char c = 'a';
};

class DerivedClass : public BaseClass
{
public:
    DerivedClass(BaseClass base, std::string a, std::vector<int> b)
        : BaseClass(base), a(a), b(b) {}

    DerivedClass(int base_a, double base_b, char base_c,
                 std::string a, std::vector<int> b)
        : DerivedClass({ base_a, base_b, base_c }, a, b) {}

    DerivedClass() = default;

    bool operator==(const DerivedClass& x) const
    {
        BaseClass const* base_x = dynamic_cast<BaseClass const*>(&x);

        return (BaseClass::operator==(*base_x) && a == x.a && b == x.b);
    }

    friend std::ostream& operator<<(std::ostream& os, const DerivedClass& x)
    {
        return os << '{' << *dynamic_cast<BaseClass const*>(&x)
                  << ", " << x.a << ", " << x.b << '}'; 
    }

private:
    friend struct Access;

    template <typename Stream>
    void serialize(Serializer<Stream> s)
    {
        BaseClass::serialize(s);
        s & a;
        s & b;
    }

    std::string a = "aaa";
    std::vector<int> b = { 0, 0, 0 }; 
};

/*  Тестовые функции для проверки корректности сериализации.
    Все функции определены в tests.cpp.
    Каждая из этих функций выбрасывает исключение, 
    если ожидаемые результаты сериализации
    не совпадают с действительными.  
*/

void TestBasicTypes();                                                          // функция для проверки сериализации базовых типов

void TestSingleValuePointers();                                                 // 
                                                                                // функции для проверки сериализации указателей
void TestMultipleValuePointers();                                               //  

void TestReferences();                                                          // функция для проверки сериализации ссылок

void TestSequenceContainers();                                                  // функция для проверки сериализации линейных контейнеров

void TestAssociativeContainers();                                               // функция для проверки сериализации ассоциативных контейнеров

void TestSerializeAccessCombinations();                                         // функция для проверки сериализациия структур (1)-(5)

void TestPodClass();                                                            // функция для проверки сериализации класса с базовыми полями

void TestClassWithNestedStruct();                                               // функция для проверки сериализации класса со структурой внутри

void TestDerivedClass();                                                        // функция для проверки сериализации класса-наследника

void TestAll();                                                                 // функция для запуска всех тестовых функций