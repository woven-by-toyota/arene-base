// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_CONDITIONAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_CONDITIONAL_HPP_

#include "arene/base/type_manipulation/static_if.hpp"  // IWYU pragma: export

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: declaration *is* preceeded by the @brief tag."
/// @brief lazily select between two types based on a boolean condition
/// @tparam B the boolean condition
/// @tparam T the type selected when @p B is @c true
/// @tparam F the type selected when @p B is @c false
///
/// Unlike @c std::conditional_t, this does not eagerly instantiate both
/// branches. Only the selected type is used; the other remains untouched.
///
/// @note Serves as a drop-in replacement for @c std::conditional_t, backed by
///   @c static_if for lazy evaluation.
/// @note A corresponding @c conditional class template is intentionally not
///   provided, to avoid unecessary class template instantiations. If the type is
///   necessary, use @c std::conditional.
template <bool B, class T, class F>
using conditional_t = typename static_if<B>::template then_else<T, F>;
// parasoft-end-suppress AUTOSAR-A2_7_3-a

/// @brief an identity alias intended for use as a no-op branch in
///   @c conditional_apply_t
/// @tparam T the type to return as-is
///
/// When used as the @c OnTrue or @c OnFalse template parameter of
/// @c conditional_apply_t, it passes the argument through unchanged.
///
/// @note @c std::type_identity_t is not suitable here because it is
/// defined in terms of a class template (@c std::type_identity<T>::type),
/// making it non-transparent. It cannot be redefined as a simple alias
/// without breaking its common use as a type deduction barrier.
template <class T>
using conditional_identity_t = T;

/// @brief lazily apply one of two template aliases based on a boolean
///   condition
/// @tparam B       the boolean condition
/// @tparam OnTrue  template alias applied to @p Args when @p B is @c true
/// @tparam OnFalse template alias applied to @p Args when @p B is @c false
/// @tparam Args    parameter pack forwarded to the selected template alias
///
/// Only the selected branch's template is instantiated with @p Args.
/// The other branch's template is never applied, avoiding unnecessary
/// instantiations and potential ill-formed types.
///
/// Example:
/// @code
/// // if T is void, apply std::add_pointer, otherwise return T as-is:
/// using result = conditional_apply_t<
///     std::is_void<T>::value,
///     std::add_pointer_t,
///     conditional_identity_t,
///     T
/// >;
/// @endcode
template <  //
    bool B,
    template <class...>
    class OnTrue,
    template <class...>
    class OnFalse,
    class... Args>
using conditional_apply_t = typename static_if<B>::template then_apply_else_apply<OnTrue, OnFalse, Args...>;

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_CONDITIONAL_HPP_
