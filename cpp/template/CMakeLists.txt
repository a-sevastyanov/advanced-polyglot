cmake_minimum_required(VERSION 3.16)
project(task CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(main main.cpp)
target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic -Werror)

# Санитайзеры в Debug: ASan ловит ошибки памяти, UBSan — неопределённое поведение.
# Для задач с потоками замени на -fsanitize=thread (с address несовместим).
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(main PRIVATE -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(main PRIVATE -fsanitize=address,undefined)
endif()
