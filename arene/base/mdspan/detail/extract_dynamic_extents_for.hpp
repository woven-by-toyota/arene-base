// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::mdspan_detail::extract_dynamic_extents_for"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_EXTRACT_DYNAMIC_EXTENTS_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_EXTRACT_DYNAMIC_EXTENTS_FOR_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/detail/de_ice.hpp"
#include "arene/base/mdspan/detail/is_nonnegative.hpp"
#include "arene/base/mdspan/detail/is_representable_as.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/utility/as_const.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object providing a static extent check as a precondition, if
///   necessary
///
/// Primary template for the case where static extents do not need to be checked.
///
template <bool = false>
class equal_static_extent_precondition  //
{
 public:
  /// @brief check if values are equal
  /// @tparam IndexType extent index type
  /// @param lhs left argument
  /// @param rhs right argument
  ///
  ///  This function is a no-op
  ///
  template <class IndexType>
  constexpr auto operator()(IndexType const& lhs, IndexType const& rhs) const noexcept -> void {
    std::ignore = lhs;
    std::ignore = rhs;
  }
};

/// @brief function object providing a static extent check as a precondition, if
///   necessary
///
/// Specialization for the case where static extents need to be checked.
///
template <>
class equal_static_extent_precondition<true>  //
{
 public:
  /// @brief check if values are equal
  /// @tparam IndexType extent index type
  /// @param lhs left argument
  /// @param rhs right argument
  /// @pre <c> lhs == rhs </c>
  template <class IndexType>
  constexpr auto operator()(IndexType const& lhs, IndexType const& rhs) const noexcept  //
      -> void                                                                           //
  {
    ARENE_PRECONDITION(lhs == rhs);

    // this is only invoked for line coverage
    equal_static_extent_precondition<false>{}(lhs, rhs);
  }
};

/// @brief function object implementing extract_dynamic_extents_for
/// @tparam DestExtents destination extents_type for the dynamic extent values
template <class DestExtents>
class extract_dynamic_extents_for_fn  //
{
  /// @brief index_type index type of the destination extent_type
  using index_type = typename DestExtents::index_type;

  /// @brief rank rank of the destination extent_type
  static constexpr auto rank = DestExtents::rank();

  /// @brief rank_dynamic dynamic rank of the destination extent_type
  static constexpr auto rank_dynamic = DestExtents::rank_dynamic();

  /// @brief extracts the dynamic extent values from a range
  /// @tparam Iterator random access iterator to source extent values
  /// @param first beginning to the range of source extent values
  /// @param last end to the range of source extent values
  ///
  /// @pre all values in the range <c> [first, last) </c> are non-negative
  /// @pre all values in the range <c> [first, last) </c> are representable in
  ///   @c index_type
  /// @pre let @c r denote the dimensions of @c DestExtents @c E_r denote the
  ///   @c DestExtents::static_extent(r),
  ///   if <c> distance(first, last) == rank </c>,
  ///   then for all @c r in <c> [0, ..., rank) </c>,
  ///   <c> E_r == dynamic_extent || E_r == *next(first, r) </c>
  ///
  /// Filters values in @c source_range, keeping only the values associated with
  /// the dynamic extents of @c DestExtents. If the size of the range
  /// <c> [first, last) </c> is equal to @c rank_dynamic, no filtering is
  /// performed.
  ///
  /// @return @c array holding only the dynamic extents
  ///
  template <class Iterator>
  // NOLINTNEXTLINE(readability-function-size)
  static constexpr auto impl(Iterator first, Iterator last) noexcept  //
      -> array<index_type, rank_dynamic>                              //
  {
    ARENE_PRECONDITION(  //
        all_of(          //
            first,
            last,
            is_nonnegative,
            de_ice
        )
    );
    ARENE_PRECONDITION(  //
        all_of(          //
            first,
            last,
            is_representable_as<index_type>,
            de_ice
        )
    );

    auto const check_static_extent = equal_static_extent_precondition<rank != rank_dynamic>{};

    auto dynamic_values = array<index_type, rank_dynamic>{};
    auto dest = dynamic_values.begin();

    bool const source_has_static_values{cmp_equal(last - first, rank) && (rank != rank_dynamic)};
    for (auto dim : sequential_values<std::size_t, rank>) {
      bool const extent_is_dynamic{DestExtents::static_extent(dim) == dynamic_extent};

      if (source_has_static_values || extent_is_dynamic) {
        auto converted = representable_cast<index_type>(as_const(*first++));

        if (extent_is_dynamic) {
          *dest++ = std::move(converted);
        } else {
          check_static_extent(
              // The static extents of 'DestExtents' must always be
              // representable in the extents type so there is no need to
              // perform a runtime check -- we can rely on this to true for any
              // 'extents' specialization.
              static_cast<index_type>(DestExtents::static_extent(dim)),
              std::move(converted)
          );
        }
      }
    }
    return dynamic_values;
  }

 public:
  /// @brief extracts the dynamic extent values from a range
  /// @tparam Range random access range type
  /// @param source_values span containing extent values
  ///
  /// @pre all values in @c source_values are non-negative
  /// @pre all values in @c source_values are representable in @c index_type
  /// @pre let @c r denote the dimensions of @c DestExtents @c E_r denote the
  ///   @c DestExtents::static_extent(r),
  ///   if <c> source_values.size() == rank </c>,
  ///   then for all @c r in <c> [0, ..., rank) </c>,
  ///   <c> E_r == dynamic_extent || E_r == source_values[r] </c>
  ///
  /// Filters values in @c source_range, keeping only the values associated with
  /// the dynamic extents of @c DestExtents. If @c source_range.size() is equal
  /// to @c rank_dynamic, no filtering is performed.
  ///
  /// @return @c array holding only the dynamic extents
  ///
  template <class Range>
  // NOLINTNEXTLINE(readability-function-size)
  constexpr auto operator()(Range const& source_values) const noexcept  //
      -> array<index_type, rank_dynamic>                                //
  {
    return impl(source_values.begin(), source_values.end());
  }
};

/// @def arene::base::mdspan_detail::extract_dynamic_extents_for
/// @tparam DestExtents destination extents_type for the dynamic extent values,
///   must be a specialization of @c extents
/// @copydoc arene::base::mdspan_detail::extract_dynamic_extents_for_fn::operator()
template <class DestExtents>
extern constexpr auto extract_dynamic_extents_for = extract_dynamic_extents_for_fn<DestExtents>{};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_EXTRACT_DYNAMIC_EXTENTS_FOR_HPP_
