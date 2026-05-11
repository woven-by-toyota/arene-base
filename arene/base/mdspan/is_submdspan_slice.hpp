// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SUBMDSPAN_SLICE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SUBMDSPAN_SLICE_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/is_tuple_like.hpp"

namespace arene {
namespace base {
namespace is_submdspan_slice_detail {

/// @brief implementation to check if a type is a submdspan slice type
/// @tparam IndexType mdspan index type
/// @tparam Slice potential submdspan slice type
template <class IndexType, class Slice, class = constraints<>>
extern constexpr bool is_submdspan_slice_v = false;

/// @brief implementation to check if a type is a submdspan slice type
/// @tparam IndexType mdspan index type
/// @tparam Slice potential submdspan slice type
///
/// Specialization that creates a hard error if @c IndexType is not an integer.
template <class IndexType, class Slice>
extern constexpr bool is_submdspan_slice_v<
    IndexType,
    Slice,
    constraints<std::enable_if_t<              //
        !std::is_integral<IndexType>::value>>  //
    > = [] {
  static_assert(  //
      std::is_integral<IndexType>::value,
      "'IndexType' must be an integer."
  );
  return false;
}();

/// @brief implementation to check if a type is a submdspan slice type
/// @tparam IndexType mdspan index type
/// @tparam Slice potential submdspan slice type
///
/// Specialization for everything except index-pair-likes. It is unnecessary to
/// explicitly test convertibility of the @c extent_slice and @c range_slice
/// type aliases.
template <class IndexType, class Slice>
extern constexpr bool is_submdspan_slice_v<
    IndexType,
    Slice,
    constraints<std::enable_if_t<
        std::is_integral<IndexType>::value &&                 //
        (std::is_convertible<Slice, full_extent_t>::value ||  //
         std::is_convertible<Slice, IndexType>::value ||      //
         is_instantiation_of_v<Slice, extent_slice> ||        //
         is_instantiation_of_v<Slice, range_slice>)>>> = true;

/// @brief implementation to check if a type is a submdspan slice type
/// @tparam IndexType mdspan index type
/// @tparam Slice potential submdspan slice type
///
/// Specialization for index-pair-likes.
template <class IndexType, class Slice>
extern constexpr bool is_submdspan_slice_v<  //
    IndexType,
    Slice,
    constraints<std::enable_if_t<              //
        std::is_integral<IndexType>::value &&  //
        is_pair_like_v<Slice>>>> =
    std::is_convertible<std::tuple_element_t<0, std::remove_reference_t<Slice>>, IndexType>::value &&
    std::is_convertible<std::tuple_element_t<1, std::remove_reference_t<Slice>>, IndexType>::value;
}  // namespace is_submdspan_slice_detail

/// @brief helper trait to check if a type is a submdspan slice type
/// @tparam IndexType mdspan index type
/// @tparam Slice potential submdspan slice type
///
/// Given a signed or unsigned integer type @c IndexType, a type @c Slice is a
/// submdspan slice type for @c IndexType if at least one of the following holds:
/// * <c> std::is_convertible<Slice, full_extent_t>::value </c> is @c true;
/// * <c> std::is_convertible<Slice, IndexType>::value </c> is @c true;
/// * @c Slice is a specialization of @c extent_slice and
///   <c> std::is_convertible<X, IndexType>::value </c> is @c true
///   for @c X denoting @c Slice::offset_type, @c Slice::extent_type, and
///   @c Slice::stride_type;
/// * @c Slice is a specialization of @c range_slice and
///   <c> std::is_convertible<X, IndexType>::value </c> is @c true
///   for @c X denoting @c Slice::first_type, @c Slice::last_type, and
///   @c Slice::stride_type; or
/// * @c Slice is pair-like, the "first type" of @c Slice is convertible to
///   @c IndexType, and the "second type" of @c Slice is convertible to
///   @c IndexType.
template <class IndexType, class Slice>
extern constexpr bool is_submdspan_slice_v = is_submdspan_slice_detail::is_submdspan_slice_v<IndexType, Slice>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_IS_SUBMDSPAN_SLICE_HPP_
