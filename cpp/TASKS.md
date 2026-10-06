# C++: практика по месяцам

## Общие требования
- Каждая задача — отдельная папка `cpp/mN_tK/` (N — месяц, K — номер задачи). Референсный шаблон: `cpp/template/`.
- Сборка: Debug (ASan + UBSan) для проверки, Release (`-O2`) для замеров и valgrind.
- Тесты — `assert` в `main.cpp` или отдельный `test.cpp`.
- Рядом с кодом лежит `notes.md`: ответы на вопросы задачи, прогнозы до запуска.
- В параграфах 2–4 внутри своих реализаций запрещены `std::string`, `std::vector`, `std::unique_ptr` и `<cstring>`. В тестах их использовать можно, например, как эталон.

---

## § 1. Основы и инструменты (месяц 1)

### 1.1. Этапы сборки
Файл `hello.cpp`:
```cpp
#include <iostream>
#define GREETING "Hello"
int add(int a, int b) { return a + b; }
int main() { std::cout << GREETING << ' ' << add(2, 3) << '\n'; }
```
Собрать вручную по шагам:
```bash
g++ -E hello.cpp -o hello.ii  # препроцессор
g++ -S hello.ii  -o hello.s   # компиляция в ассемблер
g++ -c hello.s   -o hello.o   # ассемблирование в объектный файл
g++ hello.o      -o hello     # линковка
```
Исследовать и записать в `notes.md`:
1. Сколько строк в `hello.ii` (`wc -l`) и почему так много? Во что превратился `GREETING`?
2. Найти в `hello.s` метки `main` и `add` (имя будет вида `_Z3addii`). Расшифровать имя через `c++filt`. Зачем нужен name mangling?
3. `nm -C hello.o`: какие символы помечены `T`, какие `U`? Откуда линковщик берёт символы `U`?
4. `ldd hello`: какие динамические библиотеки подключены?

### 1.2. Раздельная компиляция и ошибки сборки
Модуль `geometry`:
```cpp
// geometry.h
struct Point { double x, y; };
double distance(Point a, Point b);
double perimeter(const Point* pts, int n);  // периметр замкнутого многоугольника
```
`geometry.cpp` содержит реализацию, `main.cpp` — тесты: квадрат 1×1 имеет периметр 4, треугольник 3-4-5 — периметр 12.
Собрать вручную: `g++ -c geometry.cpp`, `g++ -c main.cpp`, `g++ geometry.o main.o -o app`.

Воспроизвести каждую ошибку отдельным коммитом. Для каждой записать текст ошибки, этап (препроцессор / компилятор / линковщик) и как это доказать (на каком шаге `-c` упал или прошёл):

| # | Как сломать | Ожидаемая ошибка |
|---|---|---|
| a | Слинковать без `geometry.o`: `g++ main.o -o app` | `undefined reference to distance(...)` |
| b | Перенести определение `distance` в `geometry.h` без `inline` | `multiple definition of distance(...)` |
| c | Убрать include guard из `geometry.h`, создать `polygon.h` с `#include "geometry.h"`, в `main.cpp` подключить оба заголовка | `redefinition of 'struct Point'` |
| d | Убрать `#include "geometry.h"` из `main.cpp` | `'distance' was not declared in this scope` |

Затем починить (b) через `inline` и объяснить, почему это работает (ODR).

### 1.3. Калькулятор
Функция и программа:
```cpp
// true при успехе; при ошибке false и текст в error
bool evaluate(const std::string& line, double& result, std::string& error);
```
- Формат строки: `<число> <op> <число>`, где op — один из `+ - * / %`. Пример: `12.5 * 4`.
- `%` разрешён только для целых операндов.
- Ошибки: нечисловой операнд, неизвестный оператор, деление на ноль, лишние токены, пустая строка.
- `main` читает stdin построчно и печатает результат в stdout, ошибку — в stderr в виде `error: <причина>`, затем продолжает работу. Выход по EOF или по строке `q`.
- Тесты: не меньше 12 `assert` на `evaluate`, включая все виды ошибок. Плюс файл `input.txt` для запуска `./calc < input.txt`.

### 1.4. Теория чисел
```cpp
std::vector<int> primes_up_to(int n);                 // решето Эратосфена
long long gcd(long long a, long long b);              // итеративно; gcd(0,0)=0, работает с отрицательными
long long lcm(long long a, long long b);              // при переполнении бросает std::overflow_error
std::string to_base(long long v, int base);           // base 2..36, цифры 0-9A-Z, минус для отрицательных
long long from_base(const std::string& s, int base);  // неверный символ → std::invalid_argument
```
Тесты: `primes_up_to(30) == {2,3,5,7,11,13,17,19,23,29}`; число простых ≤ 10⁶ равно 78498; `gcd(-12, 18) == 6`; `to_base(255, 16) == "FF"`; `to_base(-10, 2) == "-1010"`; `from_base(to_base(x, b), b) == x` для 1000 случайных x и всех b.
Замер: время `primes_up_to(10'000'000)` в Release. Сколько памяти занимает решето?

