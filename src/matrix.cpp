#include "matrix.h"
#include <cmath>
#include <iomanip>

// ===== Утилиты =====

long long gcd(long long a, long long b) {
    a = std::abs(a);
    b = std::abs(b);
    while (b) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

long long extendedGcd(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    long long x1, y1;
    long long g = extendedGcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

long long modInverse(long long a, long long mod) {
    long long x, y;
    long long g = extendedGcd(a, mod, x, y);
    if (g != 1) {
        throw std::runtime_error("Модульный обратный элемент не существует");
    }
    return ((x % mod) + mod) % mod;
}

// ===== Класс Matrix =====

using elem_t = Matrix::elem_t;

Matrix::Matrix(size_t rows, size_t cols, elem_t modulus)
    : rows_(rows), cols_(cols), modulus_(modulus), data_(rows * cols, 0) {
    if (modulus <= 0) {
        throw std::invalid_argument("Модуль должен быть положительным");
    }
}

Matrix::Matrix(size_t rows, size_t cols, elem_t modulus, const std::vector<elem_t>& values)
    : rows_(rows), cols_(cols), modulus_(modulus) {
    if (modulus <= 0) {
        throw std::invalid_argument("Модуль должен быть положительным");
    }
    if (values.size() != rows * cols) {
        throw std::invalid_argument("Количество элементов не совпадает с размерами матрицы");
    }
    data_.resize(rows * cols);
    for (size_t i = 0; i < data_.size(); ++i) {
        data_[i] = normalize(values[i]);
    }
}

Matrix Matrix::identity(size_t size, elem_t modulus) {
    Matrix mat(size, size, modulus);
    for (size_t i = 0; i < size; ++i) {
        mat(i, i) = 1;
    }
    return mat;
}

Matrix Matrix::random(size_t rows, size_t cols, elem_t modulus, std::mt19937_64& rng) {
    Matrix mat(rows, cols, modulus);
    std::uniform_int_distribution<elem_t> dist(0, modulus - 1);
    for (size_t i = 0; i < rows * cols; ++i) {
        mat.data_[i] = dist(rng);
    }
    return mat;
}

Matrix Matrix::randomInvertible(size_t size, elem_t modulus, std::mt19937_64& rng, int max_attempts) {
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        Matrix mat = random(size, size, modulus, rng);
        if (mat.isInvertible()) {
            return mat;
        }
    }
    throw std::runtime_error("Не удалось сгенерировать обратимую матрицу за отведённое число попыток");
}

elem_t Matrix::normalize(elem_t x) const {
    x = x % modulus_;
    if (x < 0) x += modulus_;
    return x;
}

elem_t& Matrix::at(size_t i, size_t j) {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Индекс за границами матрицы");
    }
    return data_[i * cols_ + j];
}

elem_t Matrix::at(size_t i, size_t j) const {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Индекс за границами матрицы");
    }
    return data_[i * cols_ + j];
}

elem_t& Matrix::operator()(size_t i, size_t j) {
    return at(i, j);
}

elem_t Matrix::operator()(size_t i, size_t j) const {
    return at(i, j);
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_ || modulus_ != other.modulus_) {
        throw std::invalid_argument("Размеры матриц или модули не совпадают");
    }
    Matrix result = *this;
    for (size_t i = 0; i < data_.size(); ++i) {
        result.data_[i] = normalize(result.data_[i] + other.data_[i]);
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_ || modulus_ != other.modulus_) {
        throw std::invalid_argument("Размеры матриц или модули не совпадают");
    }
    Matrix result = *this;
    for (size_t i = 0; i < data_.size(); ++i) {
        result.data_[i] = normalize(result.data_[i] - other.data_[i]);
    }
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols_ != other.rows_ || modulus_ != other.modulus_) {
        throw std::invalid_argument("Размеры матриц не согласованы для умножения");
    }

    Matrix result(rows_, other.cols_, modulus_);

    // Использование uint64_t для предотвращения переполнения
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < other.cols_; ++j) {
            elem_t sum = 0;
            for (size_t k = 0; k < cols_; ++k) {
                elem_t a = at(i, k);
                elem_t b = other.at(k, j);
                // Берём остаток по модулю, чтобы не было переполнения
                elem_t prod = ((a % modulus_) * (b % modulus_)) % modulus_;
                sum = (sum + prod) % modulus_;
            }
            result(i, j) = normalize(sum);
        }
    }

    return result;
}

Matrix Matrix::transpose() const {
    Matrix result(cols_, rows_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(j, i) = at(i, j);
        }
    }
    return result;
}

elem_t Matrix::determinant2x2(const Matrix& mat) const {
    // det([[a,b],[c,d]]) = ad - bc
    elem_t ad = ((mat.at(0, 0) % modulus_) * (mat.at(1, 1) % modulus_)) % modulus_;
    elem_t bc = ((mat.at(0, 1) % modulus_) * (mat.at(1, 0) % modulus_)) % modulus_;
    elem_t det = (ad - bc) % modulus_;
    return normalize(det);
}

