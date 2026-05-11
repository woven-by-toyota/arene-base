// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_SUBEXTENTS_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_SUBEXTENTS_TYPE_HPP_

#include "arene/base/mdspan/detail/canonical_slice.hpp"
#include "arene/base/mdspan/detail/deduced_static_extent.hpp"
#include "arene/base/mdspan/detail/is_valid_submdspan_slice_type_for.hpp"
#include "arene/base/mdspan/detail/map_rank.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/all_of.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace submdspan_subextents_type_detail {

// parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Used by
// 'submdspan_subextents_type_impl' in this same header"

/// @brief determines the static extent of a slice
/// @param e_k static extent of the source mdspan
/// @return the static extent @c e_k for a full_extent_t slice
static constexpr auto subextents_static_extent_for(std::size_t const e_k, full_extent_t const&) noexcept
    -> std::size_t {
  return e_k;
}

/// @brief determines the static extent of an extent_slice
/// @tparam Offset extent_slice offset type
/// @tparam Extent extent_slice extent type
/// @tparam Stride extent_slice stride type
/// @return maybe-static-ext<Extent>
template <class Offset, class Extent, class Stride>
static constexpr auto subextents_static_extent_for(  //
    std::size_t,                                     //
    extent_slice<Offset, Extent, Stride> const&      //
) noexcept -> std::size_t {
  return mdspan_detail::deduced_static_extent_v<Extent>;
}

// parasoft-end-suppress AUTOSAR-A0_1_3-a

/// @brief implementation helper for @c submdspan_subextents_type
/// @tparam Extents specialization of extents
/// @tparam CanonicalSliceSpecifiers canonical slice specifier types
/// Undefined primary template.
template <class Extents, class... CanonicalSliceSpecifiers>
class submdspan_subextents_type_impl;

/// @brief implementation helper for @c submdspan_subextents_type
/// @tparam IndexType extents index type
/// @tparam Extents parameter pack of static extent values
/// @tparam CanonicalSliceSpecifiers canonical slice specifier types
///
template <class IndexType, std::size_t... Extents, class... CanonicalSliceSpecifiers>
class submdspan_subextents_type_impl<extents<IndexType, Extents...>, CanonicalSliceSpecifiers...> {
  static_assert(
      all_of_v<mdspan_detail::is_valid_submdspan_slice_type_for_v<CanonicalSliceSpecifiers, Extents>...>,
      "each type in 'SliceSpecifiers' must be a valid 'submdspan' slice type for the associated extent"
  );

  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variables are initialized"
  /// @brief mapping of indices from the sliced extents back to the source extents
  static constexpr auto const& inverse_map_rank = mdspan_detail::inverse_map_rank<CanonicalSliceSpecifiers...>;
  // parasoft-end-suppress CERT_CPP-DCL56-a

  /// @brief helper to determine the sliced extents type
  /// @tparam Is parameter pack of indices corresponding to the (reduced) rank after slicing
  /// @return sliced extents type
  template <std::size_t... Is>
  static auto impl(std::index_sequence<Is...>) -> extents<
      IndexType,
      subextents_static_extent_for(
          extents<IndexType, Extents...>::static_extent(inverse_map_rank[Is]),
          type_lists::at_t<type_list<CanonicalSliceSpecifiers...>, inverse_map_rank[Is]>{}
      )...>;

 public:
  /// @brief sliced extents type
  using type = decltype(impl(std::make_index_sequence<inverse_map_rank.size()>{}));
};

}  // namespace submdspan_subextents_type_detail

/// @brief determines the subextents type from an extents type and slice specifiers
/// @tparam Extents specialization of extents type
/// @tparam SliceSpecifiers parameter pack specifying the subset of each rank in
///   @c Extents
///
/// Provides the member typedef @c type which is a specialization of @c extents
/// after rank reducing @c Extents and determining the static subset of each
/// remaining rank in @c Extents.
///
/// @note Mandates <br>
/// * each type @c S in @c SliceSpecifiers... is a valid submdspan slice for
///   @c Extents::index_type
/// * the canonical form of each @c S in @c SliceSpecifiers... is a valid
///   submdspan slice type for associated static extent of @c Extents
template <class Extents, class... SliceSpecifiers>
class submdspan_subextents_type
    : public submdspan_subextents_type_detail::submdspan_subextents_type_impl<
          Extents,
          mdspan_detail::canonical_slice_t<typename Extents::index_type, SliceSpecifiers>...> {};

/// @brief determines the subextents type from an extents type and slice specifiers
/// @tparam Extents specialization of extents type
/// @tparam SliceSpecifiers parameter pack specifying the subset of each rank in
///   @c Extents
template <class Extents, class... SliceSpecifiers>
using submdspan_subextents_type_t = typename submdspan_subextents_type<Extents, SliceSpecifiers...>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_SUBMDSPAN_SUBEXTENTS_TYPE_HPP_
