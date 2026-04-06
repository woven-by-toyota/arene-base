// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: builtin_signbit_is_constexpr does relate to floating point signs"
// Copyright 2025, Toyota Motor Corporation

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_SIGN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_SIGN_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

// parasoft-begin-suppress CERT_CPP-DCL51-f "False positive: these names are not reserved in namespace arene::base"
// parasoft-begin-suppress AUTOSAR-M17_0_3-a "False positive: the names are added in namespace arene::base"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

/// @brief Return a number with the magnitude of the first operand and the sign of the second
/// @param magnitude The operand to take the magnitude from
/// @param sign The operand to take the sign from
/// @return A value with the same absolute value as @c magnitude and the same sign as @c sign
constexpr auto copysign(float const magnitude, float const sign) noexcept -> float {
  return __builtin_copysignf(magnitude, sign);
}

/// @brief Return a number with the magnitude of the first operand and the sign of the second
/// @param magnitude The operand to take the magnitude from
/// @param sign The operand to take the sign from
/// @return A value with the same absolute value as @c magnitude and the same sign as @c sign
constexpr auto copysign(double const magnitude, double const sign) noexcept -> double {
  return __builtin_copysign(magnitude, sign);
}

namespace float_sign_detail {
/// @brief Type trait to check whether or not __builtin_signbit is constexpr for the given argument type
/// @tparam T Argument type with which __builtin_signbit will be called
template <typename T, typename = constraints<>>
class builtin_signbit_is_constexpr : public std::false_type {};

/// @brief Type trait to check whether or not __builtin_signbit is constexpr for the given argument type
/// @tparam T Argument type with which __builtin_signbit will be called
template <typename T>
class builtin_signbit_is_constexpr<T, constraints<std::enable_if_t<(static_cast<void>(__builtin_signbit(T{})), true)>>>
    : public std::true_type {};

/// @brief Type trait to check whether or not __builtin_signbit is constexpr for the given argument type
/// @tparam T Argument type with which __builtin_signbit will be called
template <typename T>
constexpr bool builtin_signbit_is_constexpr_v = builtin_signbit_is_constexpr<T>::value;

/// @brief Implementation of @c signbit function when @c __builtin_signbit is constexpr (all GCC versions, Clang v20.1+)
/// @tparam T Argument type to check the sign bit of
/// @param num Value to check the sign bit of
/// @return @c true if @c num is *negative* (i.e. its sign bit is 1), @c false if @c num is positive
template <typename T, constraints<std::enable_if_t<builtin_signbit_is_constexpr_v<T>>> = nullptr>
constexpr auto signbit(T const num) noexcept -> bool {
  return __builtin_signbit(num);
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Equality operand is guaranteed to be either exactly 1.0 or exactly -1.0");

/// @brief Implementation of @c signbit function when @c __builtin_signbit is *not* constexpr (Clang v19.4-)
/// @tparam T Argument type to check the sign bit of
/// @param num Value to check the sign bit of
/// @return @c true if @c num is *negative* (i.e. its sign bit is 1), @c false if @c num is positive
template <typename T, constraints<std::enable_if_t<!builtin_signbit_is_constexpr_v<T>>> = nullptr>
constexpr auto signbit(T const num) noexcept -> bool {
  // ISO/IEC 60559 does not guarantee that this will work when num is a NaN, but it does work in practice on the
  // relevant platforms. Its correctness is checked in the unit tests.
  auto sign_carrier = ::arene::base::copysign(static_cast<T>(1.0), num);
  return sign_carrier == static_cast<T>(-1.0);
}

ARENE_IGNORE_END();

}  // namespace float_sign_detail

/// @brief Check the sign bit of a floating point number
/// @param num The number to check
/// @return @c true if @c num is *negative* (i.e. its sign bit is 1), @c false if @c num is positive
constexpr auto signbit(float const num) noexcept -> bool { return float_sign_detail::signbit(num); }

/// @brief Check the sign bit of a floating point number
/// @param num The number to check
/// @return @c true if @c num is *negative* (i.e. its sign bit is 1), @c false if @c num is positive
constexpr auto signbit(double const num) noexcept -> bool { return float_sign_detail::signbit(num); }

}  // namespace base
}  // namespace arene

// parasoft-end-suppress CERT_CPP-DCL51-f
// parasoft-end-suppress AUTOSAR-M17_0_3-a
// parasoft-end-suppress AUTOSAR-A7_1_5-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_SIGN_HPP_
