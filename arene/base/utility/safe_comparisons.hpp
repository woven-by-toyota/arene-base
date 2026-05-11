// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file safe_comparisons.hpp
/// @brief Provides backports of the safe integer comparison functions from C++20
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_SAFE_COMPARISONS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_SAFE_COMPARISONS_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M4_5_3-a "Type is used as an integral type here, not a character"

namespace arene {
namespace base {

namespace safe_comparisons_detail {

///
/// @brief Helper to determine if two integral types have the same signedness
///
/// @tparam T One of the types
/// @tparam U The other type
/// @return bool Equivalent to @c std::is_signed<T>::value==std::is_signed<U>::value
///
template <typename T, typename U>
static constexpr bool is_same_signedness_v = std::is_signed<T>::value == std::is_signed<U>::value;

///
/// @brief Compares two operands of the same signdness for equality
///
/// @tparam T Type of the left hand operand.
/// @tparam U Type of the right hand operand.
/// @param lhs The left hand operand.
/// @param rhs The right hand operand.
/// @return bool Equivalent to direct comparison of the operands.
///
template <typename T, typename U, constraints<std::enable_if_t<is_same_signedness_v<T, U>>> = nullptr>
ARENE_NODISCARD constexpr auto do_cmp_equal(T const lhs, U const rhs) noexcept -> bool {
  return lhs == rhs;
}

///
/// @brief Compares a signed operand with an unsigned operand for equality.
///
/// @tparam T Type of the left hand operand.
/// @tparam U Type of the right hand operand.
/// @param lhs The left hand operand.
/// @param rhs The right hand operand.
/// @return false If @c lhs<0, or if @c lhs==rhs is false when @c lhs is cast to its unsigned equivalent.
/// @return true If @c lhs>=0 and @c lhs==rhs when @c lhs is cast to its unsigned equivalent.
///
template <
    typename T,
    typename U,
    constraints<std::enable_if_t<std::is_signed<T>::value>, std::enable_if_t<std::is_unsigned<U>::value>> = nullptr>
ARENE_NODISCARD constexpr auto do_cmp_equal(T const lhs, U const rhs) noexcept -> bool {
  if (lhs < 0) {
    return false;
  }
  return static_cast<std::make_unsigned_t<T>>(lhs) == rhs;
}

///
/// @brief Compares a unsigned operand with an signed operand for equality.
///
/// @tparam T Type of the left hand operand.
/// @tparam U Type of the right hand operand.
/// @param lhs The left hand operand.
/// @param rhs The right hand operand.
/// @return false If @c rhs<0, or if @c lhs==rhs is false when @c rhs is cast to its unsigned equivalent.
/// @return true If @c rhs>=0 and @c lhs==rhs when @c rhs is cast to its unsigned equivalent.
///
template <
    typename T,
    typename U,
    constraints<std::enable_if_t<std::is_unsigned<T>::value>, std::enable_if_t<std::is_signed<U>::value>> = nullptr>
ARENE_NODISCARD constexpr auto do_cmp_equal(T const lhs, U const rhs) noexcept -> bool {
  return do_cmp_equal(rhs, lhs);
}

///
/// @brief Compares two operands of the same signdness for less-than
///
/// @tparam T Type of the left hand operand.
/// @tparam U Type of the right hand operand.
/// @param lhs The left hand operand.
/// @param rhs The right hand operand.
/// @return bool Equivalent to direct comparison of the operands.
///
template <typename T, typename U, constraints<std::enable_if_t<is_same_signedness_v<T, U>>> = nullptr>
ARENE_NODISCARD constexpr auto do_cmp_less(T const lhs, U const rhs) noexcept -> bool {
  // parasoft-begin-suppress AUTOSAR-M4_5_1-a "This function handles generic integer types, including bool"
  // parasoft-begin-suppress CERT_C-EXP46-b "This function handles generic integer types, including bool"
  return lhs < rhs;
  // parasoft-end-suppress AUTOSAR-M4_5_1-a
  // parasoft-end-suppress CERT_C-EXP46-b
}

///
/// @brief Compares a signed operand with an unsigned operand for less-than.
///
/// @tparam T Type of the left hand operand.
/// @tparam U Type of the right hand operand.
/// @param lhs The left hand operand.
/// @param rhs The right hand operand.
/// @return false If @c lhs>=0 and @c lhs<rhs is false when @c lhs is cast to its unsigned equivalent.
/// @return true If @c lhs<0 or @c lhs<rhs when @c lhs is cast to its unsigned equivalent.
///
template <
    typename T,
    typename U,
    constraints<std::enable_if_t<std::is_signed<T>::value>, std::enable_if_t<std::is_unsigned<U>::value>> = nullptr>
ARENE_NODISCARD constexpr auto do_cmp_less(T const lhs, U const rhs) noexcept -> bool {
  if (lhs < 0) {
    return true;
  }
  // parasoft-begin-suppress AUTOSAR-M4_5_1-a "This function handles generic integer types, including bool"
  // parasoft-begin-suppress CERT_C-EXP46-b "This function handles generic integer types, including bool"
  return static_cast<std::make_unsigned_t<T>>(lhs) < rhs;
  // parasoft-end-suppress AUTOSAR-M4_5_1-a
  // parasoft-end-suppress CERT_C-EXP46-b
}

///
/// @brief Compares a signed operand with an unsigned operand for less-than.
///
/// @tparam T Type of the left hand operand.
/// @tparam U Type of the right hand operand.
/// @param lhs The left hand operand.
/// @param rhs The right hand operand.
/// @return false If @c rhs<0 or @c lhs<rhs is false when @c rhs is cast to its unsigned equivalent.
/// @return true If @c rhs>=0 and @c lhs<rhs when @c rhs is cast to its unsigned equivalent.
///
template <
    typename T,
    typename U,
    constraints<std::enable_if_t<std::is_unsigned<T>::value>, std::enable_if_t<std::is_signed<U>::value>> = nullptr>
ARENE_NODISCARD constexpr auto do_cmp_less(T const lhs, U const rhs) noexcept -> bool {
  if (rhs < 0) {
    return false;
  }
  // parasoft-begin-suppress AUTOSAR-M4_5_1-a "This function handles generic integer types, including bool"
  // parasoft-begin-suppress CERT_C-EXP46-b "This function handles generic integer types, including bool"
  return lhs < static_cast<std::make_unsigned_t<U>>(rhs);
  // parasoft-end-suppress AUTOSAR-M4_5_1-a
  // parasoft-end-suppress CERT_C-EXP46-b
}

///
/// @brief Function object that implements @c cmp_equal
///
class cmp_equal_fn {
 public:
  ///
  /// @brief Compares two integral types for equality in a mixed-signed-unsigned-type safe manner.
  ///
  /// If either @c T or @c U do not have the same signedness, then if the signed-value is non-negative, it is converted
  /// into an unsigned type via @c std::make_unsigned_t to perform the comparison.
  ///
  /// @tparam T Type of the left hand operand.
  /// @tparam U Type of the right hand operand.
  /// @param lhs The left hand operand.
  /// @param rhs The right hand operand.
  /// @return false If @c T and @c U do not have the same signed-ness and the corresponding operand is negative, or
  ///         common-frame @c lhs==rhs is @c false .
  /// @return true If @c T and @c U are both positive and common-frame @c lhs==rhs is @c true .
  ///
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_integral<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(T const lhs, U const rhs) const noexcept -> bool {
    return do_cmp_equal(lhs, rhs);
  }
};

///
/// @brief Implementation helper for @c cmp_not_equal
///
class cmp_not_equal_fn {
 public:
  ///
  /// @brief Compares two integral types for inequality in a mixed-signed-unsigned-type safe manner.
  ///
  /// If either @c T or @c U do not have the same signedness, then if the signed-value is non-negative, it is converted
  /// into an unsigned type via @c std::make_unsigned_t to perform the comparison.
  ///
  /// @tparam T Type of the left hand operand.
  /// @tparam U Type of the right hand operand.
  /// @param lhs The left hand operand.
  /// @param rhs The right hand operand.
  /// @return false If @c T and @c U are both positive and common-frame @c lhs!=rhs is @c false .
  /// @return true If @c T and @c U do not have the same signed-ness and the corresponding operand is negative, or
  ///         common-frame @c lhs!=rhs is @c true .
  ///
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_integral<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(T const lhs, U const rhs) const noexcept -> bool {
    return !do_cmp_equal(lhs, rhs);
  }
};

///
/// @brief Implementation helper for @c cmp_less
///
class cmp_less_fn {
 public:
  ///
  /// @brief Compares two integral types for less-than in a mixed-signed-unsigned-type safe manner.
  ///
  /// If either @c T or @c U do not have the same signedness, then if the signed-value is non-negative, it is converted
  /// into an unsigned type via @c std::make_unsigned_t to perform the comparison.
  ///
  /// @tparam T Type of the left hand operand.
  /// @tparam U Type of the right hand operand.
  /// @param lhs The left hand operand.
  /// @param rhs The right hand operand.
  /// @return false If @c U is signed and negative while @c T is unsigned, or if common-frame @c lhs<rhs is false
  /// @return true If @c T is signed and negative while @c U is unsigned, or if common-frame @c lhs<rhs is true
  ///
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_integral<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(T const lhs, U const rhs) const noexcept -> bool {
    return do_cmp_less(lhs, rhs);
  }
};

///
/// @brief Implementation helper for @c cmp_greater
///
class cmp_greater_fn {
 public:
  ///
  /// @brief Compares two integral types for greater-than in a mixed-signed-unsigned-type safe manner.
  ///
  /// If either @c T or @c U do not have the same signedness, then if the signed-value is non-negative, it is converted
  /// into an unsigned type via @c std::make_unsigned_t to perform the comparison.
  ///
  /// @tparam T Type of the left hand operand.
  /// @tparam U Type of the right hand operand.
  /// @param lhs The left hand operand.
  /// @param rhs The right hand operand.
  /// @return false If @c T is signed and negative while @c U is unsigned, or if common-frame @c lhs>rhs is false
  /// @return true If @c U is signed and negative while @c T is unsigned, or if common-frame @c lhs>rhs is true
  ///
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_integral<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(T const lhs, U const rhs) const noexcept -> bool {
    return do_cmp_less(rhs, lhs);
  }
};

///
/// @brief Implementation helper for @c cmp_less_equal_fn
///
class cmp_less_equal_fn {
 public:
  ///
  /// @brief Compares two integral types for less-than or equals in a mixed-signed-unsigned-type safe manner.
  ///
  /// If either @c T or @c U do not have the same signedness, then if the signed-value is non-negative, it is converted
  /// into an unsigned type via @c std::make_unsigned_t to perform the comparison.
  ///
  /// @tparam T Type of the left hand operand.
  /// @tparam U Type of the right hand operand.
  /// @param lhs The left hand operand.
  /// @param rhs The right hand operand.
  /// @return false If @c U is signed and negative while @c T is unsigned, or if common-frame @c lhs<=rhs is false
  /// @return true If @c T is signed and negative while @c U is unsigned, or if common-frame @c lhs<=rhs is true
  ///
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_integral<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(T const lhs, U const rhs) const noexcept -> bool {
    return !do_cmp_less(rhs, lhs);
  }
};

///
/// @brief Implementation helper for @c cmp_greater_equal_fn
///
class cmp_greater_equal_fn {
 public:
  ///
  /// @brief Compares two integral types for greater-than or equals in a mixed-signed-unsigned-type safe manner.
  ///
  /// If either @c T or @c U do not have the same signedness, then if the signed-value is non-negative, it is converted
  /// into an unsigned type via @c std::make_unsigned_t to perform the comparison.
  ///
  /// @tparam T Type of the left hand operand.
  /// @tparam U Type of the right hand operand.
  /// @param lhs The left hand operand.
  /// @param rhs The right hand operand.
  /// @return false If @c T is signed and negative while @c U is unsigned, or if common-frame @c lhs>=rhs is false
  /// @return true If @c U is signed and negative while @c T is unsigned, or if common-frame @c lhs>=rhs is true
  ///
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_integral<U>::value>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(T const lhs, U const rhs) const noexcept -> bool {
    return !do_cmp_less(lhs, rhs);
  }
};

}  // namespace safe_comparisons_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
/// @def arene::base::cmp_equal
/// @copydoc arene::base::safe_comparisons_detail::cmp_equal_fn::operator()
ARENE_CPP14_INLINE_VARIABLE(safe_comparisons_detail::cmp_equal_fn, cmp_equal);

/// @def arene::base::cmp_not_equal
/// @copydoc arene::base::safe_comparisons_detail::cmp_not_equal::operator()
ARENE_CPP14_INLINE_VARIABLE(safe_comparisons_detail::cmp_not_equal_fn, cmp_not_equal);

/// @def arene::base::cmp_less
/// @copydoc arene::base::safe_comparisons_detail::cmp_less::operator()
ARENE_CPP14_INLINE_VARIABLE(safe_comparisons_detail::cmp_less_fn, cmp_less);

/// @def arene::base::cmp_less_equal
/// @copydoc arene::base::safe_comparisons_detail::cmp_less_equal::operator()
ARENE_CPP14_INLINE_VARIABLE(safe_comparisons_detail::cmp_less_equal_fn, cmp_less_equal);

/// @def arene::base::cmp_greater
/// @copydoc arene::base::safe_comparisons_detail::cmp_greater::operator()
ARENE_CPP14_INLINE_VARIABLE(safe_comparisons_detail::cmp_greater_fn, cmp_greater);

/// @def arene::base::cmp_greater_equal
/// @copydoc arene::base::safe_comparisons_detail::cmp_greater_equal::operator()
ARENE_CPP14_INLINE_VARIABLE(safe_comparisons_detail::cmp_greater_equal_fn, cmp_greater_equal);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M4_5_3-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_SAFE_COMPARISONS_HPP_
