// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 CERT_C-EXP37-a
// "False positive: also defines submdspan_sub_strides and names are given for all parameters in operator()."
//

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_SUB_STRIDES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_SUB_STRIDES_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/checked_math.hpp"
#include "arene/base/mdspan/detail/de_ice.hpp"
#include "arene/base/mdspan/detail/map_rank.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {
namespace submdspan_sub_strides_detail {

/// @brief Compute the sub-stride for an @c extent_slice
///
/// Multiplies the mapping stride by the slice stride when the slice extent is greater than 1.
/// @tparam IndexType the index type of the mdspan
/// @tparam OffsetType the offset type of the extent_slice
/// @tparam ExtentType the extent type of the extent_slice
/// @tparam StrideType the stride type of the extent_slice
/// @param mapping_stride the stride from the layout mapping for this dimension
/// @param slice the slice specifier
/// @pre The multiplication of the mapping stride and the slice stride must not overflow.
/// @return The mapping stride multiplied by the slice stride, or the mapping stride if the slice extent is at most 1
template <class IndexType, class OffsetType, class ExtentType, class StrideType>
constexpr auto
compute_sub_stride(IndexType mapping_stride, extent_slice<OffsetType, ExtentType, StrideType> const& slice) noexcept
    -> IndexType {
  auto const multiplier = de_ice(slice.extent) > IndexType{1} ? slice.stride : IndexType{1};
  auto const sub_stride = mdspan_detail::checked_multiplies<IndexType>(mapping_stride, multiplier);
  ARENE_PRECONDITION(!sub_stride.overflowed);
  return sub_stride.value;
}

/// @brief Compute the sub-stride for all other slice types
///
/// For non-extent_slice types (full_extent_t, integral), the sub-stride is the mapping stride unchanged.
/// @tparam IndexType the index type of the mdspan
/// @tparam Slice the slice type
/// @param mapping_stride the stride from the layout mapping for this dimension
/// @return The mapping stride unchanged
template <class IndexType, class Slice>
constexpr auto compute_sub_stride(IndexType mapping_stride, Slice const&) noexcept -> IndexType {
  return mapping_stride;
}

// parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: No identifier named mapping is being hidden"
/// @brief Compute all sub-strides via pack expansion
/// @tparam IndexType the index type of the mdspan
/// @tparam LayoutMapping a layout mapping type
/// @tparam Ks index sequence for dimension indices
/// @tparam SliceSpecifiers types of the slice specifiers
/// @param mapping the layout mapping of the original mdspan
/// @param slices the slice specifiers defining the submdspan
/// @return an array of sub-strides, one per source dimension
template <class IndexType, class LayoutMapping, std::size_t... Ks, class... SliceSpecifiers>
constexpr auto compute_all_sub_strides(
    LayoutMapping const& mapping,
    std::index_sequence<Ks...>,
    SliceSpecifiers const&... slices
) noexcept -> array<IndexType, sizeof...(SliceSpecifiers)> {
  // parasoft-begin-suppress AUTOSAR-M8_5_2-a "False positive: The initializer is enclosed in braces"
  return {compute_sub_stride<IndexType>(mapping.stride(Ks), slices)...};
  // parasoft-end-suppress AUTOSAR-M8_5_2-a
}
// parasoft-end-suppress AUTOSAR-A2_10_1-a

}  // namespace submdspan_sub_strides_detail

/// @brief function object that computes the sub-strides for a submdspan mapping
///
/// Computes the new strides array for a submdspan. For each non-collapsing
/// dimension, the stride is the original stride multiplied by the stride of the
/// slice if the slice is a strided extent_slice with stride less than extent.
/// @tparam IndexType index type of the extents type of the submdspan
///
/// @note The slice specifiers must be in canonical form.
template <class IndexType>
class submdspan_sub_strides_fn {
  static_assert(std::is_integral<IndexType>::value, "must be an integral type");

 public:
  // parasoft-begin-suppress AUTOSAR-M0_1_3 "False positive: Variables sub_strides and mr are used"
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: Parameter mapping is used"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: No identifier named mapping is being hidden"

  /// @brief compute the sub-strides from a layout mapping and canonical slice specifiers
  /// @tparam LayoutMapping a layout mapping type
  /// @tparam SliceSpecifiers types of the slice specifiers
  /// @param mapping the layout mapping of the original mdspan
  /// @param slices the slice specifiers defining the submdspan
  /// @note Constraints <br>
  ///   @c sizeof...(SliceSpecifiers) == LayoutMapping::extents_type::rank()
  /// @return an array of strides for the submdspan
  template <
      class LayoutMapping,
      class... SliceSpecifiers,
      constraints<std::enable_if_t<sizeof...(SliceSpecifiers) == LayoutMapping::extents_type::rank()>> = nullptr>
  constexpr auto operator()(LayoutMapping const& mapping, SliceSpecifiers const&... slices) const noexcept
      -> array<IndexType, inverse_map_rank<SliceSpecifiers...>.size()> {
    using source_index_type = typename LayoutMapping::index_type;

    // parasoft-begin-suppress AUTOSAR-M8_5_2-a "False positive: there is no subobject here that can be initialized"
    auto const sub_strides = submdspan_sub_strides_detail::compute_all_sub_strides<source_index_type>(
        mapping,
        std::make_index_sequence<sizeof...(SliceSpecifiers)>{},
        slices...
    );
    // parasoft-end-suppress AUTOSAR-M8_5_2-a

    auto result = array<IndexType, inverse_map_rank<SliceSpecifiers...>.size()>{};

    for (std::size_t i{}; i != inverse_map_rank<SliceSpecifiers...>.size(); ++i) {
      result[i] = sub_strides[inverse_map_rank<SliceSpecifiers...>[i]];
    }

    return result;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
  // parasoft-end-suppress AUTOSAR-M0_1_3
};

/// @def arene::base::mdspan_detail::submdspan_sub_strides
/// @copydoc arene::base::mdspan_detail::submdspan_sub_strides_fn::operator()
template <class IndexType>
extern constexpr auto submdspan_sub_strides = submdspan_sub_strides_fn<IndexType>{};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SUBMDSPAN_SUB_STRIDES_HPP_
