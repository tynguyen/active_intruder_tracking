#pragma once

#include <Eigen/Core>

namespace manifold_elements
{
template <typename Derived, typename Scalar_t, int tangent_dim>
class ManifoldElement
{
 public:
  using Scalar = Scalar_t;
  using TangentVec = Eigen::Matrix<Scalar, tangent_dim, 1>;

  ManifoldElement() = default;
  virtual ~ManifoldElement() = default;

  // See https://stackoverflow.com/a/4782927 or
  // https://en.cppreference.com/w/cpp/language/rule_of_three#Rule_of_zero
  ManifoldElement(const ManifoldElement &) = default; // Copy constructor
  ManifoldElement(ManifoldElement &&) = default;      // Move constructor
  ManifoldElement &operator=(const ManifoldElement &) & =
      default; // Copy assignment operator
  ManifoldElement &operator=(ManifoldElement &&) & =
      default; // Move assignment operator

  virtual Derived &operator+=(const TangentVec &diff) = 0;
  virtual TangentVec operator-(const Derived &element) const = 0;

  Derived operator+(const TangentVec &diff) const
  {
    Derived ret = *static_cast<const Derived *>(this);
    ret += diff;
    return ret;
  }

  Derived &operator-=(const TangentVec &diff)
  {
    return this->operator+=(-diff);
  }

  Derived operator-(const TangentVec &diff) const
  {
    return this->operator+(-diff);
  }

  static constexpr unsigned int tangent_dim_ = tangent_dim;
};
} // namespace manifold_elements
