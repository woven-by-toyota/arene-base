// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALWAYS_FALSE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALWAYS_FALSE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

/// @brief A trait which always is false, as a type-dependent expression.
///
/// @tparam T has no impact on the result, but must result in @c always_false_v<T> being a type-dependent expression.
///
/// This is generally used in order to allow having a "fallback" case in a template specialization set which triggers a
/// more meaningful error than "no definition found." Before C++23, the expression @c static_assert(false,"message")
/// will _unconditionally_ trigger a compiler error, even if the expression is in an uninstantiated template. @c
/// always_false_v<T> works around this by making the expression type-dependant, like so:
///
/// @snippet docs/examples/type_traits_examples.cpp always_false_usage_example
template <typename T>
constexpr bool always_false_v = false;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ALWAYS_FALSE_HPP_
