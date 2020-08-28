#pragma once

#include <array>
#include <vector>
#include <string>
#include <list>
#include <deque>
#include <forward_list>
#include <iostream>

#include "serializer.h"

struct SimpleStruct
{
    SimpleStruct(int a = 1, double b = 1.0, float c = 1.0,
                 char d = 'b', std::deque<int> e = {},
                 std::forward_list<double> f = {})
        : a(a), b(b), c(c), d(d), e(e), f(f) {}

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

class SimpleClass
{
public:
    SimpleClass(int a, std::vector<int> b, float c,
                std::string d, SimpleStruct e, std::list<std::string> f)
        : a(a), b(b), c(c), d(d), e(e), f(f) {}
    SimpleClass() = default;

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
    SimpleStruct e;
    std::list<std::string> f = {"one", "two"};
};

class PodClass
{
public:
    PodClass(int a, char b, uint32_t c, int64_t d)
        : a(a), b(b), c(c), d(d) {}

    PodClass() = default;

    static void test();

    bool operator==(const PodClass& pod) const
    {
        return (a == pod.a && b == pod.b && c == pod.c && d == pod.d);
    }

    friend std::ostream& operator<< (std::ostream& os, const PodClass& pod);
;
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

void TestAll();