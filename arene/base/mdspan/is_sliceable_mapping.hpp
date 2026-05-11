// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SLICEABLE_MAPPING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SLICEABLE_MAPPING_HPP_

namespace arene {
namespace base {
namespace submdspan_detail {

/// @brief Helper trait that indicates whether a layout mapping type supports @c submdspan
///
/// Defaults to @c false; specialized to @c true in each layout header for
/// @c layout_left::mapping, @c layout_right::mapping, and @c layout_stride::mapping.
/// @tparam LayoutMapping the layout mapping type under test
template <class LayoutMapping>
extern constexpr bool is_sliceable_mapping_v = false;

}  // namespace submdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SLICEABLE_MAPPING_HPP_
