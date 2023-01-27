//
// Created by Alexander G. Pronchenkov on 27.01.2023.
//
#ifndef U7_ALGORITHM_MATRIX_H_
#define U7_ALGORITHM_MATRIX_H_

#include <memory>

namespace u7::algorithm {

template <typename T>
class Matrix {
 public:
  Matrix() = default;

  Matrix(int n, int m) : n_(n), m_(m), a_(new T[static_cast<size_t>(n) * m]) {}

  Matrix(Matrix&& rhs) = default;

  Matrix& operator=(Matrix&& rhs) = default;

  int n() const { return n_; }

  int m() const { return m_; }

  void Fill(const T& x) {
    const size_t k = static_cast<size_t>(n_) * m_;
    for (size_t i = 0; i < k; ++i) {
      a_[i] = x;
    }
  }

  const T& UnsafeAt(int i, int j) const {
    return a_[static_cast<size_t>(i) * m_ + j];
  }

  T& UnsafeAt(int i, int j) { return a_[static_cast<size_t>(i) * m_ + j]; }

 private:
  int n_ = 0;
  int m_ = 0;
  std::unique_ptr<T[]> a_;
};

}  // namespace u7::algorithm

#endif  // U7_ALGORITHM_MATRIX_H_
