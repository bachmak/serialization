/*  Мини-фреймворк для выполнения юнит-тестов и вывода отладочной информации.
*/

#pragma once

#include "traits.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <set>
#include <map>

class TestRunner                                                                // Класс TestRunner запускает тестовые функции, считает,
{                                                                               // сколько юнит-тестов выполнились успешно и при наличии ошибок 
public:                                                                         // останавливает выполнение программы.
    template <class TestFunc>                                                   // Шаблонный метод для запуска одной тестовой функции:
    void RunTest(const TestFunc& func, const std::string& func_name)            // принимает по ссылке тестовую функцию и строку с ее описанием.
    {
        try                                                                     // Блок, в котором возможны исключения:
        {
            func();                                                             // Вызываем тестовую функцию (содержащую ассерты).
            std::cerr << func_name << " OK" << std::endl;                       // Если исключений не было, выводим "_ОПИСАНИЕ_ ОК" в стандартный
        }                                                                       // поток ошибок.
        catch(std::exception& e)                                                // Если было из тестовой функции было выброшено исключение,
        {                                                                       // ловим его.
            std::cerr << func_name << " failed. " << e.what() << std::endl;     // Выводим "_ОПИСАНИЕ_ failed. _ПРИЧИНА_ОШИБКИ_" и
            ++fail_count;                                                       // инкрементируем счетчик ошибок.
        }
        catch(...)                                                              // Ловим неизвестное исключение.
        {
            std::cerr << "Unknown exception caught" << std::endl;               // Выводим информацию о неизвестном исключении в поток ошибок,
            ++fail_count;                                                       // инкрементируем счетчик ошибок.
        }
    }
    
    ~TestRunner()                                                               // Деструктор (предполагается, что он будет вызываться перед
    {                                                                           // началом выполнения основного кода программы).
        if (fail_count)                                                         // Если хотя бы один юнит-тест выполнился неудачно,
        {                                                           
            std::cerr << fail_count << " unit-tests failed."                    // Выводим отладочное сообщение в поток ошибок
                      << " Terminate" << std::endl;
            exit(1);                                                            // и завершаем выполнение программы.
        }
        std::cerr << "======== ALL TESTS OK ========" << std::endl;             // Иначе – успех.
    };
    
private:
    int fail_count = 0;                                                         // Счетчик неудачных юнит-тестов.
};


template <typename X, typename Y>                                               // Шаблонная функция для проверки переменных на равенство:
void AssertEqual(const X& x, const Y& y, const std::string& hint = {})          // принимает сравниваемые переменные и строку с описанием.
{
    if (!(x == y))                                                              // Если переменные не равны (проверка именно в таком виде, т.к.
    {                                                                           // для шаблонного типа м.б. перегружен ==, но не перегружен !=),
        std::ostringstream os;                                                  // из значений переменных формируем сообщение об ошибке.
        os << "Assertion failed: " << x << " != " << y;
        if (!hint.empty())                                                      // Если есть описание,
        {
            os << "\thint: " << hint;                                           // добавляем его в сообщение об ошибке.
        }
        throw std::runtime_error(os.str());                                     // Выбрасываем исключение со сформированной ошибкой.
    }
}

template <typename X, typename Y>                                               // Шаблонная функция для проверки переменных на неравенство:
void AssertNotEqual(const X& x, const Y& y, const std::string& hint = {})       // аналогично AssertEqual – выбрасываем исключение,
{                                                                               // если переменные равны
    if (x == y)
    {
        std::ostringstream os;
        os << "Assertion failed: " << x << " == " << y;
        if (!hint.empty())
        {
            os << "\thint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

inline void AssertTrue(bool b, const std::string& hint = {})                    // Функция для проверки истинности выражения:
{                                                                               // принимает булевое выражение и выбрасывает исключение, если 
    AssertEqual(b, true, hint);                                                 // оно ложно.
}

inline void AssertFalse(bool b, const std::string& hint = {})                   // Функция для проверки истинности выражения:
{                                                                               // аналогично AssertTrue – выбрасываем исключение,
    AssertEqual(b, false, hint);                                                // если выражение b истинно.
}

inline void PrintDiv(std::ostream& os,                                          // Функция для вывода разделителя между элементами контейнера
                     bool& first,                                               // при его выводе в поток.
                     const std::string& div = ", ")
{
    if (!first)                                                                 // Если элемент не первый,
    {
        os << div;                                                              // выводим разделитель.
    }
    else                                                                        // Иначе – не выводим,
    {
        first = false;                                                          // элемент больше не первый.
    }
}
                                                                                // Шаблонные перегрузки оператора вывода в поток:
template <typename T>                                                           // (1) подставляется, если:
typename std::enable_if<is_iterable<T>::value &&                                // – тип T поддерживает range-based for loop; и 
                        std::is_class<T>::value &&                              // – тип T создан как class или struct; и
                        !is_std_string<T>::value,                               // – тип T не является стандартной строкой (<< уже перегружен).
                        std::ostream&>::type                                    // Возвращает ссылку на выходной поток.
operator<<(std::ostream& os, const T& t)
{
    os << "[";  
    bool first = true;                                                          // Первый элемент ещё не выведен.

    for (const auto& item : t)                                                  // Итерируемся по контейнеру:                                                  
    {
        PrintDiv(os, first);                                                    // выводим разделитель (если нужно),
        os << item;                                                             // выводим элемент.
    }
    return os << "]";
}

template <typename First, typename Second>
std::ostream& operator<<(std::ostream& os, const std::pair<First, Second>& p)
{
    return os << p.first << ": " << p.second;
}

/*  Макросы для автоматического создания описаний 
    (имена переменных, название файла и номер строки,
    откуда вызывается тестовая функция) 
    и вызова функций Assert с этими описаниями.
*/

#define ASSERT_EQUAL(x, y)                  \
{                                           \
    std::ostringstream macro_os;            \
    macro_os << #x << " != " << #y << ", "  \
        << __FILE__ << ':' << __LINE__;     \
    AssertEqual(x, y, macro_os.str());      \
}

#define ASSERT_NOT_EQUAL(x, y)              \
{                                           \
    std::ostringstream macro_os;            \
    macro_os << #x << " == " << #y << ", "  \
        << __FILE__ << ':' << __LINE__;     \
    AssertNotEqual(x, y, macro_os.str());   \
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

/*  Макрос RUN_TEST(tr, func) вызывает метод
    RunTest у объекта tr и передает в качестве 
    параметров функцию func и строку с ее названием.
*/

#define RUN_TEST(tr, func)                  \
{                                           \
    tr.RunTest(func, #func);                \
}
    