### 1.5. Отладка в gdb
Программа с ошибками:
```cpp
#include <iostream>
int average(const int* a, int n) {
    int sum = 0;
    for (int i = 0; i <= n; ++i) sum += a[i];
    return sum / n;
}
int main() {
    int data[] = {4, 8, 15, 16, 23, 42};
    int n = sizeof(data) / sizeof(data[0]);
    std::cout << average(data, n) << '\n';
    std::cout << average(data, 0) << '\n';
}
```
1. Собрать **без** санитайзеров: `g++ -g -O0`. Найти обе ошибки только средствами gdb: `break average`, `run`, `next`, `print i`, `print a[i]`, `watch sum`, `bt` после падения, `info frame`.
2. Сохранить лог сессии (`set logging enabled on`) в `gdb.log`.
3. Пересобрать с ASan/UBSan и сравнить, насколько быстрее нашлись ошибки.
4. Исправить: что должна возвращать функция при `n == 0`? Почему среднее `int / int` может быть неточным и как это исправить?

**Вопросы § 1:** Чем объявление отличается от определения? Что такое ODR? Чем `int&` отличается от `const int&` при передаче в функцию? Почему `int x; std::cout << x;` — UB? Что делает include guard и чем он отличается от `#pragma once`?

---

## § 2. Память (месяц 2)

### 2.1. Карта памяти процесса
```cpp
int g_global = 1;
void probe(int depth);  // локальные: int a; double b; char c[16];
```
`probe` печатает `depth`, `&a`, `&b`, `(void*)c` и вызывает себя до глубины 5.
В `main` дополнительно напечатать адреса: `g_global`, `static int` внутри функции, строкового литерала `"abc"`, памяти от `new int`, локальной переменной `main`. В конце вывести содержимое `/proc/self/maps` (через `std::ifstream`).

Собрать с `-O0`. В `notes.md`:
1. Для каждого адреса указать, в каком сегменте он лежит (stack / heap / .data / .rodata), сверяясь с `/proc/self/maps`.
2. Определить по разнице `&a` на соседних уровнях размер стекового кадра `probe` и направление роста стека.
3. Нарисовать стек для глубины 3: кадры, адреса, переменные.
4. Пересобрать с `-O2`. Что изменилось и почему?

