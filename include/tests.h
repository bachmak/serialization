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

class NotSerializableWithFriendAccess
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

class NotSerializableWithoutFriendAccess
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

class SerializableWithoutFriendAccess
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

class SerializableWithFriendAccess
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

class PublicSerializableWithoutFriendAccess
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

struct StructWithBasicTypesAndContainers
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

class ClassWithNestedStruct
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

class PodClass
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

void TestBasicTypes();

void TestPointers();

void TestReferences();

void TestSequenceContainers();

void TestSerializeAccessCombinations();

void TestPodClass();

void TestClassWithNestedStruct();

void TestAll();