#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <set>
#include <map>

inline void PrintDiv(std::ostream& os,  bool& first, const std::string& div = ", ")
{
    if (!first)
    {
        os << div;
    }
    else
    {
        first = false;
    }
}

template <typename It>
void PrintSequence(It it1, It it2, std::ostream& os)
{
    os << "[";
    bool first = true;

    for (auto it = it1; it != it2; it++)
    {
        PrintDiv(os, first);
        os << *it;
    }
    os << "]";
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& v)
{
    PrintSequence(v.cbegin(), v.cend(), os);
    return os;
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const std::deque<T>& v)
{
    PrintSequence(v.cbegin(), v.cend(), os);
    return os;
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const std::list<T>& v)
{
    PrintSequence(v.cbegin(), v.cend(), os);
    return os;
}

template <typename T>
std::ostream& operator<< (std::ostream& os, const std::forward_list<T>& v)
{
    PrintSequence(v.cbegin(), v.cend(), os);
    return os;
}

template <class T>
std::ostream& operator<< (std::ostream& os, const std::set<T>& s)
{
    os << "{";
    bool first = true;
    for(const auto& x : s)
    {
        PrintDiv(os, first);
        os << x;
    }
    return os << "}";
}


template <class K, class V>
std::ostream& operator<< (std::ostream& os, const std::map<K, V>& m)
{
    os << "{";
    bool first = true;
    for(const auto& kv : m)
    {
        PrintDiv(os, first);
        os << kv.first << ": " << kv.second;
    }
    return os << "}";
}


class TestRunner
{
public:
    template <class TestFunc>
    void RunTest(const TestFunc& func, const std::string& func_name)
    {
        try
        {
            func();
            std::cerr << func_name << " OK" << std::endl;
        }
        catch(std::exception& e)
        {
            std::cerr << func_name << " failed. " << e.what() << std::endl;
            ++fail_count;
        }
        catch(...)
        {
            std::cerr << "Unknown exception caught" << std::endl;
            ++fail_count;
        }
    }
    
    ~TestRunner()
    {
        if (fail_count)
        {
            std::cerr << fail_count << " unit-tests failed. Terminate" << std::endl;
            exit(1);
        }
    };
    
private:
    int fail_count = 0;
};


template <typename X, typename Y>
void AssertEqual(const X& x, const Y& y, const std::string& hint = {})
{
    if (!(x == y))
    {
        std::ostringstream os;
        os << "Assertion failed: " << x << " != " << y;
        if (!hint.empty())
        {
            os << "\thint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}


inline void AssertTrue(bool b, const std::string& hint = {})
{
    AssertEqual(b, true, hint);
}

inline void AssertFalse(bool b, const std::string& hint = {})
{
    AssertEqual(b, false, hint);
}


#define ASSERT_EQUAL(x, y)                  \
{                                           \
    std::ostringstream macro_os;            \
    macro_os << #x << " != " << #y << ", "  \
        << __FILE__ << ':' << __LINE__;     \
    AssertEqual(x, y, macro_os.str());      \
}

#define ASSERT_TRUE(x)                      \
{                                           \
    std::ostringstream macro_os;            \
    macro_os << #x << " is false, "         \
        << __FILE__ << ':' << __LINE__;     \
    AssertTrue(x, macro_os.str());          \
}

#define ASSERT_FALSE(x)                     \
{                                           \
    std::ostringstream macro_os;            \
    macro_os << #x << " is true, "          \
        << __FILE__ << ':' << __LINE__;     \
    AssertFalse(x, macro_os.str());         \
}

#define RUN_TEST(tr, func)                  \
{                                           \
    tr.RunTest(func, #func);                \
}
    
