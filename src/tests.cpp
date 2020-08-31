#include "tests.h"
#include "test_runner.h"
#include "serialization.h"

#include <fstream>

using namespace std;

void TestAll()
{
    TestRunner tr;
    RUN_TEST(tr, TestPodClass);
    RUN_TEST(tr, TestBasicTypes);
    RUN_TEST(tr, TestPointers);
    RUN_TEST(tr, TestReferences);
    RUN_TEST(tr, TestSequenceContainers);
    RUN_TEST(tr, TestSerializeAccessCombinations);
    RUN_TEST(tr, TestClassWithNestedStruct);
}

template <typename T, typename Stream>
void SerializeAndCountFails(T& t,
                            Archive<Stream>& archive,
                            size_t& fail_counter)
{
    try
    {
        archive & t;
    }
    catch(const invalid_argument& ex)
    {
        fail_counter++;
    }
}

void TestBasicTypes()
{
    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        double a = -90.23;
        int    b = 5;
        char   c = 'r';
        bool   d = true;
        float  e = 2.34;

        SerializeAndCountFails(a, oa, fail_counter);
        SerializeAndCountFails(b, oa, fail_counter);
        SerializeAndCountFails(c, oa, fail_counter);
        SerializeAndCountFails(d, oa, fail_counter);
        SerializeAndCountFails(e, oa, fail_counter);
    }

    {
        ifstream output("data.bin", ios_base::binary);
        Archive<ifstream> ia(output);

        double a = 0.0;
        int    b = 0;
        char   c = 'a';
        bool   d = false;
        float  e = 0.0;

        SerializeAndCountFails(a, ia, fail_counter);
        SerializeAndCountFails(b, ia, fail_counter);
        SerializeAndCountFails(c, ia, fail_counter);
        SerializeAndCountFails(d, ia, fail_counter);
        SerializeAndCountFails(e, ia, fail_counter);

        ASSERT_EQUAL(a, -90.23);
        ASSERT_EQUAL(b, 5);
        ASSERT_EQUAL(c, 'r');
        ASSERT_EQUAL(d, true);
        ASSERT_EQUAL(e, 2.34f);
        ASSERT_EQUAL(fail_counter, 0);
    }
}

void TestPointers()
{
    double a = 293.32;
    int    b = 1;
    char   c = 'b';
    bool   d = true;
    float  e = 2.54;
    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        double* ptr_a = &a; 
        int*    ptr_b = &b; 
        char*   ptr_c = &c; 
        bool*   ptr_d = &d; 
        float*  ptr_e = &e; 

        SerializeAndCountFails(ptr_a, oa, fail_counter);
        SerializeAndCountFails(ptr_b, oa, fail_counter);
        SerializeAndCountFails(ptr_c, oa, fail_counter);
        SerializeAndCountFails(ptr_d, oa, fail_counter);
        SerializeAndCountFails(ptr_e, oa, fail_counter);
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        double *a = nullptr;
        int    *b = nullptr;
        char   *c = nullptr;
        bool   *d = nullptr;
        float  *e = nullptr;

        SerializeAndCountFails(a, ia, fail_counter);
        SerializeAndCountFails(b, ia, fail_counter);
        SerializeAndCountFails(c, ia, fail_counter);
        SerializeAndCountFails(d, ia, fail_counter);
        SerializeAndCountFails(e, ia, fail_counter);

        ASSERT_FALSE(a);
        ASSERT_FALSE(b);
        ASSERT_FALSE(c);
        ASSERT_FALSE(d);
        ASSERT_FALSE(e);
        ASSERT_EQUAL(fail_counter, 10);
    }
}

void TestReferences()
{
    double a = 293.32;
    int    b = 1;
    char   c = 'b';
    bool   d = true;
    float  e = 2.54;
    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        double& ref_a = a;
        int&    ref_b = b;
        char&   ref_c = c;
        bool&   ref_d = d;
        float&  ref_e = e;

        SerializeAndCountFails(ref_a, oa, fail_counter);
        SerializeAndCountFails(ref_b, oa, fail_counter);
        SerializeAndCountFails(ref_c, oa, fail_counter);
        SerializeAndCountFails(ref_d, oa, fail_counter);
        SerializeAndCountFails(ref_e, oa, fail_counter);
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        double new_a = 0.0;
        int    new_b = 0;
        char   new_c = 'a';
        bool   new_d = false;
        float  new_e = 0.0;

        double& ref_a = new_a;
        int&    ref_b = new_b;
        char&   ref_c = new_c;
        bool&   ref_d = new_d;
        float&  ref_e = new_e;

        SerializeAndCountFails(ref_a, ia, fail_counter);
        SerializeAndCountFails(ref_b, ia, fail_counter);
        SerializeAndCountFails(ref_c, ia, fail_counter);
        SerializeAndCountFails(ref_d, ia, fail_counter);
        SerializeAndCountFails(ref_e, ia, fail_counter);

        ASSERT_EQUAL(ref_a, a);
        ASSERT_EQUAL(ref_b, b);
        ASSERT_EQUAL(ref_c, c);
        ASSERT_EQUAL(ref_d, d);
        ASSERT_EQUAL(ref_e, e);
        ASSERT_EQUAL(fail_counter, 0);
    }
}

