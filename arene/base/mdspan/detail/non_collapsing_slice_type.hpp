// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_NON_COLLAPSING_SLICE_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_NON_COLLAPSING_SLICE_TYPE_HPP_

#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief determine if a slice will *not* collapse rank
/// @tparam CanonicalSlice canonical slice type
template <class CanonicalSlice>
extern constexpr bool non_collapsing_slice_type_v =
    std::is_same<CanonicalSlice, full_extent_t>::value || is_instantiation_of_v<CanonicalSlice, extent_slice>;

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_NON_COLLAPSING_SLICE_TYPE_HPP_
