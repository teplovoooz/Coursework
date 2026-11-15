#include "matrix.h"
#include <cassert>
#include <iostream>

void assert_equal(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "✓ " << test_name << "\n";
    } else {
        std::cout << "✗ FAILED: " << test_name << "\n";
        throw std::runtime_error("Test failed: " + test_name);
    }
}

int main() {
    std::cout << "=== Тесты Matrix ===\n\n";

    try {
        // Тест 1: Умножение матриц
        {
            Matrix A(2, 2, 26, {1, 2, 3, 4});
            Matrix B(2, 2, 26, {5, 6, 7, 8});
            Matrix C = A * B;
            
            assert_equal(C(0, 0) == 19, "Matrix multiplication (0,0)");
            assert_equal(C(0, 1) == 22, "Matrix multiplication (0,1)");
            assert_equal(C(1, 0) == 17, "Matrix multiplication (1,0)");
            assert_equal(C(1, 1) == 24, "Matrix multiplication (1,1)");
        }

        // Тест 2: Единичная матрица
        {
            Matrix A(3, 3, 26, {1, 2, 3, 4, 5, 6, 7, 8, 9});
            Matrix E = Matrix::identity(3, 26);
            Matrix AE = A * E;
            Matrix EA = E * A;
            
            assert_equal(AE == A, "Identity property: A * E = A");
            assert_equal(EA == A, "Identity property: E * A = A");
        }

        // Тест 3: Определитель 2×2
        {
            Matrix A(2, 2, 26, {1, 2, 3, 4});
            assert_equal(A.determinant() == 24, "Determinant 2x2");
        }

        // Тест 4: Определитель 3×3
        {
            Matrix A(3, 3, 26, {1, 2, 3, 0, 1, 4, 5, 6, 0});
            assert_equal(A.determinant() == 1, "Determinant 3x3");
        }

        // Тест 5: Проверка обратимости
        {
            Matrix A(2, 2, 26, {3, 2, 5, 7});
            assert_equal(A.isInvertible(), "Matrix is invertible");
            
            Matrix B(2, 2, 26, {2, 4, 3, 6});
            assert_equal(!B.isInvertible(), "Matrix is not invertible");
        }

        // Тест 6: Обратная матрица
        {
            Matrix A(2, 2, 26, {3, 2, 5, 7});
            Matrix A_inv = A.inverse();
            Matrix E = Matrix::identity(2, 26);
            Matrix result = A * A_inv;
            
            assert_equal(result == E, "Inverse property: A * A^-1 = E");
        }

        // Тест 7: Ассоциативность
        {
            std::mt19937_64 rng(123);
            Matrix A = Matrix::random(2, 2, 26, rng);
            Matrix B = Matrix::random(2, 2, 26, rng);
            Matrix C = Matrix::random(2, 2, 26, rng);
            
            Matrix AC = A * C;
            Matrix BAC1 = B * AC;
            
            Matrix BA = B * A;
            Matrix BAC2 = BA * C;
            
            assert_equal(BAC1 == BAC2, "Associativity: B(AC) = (BA)C");
        }

        // Тест 8: Трёхпроходный протокол (полный цикл)
        {
            Matrix A(2, 2, 26, {7, 4, 11, 14});
            Matrix B(2, 2, 26, {3, 2, 5, 7});
            Matrix C(2, 2, 26, {1, 4, 2, 9});
            
            // X1 = B * A
            Matrix X1 = B * A;
            assert_equal(X1(0, 0) == 17, "Protocol X1 (0,0)");
            assert_equal(X1(0, 1) == 14, "Protocol X1 (0,1)");
            assert_equal(X1(1, 0) == 8, "Protocol X1 (1,0)");
            assert_equal(X1(1, 1) == 14, "Protocol X1 (1,1)");
            
            // X2 = X1 * C
            Matrix X2 = X1 * C;
            assert_equal(X2(0, 0) == 19, "Protocol X2 (0,0)");
            assert_equal(X2(0, 1) == 12, "Protocol X2 (0,1)");
            assert_equal(X2(1, 0) == 10, "Protocol X2 (1,0)");
            assert_equal(X2(1, 1) == 2, "Protocol X2 (1,1)");
            
            // B_inv
            Matrix B_inv = B.inverse();
            assert_equal(B_inv(0, 0) == 3, "Protocol B_inv (0,0)");
            assert_equal(B_inv(0, 1) == 14, "Protocol B_inv (0,1)");
            assert_equal(B_inv(1, 0) == 9, "Protocol B_inv (1,0)");
            assert_equal(B_inv(1, 1) == 5, "Protocol B_inv (1,1)");
            
            // X3 = B_inv * X2
            Matrix X3 = B_inv * X2;
            assert_equal(X3(0, 0) == 15, "Protocol X3 (0,0)");
            assert_equal(X3(0, 1) == 12, "Protocol X3 (0,1)");
            assert_equal(X3(1, 0) == 13, "Protocol X3 (1,0)");
            assert_equal(X3(1, 1) == 14, "Protocol X3 (1,1)");
            
            // C_inv
            Matrix C_inv = C.inverse();
            assert_equal(C_inv(0, 0) == 9, "Protocol C_inv (0,0)");
            assert_equal(C_inv(0, 1) == 22, "Protocol C_inv (0,1)");
            assert_equal(C_inv(1, 0) == 24, "Protocol C_inv (1,0)");
            assert_equal(C_inv(1, 1) == 1, "Protocol C_inv (1,1)");
            
            // Результат = X3 * C_inv
            Matrix result = X3 * C_inv;
            assert_equal(result == A, "Protocol result: X3 * C_inv = A");
        }

        // Тест 9: Прямоугольные матрицы
        {
            Matrix A(3, 2, 26, {1, 2, 3, 4, 5, 6});
            Matrix B(2, 2, 26, {7, 8, 9, 10});
            Matrix C = A * B;
            
            assert_equal(C.rows() == 3, "Rectangular multiplication rows");
            assert_equal(C.cols() == 2, "Rectangular multiplication cols");
            assert_equal(C(0, 0) == 25, "Rectangular multiplication (0,0)");
            assert_equal(C(0, 1) == 2, "Rectangular multiplication (0,1)");
        }

        // Тест 10: Сложение и вычитание
        {
            Matrix A(2, 2, 26, {5, 10, 15, 20});
            Matrix B(2, 2, 26, {3, 7, 11, 13});
            
            Matrix C = A + B;
            assert_equal(C(0, 0) == 8, "Addition (0,0)");
            assert_equal(C(0, 1) == 17, "Addition (0,1)");
            
            Matrix D = A - B;
            assert_equal(D(0, 0) == 2, "Subtraction (0,0)");
            assert_equal(D(0, 1) == 3, "Subtraction (0,1)");
        }

        // Тест 11: Транспонирование
        {
            Matrix A(2, 3, 26, {1, 2, 3, 4, 5, 6});
            Matrix AT = A.transpose();
            
            assert_equal(AT.rows() == 3, "Transpose rows");
            assert_equal(AT.cols() == 2, "Transpose cols");
            assert_equal(AT(0, 0) == 1, "Transpose (0,0)");
            assert_equal(AT(1, 0) == 2, "Transpose (1,0)");
            assert_equal(AT(2, 1) == 6, "Transpose (2,1)");
        }

        std::cout << "\n=== Все тесты пройдены! ===\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\nОшибка при выполнении тестов: " << e.what() << "\n";
        return 1;
    }
}
