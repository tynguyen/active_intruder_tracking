#pragma once

#include "base.hpp"

namespace manifold_elements
{
template <typename Scalar>
class ScalarElement final
    : public ManifoldElement<ScalarElement<Scalar>, Scalar, 1>
{
 public:
  using Base = ManifoldElement<ScalarElement<Scalar>, Scalar, 1>;
  using TangentVec = typename Base::TangentVec;
  using ElementType = Scalar;

  ScalarElement(const Scalar &s = Scalar(0)) { setValue(s); }

  Scalar getValue() const { return scalar_; }
  void setValue(const Scalar &s) { scalar_ = s; }

#if 0
  ScalarElement operator+(const TangentVec &diff) const override
  {
    return ScalarElement(scalar_ + diff(0));
  }
#else
  ScalarElement &operator+=(const TangentVec &diff) override
  {
    scalar_ += diff(0);
    return *this;
  }
#endif

  TangentVec operator-(const ScalarElement &s) const override
  {
    TangentVec v;
    v(0) = scalar_ - s.scalar_;
    return v;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const ScalarElement &element)
  {
    stream << element.scalar_;
    return stream;
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
  Scalar scalar_;
};
} // namespace manifold_elements
