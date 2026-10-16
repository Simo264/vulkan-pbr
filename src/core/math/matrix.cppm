export module core.math:matrix;

import :vector;
import core.types;


template <i32 N, Arithmetic T>
requires (N >= 2 && N <= 4)
struct SquareMatrix
{
  T m[N][N];

  constexpr       T& operator()(i32 i, i32 j) noexcept { return m[i][j]; }
  constexpr const T& operator()(i32 i, i32 j) const noexcept { return m[i][j]; }
  constexpr       T* operator[](i32 i) noexcept { return m[i]; }
  constexpr const T* operator[](i32 i) const noexcept { return m[i]; }

  static constexpr SquareMatrix zero() noexcept { return SquareMatrix{}; }
  static constexpr SquareMatrix identity() noexcept { SquareMatrix r{}; for (i32 i = 0; i < N; ++i) r.m[i][i] = T{1}; return r; }

  constexpr SquareMatrix& operator+=(const SquareMatrix& rhs) noexcept { for (i32 i = 0; i < N; ++i) { for (i32 j = 0; j < N; ++j) m[i][j] += rhs.m[i][j]; } return *this; }
  constexpr SquareMatrix& operator-=(const SquareMatrix& rhs) noexcept { for (i32 i = 0; i < N; ++i) { for (i32 j = 0; j < N; ++j) m[i][j] -= rhs.m[i][j]; } return *this; }
  constexpr SquareMatrix& operator*=(T s) noexcept { for (i32 i = 0; i < N; ++i) { for (i32 j = 0; j < N; ++j) m[i][j] *= s; } return *this; }
  constexpr SquareMatrix& operator/=(T s) noexcept { for (i32 i = 0; i < N; ++i) { for (i32 j = 0; j < N; ++j) m[i][j] /= s; } return *this; }
  constexpr SquareMatrix& operator*=(const SquareMatrix& rhs) noexcept
  {
    SquareMatrix tmp{};
    for (i32 i = 0; i < N; ++i)
    {
      for (i32 k = 0; k < N; ++k)
      {
        const T aik = m[i][k];
        for (i32 j = 0; j < N; ++j)
          tmp.m[i][j] += aik * rhs.m[k][j];
      }
    }
    *this = tmp;
    return *this;
  }
};

