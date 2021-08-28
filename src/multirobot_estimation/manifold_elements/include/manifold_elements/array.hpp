#pragma once

#include "base.hpp"

namespace manifold_elements
{
template <typename T, unsigned int N>
class ArrayElement
    : public ManifoldElement<ArrayElement<T, N>, typename T::Scalar,
                             N * T::tangent_dim_>
{
 public:
  using Base = ManifoldElement<ArrayElement<T, N>, typename T::Scalar,
                               N * T::tangent_dim_>;
  using TangentVec = typename Base::TangentVec;

  using value_type = T;

  ArrayElement() = default;
  explicit ArrayElement(std::array<T, N> const &array) : array_{array} {}

  T const &operator[](unsigned int M) const { return array_[M]; }

  T &operator[](unsigned int M) { return array_[M]; }

#if 0
  ArrayElement operator+(const TangentVec &diff) const override
  {
    ArrayElement p;
    for(unsigned int i = 0; i < N; ++i)
    {
      p.array_[i] = array_[i] +
                    diff.template segment<T::tangent_dim_>(i * T::tangent_dim_);
    }
    return p;
  }
#else
  ArrayElement &operator+=(const TangentVec &diff) override
  {
    for(unsigned int i = 0; i < N; ++i)
    {
      array_[i] += diff.template segment<T::tangent_dim_>(i * T::tangent_dim_);
    }
    return *this;
  }
#endif

  TangentVec operator-(ArrayElement const &other) const override
  {
    TangentVec v;
    for(unsigned int i = 0; i < N; ++i)
    {
      v.template segment<T::tangent_dim_>(i * T::tangent_dim_) =
          array_[i] - other.array_[i];
    }
    return v;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  ArrayElement const &element)
  {
    stream << "ArrayElement with " << N << " elements:\n";
    for(unsigned int i = 0; i < N; ++i)
    {
      stream << "- [" << i << "]: " << element.array_[i] << '\n';
    }
    return stream;
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
  std::array<T, N> array_;
};
} // namespace manifold_elements
