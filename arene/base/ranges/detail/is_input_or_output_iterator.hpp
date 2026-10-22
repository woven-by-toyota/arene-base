#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_INPUT_OR_OUTPUT_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_INPUT_OR_OUTPUT_ITERATOR_HPP_

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/is_weakly_incrementable.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_void.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace ranges {
namespace detail {
/// @brief Helper alias to check if a type has a valid dereference operator.
/// @tparam Itr The type to check.
template <typename Itr>
using use_dereference = decltype(*std::declval<Itr&>());

/// @brief Checks if a given type has a valid dereference operator.
/// @tparam Itr The type to check.
/// @return bool If @c Itr has a @c operator* that is does not return @c void then @c true , else @c false.
template <typename Itr, bool = substitution_succeeds<use_dereference, Itr>>
extern constexpr bool has_valid_dereference_v = false;

/// @brief Checks if a given type has a valid dereference operator.
/// @tparam Itr The type to check.
/// @return bool If @c Itr has a @c operator* that is does not return @c void then @c true , else @c false.
template <typename Itr>
extern constexpr bool has_valid_dereference_v<Itr, true> = !std::is_void<use_dereference<Itr>>::value;

/// @brief Checks if a given type models an input or output iterator as defined by @c std::is_input_or_output_iterator
/// @tparam Itr The type to check.
/// @return bool If @c Itr models @c is_weakly_incrementable and has a valid dereference operator then @c true ,
///         else @c false.
template <typename Itr>
extern constexpr bool is_input_or_output_iterator_v = is_weakly_incrementable_v<Itr> && has_valid_dereference_v<Itr>;
}  // namespace detail

}  // namespace ranges
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DETAIL_IS_INPUT_OR_OUTPUT_ITERATOR_HPP_
