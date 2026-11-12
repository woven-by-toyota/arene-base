// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::axis_view"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_AXIS_VIEW_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_AXIS_VIEW_HPP_

// IWYU pragma: private, include "arene/base/mdspan.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/make_range_from_indexable.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/is_sliceable_mapping.hpp"
#include "arene/base/mdspan/submdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

namespace axis_view_detail {

/// @brief select the submdspan slice specifier for a non-iterated axis
/// @tparam IndexType the mdspan index type of the running index
///
/// Every axis other than the one held fixed keeps its whole extent (@c full_extent).
/// @return @c arene::base::full_extent
template <class IndexType>
constexpr auto select_slice(IndexType /*index*/, std::false_type /*is_fixed_axis*/) noexcept
    -> arene::base::full_extent_t {
  return arene::base::full_extent;
}

/// @brief select the single running index for the iterated axis
/// @tparam IndexType the mdspan index type of the running index
///
/// The axis held fixed is reduced to the single running @c index.
/// @param index the index of the slice being read
/// @return @c index
template <class IndexType>
constexpr auto select_slice(IndexType index, std::true_type /*is_fixed_axis*/) noexcept -> IndexType {
  return index;
}

/// @brief form the submdspan that fixes axis @c Axis of @c mds to @c index
/// @tparam Axis the axis held fixed at @c index; every other axis is kept in full
/// @tparam InSpan an mdspan type
/// @tparam IndexType the mdspan index type of the running index
/// @tparam Positions the source axes @c 0..rank-1
/// @param mds the mdspan to slice
/// @param index the index at which axis @c Axis is fixed
/// @return <tt> submdspan(mds, s0, ..., s_{rank-1}) </tt> where @c s_Axis is @c index and every other @c s_i is @c
/// full_extent (a rank-(N-1) view)
template <std::size_t Axis, class InSpan, class IndexType, std::size_t... Positions>
constexpr auto submdspan_at_axis(InSpan const mds, IndexType index, std::index_sequence<Positions...>) noexcept(
    noexcept(arene::base::submdspan(mds, select_slice(index, std::integral_constant<bool, (Positions == Axis)>{})...))
) -> decltype(arene::base::submdspan(mds, select_slice(index, std::integral_constant<bool, (Positions == Axis)>{})...)
) {
  return arene::base::submdspan(mds, select_slice(index, std::integral_constant<bool, (Positions == Axis)>{})...);
}

/// @brief end policy reporting the size of axis @c Axis of an mdspan
/// @tparam Axis the axis being iterated over
///
/// Supplies the one-past-last index for @c axis_view: the number of slices
/// along axis @c Axis, which is that axis's extent.
template <std::size_t Axis>
class axis_extent_fn {
 public:
  /// @brief report the extent of axis @c Axis of @c mds
  /// @tparam InSpan an mdspan type
  /// @param mds the mdspan whose slices are counted
  /// @return the extent of axis @c Axis of @c mds
  template <class InSpan>
  constexpr auto operator()(InSpan const mds) const
      noexcept(noexcept(mds.extent(static_cast<typename InSpan::rank_type>(Axis))))
          -> decltype(mds.extent(static_cast<typename InSpan::rank_type>(Axis))) {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert"
    static_assert(Axis < InSpan::rank(), "axis_view: Axis must be less than the mdspan rank");
    // parasoft-end-suppress CERT_C-PRE31-c-3
    return mds.extent(static_cast<typename InSpan::rank_type>(Axis));
  }
};

/// @brief index policy reading the @c index -th slice along axis @c Axis of an mdspan
/// @tparam Axis the axis held fixed
///
/// Supplies element access for @c axis_view: the @c index -th slice of the
/// viewed mdspan, obtained by fixing axis @c Axis to @c index and keeping
/// every other axis in full (a rank-(N-1) view).
template <std::size_t Axis>
class submdspan_axis_fn {
 public:
  /// @brief read the @c index -th slice along axis @c Axis of @c mds
  /// @tparam InSpan an mdspan type whose layout mapping is sliceable
  /// @tparam Index the offset type supplied by @c indexable_iterator (a signed distance)
  /// @param mds the mdspan to read a slice from
  /// @param index the slice to read
  /// @return the rank-(N-1) view fixing axis @c Axis of @c mds at @c index
  template <
      class InSpan,
      class Index,
      arene::base::constraints<std::enable_if_t<arene::base::is_sliceable_mapping_v<typename InSpan::mapping_type>>> =
          nullptr>
  constexpr auto operator()(InSpan const mds, Index index) const
      noexcept(noexcept(axis_view_detail::submdspan_at_axis<Axis>(
          mds,
          static_cast<typename InSpan::index_type>(index),
          std::make_index_sequence<InSpan::rank()>{}
      )))
          -> decltype(axis_view_detail::submdspan_at_axis<Axis>(
              mds,
              static_cast<typename InSpan::index_type>(index),
              std::make_index_sequence<InSpan::rank()>{}
          )) {
    // The index is a non-negative slice offset, so the cast to the mdspan index_type is well-defined.
    return axis_view_detail::submdspan_at_axis<Axis>(
        mds,
        static_cast<typename InSpan::index_type>(index),
        std::make_index_sequence<InSpan::rank()>{}
    );
  }
};

/// @brief A function object which creates an iterable range along one axis of a passed-in mdspan
/// @tparam Axis the axis over which the range iterates
template <std::size_t Axis>
constexpr auto range_factory =
    bind_back(detail::make_range_from_indexable, axis_extent_fn<Axis>{}, submdspan_axis_fn<Axis>{});

/// @brief function object implementing <c>axis_view</c>, called to get a view into one or more elements of an axis
/// @tparam Axis the axis held fixed
template <std::size_t Axis>
class axis_view_fn {
 public:
  /// @brief Overload without index: create a single-index range which, when incremented, advances along @c Axis
  /// @tparam InSpan an mdspan type whose layout mapping is sliceable
  /// @param mds the mdspan to iterate over slices of
  /// @return a range with begin and end iterators, where iteration advances along slices of @c Axis
  template <
      class InSpan,
      constraints<std::enable_if_t<is_invocable_v<decltype(range_factory<Axis>), InSpan>>> = nullptr>
  constexpr auto operator()(InSpan mds) const noexcept(is_nothrow_invocable_v<decltype(range_factory<Axis>), InSpan>) {
    return range_factory<Axis>(mds);
  }

