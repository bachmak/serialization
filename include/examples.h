#pragma once

#include <vector>
#include <string>
#include <list>
#include <deque>
#include <forward_list>

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