/*  Файл с определениями всех функций,
    объявленных в tests.h и используемых для
    тестирования программы на корректность работы.
*/

#include "tests.h"
#include "test_runner.h"
#include "serialization.h"

#include <fstream>

using namespace std;

void TestAll()                                                                  // Функция для запуска всех тестов:
{
    TestRunner tr;                                                              // Создаем объект класса TestRunner (см. test_runner.h).
    RUN_TEST(tr, TestPodClass);                                                 // С помощью макроса RUN_TEST передаем в TestRunner
    RUN_TEST(tr, TestBasicTypes);                                               // тестируемые функции (макрос вызывает метод RunTest у
    RUN_TEST(tr, TestSingleValuePointers);                                      // объекта tr и передает в качестве аргументов тестируемую
    RUN_TEST(tr, TestMultipleValuePointers);                                    // функцию и строку с названием этой функции).
    RUN_TEST(tr, TestReferences);                                               // 
    RUN_TEST(tr, TestSequenceContainers);                                       //
    RUN_TEST(tr, TestAssociativeContainers);                                    //
    RUN_TEST(tr, TestSerializeAccessCombinations);                              //
    RUN_TEST(tr, TestClassWithNestedStruct);                                    //
}

template <typename T, typename Stream>                                          // Шаблонная функция, выполняющая попытку сериализации 
void SerializeAndCountFails(T& t,                                               // или десериализации объекта t через archive.
                            Archive<Stream>& archive,                           // Инкрементирует fail_counter в случае неудачной
                            size_t& fail_counter)                               // сериализации/десериализации.
{
    try
    {
        archive & t;                                                            // Выполняем сериализацию.
    }
    catch(const invalid_argument& ex)                                           // Ловим исключение,
    {
        fail_counter++;                                                         // инкрементируем счетчик ошибок.
    }
}

/*  Тестовые функции для проверки корректности сериализации.
    Все функции работают аналогично:
    – создаются исходные объекты для сериализации;
    – создается пустой счетчик ошибок;
    – создаются выходной файловый поток и архив для сериализации;
    – в архив производится сериализация
      (или происходит исключение – инкрементация счетчика);
    – поток и архив уничтожаются;
    – создаются входной поток (на тот же файл) и архив для десериализации;
    – создаются новые объекты тех же типов, что и исходные, 
      но инициализированные по-другому;
    – производится десериализация новых объектов (или инкремент счетчика в случае ошибки);
    – производится сравнение ожидаемых и действительных результатов
      сериализации (если результаты не совпадают, выбрасывается исключение).
*/

void TestBasicTypes()                                                           // базовые типы должны сериализоваться успешно
{
    double a = -90.23;
    int    b = 5;
    char   c = 'r';
    bool   d = true;
    float  e = 2.34;

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
        ifstream output("data.bin", ios_base::binary);
        Archive<ifstream> ia(output);

        double new_a = 0.0;
        int    new_b = 0;
        char   new_c = 'a';
        bool   new_d = false;
        float  new_e = 0.0;

        SerializeAndCountFails(new_a, ia, fail_counter);
        SerializeAndCountFails(new_b, ia, fail_counter);
        SerializeAndCountFails(new_c, ia, fail_counter);
        SerializeAndCountFails(new_d, ia, fail_counter);
        SerializeAndCountFails(new_e, ia, fail_counter);

        ASSERT_EQUAL(new_a, a);
        ASSERT_EQUAL(new_b, b);
        ASSERT_EQUAL(new_c, c);
        ASSERT_EQUAL(new_d, d);
        ASSERT_EQUAL(new_e, e);
        ASSERT_FALSE(fail_counter);
    }
}

