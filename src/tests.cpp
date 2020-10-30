/*  Файл с определениями всех функций,
    объявленных в tests.h и используемых для
    тестирования программы на корректность работы.
*/

#include "tests.h"
#include "test_runner.h"
#include "serialization.h"

#include <fstream>

using namespace std;

#define CREATE_TEST_OUTPUT_ARCHIVE(x)                                           \
    ofstream output("test.bin", ios_base::binary);                              \
    Archive<ofstream> x(output);   

#define CREATE_TEST_INPUT_ARCHIVE(x)                                            \
    ifstream input("test.bin", ios_base::binary);                               \
    Archive<ifstream> x(input);

void TestAll()                                                                  // Функция для запуска всех тестов:
{
    TestRunner tr;                                                              // Создаем объект класса TestRunner (см. test_runner.h).
    RUN_TEST(tr, TestPodClass);                                                 // С помощью макроса RUN_TEST передаем в TestRunner
    RUN_TEST(tr, TestBasicTypes);                                               // тестируемые функции (макрос вызывает метод RunTest у
    RUN_TEST(tr, TestSingleStaticPointers);                                     // объекта tr и передает в качестве аргументов тестируемую
    RUN_TEST(tr, TestSingleDynamicPointers);                                    // функцию и строку с названием этой функции).
    RUN_TEST(tr, TestMultipleStaticPointers);                                   //
    RUN_TEST(tr, TestSingleValuePointers);                                      // 
    RUN_TEST(tr, TestMultipleValuePointers);                                    // 
    RUN_TEST(tr, TestReferences);                                               // 
    RUN_TEST(tr, TestSequenceContainers);                                       //
    RUN_TEST(tr, TestAssociativeContainers);                                    //
    RUN_TEST(tr, TestSerializeAccessCombinations);                              //
    RUN_TEST(tr, TestClassWithNestedStruct);                                    //
    RUN_TEST(tr, TestDerivedClass);                                             //
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
    catch(const std::exception& ex)                                             // Ловим исключение,
    {
        cerr << "Exception caught. " << ex.what() << endl;
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
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(a, oa, fail_counter);
        SerializeAndCountFails(b, oa, fail_counter);
        SerializeAndCountFails(c, oa, fail_counter);
        SerializeAndCountFails(d, oa, fail_counter);
        SerializeAndCountFails(e, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

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

char global_var;
char new_global_var;

void TestSingleStaticPointers()
{
    double stack_var = 293.32;
    int static static_var = 1;
    global_var = 'b';

    Pointer<double> stack_var_ptr  = { &stack_var,  AllocType::Static };
    Pointer<int>    static_var_ptr = { &static_var, AllocType::Static };
    Pointer<char>   global_var_ptr = { &global_var, AllocType::Static };

    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(stack_var_ptr,  oa, fail_counter);
        SerializeAndCountFails(static_var_ptr, oa, fail_counter);
        SerializeAndCountFails(global_var_ptr, oa, fail_counter);

        SerializeAndCountFails(stack_var_ptr,  oa, fail_counter);
        SerializeAndCountFails(static_var_ptr, oa, fail_counter);
        SerializeAndCountFails(global_var_ptr, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        double new_stack_var = 0.0;
        int static new_static_var = 0;
        new_global_var = '0';

        Pointer<double> empty_double_ptr;
        Pointer<int>    empty_int_ptr;
        Pointer<char>   empty_char_ptr;

        Pointer<double> new_stack_var_ptr  = { &new_stack_var, AllocType::Static };
        Pointer<int>    new_static_var_ptr = { &new_static_var, AllocType::Static };
        Pointer<char>   new_global_var_ptr = { &new_global_var, AllocType::Static };

        SerializeAndCountFails(empty_double_ptr, ia, fail_counter);
        SerializeAndCountFails(empty_int_ptr,    ia, fail_counter);
        SerializeAndCountFails(empty_char_ptr,   ia, fail_counter);

        SerializeAndCountFails(new_stack_var_ptr,  ia, fail_counter);
        SerializeAndCountFails(new_static_var_ptr, ia, fail_counter);
        SerializeAndCountFails(new_global_var_ptr, ia, fail_counter);

        ASSERT_EQUAL(*empty_double_ptr, stack_var);   
        ASSERT_EQUAL(*empty_int_ptr,    static_var);
        ASSERT_EQUAL(*empty_char_ptr,   global_var);

        ASSERT_EQUAL(*new_stack_var_ptr,  stack_var);
        ASSERT_EQUAL(*new_static_var_ptr, static_var);
        ASSERT_EQUAL(*new_global_var_ptr, global_var);

        ASSERT_FALSE(fail_counter);     
    }
}

void TestSingleDynamicPointers()
{
    double val_a = 23.90;
    int    val_b = 2;
    char   val_c = 'k';
    string val_d = "test string";

    double* a = new double { val_a };
    int*    b = new int    { val_b };
    char*   c = new char   { val_c };
    string* d = new string { val_d };

    Pointer<double> a_ptr = { a, AllocType::DynamicSingle };
    Pointer<int>    b_ptr = { b, AllocType::DynamicSingle };
    Pointer<char>   c_ptr = { c, AllocType::DynamicSingle };
    Pointer<string> d_ptr = { d, AllocType::DynamicSingle };

    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(a_ptr, oa, fail_counter);
        SerializeAndCountFails(b_ptr, oa, fail_counter);
        SerializeAndCountFails(c_ptr, oa, fail_counter);
        SerializeAndCountFails(d_ptr, oa, fail_counter);

        SerializeAndCountFails(a_ptr, oa, fail_counter);
        SerializeAndCountFails(b_ptr, oa, fail_counter);
        SerializeAndCountFails(c_ptr, oa, fail_counter);
        SerializeAndCountFails(d_ptr, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        double* new_a = new double[100];
        int*    new_b = &val_b;
        char*   new_c = new char;
        string* new_d = nullptr;

        Pointer<double> empty_double_ptr;
        Pointer<int>    empty_int_ptr;
        Pointer<char>   empty_char_ptr;
        Pointer<string> empty_string_ptr;

        Pointer<double> new_a_ptr = { new_a, AllocType::DynamicMultiple };
        Pointer<int>    new_b_ptr = { new_b, AllocType::Static };
        Pointer<char>   new_c_ptr = { new_c, AllocType::DynamicSingle };
        Pointer<string> new_d_ptr = { new_d, AllocType::Empty };

        SerializeAndCountFails(empty_double_ptr, ia, fail_counter);
        SerializeAndCountFails(empty_int_ptr,    ia, fail_counter);
        SerializeAndCountFails(empty_char_ptr,   ia, fail_counter);
        SerializeAndCountFails(empty_string_ptr, ia, fail_counter);

        SerializeAndCountFails(new_a_ptr, ia, fail_counter);
        SerializeAndCountFails(new_b_ptr, ia, fail_counter);
        SerializeAndCountFails(new_c_ptr, ia, fail_counter);
        SerializeAndCountFails(new_d_ptr, ia, fail_counter);

        ASSERT_EQUAL(*empty_double_ptr, val_a);
        ASSERT_EQUAL(*empty_int_ptr,    val_b);
        ASSERT_EQUAL(*empty_char_ptr,   val_c);
        ASSERT_EQUAL(*empty_string_ptr, val_d);

        ASSERT_EQUAL(*new_a_ptr, val_a);
        ASSERT_EQUAL(*new_b_ptr, val_b);
        ASSERT_EQUAL(*new_c_ptr, val_c);
        ASSERT_EQUAL(*new_d_ptr, val_d);

        ASSERT_FALSE(fail_counter);
    }
}

char arr_d[5] = { 'a', 'b', 'c', 'd', 'e' };
char new_arr_d[5];

void TestMultipleStaticPointers()
{
    double     arr_a[2] = { 1.1, 2.2 };
    int static arr_b[3] = { 1, 2, 3 };
    string     arr_c[4] = { "test", "array", "of", "string" };

    size_t a_size = 2;
    size_t b_size = 3;
    size_t c_size = 4;
    size_t d_size = 5;

    Pointer<double> arr_a_ptr = { arr_a, AllocType::Static, 2 };
    Pointer<int>    arr_b_ptr = { arr_b, AllocType::Static, 3 };
    Pointer<string> arr_c_ptr = { arr_c, AllocType::Static, 4 };
    Pointer<char>   arr_d_ptr = { arr_d, AllocType::Static, 5 };

    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(arr_a_ptr, oa, fail_counter);
        SerializeAndCountFails(arr_b_ptr, oa, fail_counter);
        SerializeAndCountFails(arr_c_ptr, oa, fail_counter);
        SerializeAndCountFails(arr_d_ptr, oa, fail_counter);

        SerializeAndCountFails(arr_a_ptr, oa, fail_counter);
        SerializeAndCountFails(arr_b_ptr, oa, fail_counter);
        SerializeAndCountFails(arr_c_ptr, oa, fail_counter);
        SerializeAndCountFails(arr_d_ptr, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        double* new_arr_a = new double[1000];
        int*    new_arr_b = new int;
        string* new_arr_c = nullptr;

        Pointer<double> empty_double_ptr;
        Pointer<int>    empty_int_ptr;
        Pointer<string> empty_string_ptr;
        Pointer<char>   empty_char_ptr;

        Pointer<double> new_arr_a_ptr = { new_arr_a, AllocType::DynamicMultiple, 1000 };
        Pointer<int>    new_arr_b_ptr = { new_arr_b, AllocType::DynamicSingle, 1 };
        Pointer<string> new_arr_c_ptr = { new_arr_c, AllocType::Empty, 0 };
        Pointer<char>   new_arr_d_ptr = { new_arr_d, AllocType::Static, 5 };

        SerializeAndCountFails(empty_double_ptr, ia, fail_counter);
        SerializeAndCountFails(empty_int_ptr,    ia, fail_counter);
        SerializeAndCountFails(empty_string_ptr, ia, fail_counter);
        SerializeAndCountFails(empty_char_ptr,   ia, fail_counter);

        SerializeAndCountFails(new_arr_a_ptr, ia, fail_counter);
        SerializeAndCountFails(new_arr_b_ptr, ia, fail_counter);
        SerializeAndCountFails(new_arr_c_ptr, ia, fail_counter);
        SerializeAndCountFails(new_arr_d_ptr, ia, fail_counter);

        for (size_t i = 0; i < a_size; i++)
        {
            ASSERT_EQUAL(empty_double_ptr[i], arr_a[i]);
            ASSERT_EQUAL(new_arr_a_ptr[i],    arr_a[i]);
        }

        for (size_t i = 0; i < b_size; i++)
        {
            ASSERT_EQUAL(empty_int_ptr[i], arr_b[i]);
            ASSERT_EQUAL(new_arr_b_ptr[i], arr_b[i]);
        }
        
        for (size_t i = 0; i < c_size; i++)
        {
            ASSERT_EQUAL(empty_string_ptr[i], arr_c[i]);
            ASSERT_EQUAL(new_arr_c_ptr[i],    arr_c[i]);
        }

        for (size_t i = 0; i < d_size; i++)
        {
            ASSERT_EQUAL(empty_char_ptr[i], arr_d[i]);
            ASSERT_EQUAL(new_arr_d_ptr[i],  arr_d[i]);
        }

        ASSERT_FALSE(fail_counter);
    }
}

void TestSingleValuePointers()                                                  // указатели на одиночные стековые переменные
{
    double a = 293.32;
    int    b = 1;
    char   c = 'b';
    bool   d = true;
    float  e = 2.54;

    Pointer<double> ptr_a(&a, AllocType::Static); 
    Pointer<int>    ptr_b(&b, AllocType::Static); 
    Pointer<char>   ptr_c(&c, AllocType::Static); 
    Pointer<bool>   ptr_d(&d, AllocType::Static); 
    Pointer<float>  ptr_e(&e, AllocType::Static);
    Pointer<size_t> ptr_f;

    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(ptr_a, oa, fail_counter);
        SerializeAndCountFails(ptr_b, oa, fail_counter);
        SerializeAndCountFails(ptr_c, oa, fail_counter);
        SerializeAndCountFails(ptr_d, oa, fail_counter);
        SerializeAndCountFails(ptr_e, oa, fail_counter);
        SerializeAndCountFails(ptr_f, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        double  new_a = 0.21;
        int     new_b = 40923;
        char    new_c = 'y';

        Pointer<double> new_ptr_a(&new_a, AllocType::Static);
        Pointer<int>    new_ptr_b(&new_b, AllocType::Static);
        Pointer<char>   new_ptr_c(&new_c, AllocType::Static);
        Pointer<bool>   new_ptr_d;
        Pointer<float>  new_ptr_e;
        Pointer<size_t> new_ptr_f;

        SerializeAndCountFails(new_ptr_a, ia, fail_counter);
        SerializeAndCountFails(new_ptr_b, ia, fail_counter);
        SerializeAndCountFails(new_ptr_c, ia, fail_counter);
        SerializeAndCountFails(new_ptr_d, ia, fail_counter);
        SerializeAndCountFails(new_ptr_e, ia, fail_counter);
        SerializeAndCountFails(new_ptr_f, ia, fail_counter);

        ASSERT_EQUAL(*new_ptr_a.ptr, a);
        ASSERT_EQUAL(*new_ptr_b.ptr, b);
        ASSERT_EQUAL(*new_ptr_c.ptr, c);
        ASSERT_EQUAL(*new_ptr_d.ptr, d);
        ASSERT_EQUAL(*new_ptr_e.ptr, e);
        
        ASSERT_TRUE(new_ptr_a.ptr);
        ASSERT_TRUE(new_ptr_b.ptr);
        ASSERT_TRUE(new_ptr_c.ptr);
        ASSERT_TRUE(new_ptr_d.ptr);
        ASSERT_TRUE(new_ptr_e.ptr);
        ASSERT_FALSE(new_ptr_f.ptr);

        ASSERT_FALSE(fail_counter);
    }
}

void TestMultipleValuePointers()                                                // указатели на массивы в куче
{
    double arr_a[5] = { 1.1, 2.2, 3.3, 4.4, 5.5 };
    int    arr_b[3] = { 0, 1, 2 };
    bool   arr_c[2] = { true, false };
    
    double* a = new double[5];
    int*    b = new int[3];
    bool*   c = new bool[2];

    for (size_t i = 0; i < 5; i++)
    {
        a[i] = arr_a[i];
    }
    
    for (size_t i = 0; i < 3; i++)
    {
        b[i] = arr_b[i];
    }
    
    for (size_t i = 0; i < 2; i++)
    {
        c[i] = arr_c[i];
    }

    Pointer<double> ptr_a(a, AllocType::DynamicMultiple, 5);
    Pointer<int>    ptr_b(b, AllocType::DynamicMultiple, 3);
    Pointer<bool>   ptr_c(c, AllocType::DynamicMultiple, 2);

    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(ptr_a, oa, fail_counter);
        SerializeAndCountFails(ptr_b, oa, fail_counter);
        SerializeAndCountFails(ptr_c, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        Pointer<double> new_ptr_a;
        Pointer<int>    new_ptr_b;
        Pointer<bool>   new_ptr_c;

        SerializeAndCountFails(new_ptr_a, ia, fail_counter);
        SerializeAndCountFails(new_ptr_b, ia, fail_counter);
        SerializeAndCountFails(new_ptr_c, ia, fail_counter);

        for (size_t i = 0; i < 5; i++)
        {
            ASSERT_EQUAL(new_ptr_a.ptr[i], a[i]);
        }
        
        for (size_t i = 0; i < 3; i++)
        {
            ASSERT_EQUAL(new_ptr_b.ptr[i], b[i]);
        }
        
        for (size_t i = 0; i < 2; i++)
        {
            ASSERT_EQUAL(new_ptr_c.ptr[i], c[i]);
        }
    }
}

void TestReferences()                                                           // ссылки
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
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(ref_a, oa, fail_counter);
        SerializeAndCountFails(ref_b, oa, fail_counter);
        SerializeAndCountFails(ref_c, oa, fail_counter);
        SerializeAndCountFails(ref_d, oa, fail_counter);
        SerializeAndCountFails(ref_e, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

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

void TestSequenceContainers()                                                   // последовательные контейнеры
{
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
        CREATE_TEST_OUTPUT_ARCHIVE(oa);
        
        SerializeAndCountFails(a, oa, fail_counter);
        SerializeAndCountFails(b, oa, fail_counter);
        SerializeAndCountFails(c, oa, fail_counter);
        SerializeAndCountFails(d, oa, fail_counter);
        SerializeAndCountFails(e, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

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

void TestAssociativeContainers()                                                // ассоциативные контейнеры
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
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

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
        CREATE_TEST_INPUT_ARCHIVE(ia);

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

void TestSerializeAccessCombinations()                                          // классы с различными комбинациями serialize и Access
{
    NotSerializableWithFriendAccess       a(10, 20);
    NotSerializableWithoutFriendAccess    b(30, 40);
    SerializableWithoutFriendAccess       c(50, 60);
    SerializableWithFriendAccess          d(70, 80);
    PublicSerializableWithoutFriendAccess e(90, 100);
    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(a, oa, fail_counter);
        SerializeAndCountFails(b, oa, fail_counter);
        SerializeAndCountFails(c, oa, fail_counter);
        SerializeAndCountFails(d, oa, fail_counter);
        SerializeAndCountFails(e, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

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

void TestPodClass()                                                             // класс с полями базовых типов 
{
    PodClass a(-1, 'b', 2, 500);
    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(a, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        PodClass new_a;
        SerializeAndCountFails(new_a, ia, fail_counter);

        ASSERT_EQUAL(new_a, a);
        ASSERT_FALSE(fail_counter);
    }
}

void TestClassWithNestedStruct()                                                // класс с вложенной структурой
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
        CREATE_TEST_OUTPUT_ARCHIVE(oa);
        SerializeAndCountFails(a, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        ClassWithNestedStruct new_a;
        SerializeAndCountFails(new_a, ia, fail_counter);

        ASSERT_EQUAL(new_a, a);
        ASSERT_FALSE(fail_counter);
    }
}

void TestDerivedClass()                                                         // базовый и производный класс
{
    BaseClass base_a(100, 0.25, 'W');
    DerivedClass derived_a(base_a, "test string", { 2, 4, 8, 10, 12 });
    size_t fail_counter = 0;

    {
        CREATE_TEST_OUTPUT_ARCHIVE(oa);

        SerializeAndCountFails(derived_a, oa, fail_counter);
    }

    {
        CREATE_TEST_INPUT_ARCHIVE(ia);

        DerivedClass new_derived_a;
        SerializeAndCountFails(new_derived_a, ia, fail_counter);

        const BaseClass& new_base_a =
            *dynamic_cast<BaseClass const*>(&new_derived_a);
        
        ASSERT_EQUAL(new_base_a, base_a);
        ASSERT_EQUAL(new_derived_a, derived_a);
        ASSERT_FALSE(fail_counter);
    }
}