// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::mdspan_detail::is_valid_submdspan_slice_for"
//
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_VALID_SUBMDSPAN_SLICE_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_VALID_SUBMDSPAN_SLICE_FOR_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/mdspan/detail/is_negative.hpp"
#include "arene/base/mdspan/detail/is_nonnegative.hpp"
#include "arene/base/mdspan/detail/slice_upper_bound.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object that determines if a canonical slice specifier is
///   valid for an extent value
///
/// Determines if a canonical slice specifier is valid for a specific (runtime)
/// extent value.
/// Given a canonical slice specifier @c s and an extent value @c e, @c s is
/// valid if:
/// * all the values associated with @c s are non-negative, and
/// * the interval specified by @c e contains the submdspan slice range
/// of @c s (i.e. the <c> slice-upper-bound(e, s) <= e </c), and
/// * if @c s is an @c extent_slice:
/// ** s.extent is greater than or equal to zero, and
/// ** s.extent equals zero or s.stride is greater than zero
///
/// @note This function object does not check the validity of the submdspan
///   slice type as the static extent is not a function argument.
class is_valid_submdspan_slice_for_fn {
  /// @brief overload for instantiations of @c extent_slice
  /// @tparam IndexType extent index type
  /// @tparam Offset @c extent_slice offset type
  /// @tparam Extent @c extent_slice extent type
  /// @tparam Stride @c extent_slice stride type
  /// @param slice canonical slice specifier
  /// @return @c true if all @c slice values are non-negative and either the
  ///   extent is 0 or the stride is positive. Otherwise @c false.
  template <class IndexType, class Offset, class Extent, class Stride>
  static constexpr auto slice_is_well_formed(extent_slice<Offset, Extent, Stride> const& slice) noexcept -> bool {
    if (is_negative(static_cast<IndexType>(slice.offset)) ||  //
        is_negative(static_cast<IndexType>(slice.extent)) ||  //
        is_negative(static_cast<IndexType>(slice.stride))) {
      return false;
    }

    // There can only be an implicit conversion to 'IndexType' from an
    // 'integral_constant'. This can be verified using 'de_ice', but is not done
    // as it adds an unnecessary dependency.
    // parasoft-begin-suppress AUTOSAR-M5_0_4-a "False positive: no implicit conversion between signed and unsigned"
    return (slice.extent == IndexType{}) || (slice.stride != IndexType{});
    // parasoft-end-suppress AUTOSAR-M5_0_4-a
  }

  /// @brief overload for slices convertible to @c index_type
  /// @tparam IndexType extent index type
  /// @tparam CanonicalSlice canonical slice type
  /// @param slice canonical slice specifier
  /// @return @c true if @c slice is non-negative
  template <class IndexType, class CanonicalSlice>
  static constexpr auto slice_is_well_formed(CanonicalSlice const& slice) noexcept -> bool {
    return is_nonnegative(static_cast<IndexType>(slice));
  }

  /// @brief overload for @c full_extent_t
  /// @tparam IndexType extent index type
  /// @return @c true
  template <class IndexType>
  static constexpr auto slice_is_well_formed(full_extent_t const&) noexcept -> bool {
    return true;
  }

 public:
  /// @brief determines if a slice is valid for an extent
  /// @tparam CanonicalSlice canonical slice specifier type
  /// @tparam IndexType extent index type
  /// @param slice canonical slice specifier
  /// @param extent value representing an interval [0, extent)
  /// @return @c true if the slice upper bound does not exceed @c extent, and if
  ///   @c slice is an @c extent_slice, the stride is positive if the extent is
  ///   non-zero. Otherwise @c false.
  template <class CanonicalSlice, class IndexType>
  constexpr auto operator()(CanonicalSlice const& slice, IndexType extent) const noexcept -> bool {
    return slice_is_well_formed<IndexType>(slice) &&          //
           (!slice_upper_bound(extent, slice).overflowed) &&  //
           (slice_upper_bound(extent, slice).value <= extent);
  }
};

/// @def arene::base::mdspan_detail::is_valid_submdspan_slice_for
/// @copydoc arene::base::mdspan_detail::is_valid_submdspan_slice_for_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::is_valid_submdspan_slice_for_fn, is_valid_submdspan_slice_for);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_VALID_SUBMDSPAN_SLICE_FOR_HPP_