void TestSingleValuePointers()                                                  // указатели должны сериализоваться успешно
{
    double a = 293.32;
    int    b = 1;
    char   c = 'b';
    bool   d = true;
    float  e = 2.54;

    double* ptr_a = &a; 
    int*    ptr_b = &b; 
    char*   ptr_c = &c; 
    bool*   ptr_d = &d; 
    float*  ptr_e = &e;
    size_t* ptr_f = nullptr;

    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        SerializeAndCountFails(ptr_a, oa, fail_counter);
        SerializeAndCountFails(ptr_b, oa, fail_counter);
        SerializeAndCountFails(ptr_c, oa, fail_counter);
        SerializeAndCountFails(ptr_d, oa, fail_counter);
        SerializeAndCountFails(ptr_e, oa, fail_counter);
        SerializeAndCountFails(ptr_f, oa, fail_counter);
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        double  new_a = 0.21;
        int     new_b = 40923;
        char    new_c = 'y';

        double* new_ptr_a = &new_a;
        int*    new_ptr_b = &new_b;
        char*   new_ptr_c = &new_c;
        bool*   new_ptr_d = nullptr;
        float*  new_ptr_e = nullptr;
        size_t* new_ptr_f = nullptr;

        SerializeAndCountFails(new_ptr_a, ia, fail_counter);
        SerializeAndCountFails(new_ptr_b, ia, fail_counter);
        SerializeAndCountFails(new_ptr_c, ia, fail_counter);
        SerializeAndCountFails(new_ptr_d, ia, fail_counter);
        SerializeAndCountFails(new_ptr_e, ia, fail_counter);
        SerializeAndCountFails(new_ptr_f, ia, fail_counter);

        ASSERT_EQUAL(*new_ptr_a, a);
        ASSERT_EQUAL(*new_ptr_b, b);
        ASSERT_EQUAL(*new_ptr_c, c);
        ASSERT_EQUAL(*new_ptr_d, d);
        ASSERT_EQUAL(*new_ptr_e, e);
        
        ASSERT_TRUE(new_ptr_a);
        ASSERT_TRUE(new_ptr_b);
        ASSERT_TRUE(new_ptr_c);
        ASSERT_TRUE(new_ptr_d);
        ASSERT_TRUE(new_ptr_e);
        ASSERT_FALSE(new_ptr_f);

        ASSERT_FALSE(fail_counter);
    }
}

void TestMultipleValuePointers()
{
    double a[5] = { 1.1, 2.2, 3.3, 4.4, 5.5 };
    int    b[3] = { 0, 1, 2 };
    bool   c[2] = { true, false };
    
    double* ptr_a = new double[5];
    int*    ptr_b = new int[3];
    bool*   ptr_c = new bool[2];

    for (size_t i = 0; i < 5; i++)
    {
        ptr_a[i] = a[i];
    }
    
    for (size_t i = 0; i < 3; i++)
    {
        ptr_b[i] = b[i];
    }
    
    for (size_t i = 0; i < 2; i++)
    {
        ptr_c[i] = c[i];
    }

    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        SerializeAndCountFails(ptr_a, oa, fail_counter);
        SerializeAndCountFails(ptr_b, oa, fail_counter);
        SerializeAndCountFails(ptr_c, oa, fail_counter);
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        double* new_ptr_a;
        int*    new_ptr_b;
        bool*   new_ptr_c;

        SerializeAndCountFails(new_ptr_a, ia, fail_counter);
        SerializeAndCountFails(new_ptr_b, ia, fail_counter);
        SerializeAndCountFails(new_ptr_c, ia, fail_counter);

        for (size_t i = 0; i < 5; i++)
        {
            ASSERT_EQUAL(new_ptr_a[i], a[i]);
        }
        
        for (size_t i = 0; i < 3; i++)
        {
            ASSERT_EQUAL(new_ptr_b[i], b[i]);
        }
        
        for (size_t i = 0; i < 2; i++)
        {
            ASSERT_EQUAL(new_ptr_c[i], c[i]);
        }
    }
}

void TestReferences()                                                           // ссылки должны сериализоваться успешно
{
    double a = 293.32;
    int    b = 1;
    char   c = 'b';
    bool   d = true;
    float  e = 2.54;

    double& ref_a = a;
    int&    ref_b = b;
    char&   ref_c = c;
    bool&   ref_d = d;
    float&  ref_e = e;

    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

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

        double& new_ref_a = new_a;
        int&    new_ref_b = new_b;
        char&   new_ref_c = new_c;
        bool&   new_ref_d = new_d;
        float&  new_ref_e = new_e;

        SerializeAndCountFails(new_ref_a, ia, fail_counter);
        SerializeAndCountFails(new_ref_b, ia, fail_counter);
        SerializeAndCountFails(new_ref_c, ia, fail_counter);
        SerializeAndCountFails(new_ref_d, ia, fail_counter);
        SerializeAndCountFails(new_ref_e, ia, fail_counter);

        ASSERT_EQUAL(new_ref_a, ref_a);
        ASSERT_EQUAL(new_ref_b, ref_b);
        ASSERT_EQUAL(new_ref_c, ref_c);
        ASSERT_EQUAL(new_ref_d, ref_d);
        ASSERT_EQUAL(new_ref_e, ref_e);

        ASSERT_FALSE(fail_counter);
    }
}

