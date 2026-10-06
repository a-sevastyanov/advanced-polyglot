# advanced-polyglot: полиглот языков программрования на продвинутом уровне

Так получилось, что ко мне на работу, в отдел устроился разработчик 18 лет — эникейщик, CTF'щик, который здорово шарит в сетях, реверс-инжиниринге и нахождении уязвимостей с помощью своего необъемлемого любопытства и LLM'ок :\) Но он совершенно не умеет программировать, о чём и сам говорит, поэтому я на пару с нейронкой сделал ему этот проект для последовательного изучения и (надеюсь) последующего понимания. 

Но мне мало просто сделать, интересно и самому пройти этот самодельный курс параллельно обучению в магистратуре :\) 

Структура репозитория:

```bash
advanced-polyglot/
├── cpp/
│   ├── TASKS.md
│   └── template/
│       ├── CMakeLists.txt
│       ├── main.cpp
│       └── README.md
├── python/
│   ├── 01_basics.ipynb
│   ├── 02_functions_io.ipynb
│   ├── 03_numpy.ipynb
│   └── 04_matplotlib.ipynb
└── README.md
```

Вероятно, при изучении мною других языков программирования под них будут появляться отдельные директории, но изначальная структура для изучения Python и C++ выглядела именно так, как показано выше.

Далее по тексту составленного курса.

# Онбординг: Python и C++

> Понимание каждой строчки кода — не бывает лишним.

## Правила

- **Старайся не использовать нейронки во время обучения.** ИИ — только как справочник («объясни, что делает `std::move`» и т.п.), причём ответ желательно сверять с документацией.
- **Проанализируй, потом запускай.** Перед запуском проанализируй код: проверь синтаксис и подумай над ожидаемым поведением программы и только потом запускай.
- **C++ компилируется** с `-Wall -Wextra -Werror -fsanitize=address,undefined`.
- **Еженедельное ревью.** Попробуем вместе проверять, обсуждать написанное: будем смотреть, что происходит «под капотом» (в C++ *где лежат данные* (стек / куча / статическая память) и *кто их освобождает*).

По желанию:

- **Будет плюсом, если заведёшь репозиторий в GitHub.** Удобнее следить за прогрессом: маленькие коммиты с осмысленными сообщениями, своя ветка на каждую неделю. Заодно научишься пользоваться **git'ом**.

## Структура

```
python/   4 Jupyter-блокнота: теор. минимум + задачи с автопроверкой
cpp/      TASKS.md — практика по месяцам; template/ — заготовка проекта (CMake + санитайзеры)
```

---

## Часть 1. Python

