// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::swap"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_SWAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_SWAP_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace swap_detail {

/// @brief Poison pill definition of @c swap
///
/// This is to ensure that @c swap_fn_impl only looks for a function declaration
/// and does not pick up some other global object named @c swap
///
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
template <class>
auto swap() -> void = delete;
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

/// @brief CPO implementation of @c arene::base::swap .
/// @see arene::base::swap
class swap_fn_impl {
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  ///
  /// @brief specialization for when the input does not have an ADL-discoverable @c swap implementation.
  ///
  /// @tparam T type of the elements to swap. Must satisfy @c is_default_swappable_v .
  /// @param lhs element to swap
  /// @param rhs element to swap
  /// @post @c lhs 's state will be as if it had been move-assigned from @c rhs , and vise-versa.
  template <typename T, constraints<std::enable_if_t<is_default_swappable_v<T>>> = nullptr>
  static constexpr void default_swap(T& lhs, T& rhs) noexcept(is_nothrow_default_swappable_v<T>) {
    auto temp = std::move(lhs);
    lhs = std::move(rhs);
    rhs = std::move(temp);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

 public:
  ///
  /// @brief Overload for non-c-array types that provide ADL swap.
  ///
  /// @tparam T type of the elements to swap. Must satisfy @c is_swappable_v .
  /// @param lhs element to swap.
  /// @param rhs element to swap.
  /// @post @c lhs and @c rhs will have had their states exchanged via an unqualified call to @c swap.
  ///
  template <typename T, constraints<std::enable_if_t<is_adl_swappable_v<T>>> = nullptr>
  constexpr void operator()(T& lhs, T& rhs) const noexcept(noexcept(swap(lhs, rhs))) {
    swap(lhs, rhs);
  }
  ///
  /// @brief Overload for non-c-array types that do not provide ADL swap.
  ///
  /// @tparam T type of the elements to swap. Must satisfy @c is_swappable_v .
  /// @param lhs element to swap.
  /// @param rhs element to swap.
  /// @post @c lhs and @c rhs will have had their states exchanged via an unqualified call to @c swap.
  ///
  template <
      typename T,
      constraints<std::enable_if_t<!is_adl_swappable_v<T>>, std::enable_if_t<is_default_swappable_v<T>>> = nullptr>
  constexpr void operator()(T& lhs, T& rhs) const noexcept(noexcept(default_swap(lhs, rhs))) {
    default_swap(lhs, rhs);
  }

  ///
  /// @brief Overload for c-array types.
  ///
  /// @tparam T type of the elements to swap. Must satisfy @c is_swappable_v .
  /// @param lhs c-array of elements to swap.
  /// @param rhs c-array elements to swap.
  /// @post @c lhs and @c rhs will have their elements, in order, exchanged via an unqualified call to @c swap.
  ///
  template <class T, std::size_t N, constraints<std::enable_if_t<is_swappable_v<T>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) explicitly providing c-array support
  constexpr void operator()(T (&lhs)[N], T (&rhs)[N]) const noexcept(is_nothrow_swappable_v<T>) {
    for (std::size_t idx{}; idx < N; ++idx) {
      (*this)(lhs[idx], rhs[idx]);
    }
  }
};

}  // namespace swap_detail

inline namespace swap_cpo {
///
/// @brief A Customization Point Object implementation of the @c constexpr compatible @c std::swap from C++17.
///
/// @tparam T type of the elements to swap.
/// @param lhs element(s) to swap.
/// @param rhs element(s) to swap.
/// @post @c lhs and @c rhs will have had their states exchanged by one of the described methods.
///
/// Functionally equivalent to @c std::swap , including support for c-arrays. Unlike @c std::swap , @c arene::base::swap
/// is implemented as a _Customization Point Object_. This means it has the following properties:
///
/// 1. It supports user customizations directly, performing discovery of @c swap customizations automatically.
///    Customizations are given the following preference:
///    1. ADL-discovered @c swap(lhs,rhs) , if it exists.
///    2. The default swap implementation, equivalent to @c std::swap .
/// 2. It is a function object, so it can be used directly in APIs which consume functors without the need of a wrapper.
/// 3. It will never be discoverable via ADL. This avoids ambiguous overload resolution problems with types in
///    @c arene::base which provide customizations of @c swap .
///
/// This allow simplification of user code. When using @c std::swap in a generic context, best practice requires the
/// following pattern in order to allow ADL discovery of @c swap customizations while falling back to the generic @c
/// @c std::swap implementation for the default case:
///
/// @snippet docs/examples/utility_examples.cpp swap_old_usage_example

// parasoft-begin-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"
/// This is easy to forget. With @c arene::base::swap , this becomes simply:
///
/// @snippet docs/examples/utility_examples.cpp swap_new_usage_example
///
///
// Marked maybe unused because gcc doesn't see through this alias being called for some reason.
ARENE_MAYBE_UNUSED static constexpr auto const& swap =
    ::arene::base::detail::cpp14_inline_static_const<swap_detail::swap_fn_impl>::value;
// parasoft-end-suppress CERT_CPP-DCL56-a-3
}  // namespace swap_cpo

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_SWAP_HPP_