elem_t Matrix::determinant3x3(const Matrix& mat) const {
    // Разложение по первой строке: det = a00*(a11*a22 - a12*a21) - a01*(a10*a22 - a12*a20) + a02*(a10*a21 - a11*a20)
    elem_t m00 = ((mat.at(1, 1) % modulus_) * (mat.at(2, 2) % modulus_)) % modulus_;
    elem_t m01 = ((mat.at(1, 2) % modulus_) * (mat.at(2, 1) % modulus_)) % modulus_;
    elem_t minor0 = (m00 - m01) % modulus_;
    
    elem_t m10 = ((mat.at(1, 0) % modulus_) * (mat.at(2, 2) % modulus_)) % modulus_;
    elem_t m11 = ((mat.at(1, 2) % modulus_) * (mat.at(2, 0) % modulus_)) % modulus_;
    elem_t minor1 = (m10 - m11) % modulus_;
    
    elem_t m20 = ((mat.at(1, 0) % modulus_) * (mat.at(2, 1) % modulus_)) % modulus_;
    elem_t m21 = ((mat.at(1, 1) % modulus_) * (mat.at(2, 0) % modulus_)) % modulus_;
    elem_t minor2 = (m20 - m21) % modulus_;
    
    elem_t det = ((mat.at(0, 0) * minor0) % modulus_) - ((mat.at(0, 1) * minor1) % modulus_) + ((mat.at(0, 2) * minor2) % modulus_);
    return normalize(det);
}

elem_t Matrix::determinantGeneral(Matrix mat) const {
    // Метод Гаусса с расширенным алгоритмом Евклида для работы с составным n
    if (mat.rows_ != mat.cols_) {
        throw std::invalid_argument("Определитель определён только для квадратных матриц");
    }

    elem_t det = 1;
    size_t n = mat.rows_;

    for (size_t col = 0; col < n; ++col) {
        // Ищем ненулевой опорный элемент
        int pivot_row = -1;
        for (size_t row = col; row < n; ++row) {
            if (gcd(mat.at(row, col), modulus_) == 1) {
                pivot_row = row;
                break;
            }
        }

        if (pivot_row == -1) {
            return 0;
        }

        // Обмен строк
        if (pivot_row != static_cast<int>(col)) {
            for (size_t j = 0; j < n; ++j) {
                std::swap(mat.at(col, j), mat.at(pivot_row, j));
            }
            det = normalize(-det);
        }

        // Умножение определителя на опорный элемент
        det = normalize(det * mat.at(col, col));

        // Исключение
        elem_t pivot_inv = modInverse(mat.at(col, col), modulus_);
        for (size_t j = col; j < n; ++j) {
            mat(col, j) = normalize(mat.at(col, j) * pivot_inv);
        }

        for (size_t row = col + 1; row < n; ++row) {
            elem_t factor = mat.at(row, col);
            for (size_t j = col; j < n; ++j) {
                mat(row, j) = normalize(mat.at(row, j) - factor * mat.at(col, j));
            }
        }
    }

    return det;
}

elem_t Matrix::determinant() const {
    if (rows_ != cols_) {
        throw std::invalid_argument("Определитель определён только для квадратных матриц");
    }

    if (rows_ == 1) return at(0, 0);
    if (rows_ == 2) return determinant2x2(*this);
    if (rows_ == 3) return determinant3x3(*this);

    return determinantGeneral(Matrix(*this));
}

bool Matrix::isInvertible() const {
    if (rows_ != cols_) {
        return false;
    }
    elem_t det = determinant();
    return gcd(det, modulus_) == 1;
}

Matrix Matrix::inverse() const {
    if (rows_ != cols_) {
        throw std::invalid_argument("Только квадратные матрицы могут быть обратимы");
    }

    elem_t det = determinant();
    if (gcd(det, modulus_) != 1) {
        throw std::runtime_error("Матрица не обратима: НОД(det, n) ≠ 1");
    }

    size_t n = rows_;
    elem_t det_inv = modInverse(det, modulus_);

    // Вычисляем присоединённую матрицу через миноры
    Matrix adj(n, n, modulus_);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            // Создаём минор без i-й строки и j-го столбца
            Matrix minor(n - 1, n - 1, modulus_);
            for (size_t mi = 0, ii = 0; mi < n; ++mi) {
                if (mi == i) continue;
                for (size_t mj = 0, jj = 0; mj < n; ++mj) {
                    if (mj == j) continue;
                    minor(ii, jj) = at(mi, mj);
                    ++jj;
                }
                ++ii;
            }
            elem_t minor_det = minor.determinant();
            if ((i + j) % 2 == 1) minor_det = normalize(-minor_det);
            adj(j, i) = minor_det; // Транспонирование!
        }
    }

    // A⁻¹ = det⁻¹ · adj(A) mod n
    Matrix result(n, n, modulus_);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result(i, j) = normalize(det_inv * adj(i, j));
        }
    }

    return result;
}

bool Matrix::operator==(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_ || modulus_ != other.modulus_) {
        return false;
    }
    return data_ == other.data_;
}

std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
    for (size_t i = 0; i < mat.rows_; ++i) {
        os << "[ ";
        for (size_t j = 0; j < mat.cols_; ++j) {
            os << std::setw(3) << mat.at(i, j) << " ";
        }
        os << "]\n";
    }
    return os;
}
