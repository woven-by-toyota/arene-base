// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines slice_lower_bound"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SLICE_LOWER_BOUND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SLICE_LOWER_BOUND_HPP_

#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object that extracts the lower bound index from a canonical slice specifier
/// @tparam IndexType the index type of the mdspan
///
/// Extracts the lower bound index from a canonical slice specifier.
/// Given a canonical slice specifier @c s:
/// * if @c s is @c IndexType, returns @c s
/// * if @c s is @c std::integral_constant<IndexType, N>, returns @c N
/// * if @c s is an @c extent_slice, returns @c de_ice(s.offset)
/// * if @c s is @c full_extent_t, returns @c IndexType{}
template <class IndexType>
class slice_lower_bound_fn {
 public:
  /// @brief returns the lower bound of a slice specified as an IndexType
  /// @param slice the slice specifier
  /// @return @c slice
  constexpr auto operator()(IndexType const& slice) const noexcept -> IndexType { return slice; }

  /// @brief returns the lower bound of a slice specified as an integral_constant
  /// @tparam N the value of the integral_constant
  /// @return @c N
  template <IndexType N>
  constexpr auto operator()(std::integral_constant<IndexType, N> const&) const noexcept -> IndexType {
    return N;
  }

  /// @brief returns the lower bound of an extent_slice
  /// @tparam OffsetType the offset type of the extent_slice
  /// @tparam ExtentType the extent type of the extent_slice
  /// @tparam StrideType the stride type of the extent_slice
  /// @param slice the slice specifier
  /// @return @c slice.offset
  template <class OffsetType, class ExtentType, class StrideType>
  constexpr auto operator()(extent_slice<OffsetType, ExtentType, StrideType> const& slice) const noexcept -> IndexType {
    return slice.offset;
  }

  /// @brief returns the lower bound of a full_extent_t slice
  /// @return @c IndexType{}
  constexpr auto operator()(full_extent_t const&) const noexcept -> IndexType { return IndexType{}; }
};

/// @def arene::base::mdspan_detail::slice_lower_bound
/// @copydoc arene::base::mdspan_detail::slice_lower_bound_fn::operator()
template <class IndexType>
extern constexpr auto slice_lower_bound = slice_lower_bound_fn<IndexType>{};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SLICE_LOWER_BOUND_HPP_
