// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines the extents policies bound by
// make_extents_range"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MAKE_EXTENTS_RANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MAKE_EXTENTS_RANGE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/detail/make_range_from_indexable.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @file
/// @brief view the extents of an @c mdspan as a range via @c detail::make_range_from_indexable
///
/// Provide an IndexPolicy and EndPolicy that allow an @c extents value to be viewed as a range of its per-dimension
/// extent values, and @c make_extents_range which partially applies them to @c detail::make_range_from_indexable so a
/// caller need only pass the @c extents. Because @c extents::extent returns each extent by value, this is an input
/// range rather than a random-access one.
///
/// @see arene::base::detail::indexable_iterator
/// @see arene::base::detail::make_range_from_indexable

/// @brief index policy that reads an extent via @c extent()
///
/// This index policy suits @c extents, which does not provide @c operator() and
/// instead exposes each dimension through the @c extent(dimension) member. It is
/// paired with @c rank_end_fn to view an @c extents as a range of its per-dimension
/// extent values, and is bound into @c make_extents_range.
class extent_index_fn {
 public:
  /// @brief read the extent of @c indexable at @c index
  /// @tparam Indexable indexable type providing an @c extent() member
  /// @tparam Index index type accepted by @c Indexable::extent()
  /// @param indexable the indexable value to read from
  /// @param index the dimension whose extent is read
  /// @return the result of @c indexable.extent(index)
  ///
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
  template <class Indexable, class Index>
  constexpr auto operator()(Indexable&& indexable, Index index) const
      noexcept(noexcept(std::forward<Indexable>(indexable)
                            .extent(static_cast<typename arene::base::remove_cvref_t<Indexable>::rank_type>(index))))
          -> decltype(std::forward<Indexable>(indexable)
                          .extent(static_cast<typename arene::base::remove_cvref_t<Indexable>::rank_type>(index))) {
    // The index is a dimension of a non-negative rank, so the cast to the
    // unsigned rank_type is well-defined.
    return std::forward<Indexable>(indexable).extent(
        static_cast<typename arene::base::remove_cvref_t<Indexable>::rank_type>(index)
    );
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a
};

/// @brief end policy that reports the size via the @c rank() member
///
/// This end policy suits @c extents, whose iterable sequence has one element per
/// dimension. It is paired with @c extent_index_fn to view an @c extents as a range
/// of its per-dimension extent values, and is bound into @c make_extents_range.
class rank_end_fn {
 public:
  /// @brief report the one-past-last index of @c indexable
  /// @tparam Indexable indexable type providing a @c rank() member
  /// @param indexable the indexable value to query
  /// @return the result of @c indexable.rank()
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
  template <class Indexable>
  constexpr auto operator()(Indexable const& indexable) const noexcept(noexcept(indexable.rank()))
      -> decltype(indexable.rank()) {
    return indexable.rank();
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a
};

/// @def arene::base::mdspan_detail::make_extents_range
/// @brief view an @c extents as an input range of its per-dimension extent values
/// @param exts the @c extents value to view
/// @return a range whose elements are @c exts.extent(0) ... @c exts.extent(exts.rank() - 1)
///
/// Call @c make_extents_range(exts) to obtain a range over the per-dimension
/// extents of @c exts, so that @c extents can be passed to range-based algorithms
/// and range-for loops even though it exposes no iterators of its own:
///
/// @code
/// auto const exts = arene::base::dextents<std::size_t, 3>{4, 5, 6};
/// for (auto const extent : make_extents_range(exts)) {
///   // extent takes the values 4, 5, 6 in turn
/// }
/// @endcode
///
/// The returned range's iterators are self-contained, so it may be used as a
/// temporary. A rank-0 @c extents yields an empty range.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress AUTOSAR-M3_4_1-b "This is a per-TU reference to a global used in multiple TUs"
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable is initialized"
ARENE_CPP14_INLINE_VARIABLE_INIT(
    make_extents_range,
    arene::base::bind_back(
        arene::base::detail::make_range_from_indexable,
        mdspan_detail::rank_end_fn{},
        mdspan_detail::extent_index_fn{}
    )
);
// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M3_4_1-b
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_MAKE_EXTENTS_RANGE_HPP_
