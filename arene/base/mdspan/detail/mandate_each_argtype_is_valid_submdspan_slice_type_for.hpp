// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MANDATE_EACH_ARGTYPE_IS_VALID_SUBMDSPAN_SLICE_TYPE_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MANDATE_EACH_ARGTYPE_IS_VALID_SUBMDSPAN_SLICE_TYPE_FOR_HPP_

#include "arene/base/mdspan/detail/canonical_slice.hpp"
#include "arene/base/mdspan/detail/is_valid_submdspan_slice_type_for.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief helper function to generate a hard error if one argument type is not a valid submdspan slice for its extent
/// @tparam IndexType index type
/// @tparam SliceSpecifier slice type for @c Extent
/// @tparam Extent one individual extent within a larger pack of extents
/// @return always returns @c nullptr unless compilation fails entirely
template <class IndexType, class SliceSpecifier, std::size_t Extent>
constexpr auto mandate_one_argtype_is_valid_submdspan_slice_for() noexcept -> std::nullptr_t {
  static_assert(
      is_valid_submdspan_slice_type_for_v<canonical_slice_t<IndexType, SliceSpecifier>, Extent>,
      "This is not a valid submdspan slice for the given extent (usually because it's statically out of bounds)"
  );
  return nullptr;
}

/// @brief helper function to generate a hard error if any argument type is not a valid submdspan slice for an extent
/// @tparam IndexType index type
/// @tparam Extents pack of individual extents for each rank
/// @tparam SliceSpecifiers pack of slice types for each rank in <c>Extents</c>, deduced from the unnamed parameter
/// @return always returns @c nullptr unless compilation fails entirely
template <class IndexType, std::size_t... Extents, class... SliceSpecifiers>
constexpr auto mandate_each_argtype_is_valid_submdspan_slice_type_for(type_list<SliceSpecifiers...>) noexcept
    -> std::nullptr_t {
  consume_values({mandate_one_argtype_is_valid_submdspan_slice_for<IndexType, SliceSpecifiers, Extents>()...});
  return nullptr;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MANDATE_EACH_ARGTYPE_IS_VALID_SUBMDSPAN_SLICE_TYPE_FOR_HPP_
