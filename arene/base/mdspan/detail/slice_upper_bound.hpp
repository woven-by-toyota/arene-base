// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::mdspan_detail::slice_upper_bound"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SLICE_UPPER_BOUND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SLICE_UPPER_BOUND_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/checked_math.hpp"  // IWYU pragma: export
#include "arene/base/mdspan/detail/is_nonnegative.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object that extracts the upper bound index from a canonical slice specifier
///
/// Extracts the upper bound index from a canonical slice specifier.
/// Given a canonical slice specifier @c s and an extent value @c e:
/// * if @c s is an @c full_extent_t, returns @c e;
/// * else if @c s is an @c extent_slice, and @c s.extent equals zero, returns
///   <c> IndexType(s.offset + s.extent) </c>;
/// * else if @c s is an @c extent_slice, and @c s.extent does not equal zero,
///   returns <c> IndexType(s.offset + 1 + (s.extent - 1) * s.stride) </c>;
/// * otherwise returns <c> IndexType(IndexType(s) + 1) </c>
class slice_upper_bound_fn {
 public:
  /// @brief returns the upper bound of a @c full_extent_t slice
  /// @tparam IndexType extent indext type
  /// @param extent the extent to slice
  /// @return @c checked_math_result<IndexType> with value @c extent
  template <class IndexType>
  constexpr auto operator()(IndexType const& extent, full_extent_t const&) const noexcept
      -> checked_math_result<IndexType> {
    return {extent};
  }

  /// @brief returns the upper bound of an @c extent_slice
  /// @tparam IndexType extent indext type
  /// @tparam OffsetType the offset type of the @c extent_slice
  /// @tparam ExtentType the extent type of the @c extent_slice
  /// @tparam StrideType the stride type of the @c extent_slice
  /// @param slice the slice specifier
  /// @pre @c slice.offset, @c slice.extent, and @c slice.stride are all non-negative
  /// @return @c checked_math_result<IndexType> with value @c slice.offset if @c
  ///   slice.extent is zero, otherwise <c> slice.offset + 1 + (slice.extent - 1)
  ///   * slice.stride </c>
  /// @note While we could define an upper bound with negative values, it
  ///   doesn't make sense to do so.
  template <  //
      class IndexType,
      class OffsetType,
      class ExtentType,
      class StrideType>
  constexpr auto operator()(IndexType const&, extent_slice<OffsetType, ExtentType, StrideType> const& slice)
      const noexcept -> checked_math_result<IndexType> {
    ARENE_PRECONDITION(is_nonnegative(static_cast<IndexType>(slice.offset)));
    ARENE_PRECONDITION(is_nonnegative(static_cast<IndexType>(slice.extent)));
    ARENE_PRECONDITION(is_nonnegative(static_cast<IndexType>(slice.stride)));

    if (slice.extent == IndexType{}) {
      return checked_math_result<IndexType>{slice.offset};
    }

    constexpr auto plus = checked_plus<IndexType>;
    constexpr auto times = checked_multiplies<IndexType>;

    return plus(
        plus(  //
            static_cast<IndexType>(slice.offset),
            IndexType{1}
        ),
        times(  //
            static_cast<IndexType>(slice.extent - IndexType{1}),
            static_cast<IndexType>(slice.stride)
        )
    );
  }

  /// @brief returns the upper bound of a slice convertible to @c IndexType
  /// @tparam IndexType extent indext type
  /// @tparam Slice slice type
  /// @param slice the slice specifier
  /// @pre the slice value is non-negative
  /// @return @c checked_math_result with value <c> slice + 1 </c>
  /// @note While we could define an upper bound with negative values, it
  ///   doesn't make sense to do so.
  template <class IndexType, class Slice>
  constexpr auto operator()(IndexType const&, Slice const& slice) const noexcept -> checked_math_result<IndexType> {
    ARENE_PRECONDITION(is_nonnegative(static_cast<IndexType>(slice)));

    return checked_plus<IndexType>(static_cast<IndexType>(slice), IndexType{1});
  }
};

/// @def arene::base::mdspan_detail::slice_upper_bound
/// @copydoc arene::base::mdspan_detail::slice_upper_bound_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::slice_upper_bound_fn, slice_upper_bound);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_SLICE_UPPER_BOUND_HPP_
