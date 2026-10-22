// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines
// arene::base::functional_detail::operator_less"
//
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_OPERATOR_LESS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_OPERATOR_LESS_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"

namespace arene {
namespace base {
namespace functional_detail {

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

/// @def arene::base::functional_detail::operator_less
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

}  // namespace functional_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_OPERATOR_LESS_HPP_
