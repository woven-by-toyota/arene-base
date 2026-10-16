// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_FLOAT32_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_FLOAT32_HPP_

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/float_sign.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace testing {

/// @brief wrapper around @c float for use as a user-defined floating-point type
/// in tests
///
/// Provides the same semantics as @c float, including arithmetic, comparison,
/// @c abs, and @c signbit, but as a distinct type. This allows testing that
/// generic code (e.g. linalg, ulp_distance) works correctly with user-defined
/// floating-point types and not just built-in ones.
class float32 {
  float value_{};

 public:
  float32() = default;

  // This is only for test code, and implicit conversions make this easier to initialize.
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr float32(float value) noexcept
      : value_{value} {}

  constexpr auto value() const noexcept -> float { return value_; }

  friend constexpr auto operator+(float32 arg) noexcept -> float32 { return {+arg.value_}; }
  friend constexpr auto operator-(float32 arg) noexcept -> float32 { return {-arg.value_}; }

  friend constexpr auto operator+(float32 lhs, float32 rhs) noexcept -> float32 { return {lhs.value_ + rhs.value_}; }
  friend constexpr auto operator-(float32 lhs, float32 rhs) noexcept -> float32 { return {lhs.value_ - rhs.value_}; }
  friend constexpr auto operator*(float32 lhs, float32 rhs) noexcept -> float32 { return {lhs.value_ * rhs.value_}; }
  friend constexpr auto operator/(float32 lhs, float32 rhs) noexcept -> float32 { return {lhs.value_ / rhs.value_}; }

  constexpr auto operator+=(float32 rhs) noexcept -> float32& {
    value_ += rhs.value_;
    return *this;
  }
  constexpr auto operator-=(float32 rhs) noexcept -> float32& {
    value_ -= rhs.value_;
    return *this;
  }
  constexpr auto operator*=(float32 rhs) noexcept -> float32& {
    value_ *= rhs.value_;
    return *this;
  }
  constexpr auto operator/=(float32 rhs) noexcept -> float32& {
    value_ /= rhs.value_;
    return *this;
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "These operators are implementations of exact equality.");

  friend constexpr auto operator==(float32 lhs, float32 rhs) -> bool { return lhs.value() == rhs.value(); }
  friend constexpr auto operator!=(float32 lhs, float32 rhs) -> bool { return lhs.value() != rhs.value(); }

  ARENE_IGNORE_END();

  friend constexpr auto operator<(float32 lhs, float32 rhs) -> bool { return lhs.value() < rhs.value(); }
  friend constexpr auto operator>(float32 lhs, float32 rhs) -> bool { return lhs.value() > rhs.value(); }
  friend constexpr auto operator<=(float32 lhs, float32 rhs) -> bool { return lhs.value() <= rhs.value(); }
  friend constexpr auto operator>=(float32 lhs, float32 rhs) -> bool { return lhs.value() >= rhs.value(); }

  friend constexpr auto abs(float32 arg) noexcept -> float32 { return arene::base::abs(arg.value()); }
  friend constexpr auto signbit(float32 arg) noexcept -> bool { return arene::base::signbit(arg.value()); }
  friend constexpr auto isnan(float32 arg) noexcept -> bool { return arene::base::isnan(arg.value()); }
  friend constexpr auto isinf(float32 arg) noexcept -> bool { return arene::base::isinf(arg.value()); }
  friend constexpr auto isfinite(float32 arg) noexcept -> bool { return arene::base::isfinite(arg.value()); }
  friend constexpr auto isnormal(float32 arg) noexcept -> bool { return arene::base::isnormal(arg.value()); }
};

}  // namespace testing

template <>
class std::numeric_limits<testing::float32> : public std::numeric_limits<float> {
 public:
  static constexpr auto min() noexcept -> testing::float32 { return {numeric_limits<float>::min()}; }
  static constexpr auto max() noexcept -> testing::float32 { return {numeric_limits<float>::max()}; }
  static constexpr auto lowest() noexcept -> testing::float32 { return {numeric_limits<float>::lowest()}; }
  static constexpr auto epsilon() noexcept -> testing::float32 { return {numeric_limits<float>::epsilon()}; }
  static constexpr auto infinity() noexcept -> testing::float32 { return {numeric_limits<float>::infinity()}; }
  static constexpr auto quiet_NaN() noexcept -> testing::float32 { return {numeric_limits<float>::quiet_NaN()}; }
  static constexpr auto signaling_NaN() noexcept -> testing::float32 {
    return {numeric_limits<float>::signaling_NaN()};
  }
  static constexpr auto denorm_min() noexcept -> testing::float32 { return {numeric_limits<float>::denorm_min()}; }
  static constexpr auto round_error() noexcept -> testing::float32 { return {numeric_limits<float>::round_error()}; }
};
#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_FLOAT32_HPP_
