// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file forward_like.hpp
/// @brief Implementation of a backport of @c std::forward_like
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_FORWARD_LIKE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_FORWARD_LIKE_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/add_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/add_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/is_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_manipulation/give_qualifiers_to.hpp"
#include "arene/base/type_manipulation/static_if.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace forward_like_detail {

/// @brief implementation helper for @c arene::base::forward_like
/// @see arene::base::forward_like
template <typename From>
class forward_like {
  /// @brief Trait which deduces the return type based on From
  /// @tparam To the type which the qualifiers from From are applied to.
  /// @return A type which will be a reference of base type @c To with the following properties:
  ///         * If @c std::remove_reference<From> or @c std::remove_reference<To> is a const-qualified type, the type is
  ///           const-qualified.
  ///         * If @c From is an lvalue reference, then the return type is also an lvalue reference, otherwise it is an
  ///           rvalue reference.
  template <typename To>
  using forward_like_result_t =
      typename static_if<std::is_lvalue_reference<From>::value>::template then_apply_else_apply<
          std::add_lvalue_reference_t,
          std::add_rvalue_reference_t,
          give_qualifiers_to<std::remove_reference_t<From>, std::remove_reference_t<To>>>;

 public:
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Value is forwarded as appropriate"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Value is forwarded as appropriate"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Value is forwarded as appropriate"
  /// @brief applies forward_like_result_t to the input value reference.
  /// @tparam From The type to take the reference and qualification properties from.
  /// @tparam To The type to apply the reference and qualification properties to.
  /// @param val The value to apply the reference and qualification properties to.
  /// @return A reference of base type @c To with the following properties:
  ///         * If @c std::remove_reference<From> or @c std::remove_reference<To> is a const-qualified type, the type is
  ///           const-qualified.
  ///         * If @c From is an lvalue reference, then the return type is also an lvalue reference, otherwise it is an
  ///           rvalue reference.
  /// @see arene::base::forward_like
  template <typename To>
  ARENE_NODISCARD constexpr auto operator()(To&& val) const noexcept -> auto&& {
    return static_cast<forward_like_result_t<To>>(val);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
};

}  // namespace forward_like_detail

///
/// @brief A backport of C++23's [@c std::forward_like](https://en.cppreference.com/w/cpp/utility/forward_like) , it
///        allows forwarding a type with the same reference and const qualifications as another type.
///
/// @tparam From The type to take the reference and qualification properties from.
/// @tparam To The type to apply the reference and qualification properties to.
/// @param val The value to apply the reference and qualification properties to.
/// @return A reference of base type @c To with the following properties:
///         * If @c std::remove_reference<From> or @c std::remove_reference<To> is a const-qualified type, the type is
///           const-qualified.
///         * If @c From is an lvalue reference, then the return type is also an lvalue reference, otherwise it is an
///           rvalue reference.
template <typename From>
static constexpr auto const& forward_like =
    detail::cpp14_inline_static_const<forward_like_detail::forward_like<From>>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_FORWARD_LIKE_HPP_
