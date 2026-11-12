// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::functional_detail::flip"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_FLIP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_FLIP_HPP_

#include "arene/base/stdlib_choice/forward.hpp"

namespace arene {
namespace base {
namespace functional_detail {

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
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

}  // namespace functional_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_FLIP_HPP_