void TestSequenceContainers()                                                   // последовательные контейнеры должны сериализоваться успешно
{                                                                               // кроме статических массивов разных размеров
    vector<list<int>>            a = {{ 1, 2, 3, 4, 5 },
                                      { 897, 231 },
                                      { 0, 0, 0 },
                                      { 12, 23, 54, 90 }};

    list<deque<string>>          b = {{ "abc", "def", "ghijk" },
                                      { "123", "45678", "910a", "0909" },
                                      { "Moscow", "Tomsk", "London" }};
    
    forward_list<vector<double>> c = {{ 3.14, 9.20, -232.54 },
                                      { 90832.87, 9032.39, 84343.987 }};
    
    array<vector<string>, 3>     d = {{{ "Lorem", "ipsum", "dolor", "sit" },
                                       { "amet", "consectetur", "adipiscin" },
                                       { "elit", "sed" }}};

    array<int, 5>                e = {{ 500, 400, 300, 200, 100 }};

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

        vector<list<int>>            new_a;
        list<deque<string>>          new_b;
        forward_list<vector<double>> new_c;
        array<vector<string>, 3>     new_d;
        array<int, 6>                new_e;

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

void TestAssociativeContainers()                                                // ассоциативные контейнеры должны сериализоваться успешно
{
    set<int>                     a = { 1, 3, 2, 6, 4, 5 };
    
    multiset<int>                b = { 10, 30, 10, 20, 60, 50, 50, 40 };

    set<vector<string>>          c = {{ "abc", "cde", "efg", "ghi" },
                                      { "hello", "goodbye", "1234" }};
    
    multiset<set<double>>        d = {{ 0.5, 0.6, 0.7 },
                                      { 0.8, 0.9, 1.0 },
                                      { 1.1, 1.2, 1.3 }};

    multiset<multiset<uint32_t>> e = {{ 1, 1, 2, 2 },
                                      { 2, 2, 3, 4 }};

    map<int, string>             f = {{ 1, "one" },
                                      { 2, "two" },
                                      { 3, "three" }};

    multimap<string, set<int>>   g = {{ "even", { 0, 2, 4 }},
                                      { "even", { 252, 254, 256 }},
                                      { "neg", { -2, -3, -4 }},
                                      { "pos", { 2, 3, 4 }}};

    map<string, int>             h = {{ "one", 1 },
                                      { "two", 2 },
                                      { "three", 3 }};

    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        SerializeAndCountFails(a, oa, fail_counter);
        SerializeAndCountFails(b, oa, fail_counter);
        SerializeAndCountFails(c, oa, fail_counter);
        SerializeAndCountFails(d, oa, fail_counter);
        SerializeAndCountFails(e, oa, fail_counter);
        SerializeAndCountFails(f, oa, fail_counter);
        SerializeAndCountFails(g, oa, fail_counter);
        SerializeAndCountFails(h, oa, fail_counter);
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        set<int>                     new_a;
        multiset<int>                new_b;
        set<vector<string>>          new_c;
        multiset<set<double>>        new_d;
        multiset<multiset<uint32_t>> new_e;
        map<int, string>             new_f;
        multimap<string, set<int>>   new_g;
        map<string, int>             new_h;

        SerializeAndCountFails(new_a, ia, fail_counter);
        SerializeAndCountFails(new_b, ia, fail_counter);
        SerializeAndCountFails(new_c, ia, fail_counter);
        SerializeAndCountFails(new_d, ia, fail_counter);
        SerializeAndCountFails(new_e, ia, fail_counter);
        SerializeAndCountFails(new_f, ia, fail_counter);
        SerializeAndCountFails(new_g, ia, fail_counter);
        SerializeAndCountFails(new_h, ia, fail_counter);

        ASSERT_EQUAL(new_a, a);
        ASSERT_EQUAL(new_b, b);
        ASSERT_EQUAL(new_c, c);
        ASSERT_EQUAL(new_d, d);
        ASSERT_EQUAL(new_e, e);
        ASSERT_EQUAL(new_f, f);
        ASSERT_EQUAL(new_g, g);
        ASSERT_EQUAL(new_h, h);

        ASSERT_FALSE(fail_counter);
    }
}

void TestSerializeAccessCombinations()                                          // классы должны сериализоваться при наличии serialize и Access
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

void TestPodClass()                                                             // класс с базовыми типами должен сериализоваться успешно
{
    PodClass a(-1, 'b', 2, 500);
    size_t fail_counter = 0;

    {
        ofstream output("data.bin", ios_base::binary);
        Archive<ofstream> oa(output);

        SerializeAndCountFails(a, oa, fail_counter);
    }

    {
        ifstream input("data.bin", ios_base::binary);
        Archive<ifstream> ia(input);

        PodClass new_a;
        SerializeAndCountFails(new_a, ia, fail_counter);

        ASSERT_EQUAL(new_a, a);
        ASSERT_FALSE(fail_counter);
    }
}

void TestClassWithNestedStruct()                                                // класс с вложенной структурой должен сериализоваться успешно
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