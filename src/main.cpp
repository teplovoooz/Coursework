#include <iostream>
#include <windows.h>
#include "matrix.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "=== Пример использования Matrix ===\n\n";

    // Пример 1: Базовые операции
    std::cout << "1. Создание матриц по модулю 26\n";
    Matrix A(2, 2, 26, {7, 4, 11, 14});
    Matrix B(2, 2, 26, {3, 2, 5, 7});
    Matrix C(2, 2, 26, {1, 4, 2, 9});

    std::cout << "A =\n" << A;
    std::cout << "B =\n" << B;
    std::cout << "C =\n" << C;

    // Пример 2: Умножение
    std::cout << "\n2. Умножение матриц\n";
    Matrix BA = B * A;
    std::cout << "B * A =\n" << BA;

    Matrix BAC = BA * C;
    std::cout << "(B * A) * C =\n" << BAC;

    // Пример 3: Обратная матрица
    std::cout << "\n3. Обратные матрицы\n";
    Matrix B_inv = B.inverse();
    std::cout << "B⁻¹ =\n" << B_inv;

    Matrix C_inv = C.inverse();
    std::cout << "C⁻¹ =\n" << C_inv;

    // Пример 4: Проверка протокола
    std::cout << "\n4. Проверка трёхпроходного протокола\n";
    Matrix X1 = B * A;
    Matrix X2 = X1 * C;
    Matrix X3 = B_inv * X2;
    Matrix result = X3 * C_inv;

    std::cout << "X1 = B * A =\n" << X1;
    std::cout << "X2 = X1 * C =\n" << X2;
    std::cout << "X3 = B⁻¹ * X2 =\n" << X3;
    std::cout << "Результат = X3 * C⁻¹ =\n" << result;
    std::cout << "Результат == A? " << (result == A ? "ДА" : "НЕТ") << "\n";

    // Пример 5: Единичная матрица
    std::cout << "\n5. Проверка A * E = A\n";
    Matrix E = Matrix::identity(2, 26);
    Matrix AE = A * E;
    std::cout << "A * E =\n" << AE;
    std::cout << "A * E == A? " << (AE == A ? "ДА" : "НЕТ") << "\n";

    return 0;
}
