// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::mdspan_detail::deduced_static_extent"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DEDUCED_STATIC_EXTENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DEDUCED_STATIC_EXTENT_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_integral_constant_like.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief deduce the static extent value of an integral-ish parameter
/// @tparam Type type to deduce the static extent from
///
/// Primary template that always returns @c dynamic_extent.
///
template <class Type, class = constraints<>>
static constexpr auto deduced_static_extent_v = dynamic_extent;

/// @brief deduce the static extent value of an integral-ish parameter
/// @tparam Type integral type
/// @tparam Value static extent value
///
/// Specialization the provides the static extent value.
///
template <class Type>
static constexpr auto deduced_static_extent_v<  //
    Type,
    constraints<std::enable_if_t<is_integral_constant_like_v<Type>>>> = std::size_t{Type::value};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DEDUCED_STATIC_EXTENT_HPP_
