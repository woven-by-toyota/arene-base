// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_EXTENT_SEQUENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_EXTENT_SEQUENCE_HPP_

#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/detail/rank_scan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {
namespace extent_sequence_detail {

/// @brief sequence of static extents as an array
/// @tparam Extents static extent values
template <std::size_t... Extents>
extern constexpr auto extent_sequence_values = array<std::size_t, sizeof...(Extents)>{Extents...};

/// @brief index mapping into an array holding only dynamic extents
/// @tparam Extents static extent values
template <std::size_t... Extents>
extern constexpr auto dynamic_index_map = rank_scan(  //
    array<bool, sizeof...(Extents)>{(Extents == dynamic_extent)...}
);

/// @brief number of static extents equal to @c dynamic_extent
/// @tparam Extents static extent values
template <std::size_t... Extents>
extern constexpr auto dynamic_rank_v = dynamic_index_map<Extents...>.codomain_size();

}  // namespace extent_sequence_detail

/// @brief a compile-time sequence of static extents
/// @tparam Extents sequence of static extents
///
/// A replacement for @c std::index_sequence. Used in the implementation of
/// @c extents instead of @c std::index_sequence.
///
/// Using @c std::index_sequence as a template parameter of @c extents will add
/// @c std::swap as a candidate during overload resolution when swapping mapping
/// values, since all the mapping types in Arene Base inherit from @c extents.
/// We want to avoid use of @c std::swap as it is not @c constexpr in C++14.
///
template <std::size_t... Extents>
class extent_sequence  //
{
 public:
  /// @brief type of the rank
  using rank_type = std::size_t;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: none of these are 'member data', all are public
  // properties, exposed as part of the API"

  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variables are initialized"

  /// @brief obtain the sequence as an array
  static constexpr auto const& as_array = extent_sequence_detail::extent_sequence_values<Extents...>;

  /// @brief index mapping into an array holding only dynamic extents
  static constexpr auto const& dynamic_index_map = extent_sequence_detail::dynamic_index_map<Extents...>;

  // parasoft-end-suppress CERT_CPP-DCL56-a

  /// @brief number of dimensions in the extents
  static constexpr std::integral_constant<rank_type, sizeof...(Extents)> rank{};

  /// @brief number of dimensions with dynamic extent
  static constexpr std::integral_constant<rank_type, extent_sequence_detail::dynamic_rank_v<Extents...>> rank_dynamic{};

  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief static extent for the given dimension
  /// @param dim index of the dimension for which to get the extent
  /// @return value from @c Extents for the specified dimension
  static constexpr auto static_extent(std::size_t dim) noexcept  //
      -> std::size_t                                             //
  {
    return as_array[dim];
  }
};

/// @brief number of dimensions in the extents
/// @tparam Extents sequence of static extents
template <std::size_t... Extents>
constexpr std::integral_constant<typename extent_sequence<Extents...>::rank_type, sizeof...(Extents)>
    extent_sequence<Extents...>::rank;

/// @brief number of dimensions with dynamic extent
/// @tparam Extents sequence of static extents
template <std::size_t... Extents>
constexpr std::integral_constant<
    typename extent_sequence<Extents...>::rank_type,
    extent_sequence_detail::dynamic_rank_v<Extents...>>
    extent_sequence<Extents...>::rank_dynamic;

/// @brief determine if two @c extent_sequence s are equal
/// @tparam Is values of the first @c extent_sequence
/// @tparam Js values of the second @c extent_sequence
/// @tparam Compare comparison type
/// @param lhs first @c extent_sequence
/// @param rhs second @c extent_sequence
/// @param cmp comparison function
/// @return @c true if <c> cmp(x, y) </c> is @c true for all pairs of
///   corresponding elements @c x and @c y, where @c x is an element in @c lhs
///   and @c y is an element in @c rhs. Otherwise, @c false,
/// @note @c cmp is always required for this function
///
template <std::size_t... Is, std::size_t... Js, class Compare>
constexpr auto extent_sequence_equal(extent_sequence<Is...> lhs, extent_sequence<Js...> rhs, Compare cmp)  //
    -> bool {
  return equal(  //
      lhs.as_array.cbegin(),
      lhs.as_array.cend(),
      rhs.as_array.cbegin(),
      rhs.as_array.cend(),
      cmp
  );
}

/// @brief determines if the left extent is a static extent and the right
///   extent is a dynamic extent
class left_static_extent_and_right_dynamic_extent  //
{
 public:
  /// @brief determines if the left extent is a static extent and the right
  ///   extent is a dynamic extent
  /// @param lhs first extent
  /// @param rhs second extent
  /// @return <c> (lhs != dynamic_extent) && (rhs == dynamic_extent) </c>
  constexpr auto operator()(std::size_t const lhs, std::size_t const rhs) const noexcept  //
      -> bool                                                                             //
  {
    return (lhs != dynamic_extent) && (dynamic_extent == rhs);
  }
};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_EXTENT_SEQUENCE_HPP_
