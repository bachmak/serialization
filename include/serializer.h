/*  Шаблон класса Serializer выполняет, непосредственно, сериализацию 
    и десериализацию объектов путем выбора шаблонной перегрузки,
    соответствующей типу сериализуемого объекта.
    Создание экземпляров класса Serializer<Stream> выполняется только
    в классе Archive<Stream>.
*/

#pragma once

#include <stdexcept>
#include <sstream>

#include "traits.h"
#include "access.h"

template <typename Stream>                                                      // Объявляем класс Archive для дальнейшего объявления его
class Archive;                                                                  // дружественным к классу Serializer.

template <typename Stream>
class Serializer
{
public:
    template <typename T>                                                       // Единственный публичный метод – оператор ввода/вывода
    void operator&(T& t)                                                        // для определения процедуры сериализации в методе serialize
    {                                                                           // сериализуемого класса.
        serialize(t);
    }

private:
    friend class Archive<Stream>;                                               // Дружественный класс Archive<Stream> – 
                                                                                // предоставляет интерфейс взаимодействия.
    friend struct Access;                                                       // Дружественная структура Access – для возможности вызова
                                                                                // конструктора класса Serializer при проверке is_serializable.
    Serializer(Stream& stream) : stream(stream) {}                              // Конструктор с передачей потока для сериализации по ссылке. 

    Stream& stream;                                                             // Ссылка на поток для записи/чтения.

                                                                                // Шаблонные перегрузки метода serialize:
    template <typename T>                                                       // (1) подставляется, если:
    enable_if_t<is_serializable<T>::value>                                      // – у объекта t есть метод serialize.
    serialize(T& t)                                                             // Возвращает void (тип по умолчанию для enable_if).
    {
        Access::serialize(*this, t);                                            // Вызываем метод serialize у объекта через структуру Access
    }                                                                           // (на случай, если метод serialize приватный)


    template <typename T>                                                       // (2) подставляется, если:
    enable_if_t<is_iterable<T>::value &&                                        // – тип T поддерживает range-based for loop; и
                has_size<T>::value   &&                                         // – имеет метод size() (все контейнеры, кроме forward_list); и
                is_ostream<Stream>::value>                                      // – поток, которым инстанцирован шаблон класса – выходной.
    serialize(T& t)                                                             // Возвращает void.
    {
        auto size = static_cast<uint32_t>(t.size());                            // Считываем размер контейнера и приводим к размеру 4 байта,
        serialize(size);                                                        // сериализуем размер,
        serialize_container(t);                                                 // сериализуем элементы контейнера.
    }


    template <typename T>                                                       // (3) подставляется, если:
    enable_if_t<is_std_forward_list<T>::value &&                                // – тип T – стандартный односвязный список 
                is_ostream<Stream>::value>                                      // (т.к. у std::forward_list нет метода size()); и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – выходной.
    {                                                                           // Возвращает void.
        auto size = static_cast<uint32_t>(std::distance(t.begin(), t.end()));   // Вычисляем размер контейнера прохождением от начала до конца,
        serialize(size);                                                        // сериализуем его,
        serialize_container(t);                                                 // сериализуем элементы контейнера.
    }


    template <typename T>                                                       // (4) подставляется, если:
    enable_if_t<is_std_array<T>::value &&                                       // – тип T – стандартный статический массив;
                is_istream<Stream>::value>                                      // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – входной.
    {                                                                           // Возвращает void.
        uint32_t size = 0;                                                      // Создаем переменную для размера массива и
        serialize(size);                                                        // десериализуем в нее данные о размере.
        
        if (size != t.size())                                                   // Если размер ранее сериализованного и десериализуемого
        {                                                                       // отличаются (для статических массивов это недопустимо),
            std::ostringstream os;                                              // формируем сообщение об ошибке
            os << "Different sizes of static std arrays. "                      //
               << "Serialized array size: " << size                             //
               << ". Deserializing array size: " << t.size()                    //
               << ". Deserialization failed.";                                  //
            throw std::invalid_argument(os.str());                              // и выбрасываем исключение.
        }

        for (auto& item : t)                                                    // Если размеры равны, итерируемся по массиву и десериализуем
        {                                                                       // его поэлементно.
            serialize(item);
        }
    }


    template <typename T>                                                       // (5) подставляется, если:
    enable_if_t<(is_std_vector<T>::value  ||                                    // – тип T – стандартный последовательный контейнер, 
                 is_std_string<T>::value) &&                                    // хранящий данные в куче (вектор или строка);
                 is_istream<Stream>::value>                                     // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – входной.
    {                                                                           // Возвращает void.
        uint32_t size = 0;                                                      // Создаем переменную для размера контейнера и
        serialize(size);                                                        // десериализуем в нее данные о размере.
        t.resize(size);                                                         // Изменяем размер десериализуемого контейнера.

        for (auto& item : t)                                                    // Итерируемся по контейнеру и десериализуем его поэлементно.
        {
            serialize(item);
        }
    }


    template <typename T>                                                       // (6) подставляется, если:
    enable_if_t<(is_std_list<T>::value   ||                                     // – тип T – стандартный двусвязный список (лист или дек);
                 is_std_deque<T>::value) &&                                     // и
                 is_istream<Stream>::value>                                     // – поток, которым инстанцирован шаблон класса – входной.
    serialize(T& t)                                                             // Возвращает void.
    {
        uint32_t size = 0;                                                      // Создаем переменную для размера списка и
        serialize(size);                                                        // десериализуем в нее данные о размере.
        t.clear();                                                              // Очищаем текущее содержимое списка.

        for (uint32_t i = 0; i < size; i++)                                     // Делаем size итераций:
        {
            typename T::value_type item;                                        // Создаем элемент типа, от которого инстанцирован список,
            serialize(item);                                                    // и десериализуем его.
            t.push_back(std::move(item));                                       // Перемещаем десериализованный элемент в конец списка
        }                                                                       // (чтобы избежать копирования).
    }


