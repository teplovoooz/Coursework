#pragma once

#include <vector>
#include <iostream>
#include <random>
#include <stdexcept>
#include <algorithm>

class Matrix {
public:
    using elem_t = long long;

    // Конструкторы
    Matrix(size_t rows, size_t cols, elem_t modulus);
    Matrix(size_t rows, size_t cols, elem_t modulus, const std::vector<elem_t>& values);
    static Matrix identity(size_t size, elem_t modulus);
    static Matrix random(size_t rows, size_t cols, elem_t modulus, std::mt19937_64& rng);
    static Matrix randomInvertible(size_t size, elem_t modulus, std::mt19937_64& rng, int max_attempts = 100);

    // Размеры
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    elem_t modulus() const { return modulus_; }

    // Доступ к элементам
    elem_t& at(size_t i, size_t j);
    elem_t at(size_t i, size_t j) const;
    elem_t& operator()(size_t i, size_t j);
    elem_t operator()(size_t i, size_t j) const;

    // Арифметические операции
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix transpose() const;

    // Определитель и обратная матрица
    elem_t determinant() const;
    bool isInvertible() const;
    Matrix inverse() const;

    // Сравнение
    bool operator==(const Matrix& other) const;

    // Вывод
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat);

private:
    size_t rows_, cols_;
    elem_t modulus_;
    std::vector<elem_t> data_;

    // Нормализация элемента к диапазону [0, modulus_)
    elem_t normalize(elem_t x) const;

    // Вспомогательные функции для вычисления определителя
    elem_t determinant2x2(const Matrix& mat) const;
    elem_t determinant3x3(const Matrix& mat) const;
    elem_t determinantGeneral(Matrix mat) const;
};

// Утилиты
long long gcd(long long a, long long b);
long long extendedGcd(long long a, long long b, long long& x, long long& y);
long long modInverse(long long a, long long mod);
