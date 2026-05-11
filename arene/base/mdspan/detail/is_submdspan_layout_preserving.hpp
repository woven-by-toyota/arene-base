// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_SUBMDSPAN_LAYOUT_PRESERVING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_SUBMDSPAN_LAYOUT_PRESERVING_HPP_

// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

#include "arene/base/mdspan/detail/is_unit_stride_slice_type.hpp"
#include "arene/base/mdspan/detail/layout_common.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/reverse.hpp"
#include "arene/base/type_list/take.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/all_of.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

namespace is_layout_preserving_detail {

/// @brief check if all types are @c full_extent_t
/// @tparam CanonicalSlices canonical slice types
template <class... CanonicalSlices>
class all_full_extent
    : public std::integral_constant<bool, all_of_v<std::is_same<CanonicalSlices, full_extent_t>::value...>> {};

/// @brief check if the first @c N types in a type list are all @c full_extent_t
/// @tparam N number of leading slices to check
/// @tparam SliceList type list of canonical slice types
template <std::size_t N, class SliceList>
extern constexpr bool leading_slices_are_full_extent_v =
    type_lists::apply_all_t<type_lists::take_t<SliceList, N>, all_full_extent>::value;

/// @brief determine if leading slices and a unit-stride boundary preserve a contiguous layout
/// @tparam SubRank rank of the resulting sub-extents
/// @tparam SliceList type list of canonical slice types in the order to check
///
/// The layout is preserved when @c SubRank is zero, or when every leading
/// dimension (indices 0 through @c SubRank-2) has a @c full_extent_t
/// slice and the boundary dimension (@c SubRank-1) has a unit-stride
/// slice type.
template <std::size_t SubRank, class SliceList>
extern constexpr bool is_preserving_v = leading_slices_are_full_extent_v<SubRank - 1, SliceList> &&
                                        is_unit_stride_slice_type_v<type_lists::at_t<SliceList, SubRank - 1>>;

/// @brief specialization for rank-zero result
/// @tparam SliceList type list of canonical slice types
template <class SliceList>
extern constexpr bool is_preserving_v<0, SliceList> = true;

}  // namespace is_layout_preserving_detail

/// @brief determine if a submdspan mapping can preserve a given layout
/// @tparam Layout the layout policy type (@c layout_left or @c layout_right)
/// @tparam SubRank rank of the resulting sub-extents
/// @tparam CanonicalSlices canonical slice types
template <class Layout, std::size_t SubRank, class... CanonicalSlices>
extern constexpr bool is_submdspan_layout_preserving_v = false;

/// @brief @c layout_left specialization - slices are already in left-to-right order
/// @tparam SubRank rank of the resulting sub-extents
/// @tparam CanonicalSlices canonical slice types
///
/// The layout is preserved when @c SubRank is zero, or when every leading
/// source dimension (indices 0 through @c SubRank-2) has a @c full_extent_t
/// slice and the boundary source dimension (@c SubRank-1) has a unit-stride
/// slice type.
template <std::size_t SubRank, class... CanonicalSlices>
extern constexpr bool is_submdspan_layout_preserving_v<layout_left, SubRank, CanonicalSlices...> =
    is_layout_preserving_detail::is_preserving_v<SubRank, type_list<CanonicalSlices...>>;

/// @brief @c layout_right specialization - reverses the slice pack then applies the same check
/// @tparam SubRank rank of the resulting sub-extents
/// @tparam CanonicalSlices canonical slice types
///
/// The layout is preserved when @c SubRank is zero, or when every trailing
/// source dimension (indices <c>rank()-SubRank+1</c> through <c>rank()-1</c>)
/// has a @c full_extent_t slice and the boundary source dimension
/// (<c>rank()-SubRank</c>) has a unit-stride slice type.
///
/// Implemented by reversing the slice pack and delegating to the left-order check.
template <std::size_t SubRank, class... CanonicalSlices>
extern constexpr bool is_submdspan_layout_preserving_v<layout_right, SubRank, CanonicalSlices...> =
    is_layout_preserving_detail::is_preserving_v<SubRank, type_lists::reverse_t<type_list<CanonicalSlices...>>>;

/// @brief trait type for @c is_submdspan_layout_preserving_v
/// @tparam Layout the layout policy type (@c layout_left or @c layout_right)
/// @tparam SubRank rank of the resulting sub-extents
/// @tparam CanonicalSlices canonical slice types
template <class Layout, std::size_t SubRank, class... CanonicalSlices>
class is_submdspan_layout_preserving
    : public std::integral_constant<bool, is_submdspan_layout_preserving_v<Layout, SubRank, CanonicalSlices...>> {};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_SUBMDSPAN_LAYOUT_PRESERVING_HPP_
