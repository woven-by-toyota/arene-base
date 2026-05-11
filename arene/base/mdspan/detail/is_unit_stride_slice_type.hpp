// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_UNIT_STRIDE_SLICE_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_UNIT_STRIDE_SLICE_TYPE_HPP_

#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief determine if a canonical slice type is a unit-stride slice type
/// @tparam CanonicalSlice canonical slice type
///
/// A canonical slice type is a unit-stride slice type if it is @c full_extent_t
/// or a specialization of @c extent_slice whose stride type is
/// @c std::integral_constant<IndexType, 1> for some @c IndexType.
template <class CanonicalSlice>
extern constexpr bool is_unit_stride_slice_type_v = std::is_same<CanonicalSlice, full_extent_t>::value;

/// @brief specialization for @c extent_slice with unit stride
/// @tparam Offset extent_slice offset type
/// @tparam Extent extent_slice extent type
/// @tparam IndexType index type of the unit stride
template <class Offset, class Extent, class IndexType>
extern constexpr bool is_unit_stride_slice_type_v<extent_slice<Offset, Extent, std::integral_constant<IndexType, 1>>> =
    true;

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_UNIT_STRIDE_SLICE_TYPE_HPP_
