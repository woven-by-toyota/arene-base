// Copyright 2025, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_PROPERTIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_PROPERTIES_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
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
template <typename T, constraints<std::enable_if_t<std::is_floating_point<T>::value>> = nullptr>
constexpr auto isnan(T const num) noexcept -> bool {
  // NOLINTNEXTLINE(misc-redundant-expression) This expression is not trivially false for floating point numbers
  return num != num;
}

/// @brief Check whether or not the argument has a (positive or negative) infinite value
/// @tparam T Type of the number to check
/// @param num The number to check
/// @return @c true if @c num has an infinite value, otherwise @c false
template <typename T, constraints<std::enable_if_t<std::is_floating_point<T>::value>> = nullptr>
constexpr auto isinf(T const num) noexcept -> bool {
  return (num == std::numeric_limits<T>::infinity()) || (num == -std::numeric_limits<T>::infinity());
}

/// @brief Check whether or not the argument is finite, i.e. not infinity or NaN
/// @tparam T Type of the number to check
/// @param num The number to check
/// @return @c true if @c num has a finite value, otherwise @c false
template <typename T, constraints<std::enable_if_t<std::is_floating_point<T>::value>> = nullptr>
constexpr auto isfinite(T const num) noexcept -> bool {
  return (!::arene::base::float_properties_detail::isnan(num)) && (!::arene::base::float_properties_detail::isinf(num));
}

/// @brief Check whether or not the argument is normal, i.e. not infinity, NaN, subnormal, or zero
/// @tparam T Type of the number to check
/// @param num The number to check
/// @return @c true if @c num has a normal value, otherwise @c false
template <typename T, constraints<std::enable_if_t<std::is_floating_point<T>::value>> = nullptr>
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

/// @brief Implementation for the @c isnan CPO
class isnan_impl {
 public:
  /// @brief Check whether or not the argument is a NaN value
  /// @tparam T Type of the value to check
  /// @param value Value to check
  /// @return @c isnan(std::forward<T>(value)) using ordinary lookup combined with ADL
  template <typename T>
  constexpr auto operator()(T&& value) const noexcept(noexcept(isnan(std::forward<T>(value))))
      -> decltype(static_cast<void>(isnan(std::forward<T>(value))), bool{}) {
    // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(isnan(std::forward<T>(value))), bool>::value,
        "ADL overload of isnan must return bool"
    );
    // parasoft-end-suppress CERT_C-PRE31-c
    return isnan(std::forward<T>(value));
  }
};

/// @brief Implementation for the @c isinf CPO
class isinf_impl {
 public:
  /// @brief Check whether or not the argument has a (positive or negative) infinite value
  /// @tparam T Type of the value to check
  /// @param value Value to check
  /// @return @c isinf(std::forward<T>(value)) using ordinary lookup combined with ADL
  template <typename T>
  constexpr auto operator()(T&& value) const noexcept(noexcept(isinf(std::forward<T>(value))))
      -> decltype(static_cast<void>(isinf(std::forward<T>(value))), bool{}) {
    // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(isinf(std::forward<T>(value))), bool>::value,
        "ADL overload of isinf must return bool"
    );
    // parasoft-end-suppress CERT_C-PRE31-c
    return isinf(std::forward<T>(value));
  }
};

/// @brief Implementation for the @c isfinite CPO
class isfinite_impl {
 public:
  /// @brief Check whether or not the argument is finite, i.e. not infinity or NaN
  /// @tparam T Type of the value to check
  /// @param value Value to check
  /// @return @c isfinite(std::forward<T>(value)) using ordinary lookup combined with ADL
  template <typename T>
  constexpr auto operator()(T&& value) const noexcept(noexcept(isfinite(std::forward<T>(value))))
      -> decltype(static_cast<void>(isfinite(std::forward<T>(value))), bool{}) {
    // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(isfinite(std::forward<T>(value))), bool>::value,
        "ADL overload of isfinite must return bool"
    );
    // parasoft-end-suppress CERT_C-PRE31-c
    return isfinite(std::forward<T>(value));
  }
};

/// @brief Implementation for the @c isnormal CPO
class isnormal_impl {
 public:
  /// @brief Check whether or not the argument is normal, i.e. not infinity, NaN, subnormal, or zero
  /// @tparam T Type of the value to check
  /// @param value Value to check
  /// @return @c isnormal(std::forward<T>(value)) using ordinary lookup combined with ADL
  template <typename T>
  constexpr auto operator()(T&& value) const noexcept(noexcept(isnormal(std::forward<T>(value))))
      -> decltype(static_cast<void>(isnormal(std::forward<T>(value))), bool{}) {
    // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        std::is_same<decltype(isnormal(std::forward<T>(value))), bool>::value,
        "ADL overload of isnormal must return bool"
    );
    // parasoft-end-suppress CERT_C-PRE31-c
    return isnormal(std::forward<T>(value));
  }
};

}  // namespace float_properties_detail

// parasoft-end-suppress AUTOSAR-M0_1_2-d
// parasoft-end-suppress AUTOSAR-M6_2_2-a

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
/// @brief Check whether a value is NaN using ordinary lookup and ADL
ARENE_CPP14_INLINE_VARIABLE(float_properties_detail::isnan_impl, isnan);

/// @brief Check whether a value is infinite using ordinary lookup and ADL
ARENE_CPP14_INLINE_VARIABLE(float_properties_detail::isinf_impl, isinf);

/// @brief Check whether a value is finite using ordinary lookup and ADL
ARENE_CPP14_INLINE_VARIABLE(float_properties_detail::isfinite_impl, isfinite);

/// @brief Check whether a value is normal using ordinary lookup and ADL
ARENE_CPP14_INLINE_VARIABLE(float_properties_detail::isnormal_impl, isnormal);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress CERT_CPP-DCL51-f
// parasoft-end-suppress AUTOSAR-A7_1_5-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_FLOAT_PROPERTIES_HPP_
