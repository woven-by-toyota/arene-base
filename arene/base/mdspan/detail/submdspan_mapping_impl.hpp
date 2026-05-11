// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_MAPPING_IMPL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_MAPPING_IMPL_HPP_

// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

#include "arene/base/mdspan/detail/submdspan_offset.hpp"
#include "arene/base/mdspan/layout_stride.hpp"
#include "arene/base/mdspan/submdspan_extents.hpp"
#include "arene/base/mdspan/submdspan_mapping_result.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

// parasoft-begin-suppress AUTOSAR-A2_7_3-b "False Positive: The @param tags are followed by the correct parameter
// names"
/// @brief layout-preserving implementation of @c submdspan_mapping
/// @tparam SubExtents the sub-extents type of the result
/// @tparam Mapping the source mapping type
/// @tparam Strides the strides array type
/// @tparam CanonicalSliceSpecifiers types of the slice specifiers
/// @param src the source mapping
/// @param passkey unused; accepted for uniform call signature with the @c false_type overload
/// @param strides unused; accepted for uniform call signature with the @c false_type overload
/// @param slices the slice specifiers
/// @return a @c submdspan_mapping_result with a same-layout mapping
template <class SubExtents, class Mapping, class Strides, class... CanonicalSliceSpecifiers>
constexpr auto submdspan_mapping_impl(
    std::true_type /*preserving*/,
    Mapping const& src,
    layout_detail::check_bypasser /*passkey*/,
    Strides const& /*strides*/,
    CanonicalSliceSpecifiers... slices
) noexcept(is_nothrow_invocable_v<decltype(submdspan_extents), typename Mapping::extents_type const&, CanonicalSliceSpecifiers&&...>)
    -> submdspan_mapping_result<typename Mapping::layout_type::template mapping<SubExtents>> {
  auto offset = submdspan_offset(src, slices...);
  auto sub_ext = submdspan_extents(src.extents(), std::move(slices)...);

  // SubExtents is explicitly constructed here to bypass the precondition in the mapping's converting extents
  // constructor. The submdspan_extents function returns extents that are guaranteed not to overflow.
  using result_mapping = typename Mapping::layout_type::template mapping<SubExtents>;
  return {result_mapping{SubExtents{sub_ext}}, offset};
}
// parasoft-end-suppress AUTOSAR-A2_7_3-b

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
/// @brief layout_stride fallback implementation of @c submdspan_mapping
/// @tparam SubExtents the sub-extents type of the result
/// @tparam Mapping the source mapping type
/// @tparam Strides the strides array type
/// @tparam CanonicalSliceSpecifiers types of the slice specifiers
/// @param src the source mapping
/// @param passkey check_bypasser to skip redundant precondition checks
/// @param strides the strides array from the source mapping
/// @param slices the slice specifiers
/// @return a @c submdspan_mapping_result with a @c layout_stride::mapping
template <class SubExtents, class Mapping, class Strides, class... CanonicalSliceSpecifiers>
constexpr auto submdspan_mapping_impl(
    std::false_type /*preserving*/,
    Mapping const& src,
    layout_detail::check_bypasser passkey,
    Strides const& strides,
    CanonicalSliceSpecifiers... slices
) noexcept(is_nothrow_invocable_v<decltype(submdspan_extents), typename Mapping::extents_type const&, CanonicalSliceSpecifiers&&...>)
    -> submdspan_mapping_result<layout_stride::mapping<SubExtents>> {
  using stride_mapping = layout_stride::mapping<typename Mapping::extents_type>;
  return submdspan_mapping(stride_mapping{src.extents(), strides, std::move(passkey)}, std::move(slices)...);
}
// parasoft-end-suppress AUTOSAR-M2_10_1-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_MAPPING_IMPL_HPP_