| Неделя | Тема | Теория | Практика |
|---|---|---|---|
| 1 | Синтаксис, типы, коллекции, функции. **Ссылки и изменяемость** | [Stepik: Программирование на Python](https://stepik.org/course/67)  и/или [Поколение Python](https://stepik.org/course/58852) | `01_basics.ipynb` + визуализация кода в [Python Tutor](https://pythontutor.com/) |
| 2 | Исключения, файлы, модули, классы, генераторы, venv, pytest | [Официальный tutorial](https://docs.python.org/3/tutorial/) (главы 4–9) и [Stepik: Программирование на Python](https://stepik.org/course/67) (модуль 3) | `02_functions_io.ipynb` |
| 3 | NumPy: dtype, view/copy, broadcasting, векторизация. Matplotlib: Figure/Axes | [NumPy for beginners](https://numpy.org/doc/stable/user/absolute_beginners.html), [Matplotlib Quick start](https://matplotlib.org/stable/users/explain/quick_start.html) | `03_numpy.ipynb`, `04_matplotlib.ipynb`, [numpy-100](https://github.com/rougier/numpy-100) |
| 4 | Мини-проект | [MIT Missing Semester](https://missing.csail.mit.edu/) (shell, git, отладка) | Скрипт: CSV лаборатории → очистка → статистика → графики в PDF. CLI через `argparse`, тесты `pytest` |

Задачки для набивки руки: [Exercism Python](https://exercism.org/tracks/python).
Глубже про NumPy: [From Python to NumPy](https://www.labri.fr/perso/nrougier/from-python-to-numpy/).

---

## Часть 2. C++

**База:** [learncpp.com](https://www.learncpp.com/) читается целиком, по порядку, с упражнениями, Тимофей Хирьянов (МФТИ) — [Курс молодого бойца МФТИ (Яызк Си)](https://youtube.com/playlist?list=PLRDzFCPr95fLjzcv6nNdjMu_9RcZgIM9U&si=MLOYTucAgd248esR), [Основы С++ — хендбук от Яндекса](https://contest.yandex.ru/tracks/cpp/).
**Лекции:** Константин Владимиров (МФТИ) — [*Базовый курс C++, MIPT, 2021-2022*](https://youtube.com/playlist?list=PL3BR09unfgciJ1_K_E914nohpiOiHnpsK&si=57niQ13vLX9WQCtD), Илья Мещерин (МФТИ) — [*Лекции C++ продвинутый поток ФПМИ 2023-24*](https://youtube.com/playlist?list=PLmSYEYYGhnBviRYhIDty-CSTDS16a3whl&si=fEVK5NQ5HlifnPg6); CppCon — *Back to Basics* (плейлисты по годам).
**Справочник:** [cppreference](https://en.cppreference.com/).
**Смотреть, во что компилируется код:** [Compiler Explorer](https://godbolt.org/), [C++ Insights](https://cppinsights.io/).

| Месяц | Тема | Теория | Практика |
|---|---|---|---|
| 1 | Как код становится программой: препроцессор → компиляция → линковка. Типы, функции, ссылки, `const`. CMake, gdb | [learncpp.com](https://www.learncpp.com/) (главы 0–12); [Stepik: Введение в программирование (C++)](https://stepik.org/course/363) | Все задачи в [Stepik: Введение в программирование (C++)](https://stepik.org/course/363), `TASKS.md` (параграф 1) |
| 2 | **Память:** стек и стековый кадр, куча, указатели, массивы, арифметика указателей, `new/delete`, UB. Valgrind, ASan | [learncpp.com](https://www.learncpp.com/) (главы 12, 17–19); [CS50x](https://cs50.harvard.edu/x/) (Week 4 Memory) | `TASKS.md` (параграф 2): свой `strlen`/`strcpy`, связный список, разбор ассемблера вызова функции |
| 3 | Классы, конструкторы/деструкторы, **RAII**, правило 0/3/5, lvalue/rvalue, move-семантика, умные указатели | [learncpp.com](https://www.learncpp.com/) (главы 13–15, 21–22); [Stepik: Программирование на языке C++](https://stepik.org/course/7) | `TASKS.md` (параграф 3): свои `String` и `UniquePtr` |
| 4 | Шаблоны, STL: контейнеры, итераторы, алгоритмы, лямбды. Устройство `vector`/`list`/`map`/`unordered_map` в памяти, инвалидация итераторов | [learncpp.com](https://www.learncpp.com/) (главы 11, 16, 20, 26); [Основы С++ — хендбук от Яндекса](https://contest.yandex.ru/tracks/cpp/) (глава 3) | `TASKS.md` (параграф 4): свой `Vector<T>` с placement new, хеш-таблица |
| 5 | Наследование, `virtual` и vtable, исключения и exception safety. Кэш, выравнивание, профилирование. Потоки, `mutex`, `atomic`, гонки (TSan) | [learncpp.com](https://www.learncpp.com/) (главы 24–25, 27); [Algorithms for Modern Hardware](https://en.algorithmica.org/hpc/) (глава 9) | `TASKS.md` (параграф 5): бенчмарки кэша, thread pool |
| 6 | Итоговый проект: C++-библиотека + модульные тесты (GoogleTest) + привязка к Python через pybind11 | [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html), документация [pybind11](https://pybind11.readthedocs.io/en/stable/) | `TASKS.md` (параграф 6), свой проект |

**Книги (справочно):** Страуструп — «Программирование: принципы и практика с использованием C++»; Мейерс — «Эффективный и современный C++»; Стивен Прата — «Язык программирования C++. Лекции и упражнения».