export {
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> operator+(SquareMatrix<N, T> a, const SquareMatrix<N, T>& b) noexcept { a += b; return a; }
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> operator-(SquareMatrix<N, T> a, const SquareMatrix<N, T>& b) noexcept { a -= b; return a; }
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> operator*(SquareMatrix<N, T> a, T s) noexcept { a *= s; return a; }
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> operator*(T s, SquareMatrix<N, T> a) noexcept { a *= s; return a; }
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> operator/(SquareMatrix<N, T> a, T s) noexcept { a /= s; return a; }

  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> operator*(const SquareMatrix<N, T>& a, const SquareMatrix<N, T>& b) noexcept { SquareMatrix<N, T> r{}; for (i32 i = 0; i < N; ++i) { for (i32 k = 0; k < N; ++k) { const T aik = a.m[i][k]; for (i32 j = 0; j < N; ++j) r.m[i][j] += aik * b.m[k][j]; } } return r; }
  template <i32 N, Arithmetic T>
  constexpr bool operator==(const SquareMatrix<N, T>& a, const SquareMatrix<N, T>& b) noexcept { for (i32 i = 0; i < N; ++i) {for (i32 j = 0; j < N; ++j) if (a.m[i][j] != b.m[i][j]) return false;} return true; }
  template <i32 N, Arithmetic T>
  constexpr bool operator!=(const SquareMatrix<N, T>& a, const SquareMatrix<N, T>& b) noexcept { return !(a == b); }

  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> zero() noexcept { return SquareMatrix<N, T>::zero(); }
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> identity() noexcept { return SquareMatrix<N, T>::identity(); }
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> transpose(const SquareMatrix<N, T>& a) noexcept { SquareMatrix<N, T> r{};for (i32 i = 0; i < N; ++i){ for (i32 j = 0; j < N; ++j) r.m[j][i] = a.m[i][j]; } return r; }
  template <i32 N, Arithmetic T>
  constexpr T determinant(const SquareMatrix<N, T>& a) noexcept
  {
    if constexpr (N == 2)
    {
      return a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0];
    }
    else if constexpr (N == 3)
    {
      return a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1])
            - a.m[0][1] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0])
            + a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
    }
    else if constexpr (N == 4)
    {
      T det = T{0};
      for (i32 j = 0; j < 4; ++j)
      {
        SquareMatrix<3, T> sub{};
        for (i32 row = 1, sr = 0; row < 4; ++row, ++sr)
        {
          for (i32 col = 0, sc = 0; col < 4; ++col)
          {
            if (col == j) continue;
            sub.m[sr][sc++] = a.m[row][col];
          }
        }
        const T sign = (j % 2 == 0) ? T{1} : T{-1};
        det += sign * a.m[0][j] * determinant(sub);
      }
      return det;
    }
  }
  template <i32 N, Arithmetic T>
  constexpr SquareMatrix<N, T> inverse(const SquareMatrix<N, T>& a) noexcept
  {
    if constexpr (N == 2)
    {
      const T det     = a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0];
      const T inv_det = T{1} / det;

      SquareMatrix<2, T> r{};
      r.m[0][0] =  a.m[1][1] * inv_det;
      r.m[0][1] = -a.m[0][1] * inv_det;
      r.m[1][0] = -a.m[1][0] * inv_det;
      r.m[1][1] =  a.m[0][0] * inv_det;
      return r;
    }
    else if constexpr (N == 3)
    {
      const T det =
          a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1])
        - a.m[0][1] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0])
        + a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
      const T inv_det = T{1} / det;

      SquareMatrix<3, T> r{};
      r.m[0][0] =  (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) * inv_det;
      r.m[0][1] = -(a.m[0][1] * a.m[2][2] - a.m[0][2] * a.m[2][1]) * inv_det;
      r.m[0][2] =  (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1]) * inv_det;

      r.m[1][0] = -(a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0]) * inv_det;
      r.m[1][1] =  (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0]) * inv_det;
      r.m[1][2] = -(a.m[0][0] * a.m[1][2] - a.m[0][2] * a.m[1][0]) * inv_det;

      r.m[2][0] =  (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]) * inv_det;
      r.m[2][1] = -(a.m[0][0] * a.m[2][1] - a.m[0][1] * a.m[2][0]) * inv_det;
      r.m[2][2] =  (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0]) * inv_det;
      return r;
    }
    else if constexpr (N == 4)
    {
      // Gauss-Jordan elimination with partial pivoting.
      SquareMatrix<4, T> aug = a;
      SquareMatrix<4, T> inv = SquareMatrix<4, T>::identity();
      for (i32 col = 0; col < 4; ++col)
      {
        i32 pivot   = col;
        T   max_abs = aug.m[col][col] < T{0} ? -aug.m[col][col] : aug.m[col][col];
        for (i32 row = col + 1; row < 4; ++row)
        {
          const T v  = aug.m[row][col];
          const T av = v < T{0} ? -v : v;
          if (av > max_abs) { max_abs = av; pivot = row; }
        }
        if (pivot != col)
        {
          for (i32 j = 0; j < 4; ++j)
          {
            T t = aug.m[col][j]; aug.m[col][j] = aug.m[pivot][j]; aug.m[pivot][j] = t;
            t   = inv.m[col][j]; inv.m[col][j] = inv.m[pivot][j]; inv.m[pivot][j] = t;
          }
        }

        const T diag = aug.m[col][col];
        for (i32 j = 0; j < 4; ++j)
        {
          aug.m[col][j] /= diag;
          inv.m[col][j] /= diag;
        }
        for (i32 row = 0; row < 4; ++row)
        {
          if (row == col) continue;
          const T factor = aug.m[row][col];
          if (factor == T{0}) continue;
          for (i32 j = 0; j < 4; ++j)
          {
            aug.m[row][j] -= factor * aug.m[col][j];
            inv.m[row][j] -= factor * inv.m[col][j];
          }
        }
      }
      return inv;
    }
  }

  using Matrix2f = SquareMatrix<2, f32>;
  using Matrix2d = SquareMatrix<2, f64>;
  using Matrix2i = SquareMatrix<2, i32>;
  using Matrix3f = SquareMatrix<3, f32>;
  using Matrix3d = SquareMatrix<3, f64>;
  using Matrix3i = SquareMatrix<3, i32>;
  using Matrix4f = SquareMatrix<4, f32>;
  using Matrix4d = SquareMatrix<4, f64>;
  using Matrix4i = SquareMatrix<4, i32>;
};