### 2.2. Стековый кадр в ассемблере
В [godbolt.org](https://godbolt.org/) (компилятор x86-64 gcc):
```cpp
int sum3(int a, int b, int c) { int s = a + b; return s + c; }
int caller() { return sum3(1, 2, 3) + 10; }
int many(int a, int b, int c, int d, int e, int f, int g, int h) { return a + h; }
int call_many() { return many(1, 2, 3, 4, 5, 6, 7, 8); }
```
С флагом `-O0` прокомментировать **каждую** строку ассемблера `sum3` и `caller` в `notes.md`:
- пролог и эпилог (`push rbp`, `mov rbp, rsp`, `pop rbp`, `ret`);
- в каких регистрах пришли `a, b, c` (`edi, esi, edx`) и куда сохранены (`[rbp-N]`);
- где лежит адрес возврата и кто его туда положил;
- как аргументы 7 и 8 функции `many` передаются через стек (соглашение System V AMD64).

С `-O2`: во что превратился `caller` и почему?

### 2.3. Си-строки
```cpp
std::size_t my_strlen(const char* s);
char* my_strcpy(char* dst, const char* src);       // возвращает dst
char* my_strcat(char* dst, const char* src);       // возвращает dst
int   my_strcmp(const char* a, const char* b);     // <0, 0, >0; байты сравниваются как unsigned char
char* my_strdup(const char* s);                    // память через new[], освобождает вызывающий через delete[]
```
Тесты: сравнить с `std::strlen/strcpy/strcat/strcmp` (для `strcmp` сравнивать знак) на строках `""`, `"a"`, `"hello"`, `"привет"` (байты > 127), на строках с общим префиксом и разной длины.
Эксперимент: вызвать `my_strcpy` в буфер `char buf[4]` со строкой `"overflow"` и получить отчёт ASan `stack-buffer-overflow`. Разобрать отчёт.

### 2.4. Динамический массив в стиле Си
```cpp
struct IntArray { int* data; std::size_t size; std::size_t capacity; };
IntArray    ia_create();                                   // data = nullptr, size = capacity = 0
void        ia_push_back(IntArray& a, int v);              // рост capacity: 0 → 1 → 2 → 4 → 8 ...
int         ia_get(const IntArray& a, std::size_t i);      // assert(i < a.size)
void        ia_erase(IntArray& a, std::size_t i);          // удаление со сдвигом хвоста
void        ia_destroy(IntArray& a);                       // освободить память, обнулить поля
```
- Тест: 10⁶ `push_back`, проверить содержимое и число перевыделений (должно быть 21). Проверить `erase` первого, среднего и последнего элемента.
- valgrind (Release): `All heap blocks were freed`.
- Эксперимент: заменить рост ×2 на +1 и замерить время для 10⁵ элементов. Объяснить разницу (амортизированная сложность).

### 2.5. Односвязный список
```cpp
struct Node { int value; Node* next; };
void        push_front(Node*& head, int v);
void        push_back(Node*& head, int v);
bool        remove_first(Node*& head, int v);   // удалить первое вхождение, true если нашли
void        reverse(Node*& head);               // на месте, O(1) доп. памяти
std::size_t length(const Node* head);
void        clear(Node*& head);                 // освободить все узлы, head = nullptr
```
- Тесты: пустой список, один элемент, удаление головы, середины и хвоста, `reverse` для 0, 1 и 3 элементов.
- valgrind: 0 утечек.
- Нарисовать в `notes.md` указатели до и после `reverse` для списка 1→2→3. Почему `head` передаётся как `Node*&`?

### 2.6. Зоопарк UB
Семь программ, каждая не длиннее 15 строк. Индексы и значения брать из `std::cin`, иначе компилятор поймает ошибку на этапе компиляции, и эксперимента не будет.

| # | Ошибка | Чем ловить |
|---|---|---|
| 1 | Запись за границу `new int[10]` | ASan: heap-buffer-overflow |
| 2 | Чтение за границу локального `int a[10]` | ASan: stack-buffer-overflow |
| 3 | Использование памяти после `delete` | ASan: heap-use-after-free |
| 4 | Двойной `delete` | ASan: attempting double-free |
| 5 | Возврат указателя на локальную переменную | Сначала прочитать предупреждение компилятора; для эксперимента отключить `-Werror` для этого файла. ASan: stack-use-after-return (в GCC может понадобиться `ASAN_OPTIONS=detect_stack_use_after_return=1`) |
| 6 | Переполнение `int` (значение из cin + 1) | UBSan: signed integer overflow |
| 7 | Чтение неинициализированной памяти из `new int[10]` в условии `if` | ASan **не** ловит → valgrind: Conditional jump depends on uninitialised value |

Для каждой программы:
1. Запустить без санитайзеров с `-O0` и `-O2` и записать поведение. Скорее всего, программа «работает».
2. Запустить с инструментом из таблицы и сохранить отчёт.
3. Объяснить по отчёту: тип ошибки, строку ошибки, где память выделена и где освобождена (ASan показывает до трёх стеков вызовов).

### 2.7. Переполнение стека
```cpp
void dive(int depth) {
    char buf[1024];
    buf[0] = static_cast<char>(depth);    // чтобы буфер не выкинул оптимизатор
    if (depth % 1000 == 0) std::cout << depth << '\n';
    dive(depth + 1);
    std::cout << buf[0];                  // мешает превращению в цикл (хвостовой вызов)
}
```
1. Найти максимальную глубину до падения (Release, без ASan).
2. Сравнить с `ulimit -s` (размер стека в КБ). Совпадает ли с оценкой «глубина × размер кадра»?
3. Запустить после `ulimit -s 16384`. Как изменилась глубина?
4. Запустить с ASan и прочитать отчёт stack-overflow.
5. Почему массив `double m[1000][1000]` нельзя объявлять локальной переменной? Переписать через кучу.

**Вопросы § 2:** Что хранится в стековом кадре? Почему нельзя вернуть указатель на локальную переменную? Чему равен `sizeof` у `int* p` и у `int a[10]`? Что такое array-to-pointer decay? Где хранятся строковые литералы и почему их нельзя менять? Чем `delete` отличается от `delete[]`?

---

## § 3. Классы, RAII, move (месяц 3)

### 3.1. Класс String
```cpp
class String {
public:
    String();                                   // пустая строка ""
    String(const char* s);
    String(const String& other);
    String(String&& other) noexcept;
    String& operator=(const String& other);     // корректно при самоприсваивании
    String& operator=(String&& other) noexcept;
    ~String();

    std::size_t size() const;
    const char* c_str() const;                  // всегда завершается '\0'
    char&       operator[](std::size_t i);
    const char& operator[](std::size_t i) const;
    String&     operator+=(const String& rhs);
private:
    char* data_;
    std::size_t size_;
};
String operator+(const String& a, const String& b);
bool operator==(const String& a, const String& b);
std::ostream& operator<<(std::ostream& os, const String& s);
```
Каждая спецфункция печатает своё имя (`ctor(const char*)`, `copy ctor`, `move ctor`, `copy assign`, `move assign`, `dtor`), если определён `STRING_LOG`.

Для каждого сценария **сначала** записать прогноз лога, потом запустить и объяснить расхождения:
1. `String a = "hi";`
2. `String b = a;`
3. `String c = std::move(a);` — в каком состоянии теперь `a`?
4. `b = c;`
5. `b = String("tmp");`
6. `String d = b + c;`
7. `void f(String s);` и вызов `f(b);`
8. `f(std::move(b));`
9. `String g() { String s = "x"; return s; }` и `String e = g();` — что такое RVO/NRVO?
10. `std::vector<String> v; v.push_back("a"); v.push_back("b"); v.push_back("c");` — затем убрать `noexcept` у move-конструктора и сравнить логи. Почему вектор начал копировать?

Тесты: самоприсваивание `a = a`, `+=` с самим собой, пустые строки. ASan чист.

### 3.2. UniquePtr и SharedPtr
Понадобятся простые шаблоны классов (learncpp, раздел про class templates).
```cpp
template <class T> class UniquePtr {
public:
    explicit UniquePtr(T* p = nullptr);
    ~UniquePtr();
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    UniquePtr(UniquePtr&& other) noexcept;
    UniquePtr& operator=(UniquePtr&& other) noexcept;
    T* get() const;
    T* release();                // отдать владение, вернуть указатель
    void reset(T* p = nullptr);  // удалить старый объект, взять новый
    T& operator*() const;
    T* operator->() const;
    explicit operator bool() const;
};
template <class T> class SharedPtr {  // управляющий блок: { T* ptr; long count; }
public:
    explicit SharedPtr(T* p = nullptr);
    SharedPtr(const SharedPtr&);  SharedPtr& operator=(const SharedPtr&);
    SharedPtr(SharedPtr&&) noexcept;  SharedPtr& operator=(SharedPtr&&) noexcept;
    ~SharedPtr();
    long use_count() const;
    T* get() const;  T& operator*() const;  T* operator->() const;
};
```
- Для тестов: класс `Tracker` со `static int alive` (+1 в конструкторах, −1 в деструкторе). После выхода из любой области видимости `alive == 0`.
- Тест `SharedPtr`: 5 копий в `std::vector`, проверить `use_count()` после каждого `push_back`, `pop_back`, присваивания.
- Эксперимент с циклом: `struct N { SharedPtr<N> next; Tracker t; };` — два узла ссылаются друг на друга. После выхода из области видимости `alive == 2`, valgrind показывает утечку. Объяснить причину. Переписать на `std::shared_ptr` + `std::weak_ptr` и показать, что утечки нет.

### 3.3. RAII-обёртки
```cpp
class File {
public:
    File(const char* path, const char* mode);  // fopen; при ошибке std::runtime_error с текстом strerror(errno)
    ~File();                                    // fclose
    File(const File&) = delete;  File& operator=(const File&) = delete;
    File(File&&) noexcept;       File& operator=(File&&) noexcept;
    void write(const std::string& s);
    std::string read_line();                    // без '\n'; при EOF пустая строка
    static int open_count();                    // число открытых сейчас файлов
};
class LockGuard {                               // над std::mutex
public:
    explicit LockGuard(std::mutex& m);          // lock
    ~LockGuard();                               // unlock
    LockGuard(const LockGuard&) = delete;  LockGuard& operator=(const LockGuard&) = delete;
};
```
Тесты:
- открытие несуществующего файла бросает исключение;
- функция открывает файл, пишет и бросает исключение посередине; после `catch` проверить `open_count() == 0`;
- функция с `LockGuard` и ранним `return` или исключением; после неё `m.try_lock()` возвращает `true`.

### 3.4. Ломаем правило трёх
1. В `String` закомментировать конструктор копирования и копирующее присваивание (move-операции тоже убрать, иначе копирование станет `deleted`).
2. Сценарий: `String a = "x"; { String b = a; }  std::cout << a;`
3. Получить отчёт ASan (heap-use-after-free или double-free) и объяснить, что сгенерировал компилятор и почему это ломается.
4. Объявить копирование `= delete` и прочитать ошибку компиляции. Вернуть правильную реализацию.

**Вопросы § 3:** Правило 0/3/5 своими словами. Когда вызывается move-конструктор, а когда копирующий? Что на самом деле делает `std::move`? Зачем `noexcept` у move-операций? Почему `SharedPtr` с циклической ссылкой течёт?

---

## § 4. Шаблоны и STL (месяц 4)

### 4.1. Vector<T>
```cpp
template <class T> class Vector {
public:
    Vector();  ~Vector();
    Vector(const Vector&);  Vector& operator=(const Vector&);
    Vector(Vector&&) noexcept;  Vector& operator=(Vector&&) noexcept;
    void reserve(std::size_t n);
    std::size_t size() const;  std::size_t capacity() const;
    void push_back(const T& v);
    void push_back(T&& v);
    template <class... Args> T& emplace_back(Args&&... args);
    void pop_back();
    void clear();
    T& operator[](std::size_t i);  const T& operator[](std::size_t i) const;
    T* begin();  T* end();  const T* begin() const;  const T* end() const;
};
```
Требования к памяти: выделение `::operator new(n * sizeof(T))`, конструирование через placement new, уничтожение явным вызовом `p->~T()`, освобождение `::operator delete`. При росте элементы **перемещаются**.
Тесты:
- `Tracker`: после деструктора вектора `alive == 0`; после `clear()` то же;
- тип без конструктора по умолчанию `struct NoDefault { explicit NoDefault(int); };` компилируется и работает;
- move-only тип `std::unique_ptr<int>` работает через `push_back(std::move(p))` и `emplace_back`;
- range-for `for (auto& x : v)` работает;
- ASan и valgrind чисты.

Замер: 10⁶ `push_back` у своего `Vector` и у `std::vector`.
Вопрос: почему нельзя выделять память через `new T[n]`?

### 4.2. HashMap<K, V>
Цепочки: массив бакетов, в каждом — односвязный список узлов `{K key; V value; Node* next;}`. Для массива бакетов можно взять свой `Vector` из 4.1.
```cpp
template <class K, class V, class Hash = std::hash<K>> class HashMap {
public:
    bool insert_or_assign(const K& k, const V& v);  // true, если ключ новый
    V*   find(const K& k);                           // nullptr, если нет
    bool erase(const K& k);
    V&   operator[](const K& k);                     // создаёт V{}, если ключа нет
    std::size_t size() const;
    // при size / bucket_count > 1.0 — рехеширование в 2 раза больше бакетов
};
```
Тест-оракул: 10⁶ случайных операций (insert / erase / find, ключи от 0 до 10⁵, фиксированный seed) параллельно над `HashMap` и `std::unordered_map`; после каждой операции результаты совпадают, в конце совпадает `size()`. valgrind чист.

### 4.3. Бенчмарки контейнеров
Правила замера: Release, `std::chrono::steady_clock`, каждый замер 5 раз, брать медиану, результат вычисления печатать, чтобы оптимизатор не выкинул код.
1. Сумма 10⁷ `int`: `std::vector` против `std::list`.
2. 10⁶ ключей, затем 10⁶ случайных поисков: `std::map`, `std::unordered_map`, отсортированный `std::vector` + `std::lower_bound`. Отдельно замерить время вставки.

Результаты оформить таблицей в `notes.md` и объяснить через устройство контейнеров в памяти (узлы в куче против непрерывного массива, кэш).

### 4.4. Инвалидация итераторов
```cpp
std::vector<int> v = {1, 2, 3};
int& first = v[0];
auto it = v.begin();
v.push_back(4);
std::cout << first << ' ' << *it << '\n';
```
1. Напечатать `capacity()` до и после `push_back`. Получить отчёт ASan heap-use-after-free и объяснить.
2. Добавить `v.reserve(10)` в начало. Почему падения нет и почему код всё равно плохой?
3. Написать **неправильное** удаление чётных в цикле: `for (auto it = v.begin(); it != v.end(); ++it) if (*it % 2 == 0) v.erase(it);` и поймать ошибку ASan. Исправить тремя способами: `it = v.erase(it)`, erase-remove идиома, `std::erase_if` (C++20).

### 4.5. Алгоритмы без циклов
Каждая функция — без `for`/`while`, только `<algorithm>`, `<numeric>` и лямбды. Не меньше 2 `assert` на функцию.
1. Отсортировать `std::vector<Person>` (`{std::string name; int age;}`) по возрасту, при равенстве — по имени.
2. То же через `std::stable_sort` только по возрасту — показать, что сохраняется исходный порядок равных.
3. Посчитать элементы больше порога (`count_if`).
4. Сумма квадратов (`transform_reduce` или `accumulate`).
5. Удалить все отрицательные (`std::erase_if`).
6. Оставить уникальные значения (`sort` + `unique` + `erase`).
7. k наибольших элементов в порядке убывания (`partial_sort`).
8. Медиана за O(n) в среднем (`nth_element`).
9. Чётные в начало (`partition`), затем с сохранением порядка (`stable_partition`).
10. Все ли элементы положительны (`all_of`).
11. Первый элемент больше x в отсортированном массиве (`upper_bound`).
12. Префиксные суммы (`partial_sum`).
13. Циклический сдвиг влево на k (`rotate`).
14. Слить два отсортированных вектора (`merge`).
15. Минимум и максимум за один проход (`minmax_element`).

**Вопросы § 4:** Почему реализация шаблона обычно лежит в заголовке? Какова сложность основных операций `vector`, `list`, `map`, `unordered_map`? Что означает амортизированное O(1) у `push_back`? Какие операции инвалидируют итераторы `vector`?

---

## § 5. Полиморфизм, производительность, потоки (месяц 5)

### 5.1. Виртуальные функции
```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};
// Circle(r), Rect(w, h), Triangle(a, b, c) — площадь треугольника по формуле Герона
```
1. `std::vector<std::unique_ptr<Shape>>` из 6 фигур: напечатать имена и суммарную площадь (тесты на площади).
2. Сравнить `sizeof` класса с виртуальными функциями и такого же класса без них. Откуда взялись 8 байт?
3. Убрать `virtual` у деструктора `Shape`, добавить в `Circle` поле `Tracker`, удалить `Circle` через `Shape*`. Показать, что `alive != 0`, и объяснить.
4. В godbolt найти `vtable for Circle` и косвенный вызов через vtable (`call [rax+N]`) в функции, вызывающей `shape->area()`.

### 5.2. Кэш и раскладка данных
Release, правила замера из 4.3.
1. Матрица `N×N` из `double` в одном `std::vector` (row-major). Сумма обходом по строкам и по столбцам для N = 512, 1024, 2048, 4096. Таблица времён и отношение «столбцы / строки».
2. AoS против SoA: `struct Particle { double x, y, z, vx, vy, vz, mass; int id; };`, 10⁶ частиц, 50 раз посчитать сумму `x`. Сравнить с вариантом, где `x` лежит в отдельном `std::vector<double>`. Сколько полезных байт из каждой 64-байтной кэш-линии используется в каждом варианте?
3. Выравнивание: для `struct A { char c; double d; char e; };` **предсказать**, затем напечатать `sizeof`, `alignof` и `offsetof` каждого поля. Переставить поля так, чтобы `sizeof` стал минимальным.

### 5.3. Профилирование
1. Взять бенчмарк `HashMap` из 4.2 (10⁶ вставок и 10⁶ поисков) и собрать с `-O2 -g`.
2. `perf record -g ./bench` и `perf report` (если `perf` недоступен — `valgrind --tool=callgrind` + `kcachegrind`). Найти функцию с наибольшей долей времени.
3. Сделать одну оптимизацию (например, `reserve`, хранение хеша в узле или переход на открытую адресацию) и получить ускорение не меньше ×1.5.
4. В `notes.md` — вывод профилировщика до и после и замеры по правилам из 4.3.

### 5.4. Гонка данных
4 потока, каждый увеличивает общий счётчик 10⁶ раз. Ожидаемый результат — 4·10⁶.

| Вариант | Реализация |
|---|---|
| a | Обычный `long counter` |
| b | `std::mutex` + `std::lock_guard` на каждый инкремент |
| c | `std::atomic<long>` + `fetch_add` |
| d | Локальный счётчик в каждом потоке, сложение в конце |
| e | Массив `long[4]`, каждый поток пишет в свой элемент; затем то же с `struct alignas(64) Padded { long v; };` |

1. Для (a) показать неверный результат и получить отчёт TSan (отдельная сборка `-fsanitize=thread`, с ASan несовместим).
2. Замерить время всех вариантов в Release без санитайзеров. Таблица и объяснение, в том числе разница внутри (e): что такое false sharing.

### 5.5. Пул потоков
```cpp
class ThreadPool {
public:
    explicit ThreadPool(std::size_t n_threads);
    ~ThreadPool();                              // выполнить оставшиеся задачи, затем join всех потоков
    ThreadPool(const ThreadPool&) = delete;  ThreadPool& operator=(const ThreadPool&) = delete;
    void submit(std::function<void()> task);
    void wait_idle();                           // ждать, пока очередь пуста и ни одна задача не выполняется
};
```
Реализация: `std::queue`, `std::mutex`, `std::condition_variable`, флаг остановки. Активное ожидание (busy-wait) запрещено.
Тесты:
- 10⁴ задач увеличивают `std::atomic<int>`; после `wait_idle()` значение равно 10⁴;
- сумма квадратов 10⁷ чисел, разбитая на 100 задач, совпадает с последовательным подсчётом;
- деструктор при непустой очереди выполняет все задачи;
- TSan чист;
- в простое пул не грузит CPU (проверить в `top`).

Бонус: `template <class F> auto submit(F f) -> std::future<decltype(f())>` через `std::packaged_task`.

**Вопросы § 5:** Почему деструктор базового класса должен быть виртуальным? Какие бывают гарантии безопасности исключений (basic / strong / nothrow)? Что такое false sharing? Почему `condition_variable::wait` нужно вызывать с предикатом?

---

## § 6. Итоговый проект: библиотека `labsignal`

Учебная C++ библиотека обработки одномерных сигналов с привязкой к Python. Пишется с нуля. NumPy служит только эталоном для проверки результатов и для сравнения скорости.

### 1. Структура репозитория

```
labsignal/
├── CMakeLists.txt
├── include/labsignal/labsignal.h   # публичный API
├── src/
│   ├── validate.h / validate.cpp   # внутренние проверки входа
│   ├── smoothing.cpp               # moving_average, median_filter
│   ├── integrate.cpp               # trapz
│   └── fit.cpp                     # LinFit, linear_fit
├── tests/
│   ├── test_smoothing.cpp
│   ├── test_integrate.cpp
│   ├── test_fit.cpp
│   └── test_python.py
├── python/bindings.cpp             # pybind11
├── scripts/compare.py              # сверка с NumPy и замеры
├── .github/workflows/ci.yml
└── README.md
```

Весь код лежит в `namespace labsignal`, внутренние функции — в `namespace labsignal::detail`. Глобального состояния нет: каждая функция зависит только от своих аргументов.

### 2. Общие правила для всех функций

- Вход передаётся как `std::span<const double>`: данные не копируются и не меняются. Нужно объяснить, чем `span` лучше `const std::vector<double>&`.
- При некорректных аргументах бросается `std::invalid_argument` с сообщением вида `"<имя_функции>: <причина>"`, например `"moving_average: k must be in [1, x.size()]"`.
- Если во входе есть `NaN` или `±inf`, бросается `std::invalid_argument`.
- Сырых `new`/`delete` в библиотеке нет. Вся память выделяется через `std::vector`.

### 3. Внутренний модуль `detail` (validate.h/.cpp)

```cpp
namespace labsignal::detail {
void require_non_empty(std::span<const double> x, const char* func);
void require_finite(std::span<const double> x, const char* func);
void require_same_size(std::span<const double> a, std::span<const double> b, const char* func);
}
```

Каждая функция бросает `std::invalid_argument` с именем вызывающей функции в сообщении. Этот модуль не входит в публичный заголовок. Тесты на него проходят косвенно, через тесты публичных функций.

### 4. Функция `moving_average`

```cpp
std::vector<double> moving_average(std::span<const double> x, std::size_t k);
```

**Семантика** (режим valid): возвращает вектор длины `n − k + 1`, где `y[i] = (x[i] + … + x[i+k−1]) / k`.

**Ошибки:** пустой `x`; `k == 0`; `k > n`; нечисловые значения во входе.

**Граничные случаи:** при `k == 1` возвращается копия `x`, при `k == n` — один элемент, среднее всего массива.

**Сложность:** O(n) по времени через скользящую сумму: прибавить новый элемент, вычесть ушедший. Наивный вариант O(n·k) не принимается.

**Тонкость, которую нужно объяснить:** скользящая сумма накапливает ошибку округления. Требование: для `n = 10⁷` случайных значений из [0, 1) максимальная относительная ошибка не больше 1e-9 по сравнению с наивным пересчётом каждого окна. Если требование не выполняется, нужно исправить: например, пересчитывать сумму окна заново каждые `k` шагов или использовать суммирование Кэхэна.

**Эталон NumPy:** `np.convolve(x, np.ones(k) / k, mode="valid")`.

**Тесты (GoogleTest):**
1. `{1, 2, 3, 4, 5}` при `k = 2` даёт `{1.5, 2.5, 3.5, 4.5}`.
2. При `k = 1` результат совпадает с входом.
3. При `k = n` результат — один элемент, равный среднему.
4. Константный сигнал даёт тот же константный сигнал.
5. `k = 0`, `k = n + 1` и пустой вход бросают `std::invalid_argument`.
6. Вход с `NaN` бросает `std::invalid_argument`.
7. Точность на `n = 10⁷`: требование выше.

### 5. Функция `median_filter`

```cpp
std::vector<double> median_filter(std::span<const double> x, std::size_t k);
```

**Семантика** (режим valid): длина результата `n − k + 1`, `y[i]` — медиана окна `x[i..i+k−1]`. Здесь `k` должно быть нечётным, тогда медиана — средний элемент отсортированного окна.

**Ошибки:** пустой `x`; `k == 0`; чётное `k`; `k > n`; нечисловые значения.

**Реализация** — две версии в одном файле:
1. `median_filter_naive`: для каждого окна копия во временный буфер и `std::nth_element`. Сложность O(n·k). Буфер выделяется **один раз** до цикла, а не на каждой итерации.
2. `median_filter` (публичная): O(n log k), окно хранится в двух `std::multiset` (нижняя и верхняя половины) или в отсортированном буфере с бинарным поиском для вставки и удаления.

`median_filter_naive` объявляется в `detail` и используется только как оракул в тестах.

**Эталон NumPy:** `np.median(np.lib.stride_tricks.sliding_window_view(x, k), axis=1)`.

**Тесты:**
1. `{1, 9, 2, 8, 3}` при `k = 3` даёт `{2, 8, 3}`.
2. Одиночный выброс в константном сигнале полностью убирается при `k = 3`.
3. При `k = 1` результат совпадает с входом.
4. Чётное `k`, `k = 0`, `k > n` и пустой вход бросают исключение.
5. 1000 случайных массивов (длина 1–200, случайное нечётное `k`, фиксированный seed): быстрая версия совпадает с `median_filter_naive`.
6. Повторяющиеся значения в окне (`{5, 5, 5, 1, 5}`) обрабатываются корректно: это проверка удаления из `multiset` ровно одного элемента.

**Вопрос:** почему `multiset::erase(value)` здесь — ошибка и нужен `erase(find(value))`?

### 6. Функция `trapz` (две перегрузки)

```cpp
double trapz(std::span<const double> y, double dx);                  // равномерная сетка
double trapz(std::span<const double> y, std::span<const double> x);  // произвольная сетка
```

**Семантика:**
- `trapz(y, dx)` вычисляет `dx · (y₀/2 + y₁ + … + y_{n−2} + y_{n−1}/2)`.
- `trapz(y, x)` вычисляет `Σ (x[i+1] − x[i]) · (y[i] + y[i+1]) / 2`.

**Ошибки:**
- для обеих перегрузок: `n < 2`, нечисловые значения во входе;
- для `trapz(y, dx)`: `dx ≤ 0`, нечисловой `dx`;
- для `trapz(y, x)`: размеры `y` и `x` не совпадают, `x` не строго возрастает.

**Эталон NumPy:** `np.trapezoid(y, dx=dx)` и `np.trapezoid(y, x=x)`. Функция есть в NumPy ≥ 2.0, в старых версиях она называется `np.trapz`.

**Тесты:**
1. Линейная функция интегрируется точно: интеграл `y = 2x + 1` на [0, 1] равен 2.
2. Интеграл `sin` на [0, π] по 1001 точке: `|result − 2| < 3e-6`. Теоретическая оценка ошибки метода трапеций ≈ 2.6e-6, и её нужно вывести.
3. Перегрузка с `x` на равномерной сетке совпадает с перегрузкой с `dx`.
4. Неравномерная сетка: результат совпадает с ручным расчётом на 4 точках.
5. `n = 1`, `dx = 0`, `dx = −1`, невозрастающий `x` и разные размеры бросают исключение.

### 7. Структура `LinFit` и функция `linear_fit`

```cpp
struct LinFit {
    double slope;      // a в y = a·x + b
    double intercept;  // b
    double r2;         // коэффициент детерминации
};
std::ostream& operator<<(std::ostream& os, const LinFit& f);  // "LinFit(slope=..., intercept=..., r2=...)"

LinFit linear_fit(std::span<const double> x, std::span<const double> y);
```

**`LinFit`** — агрегат без конструкторов и без инвариантов, все поля публичные. Нужно объяснить, почему здесь `struct`, а не класс с геттерами.

**Семантика `linear_fit`** — метод наименьших квадратов, **двухпроходный** алгоритм:
1. Первый проход: средние `x̄` и `ȳ`.
2. Второй проход: `Sxx = Σ(x − x̄)²`, `Sxy = Σ(x − x̄)(y − ȳ)`, `Syy = Σ(y − ȳ)²`.
3. `slope = Sxy / Sxx`, `intercept = ȳ − slope · x̄`.
4. `r2 = Sxy² / (Sxx · Syy)`. Если `Syy == 0` (константный `y`), по определению `r2 = 1.0`.

**Ошибки:** разные размеры; `n < 2`; все `x` одинаковы (`Sxx == 0`); нечисловые значения.

**Обязательный эксперимент** (тест и `notes.md`): реализовать в тесте однопроходную формулу через `Σx²` и `(Σx)²` и сравнить её с основной на данных `x = 1e9 + i`, `y = 3x + 2`, `i = 0..999`. Однопроходная формула теряет точность из-за вычитания близких больших чисел, и нужно объяснить почему.

**Эталон NumPy:** `np.polyfit(x, y, 1)` возвращает `[slope, intercept]`; `r2` сверяется с `np.corrcoef(x, y)[0, 1] ** 2`.

**Тесты:**
1. Точные данные `y = 3x + 2` дают `slope = 3`, `intercept = 2`, `r2 = 1`.
2. Зашумлённые данные (seed фиксирован, σ = 0.1): `slope` в пределах ±0.05 от истинного, `0.99 < r2 < 1`.
3. Константный `y` даёт `slope = 0`, `r2 = 1`.
4. Эксперимент с `x = 1e9 + i` из требований выше.
5. Одинаковые `x`, `n = 1` и разные размеры бросают исключение.
6. `operator<<` выводит строку ожидаемого формата.

### 8. Сборка (CMake)

- `cmake_minimum_required(VERSION 3.20)`, C++20.
- Опции:
  - `LABSIGNAL_BUILD_TESTS` (ON);
  - `LABSIGNAL_BUILD_PYTHON` (ON);
  - `LABSIGNAL_SANITIZE` (OFF): включает ASan и UBSan для библиотеки и C++-тестов.
- Цели:
  - `labsignal`: статическая библиотека; `include/` подключается как PUBLIC; флаги `-Wall -Wextra -Wpedantic -Werror` — PRIVATE; `POSITION_INDEPENDENT_CODE ON`, потому что библиотека линкуется в Python-модуль (объяснить, что такое PIC);
  - `labsignal_tests`: GoogleTest, регистрация через `gtest_discover_tests`, запуск через `ctest`;
  - Python-модуль `labsignal` через `pybind11_add_module`.
- GoogleTest и pybind11 подключаются через `FetchContent` с **зафиксированным тегом релиза**, а не веткой `main`. Почему — объяснить.
- С `LABSIGNAL_SANITIZE=ON` Python-модуль не собирается: загрузка ASan-библиотеки в обычный интерпретатор Python требует отдельной настройки (`LD_PRELOAD`), и эта настройка в проект не входит.

### 9. Python-привязки (python/bindings.cpp)

**Приём массивов:** `py::array_t<double, py::array::c_style | py::array::forcecast>`.
- Если массив не одномерный (`ndim != 1`), бросается `ValueError`.
- Для непрерывного `float64` копирования нет: `span` строится прямо из `arr.data()` и `arr.size()`.
- Массивы других типов (`int`, срезы с шагом) pybind11 конвертирует с копированием. Это допустимо, но поведение должно быть описано в docstring.

**Возврат массивов без копирования:** результат `std::vector<double>` переносится в кучу, и его владельцем становится `py::capsule`:

```cpp
auto* v = new std::vector<double>(std::move(result));
py::capsule owner(v, [](void* p) { delete static_cast<std::vector<double>*>(p); });
return py::array_t<double>(v->size(), v->data(), owner);
```

Нужно объяснить, кто и когда освобождает эту память.

**GIL:** вычисление выполняется внутри блока с `py::gil_scoped_release`. Создание выходного `py::array` идёт **после** блока, когда GIL снова захвачен. Нужно объяснить, почему нельзя трогать Python-объекты без GIL.

**Исключения:** `std::invalid_argument` pybind11 автоматически превращает в `ValueError`, и это нужно проверить тестом.

**`LinFit`** экспортируется как класс `py::class_<LinFit>` с полями только для чтения `slope`, `intercept`, `r2` и методом `__repr__`, который совпадает с `operator<<`.

**Docstring:** у каждой функции описаны аргументы, возвращаемое значение и исключения.

**Тесты (`tests/test_python.py`, pytest):**
1. Каждая функция совпадает со своим эталоном NumPy по `np.allclose` на случайных данных.
2. Некорректные аргументы бросают `ValueError` (`pytest.raises`).
3. Двумерный массив бросает `ValueError`.
4. `int`-массив и срез `x[::2]` дают правильный результат.
5. Возвращённый массив не ломается после удаления входного (`del x; gc.collect()`): это проверка владения памятью.

### 10. Скрипт сравнения (scripts/compare.py)

- Данные: `np.random.default_rng(42)`, размеры 10³, 10⁶, 10⁷.
- Для каждой функции и каждого размера скрипт:
  1. проверяет совпадение с эталоном по `np.allclose` и останавливается с ошибкой при расхождении;
  2. замеряет время своей реализации и эталона: `time.perf_counter`, 5 повторов, медиана;
  3. выводит таблицу в Markdown: функция, n, время `labsignal`, время NumPy, отношение.
- `median_filter` при n = 10⁷ сравнивается с `scipy.signal.medfilt` вместо `sliding_window_view`, которому для такого n не хватит памяти. Учтите, что `medfilt` возвращает массив той же длины, так что для сверки берётся его центральная часть.
- Результаты прогона сохраняются в `scripts/results.md`. В `notes.md` нужно объяснить, где NumPy быстрее, где медленнее и почему.

### 11. CI (.github/workflows/ci.yml)

Два задания на `ubuntu-latest`, запуск на каждый push и pull request:
1. `sanitize`: конфигурация с `LABSIGNAL_SANITIZE=ON` и `LABSIGNAL_BUILD_PYTHON=OFF`, сборка, `ctest --output-on-failure`.
2. `release-python`: Release-сборка, установка `numpy`, `scipy`, `pytest`, сборка модуля, `ctest`, затем `pytest tests/test_python.py`.

Задача сдана, только если оба задания зелёные.

### 12. README проекта

Содержит:
- зависимости;
- команды сборки для обоих режимов;
- как запустить C++- и Python-тесты;
- пример использования из Python (5–10 строк);
- таблицу из `results.md`;
- ограничения: только `float64`, только одномерные массивы, режим valid.

Объём — не больше одного экрана.

### 13. План на месяц

| Неделя | Результат |
|---|---|
| 1 | Структура, CMake, `detail`, `moving_average`, `trapz` с тестами |
| 2 | `median_filter` (обе версии), `LinFit`/`linear_fit`, эксперимент с точностью |
| 3 | pybind11-привязки, capsule, GIL, `test_python.py` |
| 4 | CI, `compare.py`, README |
