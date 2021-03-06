#pragma once

#include <iostream>
#include <random>

#include <Eigen/Cholesky>
#include <Eigen/SVD>

#include "km_utils/types.hpp"

namespace km
{
namespace math
{
template <typename Scalar_t, int vector_size>
constexpr Scalar_t multivariateNormalProb(
    types::Vec<Scalar_t, vector_size> const &x,
    types::Vec<Scalar_t, vector_size> const &mean,
    types::SquareMat<Scalar_t, vector_size> const &cov_inverse,
    Scalar_t const normal_pdf_constant)
{
  types::Vec<Scalar_t, vector_size> const dx = x - mean;
  Scalar_t const normal_pdf_exponent =
      Scalar_t{-0.5} * (dx.transpose() * cov_inverse * dx).value();
  return normal_pdf_constant * std::exp(normal_pdf_exponent);
}

template <typename Scalar_t, int vector_size>
constexpr Scalar_t multivariateNormalProb(
    types::Vec<Scalar_t, vector_size> const &dx,
    types::SquareMat<Scalar_t, vector_size> const &cov_inverse,
    Scalar_t const normal_pdf_constant)
{
  Scalar_t const normal_pdf_exponent =
      Scalar_t{-0.5} * (dx.transpose() * cov_inverse * dx).value();
  return normal_pdf_constant * std::exp(normal_pdf_exponent);
}

template <typename Scalar_t, int vector_size>
constexpr Scalar_t multivariateNormalProb(
    types::Vec<Scalar_t, vector_size> const &x,
    types::Vec<Scalar_t, vector_size> const &mean,
    types::SquareMat<Scalar_t, vector_size> const &cov_inverse)
{
  Scalar_t const normal_pdf_constant =
      std::sqrt((cov_inverse / Scalar_t{2 * M_PI}).determinant());
  return multivariateNormalProb(x, mean, cov_inverse, normal_pdf_constant);
}

template <typename Derived>
constexpr auto getNormalPDFConstant(
    Eigen::MatrixBase<Derived> const &cov_inverse)
{
  using Scalar = typename Eigen::internal::traits<Derived>::Scalar;
  return std::sqrt((cov_inverse / Scalar{2 * M_PI}).determinant());
}

template <typename Scalar_t, int mat_size>
constexpr std::vector<Scalar_t> getNormalPDFConstant(
    types::vector_aligned<types::SquareMat<Scalar_t, mat_size>> const
        &vec_cov_inverse)
{
  std::vector<Scalar_t> ret;
  ret.reserve(vec_cov_inverse.size());
  for(auto const &M : vec_cov_inverse)
    ret.push_back(getNormalPDFConstant(M));
  return ret;
}

/**
 * Calculate the matrix square root of a positive (semi-)definite matrix.
 *
 * @param mat The input matrix.
 * @param semidefinite_mat Indicates whether the input is a semidefinite matrix.
 *
 * @return The matrix square root of the input mat.
 */
template <typename Derived>
constexpr typename Derived::PlainObject matrixSquareRoot(
    Eigen::MatrixBase<Derived> const &mat, bool semidefinite_mat = false)
{
  constexpr auto M = Derived::RowsAtCompileTime;
  constexpr auto N = Derived::ColsAtCompileTime;
  static_assert(M == N, "Only valid for a square matrix");
  if constexpr(M == 0 || N == 0)
    return Derived::PlainObject::Zero(mat.rows(), mat.cols());
  else
  {
    if(!semidefinite_mat)
    {
      Eigen::LLT<typename Derived::PlainObject> cov_chol{mat};
      if(cov_chol.info() == Eigen::Success)
        return cov_chol.matrixL();
    }
    Eigen::LDLT<typename Derived::PlainObject> cov_chol{mat};
    if(cov_chol.info() == Eigen::Success)
    {
      typename Derived::PlainObject const L = cov_chol.matrixL();
      auto const P = cov_chol.transpositionsP();
      auto const D_sqrt =
          cov_chol.vectorD().array().sqrt().matrix().asDiagonal();
      return P.transpose() * L * D_sqrt;
    }
    return Derived::PlainObject::Zero(mat.rows(), mat.cols());
  }
}

template <typename Vec, typename Cov>
types::vector_aligned<Vec> randomSamplesGaussian(Vec const &mean,
                                                 Cov const &covariance_sqrt,
                                                 unsigned int n,
                                                 std::random_device &rd)
{
  std::mt19937 rng{rd()};
  std::normal_distribution<> dist;

  types::vector_aligned<Vec> ret;
  ret.reserve(n);
  for(unsigned int i = 0; i < n; ++i)
  {
    Vec z = Vec::Zero(mean.size());
    for(unsigned int j = 0; j < mean.size(); ++j)
      z(j) = dist(rng);
    ret.push_back(mean + covariance_sqrt * z);
  }
  return ret;
}

template <typename Derived>
constexpr typename Derived::PlainObject pseudoInverse(
    Eigen::MatrixBase<Derived> const &m)
{
  // JacobiSVD: thin U and V are only available when your matrix has a dynamic
  // number of columns.
  constexpr auto flags = (Derived::ColsAtCompileTime == Eigen::Dynamic) ?
                             (Eigen::ComputeThinU | Eigen::ComputeThinV) :
                             (Eigen::ComputeFullU | Eigen::ComputeFullV);
  Eigen::JacobiSVD<typename Derived::PlainObject> m_svd(m, flags);
  // std::cout << "singular values: " << m_svd.singularValues().transpose()
  //           << "\n";
  return m_svd.solve(Derived::PlainObject::Identity(m.rows(), m.rows()));
}

template <unsigned int num_repeat, typename Derived>
constexpr auto blockDiagonalMatrix(Eigen::EigenBase<Derived> const &X)
{
  using Scalar = typename Eigen::internal::traits<Derived>::Scalar;
  constexpr auto M = Eigen::internal::traits<Derived>::RowsAtCompileTime;
  constexpr auto N = Eigen::internal::traits<Derived>::ColsAtCompileTime;

  auto const in_rows = (M == Eigen::Dynamic) ? X.rows() : M;
  auto const in_cols = (N == Eigen::Dynamic) ? X.cols() : N;
  auto const out_rows = num_repeat * in_rows;
  auto const out_cols = num_repeat * in_cols;
  constexpr auto out_template_rows =
      (M == Eigen::Dynamic) ? Eigen::Dynamic : out_rows;
  constexpr auto out_template_cols =
      (N == Eigen::Dynamic) ? Eigen::Dynamic : out_cols;

  auto ret = Eigen::Matrix<Scalar, out_template_rows, out_template_cols>::Zero(
                 out_rows, out_cols)
                 .eval();

  if constexpr(M != Eigen::Dynamic && N != Eigen::Dynamic)
  {
    for(unsigned int i = 0; i < num_repeat; ++i)
      ret.template block<M, N>(i * M, i * N) = X;
  }
  else
  {
    for(unsigned int i = 0; i < num_repeat; ++i)
      ret.block(i * in_rows, i * in_cols, in_rows, in_cols) = X;
  }
  return ret;
}

template <typename Derived>
constexpr auto blockDiagonalMatrix(Eigen::EigenBase<Derived> const &X,
                                   unsigned int const num_repeat)
{
  using Scalar = typename Eigen::internal::traits<Derived>::Scalar;
  constexpr auto M = Eigen::internal::traits<Derived>::RowsAtCompileTime;
  constexpr auto N = Eigen::internal::traits<Derived>::ColsAtCompileTime;

  auto const in_rows = X.rows();
  auto const in_cols = X.cols();

  auto ret = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(
                 num_repeat * in_rows, num_repeat * in_cols)
                 .eval();

  if constexpr(M != Eigen::Dynamic && N != Eigen::Dynamic)
  {
    for(unsigned int i = 0; i < num_repeat; ++i)
      ret.template block<M, N>(i * M, i * N) = X;
  }
  else
  {
    for(unsigned int i = 0; i < num_repeat; ++i)
      ret.block(i * in_rows, i * in_cols, in_rows, in_cols) = X;
  }
  return ret;
}

template <typename T>
constexpr T square(T p)
{
  return p * p;
}

// From https://stackoverflow.com/a/33454406
template <typename T>
constexpr T powInt(T x, unsigned int n)
{
  if(n == 0)
    return T{1};
  else if(n == 1)
    return x;
  else if(n == 2)
    return x * x;

  auto y = x * x;
  n -= 2;
  while(n > 1)
  {
    if(n % 2 == 1)
      y *= x;
    x *= x;
    n /= 2;
  }
  return x * y;
}

/**
 * @brief Normalize the given angle in radians to (-PI, PI]
 *
 * @param[in] x angle in radians
 *
 * @return normalized angle in the range (-PI, PI]
 */
template <typename Scalar_t>
constexpr Scalar_t normalize_angle(Scalar_t x)
{
  constexpr Scalar_t pi = M_PI, two_pi = 2 * M_PI;
  return x - (std::ceil((x + pi) / two_pi) - Scalar_t{1}) * two_pi;
}
} // namespace math
} // namespace km