    template <typename T>                                                       // (7) подставляется, если:
    enable_if_t<is_std_forward_list<T>::value &&                                // – тип T – стандартный односвязный список;
                is_istream<Stream>::value>                                      // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – входной.
    {                                                                           // Возвращает void.
        uint32_t size = 0;                                                      // Создаем переменную для размера списка и
        serialize(size);                                                        // десериализуем в нее данные о размере.
        t.clear();                                                              // Очищаем текущее содержимое списка.

        for (uint32_t i = 0; i < size; i++)                                     // Делаем size итераций:
        {
            typename T::value_type item;                                        // Создаем элемент типа, от которого инстанцирован список,
            serialize(item);                                                    // и десериализуем его.
            t.push_front(std::move(item));                                      // Перемещаем десериализованный элемент в начало списка
        }                                                                       // (т.к. у std::forward_list нет метода push_back).

        t.reverse();                                                            // Инвертируем порядок элементов.
    }


    template <typename T>                                                       // (8) подставляется, если:
    enable_if_t<is_iterable<T>::value &&                                        // – тип T – ассоциативный контейнер (проверяем, поддерживается
                has_insert<T>::value &&                                         // ли range-based for loop, и наличие метода insert());
                is_istream<Stream>::value>                                      // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – входной.
    {
        uint32_t size = 0;                                                      // Создаем переменную для размера контейнера и
        serialize(size);                                                        // десериализуем в нее данные о размере.
        t.clear();                                                              // Очищаем текущее содержимое контейнера.

        for (uint32_t i = 0; i < size; i++)                                     // Делаем size итераций:
        {
            typename T::value_type item;                                        // Создаем элемент типа, от которого инстанцирован контейнер,
            serialize(item);                                                    // и десериализуем его.
            t.insert(std::move(item));                                          // Перемещаем десериализованный элемент в контейнер
        }
    }


    template <typename First, typename Second>                                  // (9) подставляется для стандартных пар:
    void serialize(std::pair<const First, Second>& t)                           // первый тип константный, т.к. элементы std::map имеют тип
    {                                                                           // std::pair<const Key, Value> – иначе не подставляется.
        serialize(*const_cast<First*>(&t.first));                               // Разыменовываем и сериализуем неконстантный указатель на
        serialize(t.second);                                                    // первое поле и просто сериализуем второе.
    }


    template <typename T>                                                       // (10) подставляется, если:
    enable_if_t<std::is_pointer<T>::value &&                                    // – тип T – указатель;
                is_ostream<Stream>::value>                                      // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – выходной.
    {
        bool ptr_is_not_null = t ? true : false;                                // Создаем и инициализируем флаг для индикации того,
        serialize(ptr_is_not_null);                                             // что указатель не пустой. Сериализуем его.

        if (ptr_is_not_null)                                                    // Если указатель не пустой,
        {
            serialize(*t);                                                      // сериализуем его содержимое.
        }
    }


    template <typename T>                                                       // (11) подставляется, если:
    enable_if_t<std::is_pointer<T>::value &&                                    // – тип T – указатель;
                is_istream<Stream>::value>                                      // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – входной.
    {
        bool ptr_is_not_null;                                                   // Создаем флаг для индикации того, что указатель не пустой,
        serialize(ptr_is_not_null);                                             // и десериализуем его.

        if (ptr_is_not_null)                                                    // Если сериализованный указатель не был пуст:
        {
            t = new typename std::remove_pointer<T>::type;                      // выделяем новую память для указателя (remove_pointer позволяет
            serialize(*t);                                                      // получить тип, на который указывает указатель);
        }                                                                       // десериализуем содержимое в новый участок памяти.
        else                                                                    // Иначе:
        {   
            t = nullptr;                                                        // инициализируем указатель нулевым указателем.
        }
    }

    template <typename T>                                                       // (12) подставляется, если:
    enable_if_t<std::is_fundamental<T>::value &&                                // – T – фундаментальный тип (арифметический, void, nullptr_t);
                is_ostream<Stream>::value>                                      // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – выходной.
    {                                                                           // Возвращает void.
        stream.write(reinterpret_cast<const char*>(&t), sizeof(t));             // Приводим указатель на t к указателю на const char (сигнатура
    }                                                                           // метода ostream::write) и записываем массив байт размером
                                                                                // sizeof(t) в поток

    template <typename T>                                                       // (13) подставляется, если:
    enable_if_t<std::is_fundamental<T>::value &&                                // – T – фундаментальный тип;
                is_istream<Stream>::value>                                      // и
    serialize(T& t)                                                             // – поток, которым инстанцирован шаблон класса – входной.
    {                                                                           // Возвращает void.
        stream.read(const_cast<char*>(reinterpret_cast<const char*>(&t)),       // Приводим указатель на t к неконстантному указателю на char и
                    sizeof(t));                                                 // записываем массив байт размера sizeof(t) по этому указателю
    }


    void serialize(...)                                                         // (14) подставляется, если ни одна из вышеперечисленных
    {                                                                           // перегрузок не является допустимой.
        throw std::invalid_argument("Unsupported type. Serialization failed."); // Выбрасываем исключение с сообщением о том, что сериализация
    }                                                                           // для требуемого типа не поддерживается.


    template <typename T>                                                       // Метод для сериализации (только запись в поток) контейнера,
    void serialize_container(T& t)                                              // поддерживающего range-based for loop.
    {
        for (auto& item : t)                                                    // Итерируемся по контейнеру с обращением к элементу по ссылке и
        {                                                                       // сериализуем каждый элемент
            serialize(item);
        }
    }
};
