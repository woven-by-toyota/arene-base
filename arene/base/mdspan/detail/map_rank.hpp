// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MAP_RANK_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MAP_RANK_HPP_

#include "arene/base/array/array.hpp"  // IWYU pragma: export
#include "arene/base/mdspan/detail/masked_array.hpp"
#include "arene/base/mdspan/detail/non_collapsing_slice_type.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief associated dimensions of a submapping
/// @tparam CanonicalSlices slice specifiers for the submapping
///
/// For each dimension of a base mapping, @c map_rank returns the index of the
/// associated dimension in the submapping. @c is_valid(index) can be used to
/// determine if the dimension is present or not present in the submapping (i.e.
/// due to rank reduction).
///
/// This value is an indexable type where for k in [0,
/// sizeof...(CanonicalSlices)), S_k is the kth slice type, and for each element
/// in the returned array:
/// * invalid access (i.e. precondition violation) if S_k is a collapsing slice
///   type
/// * the number of types @c S_j with <c> j < k </c> that are not collapsing
///   slice types if S_k is not a collapsing slice type
template <class... CanonicalSlices>
extern constexpr auto map_rank = masked_array<sizeof...(CanonicalSlices)>{
    //
    array<bool, sizeof...(CanonicalSlices)>{non_collapsing_slice_type_v<std::remove_cv_t<CanonicalSlices>>...}
};

namespace map_rank_detail {

/// @brief determine the inverse mapping of 'map_rank'
/// @tparam CanonicalSlices slice specifiers for 'map_rank'
/// @return an @c array where each element of the @c array specifies the
/// associated dimension in the source mapping given the element's index.
template <class... CanonicalSlices>
constexpr auto inverse_map_rank_impl() noexcept -> array<std::size_t, map_rank<CanonicalSlices...>.codomain_size()> {
  auto inverse_mapping = array<std::size_t, map_rank<CanonicalSlices...>.codomain_size()>{};

  for (std::size_t k{}; k != sizeof...(CanonicalSlices); ++k) {
    if (map_rank<CanonicalSlices...>.is_valid(k)) {
      inverse_mapping[map_rank<CanonicalSlices...>[k]] = k;
    }
  }

  return inverse_mapping;
}

}  // namespace map_rank_detail

/// @brief inverse mapping of 'map_rank'
/// @tparam CanonicalSlices slice specifiers for 'map_rank'
///
/// For each index in a submapping, @c inverse_map_rank returns the index of the
/// associated dimension in the source mapping.
template <class... CanonicalSlices>
extern constexpr auto inverse_map_rank = map_rank_detail::inverse_map_rank_impl<CanonicalSlices...>();

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MAP_RANK_HPP_
