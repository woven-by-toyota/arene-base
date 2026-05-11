// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_FUNCTIONAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_FUNCTIONAL_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace algorithm_detail {

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: There is no template constructor"

/// @brief function object adaptor that reverses argument order
/// @tparam F function object type
///
/// Returns a function object which adapts @c func by reversing the order of
/// arguments as they are passed. This function returns an object that contains
/// references and is intended to be short lived (e.g. used within the
/// implementation of algorithms).
///
/// @note The lifetime of the returned value must be bound by lifetime of @c
///     func.
///
template <class F>
class flip_wrapper {
  // NOLINTBEGIN(cppcoreguidelines-avoid-const-or-ref-data-members)

  /// @brief function object to partially invoke
  ///
  F& func_;

  // NOLINTEND(cppcoreguidelines-avoid-const-or-ref-data-members)

 public:
  /// @brief construct a @c flip_wrapper
  /// @param func function to invoke
  ///
  constexpr explicit flip_wrapper(F& func) noexcept
      : func_{func} {}

  /// @brief invoke the contained function object with reversed arguments
  /// @tparam Arg1 first argument type
  /// @tparam Arg2 second argument type
  /// @param arg1 first argument
  /// @param arg2 second argument
  /// @return @c func_(arg2, arg1) with appropriate forwarding
  ///
  template <class Arg1, class Arg2>
  constexpr auto operator()(Arg1&& arg1, Arg2&& arg2) const
      noexcept(noexcept(func_(std::forward<Arg2>(arg2), std::forward<Arg1>(arg1))))
          -> decltype(func_(std::forward<Arg2>(arg2), std::forward<Arg1>(arg1))) {
    return func_(std::forward<Arg2>(arg2), std::forward<Arg1>(arg1));
  }
};

// parasoft-end-suppress AUTOSAR-A14_5_1-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief adapts a binary function object to reverse argument order
/// @tparam F function object type
/// @param func function object to adapt
///
/// Returns a function object which adapts @c func by reversing the order of
/// arguments as they are passed. This function returns an object that contains
/// references and is intended to be short lived (e.g. used within the
/// implementation of algorithms).
///
/// @return function object with a reversed argument order compared to @c func.
///
/// @note The lifetime of the returned value must be bound by lifetime of @c
///     func.
///
template <class F>
constexpr auto flip(F& func) noexcept -> flip_wrapper<F> {
  return flip_wrapper<F>{func};
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: Declaration *is* preceeded by the @brief tag."
/// @brief function object used to invoke @c operator<
///
/// @note This function object is designed to be used in the implementation of
///     C++14/C++17 algorithms, where the arguments will always be a reference.
///
class operator_less_t {
 public:
  /// @brief determine if one argument is less than another
  /// @tparam T1 first argument type
  /// @tparam T2 second argument type
  /// @param lhs first argument
  /// @param rhs second argument
  /// @return <tt>lhs < rhs</tt>
  ///
  template <class T1, class T2>
  constexpr auto operator()(T1&& lhs, T2&& rhs) const noexcept(noexcept(std::declval<T1&&>() < std::declval<T2&&>()))
      -> decltype(std::forward<T1>(lhs) < std::forward<T2>(rhs)) {
    return std::forward<T1>(lhs) < std::forward<T2>(rhs);
  }
};
// parasoft-end-suppress AUTOSAR-A2_7_3-a

/// @brief function object used to invoke @c operator==
///
/// @note This function object is designed to be used in the implementation of
///     C++14/C++17 algorithms, where the arguments will always be a reference.
///
class operator_equal_t {
 public:
  /// @brief determine if one argument is equal to another
  /// @tparam T1 first argument type
  /// @tparam T2 second argument type
  /// @param lhs first argument
  /// @param rhs second argument
  /// @return <tt>lhs == rhs</tt>
  ///
  template <class T1, class T2>
  constexpr auto operator()(T1&& lhs, T2&& rhs) const noexcept(noexcept(std::declval<T1&&>() == std::declval<T2&&>()))
      -> decltype(std::forward<T1>(lhs) == std::forward<T2>(rhs)) {
    return std::forward<T1>(lhs) == std::forward<T2>(rhs);
  }
};

// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variables are initialized"

/// @def arene::base::algorithm_detail::operator_less
/// @brief function object used to invoke @c operator<
///
/// @note This function object is designed to be used in the implementation of
///     C++14/C++17 algorithms, where the arguments will always be a reference.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(operator_less_t, operator_less);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @def arene::base::algorithm_detail::operator_equal
/// @brief function object used to invoke @c operator==
///
/// @note This function object is designed to be used in the implementation of
///     C++14/C++17 algorithms, where the arguments will always be a reference.
///
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(operator_equal_t, operator_equal);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

// parasoft-end-suppress CERT_CPP-DCL56-a

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_FUNCTIONAL_HPP_
