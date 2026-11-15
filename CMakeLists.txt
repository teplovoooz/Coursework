cmake_minimum_required(VERSION 3.10)
project(MatrixProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Добавляем include директорию
include_directories(${CMAKE_SOURCE_DIR}/include)

# Исходные файлы библиотеки
set(MATRIX_SOURCES
    src/matrix.cpp
)

# Создаём статическую библиотеку
add_library(matrix_lib ${MATRIX_SOURCES})

# Основная программа
add_executable(matrix_demo src/main.cpp)
target_link_libraries(matrix_demo matrix_lib)

# Тесты (Catch2)
find_package(Catch2 3 QUIET)

if(Catch2_FOUND)
    add_executable(test_matrix tests/test_matrix.cpp)
    target_link_libraries(test_matrix matrix_lib Catch2::Catch2WithMain)
    
    enable_testing()
    add_test(NAME MatrixTests COMMAND test_matrix)
else()
    message(STATUS "Catch2 не найден. Используй: vcpkg install catch2:x64-windows или установи вручную")
endif()