  /// @brief Overload with index: return the slice at index @c index of the range created by the non-indexed version
  /// @tparam InSpan an mdspan type whose layout mapping is sliceable
  /// @tparam Index any mdspan index type (probably a @c std::integral_constant or a built-in integral type)
  /// @param mds the mdspan to return a slice of
  /// @param index index of the slice to read
  /// @return the rank-(N-1) view fixing axis @c Axis of @c mds at @c index
  template <
      class InSpan,
      class Index,
      constraints<std::enable_if_t<is_invocable_v<decltype(range_factory<Axis>), InSpan>>> = nullptr>
  constexpr auto operator()(InSpan mds, Index index) const
      noexcept(noexcept(submdspan_at_axis<Axis>(mds, index, std::make_index_sequence<InSpan::rank()>{}))) {
    return submdspan_at_axis<Axis>(mds, index, std::make_index_sequence<InSpan::rank()>{});
  }
};

}  // namespace axis_view_detail

/// @brief Create a range of slices along an mdspan axis.
///
/// Returns a range containing the slices of an mdspan taken along axis
/// @c Axis. The @c i -th element of the returned range is equivalent to
/// <tt> submdspan(A, s0, ..., s_{rank-1}) </tt> where the specifier for
/// axis @c Axis is @c i and every other specifier is @c full_extent, i.e. a
/// rank-(N-1) view of @c A with axis @c Axis fixed at @c i.
///
/// The returned value is a range: it defines @c begin and @c end members and may
/// be used in a range-based for loop and with functions that operate on ranges.
///
/// Since every slice is formed with @c submdspan, the layout mapping of @c A must
/// be sliceable, i.e. <tt> is_sliceable_mapping_v<decltype(A)::mapping_type> </tt>
/// must be @c true. Note: this restriction could be lifted in the future, if an
/// alternative implementation is provided for non-sliceable layouts.
///
/// @snippet docs/examples/axis_view_examples.cpp axis_view_usage_example
///
/// @tparam Axis the axis iterated over, must be less than the rank of the mdspan
template <std::size_t Axis>
extern constexpr auto axis_view = axis_view_detail::axis_view_fn<Axis>{};

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_AXIS_VIEW_HPP_
