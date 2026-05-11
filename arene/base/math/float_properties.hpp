// Copyright 2025, Toyota Motor Corporation

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_PROPERTIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_PROPERTIES_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

// parasoft-begin-suppress CERT_CPP-DCL51-f "False positive: these names are not reserved in namespace arene::base"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M0_1_2-d "False positive: comparison result is not always false for floating point"
// parasoft-begin-suppress AUTOSAR-M6_2_2-a "Equality is to check the properties of one value, not compare two values"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Equality is used to check the properties of a single value, not compare two values");

namespace float_properties_detail {

/// @brief Check whether or not the argument has a NaN value
/// @tparam T Type of the number to check
/// @param num The number to check
/// @return @c true if @c num has a NaN value, otherwise @c false
template <typename T>
constexpr auto isnan(T const num) noexcept -> bool {
  // NOLINTNEXTLINE(misc-redundant-expression) This expression is not trivially false for floating point numbers
  return num != num;
}

/// @brief Check whether or not the argument has a (positive or negative) infinite value
/// @tparam T Type of the number to check
/// @param num The number to check
/// @return @c true if @c num has an infinite value, otherwise @c false
template <typename T>
constexpr auto isinf(T const num) noexcept -> bool {
  return (num == std::numeric_limits<T>::infinity()) || (num == -std::numeric_limits<T>::infinity());
}

/// @brief Check whether or not the argument is finite, i.e. not infinity or NaN
/// @tparam T Type of the number to check
/// @param num The number to check
/// @return @c true if @c num has a finite value, otherwise @c false
template <typename T>
constexpr auto isfinite(T const num) noexcept -> bool {
  return (!::arene::base::float_properties_detail::isnan(num)) && (!::arene::base::float_properties_detail::isinf(num));
}

/// @brief Check whether or not the argument is normal, i.e. not infinity, NaN, subnormal, or zero
/// @tparam T Type of the number to check
/// @param num The number to check
/// @return @c true if @c num has a normal value, otherwise @c false
template <typename T>
constexpr auto isnormal(T num) noexcept -> bool {
  // This explicit check for NaN is needed for GCC, because it erroneously thinks that using a NaN makes an expression
  // no longer eligible for constexpr.
  if (::arene::base::float_properties_detail::isnan(num)) {
    return false;
  }

  if (num < static_cast<T>(0.0)) {
    // This is faster than a fully correct abs because it doesn't need to deal with corner cases like signed zero/NaN;
    // for the purposes of determining normality it's fine.
    num = -num;
  }

  // numeric_limits::min() for a floating-point type is the smallest *normal* value.
  return (num >= std::numeric_limits<T>::min()) && (num < std::numeric_limits<T>::infinity());
}

ARENE_IGNORE_END();

// parasoft-end-suppress AUTOSAR-M0_1_2-d
// parasoft-end-suppress AUTOSAR-M6_2_2-a

}  // namespace float_properties_detail

/// @brief Check whether or not the argument has a NaN value
/// @param num The number to check
/// @return @c true if @c num has a NaN value, otherwise @c false
constexpr auto isnan(float const num) noexcept -> bool { return float_properties_detail::isnan(num); }

/// @brief Check whether or not the argument has a NaN value
/// @param num The number to check
/// @return @c true if @c num has a NaN value, otherwise @c false
constexpr auto isnan(double const num) noexcept -> bool { return float_properties_detail::isnan(num); }

/// @brief Check whether or not the argument has a (positive or negative) infinite value
/// @param num The number to check
/// @return @c true if @c num has an infinite value, otherwise @c false
constexpr auto isinf(float const num) noexcept -> bool { return float_properties_detail::isinf(num); }

/// @brief Check whether or not the argument has a (positive or negative) infinite value
/// @param num The number to check
/// @return @c true if @c num has an infinite value, otherwise @c false
constexpr auto isinf(double const num) noexcept -> bool { return float_properties_detail::isinf(num); }

/// @brief Check whether or not the argument is finite, i.e. not infinity or NaN
/// @param num The number to check
/// @return @c true if @c num has a finite value, otherwise @c false
constexpr auto isfinite(float const num) noexcept -> bool { return float_properties_detail::isfinite(num); }

/// @brief Check whether or not the argument is finite, i.e. not infinity or NaN
/// @param num The number to check
/// @return @c true if @c num has a finite value, otherwise @c false
constexpr auto isfinite(double const num) noexcept -> bool { return float_properties_detail::isfinite(num); }

/// @brief Check whether or not the argument is normal, i.e. not infinity, NaN, subnormal, or zero
/// @param num The number to check
/// @return @c true if @c num has a normal value, otherwise @c false
constexpr auto isnormal(float const num) noexcept -> bool { return float_properties_detail::isnormal(num); }

/// @brief Check whether or not the argument is normal, i.e. not infinity, NaN, subnormal, or zero
/// @param num The number to check
/// @return @c true if @c num has a normal value, otherwise @c false
constexpr auto isnormal(double const num) noexcept -> bool { return float_properties_detail::isnormal(num); }

}  // namespace base
}  // namespace arene

// parasoft-end-suppress CERT_CPP-DCL51-f
// parasoft-end-suppress AUTOSAR-A7_1_5-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_PROPERTIES_HPP_
