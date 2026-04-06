// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_VALID_SUBMDSPAN_SLICE_TYPE_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_VALID_SUBMDSPAN_SLICE_TYPE_FOR_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/detail/de_ice.hpp"
#include "arene/base/mdspan/detail/slice_upper_bound.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_integral_constant_like.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief determine if a canonical slice type is valid for a specific static extent
/// @tparam CanonicalSlice canonical slice type
/// @tparam StaticExtent static extent
template <class CanonicalSlice, std::size_t StaticExtent, class = constraints<>>
extern constexpr bool is_valid_submdspan_slice_type_for_v = true;

/// @brief determine if a canonical slice type is valid for a specific static extent
/// @tparam CanonicalSlice canonical slice type
/// @tparam StaticExtent static extent
///
/// Specialization for a canonical slice type that is a specialization of
/// @c std::integral_constant.
template <class IntegralConstant, std::size_t StaticExtent>
extern constexpr bool is_valid_submdspan_slice_type_for_v<
    IntegralConstant,
    StaticExtent,
    constraints<std::enable_if_t<is_integral_constant_like_v<IntegralConstant>>>> =
    (StaticExtent == dynamic_extent) || (IntegralConstant::value < StaticExtent);

/// @brief obtain the minimum possible slice upper bound for an @c extent_slice
/// @tparam ExtentSlice canonical @c extent_slice type
/// @tparam IndexType index type of @c extent_slice (deduced)
///
/// Obtains the minimum possible slice upper bound value for @c ExtentSlice.
/// If @c ExtentSlice::offset_type is integral-constant like, the compile-time
/// value is used. Otherwise the offset value is 0.
/// If @c ExtentSlice::extent_type is integral-constant like, the compile-time
/// value is used. Otherwise the extent value is 0.
/// If @c ExtentSlice::stride_type is integral-constant like, the compile-time
/// value is used. Otherwise the stride value is 1.
template <class ExtentSlice, class IndexType = remove_cvref_t<decltype(de_ice(typename ExtentSlice::offset_type{}))>>
extern constexpr auto minimum_possible_slice_upper_bound = slice_upper_bound(
    // The extent value passed to 'slice_upper_bound' doesn't matter, we
    // simply need to get the index type.
    IndexType{},
    extent_slice<IndexType, IndexType, IndexType>{
        typename ExtentSlice::offset_type{},
        typename ExtentSlice::extent_type{},
        is_integral_constant_like_v<typename ExtentSlice::stride_type>  //
            ? typename ExtentSlice::stride_type{}
            : IndexType{1}
    }
);

/// @brief determine if a canonical slice type is valid for a specific static extent
/// @tparam CanonicalSlice canonical slice type
/// @tparam StaticExtent static extent
///
/// Specialization for a canonical slice type that is a specialization of
/// @c extent_slice
template <class Offset, class Extent, class Stride, std::size_t StaticExtent>
extern constexpr bool is_valid_submdspan_slice_type_for_v<
    extent_slice<Offset, Extent, Stride>,
    StaticExtent,
    constraints<std::enable_if_t<StaticExtent != dynamic_extent>>> =
    (de_ice(Offset{}) <= StaticExtent) &&  //
    (de_ice(Extent{}) <= StaticExtent) &&  //
    (!is_integral_constant_like_v<Extent> ||
     ((!minimum_possible_slice_upper_bound<extent_slice<Offset, Extent, Stride>>.overflowed) &&
      (minimum_possible_slice_upper_bound<extent_slice<Offset, Extent, Stride>>.value <= StaticExtent)));

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_VALID_SUBMDSPAN_SLICE_TYPE_FOR_HPP_
