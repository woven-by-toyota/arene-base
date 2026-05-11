// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_ALL_VALID_SUBMDSPAN_SLICE_TYPES_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_ALL_VALID_SUBMDSPAN_SLICE_TYPES_FOR_HPP_

#include "arene/base/mdspan/detail/is_valid_submdspan_slice_type_for.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/type_traits/all_of.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief Check if all CanonicalSliceSpecifiers are valid submdspan slice types for each extent in Extents
template <class Extents, class... CanonicalSliceSpecifiers>
extern constexpr bool all_valid_submdspan_slice_types_for_v{false};

template <class IndexType, std::size_t... Extents, class... CanonicalSliceSpecifiers>
extern constexpr bool
    all_valid_submdspan_slice_types_for_v<extents<IndexType, Extents...>, CanonicalSliceSpecifiers...>{all_of_v<
        is_valid_submdspan_slice_type_for_v<CanonicalSliceSpecifiers, Extents>...>};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_ALL_VALID_SUBMDSPAN_SLICE_TYPES_FOR_HPP_
