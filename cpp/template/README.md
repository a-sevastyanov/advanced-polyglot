# Заготовка задачи

```bash
cmake -B build  # Debug + ASan/UBSan по умолчанию
cmake --build build
./build/main

cmake -B build-rel -DCMAKE_BUILD_TYPE=Release  # для бенчмарков
gdb ./build/main                               # отладка
valgrind --leak-check=full ./build-rel/main    # valgrind — только без ASan
```