void TestSequenceContainers()
{
    vector<list<int>> a = {{ 1, 2, 3, 4, 5 },
                           { 897, 231 },
                           { 0, 0, 0 },
                           { 12, 23, 54, 90 }};

    list<deque<string>> b = {{ "abc", "def", "ghijk" },
                             { "123", "45678", "910a", "0909" },
                             { "Moscow", "Tomsk", "London" }};
    
    forward_list<vector<double>> c = {{ 3.14, 9.20, -232.54 },
                                      { 90832.87, 9032.39, 84343.987 }};
    
    array<vector<string>, 3> d = {{{ "Lorem", "ipsum", "dolor", "sit" },
                                   { "amet", "consectetur", "adipiscing" },
                                   { "elit", "sed" }}};

    array<int, 5> e = {{ 500, 400, 300, 200, 100 }};

    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        auto new_a = a;
        auto new_b = b;
        auto new_c = c;
        auto new_d = d;
        auto new_e = e;
        
        SerializeAndCountFails(new_a, oa, fail_counter);
        SerializeAndCountFails(new_b, oa, fail_counter);
        SerializeAndCountFails(new_c, oa, fail_counter);
        SerializeAndCountFails(new_d, oa, fail_counter);
        SerializeAndCountFails(new_e, oa, fail_counter);
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        vector<int> new_A;
        vector<list<int>> new_a;
        list<deque<string>> new_b;
        forward_list<vector<double>> new_c;
        array<vector<string>, 3> new_d;
        array<int, 6> new_e;

        SerializeAndCountFails(new_a, ia, fail_counter);
        SerializeAndCountFails(new_b, ia, fail_counter);
        SerializeAndCountFails(new_c, ia, fail_counter);
        SerializeAndCountFails(new_d, ia, fail_counter);
        SerializeAndCountFails(new_e, ia, fail_counter);

        ASSERT_EQUAL(new_a, a);
        ASSERT_EQUAL(new_b, b);
        ASSERT_EQUAL(new_c, c);
        ASSERT_EQUAL(new_d, d);
        ASSERT_EQUAL(fail_counter, 1);
    }
}

void TestSerializeAccessCombinations()
{
    NotSerializableWithFriendAccess       a(10, 20);
    NotSerializableWithoutFriendAccess    b(30, 40);
    SerializableWithoutFriendAccess       c(50, 60);
    SerializableWithFriendAccess          d(70, 80);
    PublicSerializableWithoutFriendAccess e(90, 100);
    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        SerializeAndCountFails(a, oa, fail_counter);
        SerializeAndCountFails(b, oa, fail_counter);
        SerializeAndCountFails(c, oa, fail_counter);
        SerializeAndCountFails(d, oa, fail_counter);
        SerializeAndCountFails(e, oa, fail_counter);
    }
    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        NotSerializableWithFriendAccess       new_a;
        NotSerializableWithoutFriendAccess    new_b;
        SerializableWithoutFriendAccess       new_c;
        SerializableWithFriendAccess          new_d;
        PublicSerializableWithoutFriendAccess new_e;

        SerializeAndCountFails(new_a, ia, fail_counter);
        SerializeAndCountFails(new_b, ia, fail_counter);
        SerializeAndCountFails(new_c, ia, fail_counter);
        SerializeAndCountFails(new_d, ia, fail_counter);
        SerializeAndCountFails(new_e, ia, fail_counter);

        ASSERT_NOT_EQUAL(new_a, a);
        ASSERT_NOT_EQUAL(new_b, b);
        ASSERT_NOT_EQUAL(new_c, c);
        ASSERT_EQUAL(new_d, d);
        ASSERT_EQUAL(new_e, e);

        ASSERT_EQUAL(fail_counter, 6);
    }
}

void TestPodClass()
{
    size_t fail_counter = 0;
    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        PodClass podClassInstance(-1, 'b', 2, 500);
        SerializeAndCountFails(podClassInstance, oa, fail_counter);
    }
    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        PodClass podClassInstance;
        SerializeAndCountFails(podClassInstance, ia, fail_counter);

        ASSERT_EQUAL(podClassInstance, PodClass(-1, 'b', 2, 500));
        ASSERT_FALSE(fail_counter);
    }
}

void TestClassWithNestedStruct()
{
    ClassWithNestedStruct a(1,
                            { 20, 30, 40 },
                            500.00,
                            "6k",
                            StructWithBasicTypesAndContainers(2,
                                                              0.35,
                                                              3.1,
                                                              'u',
                                                              { 90, 100, 30 },
                                                              { 0.5, 0.6 }),
                            { "70k", "80k", "90k" });
    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        SerializeAndCountFails(a, oa, fail_counter);
    }
    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        ClassWithNestedStruct new_a;

        SerializeAndCountFails(new_a, ia, fail_counter);

        ASSERT_EQUAL(new_a, a);
        ASSERT_FALSE(fail_counter);
    }
}