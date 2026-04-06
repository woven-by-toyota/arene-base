// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_TYPE_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_TYPE_TRAITS_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/pointer/.*"

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"

namespace arene {
namespace base {
namespace pointer {
namespace detail {

/// @brief Concept for determining if a type can be compared to nullptr.
/// @{
template <typename Pointer>
using use_comparison_to_nullptr = decltype(std::declval<Pointer>() != nullptr);
template <typename Pointer>
constexpr bool has_comparison_to_nullptr = substitution_succeeds<use_comparison_to_nullptr, Pointer>;
/// @}

/// @brief Concept for determining if a type has an dereference operator.
/// @{
template <typename Pointer>
using use_dereference_operator = decltype(*std::declval<Pointer>());
template <typename Pointer>
constexpr bool has_dereference_operator = substitution_succeeds<use_dereference_operator, Pointer>;
/// @}

/// @brief Concept for determining if a type has an arrow operator.
/// @{
template <typename Pointer>
using use_arrow_operator = decltype(std::declval<Pointer>().operator->());
template <typename Pointer>
constexpr bool has_arrow_operator =
    std::is_pointer<Pointer>::value || substitution_succeeds<use_arrow_operator, Pointer>;
/// @}

/// @brief Trait for determining if a type is "pointer-like".
/// @return true If @c Pointer is either a raw-pointer, or has all of: a valid comparison with @c nullptr , dereference
///         and arrow operators can be called against it
template <typename Pointer>
constexpr bool is_pointer_like_v =
    std::is_pointer<Pointer>::value ||
    (has_comparison_to_nullptr<Pointer> && has_dereference_operator<Pointer> && has_arrow_operator<Pointer>);

/// @brief Concept for determining if a type has a reset API.
/// @return true if @c Pointer::reset(With) is well-formed.
/// @{
template <typename Pointer, typename With = Pointer>
using use_reset = decltype(std::declval<Pointer>().reset(std::declval<With>()));
template <typename Pointer, typename With = Pointer>
constexpr bool has_reset = substitution_succeeds<use_reset, Pointer, With>;
/// @}

}  // namespace detail
}  // namespace pointer
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_TYPE_TRAITS_HPP_
