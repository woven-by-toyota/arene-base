// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines canonical_slice"

// Copyright 2026, Toyota Motor Corporation
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CANONICAL_SLICE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CANONICAL_SLICE_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/canonical_index.hpp"
#include "arene/base/mdspan/detail/de_ice.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/is_submdspan_slice.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/type_traits/is_integral_constant_like.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/priority_tag.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace mdspan_detail {
namespace canonical_slice_detail {

/// @brief helper to check if an implicit conversion allowed and @c noexcept
/// @tparam From type to convert from
/// @tparam To type to convert to
template <class From, class To, class = constraints<>>
extern constexpr bool is_nothrow_convertible_v = is_nothrow_implicitly_constructible_v<To, From>;

// parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Used by
// 'canonical_slice_fn' in this same header"
/// @brief determine the number of extents in a strided interval
/// @tparam Integer integer type
/// @param distance interval distance
/// @param stride stride
/// @return number of extents in the strided interval
template <class Integer>
static constexpr auto extent_in_strided_interval(Integer distance, Integer stride) noexcept -> Integer {
  if (distance == Integer{}) {
    return {};
  }

  --distance;
  return static_cast<Integer>(Integer{1} + static_cast<Integer>(distance / stride));
}
// parasoft-end-suppress AUTOSAR-A0_1_3-a

}  // namespace canonical_slice_detail

/// @brief convert a slice to canonical form: one of @c full_extent_t, a single
/// index, or @c extent_slice
/// @tparam IndexType index type
template <class IndexType>
class canonical_slice_fn {
  /// @brief canonical index type
  /// @tparam Slice slice type
  template <class Slice>
  using canonical_index_t = invoke_result_t<canonical_index_fn<IndexType>, Slice>;

  /// @brief overload for @c Slice types convertible to @c full_extent_t
  /// @tparam Slice slice type
  /// @param slice slice value
  /// @return @c slice converted to @c full_extent_t
  template <  //
          class Slice,
          constraints<std::enable_if_t<  //
              std::is_convertible<Slice, full_extent_t>::value>> = nullptr>
              // NOLINTNEXTLINE(readability-magic-numbers) : number is priority
      static constexpr auto impl(priority_tag<5>, Slice&& slice) noexcept(  //
          canonical_slice_detail::is_nothrow_convertible_v<Slice, full_extent_t>
      ) -> full_extent_t {
    static_assert(std::is_rvalue_reference<Slice&&>::value, "'Slice&&' must be an rvalue reference");
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a "'slice' is always an rvalue
    // reference by construction in this implementation detail function"
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a-2 "False positive: C-style cast not used"
    // NOLINTNEXTLINE(bugprone-move-forwarding-reference)
    return static_cast<full_extent_t>(std::move(slice));
    // parasoft-end-suppress AUTOSAR-A5_2_2-a-2
    // parasoft-end-suppress AUTOSAR-A18_9_2-a
  }

  /// @brief overload for @c Slice types convertible to @c IndexType
  /// @tparam Slice slice type
  /// @param slice slice value
  /// @return canonical-index of @c slice
  template <  //
          class Slice,
          constraints<std::enable_if_t<  //
              std::is_convertible<Slice, IndexType>::value>> = nullptr>
      static constexpr auto impl(priority_tag<4>, Slice&& slice) noexcept(  //
          is_nothrow_invocable_v<canonical_index_fn<IndexType>, Slice&&>
      ) -> canonical_index_t<Slice&&> {
    static_assert(std::is_rvalue_reference<Slice&&>::value, "'Slice&&' must be an rvalue reference");
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a "'slice' is always an rvalue
    // reference by construction in this implementation detail function"
    // NOLINTNEXTLINE(bugprone-move-forwarding-reference)
    return canonical_index<IndexType>(std::move(slice));
    // parasoft-end-suppress AUTOSAR-A18_9_2-a
  }

  /// @brief overload for @c extent_slice types
  /// @tparam Offset extent_slice offset type
  /// @tparam Extent extent_slice extent type
  /// @tparam Stride extent_slice stride type
  /// @param slice slice value
  /// @return canonical specialization of @c extent_slice
  template <  //
      class Offset,
      class Extent,
      class Stride>
  static constexpr auto impl(priority_tag<3>, extent_slice<Offset, Extent, Stride>&& slice) noexcept
      -> extent_slice<canonical_index_t<Offset>, canonical_index_t<Extent>, canonical_index_t<Stride>> {
    return {
        canonical_index<IndexType>(std::move(slice).offset),
        canonical_index<IndexType>(std::move(slice).extent),
        canonical_index<IndexType>(std::move(slice).stride)
    };
  }

  /// @brief overload for @c range_slice types with all integral-constant-likes
  /// @tparam First range_slice first type
  /// @tparam Last range_slice last type
  /// @tparam Stride range_slice stride type
  /// @tparam ExtentResult deduced integer type for the extent expression
  /// @tparam ExtentType deduced extent_slice extent type
  /// @return canonical specialization of @c extent_slice
  template <  //
      class First,
      class Last,
      class Stride,
      constraints<std::enable_if_t<
          is_integral_constant_like_v<First> &&  //
          is_integral_constant_like_v<Last> &&   //
          is_integral_constant_like_v<Stride>>> = nullptr>
  static constexpr auto impl(priority_tag<2>, range_slice<First, Last, Stride> const&) noexcept -> extent_slice<
      canonical_index_t<First>,
      std::integral_constant<
          IndexType,
          canonical_slice_detail::extent_in_strided_interval(  //
              IndexType{IndexType{Last::value} - IndexType{First::value}},
              IndexType{Stride::value}
          )>,
      canonical_index_t<Stride>> {
    return {};
  }

  /// @brief overload for @c range_slice types with at least one non integral-constant-like
  /// @tparam First range_slice first type
  /// @tparam Last range_slice last type
  /// @tparam Stride range_slice stride type
  /// @param slice slice value
  /// @tparam ExtentResult deduced integer type for the extent expression
  /// @tparam ExtentType deduced extent_slice extent type
  /// @return canonical specialization of @c extent_slice
  /// @pre <c> slice.first <= slice.last </c> is @c true
  /// @pre <c> slice.stride != 0 </c> is @c true
  template <  //
      class First,
      class Last,
      class Stride>
  static constexpr auto impl(priority_tag<1>, range_slice<First, Last, Stride> const& slice) noexcept
      -> extent_slice<  //
          canonical_index_t<First>,
          IndexType,
          canonical_index_t<Stride>> {
    using mdspan_detail::de_ice;
    ARENE_PRECONDITION(cmp_less_equal(de_ice(slice.first), de_ice(slice.last)));
    ARENE_PRECONDITION(cmp_less(IndexType{}, de_ice(slice.stride)));

    constexpr auto to_index = mdspan_detail::representable_cast<IndexType>;

    return {
        canonical_index<IndexType>(slice.first),
        to_index(canonical_slice_detail::extent_in_strided_interval(  //
            to_index(to_index(slice.last) - to_index(slice.first)),
            to_index(slice.stride)
        )),
        canonical_index<IndexType>(slice.stride),
    };
  }

  /// @brief overload for index-pair-likes
  /// @tparam Slice slice type
  /// @param slice slice value
  /// @return specialization of @c extent_slice
  /// @note @c Slice has already been checked to be a submdspan-slice and
  ///   index-pair-like is the last remaining option.
  template <class Slice>
  static constexpr auto impl(priority_tag<0>, Slice&& slice) noexcept(
      noexcept(canonical_index<IndexType>(tuple_detail::get<0>(std::declval<Slice&&>()))) &&  //
      noexcept(canonical_index<IndexType>(tuple_detail::get<1>(std::declval<Slice&&>())))
  )
      // NOLINTBEGIN(bugprone-move-forwarding-reference)
      -> decltype(impl(
          priority_tag<2>{},
          std::declval<range_slice<
              canonical_index_t<decltype(tuple_detail::get<0>(std::move(slice)))>,
              canonical_index_t<decltype(tuple_detail::get<1>(std::move(slice)))>>>()
      )) {
    static_assert(std::is_rvalue_reference<Slice&&>::value, "'Slice&&' must be an rvalue reference");
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a "'slice' is always an rvalue
    // reference by construction in this implementation detail function"
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a-2 "False positive: C-style cast not used"
    // NOLINTNEXTLINE(bugprone-move-forwarding-reference)
    return impl(
        priority_tag<2>{},
        range_slice<
            canonical_index_t<decltype(tuple_detail::get<0>(std::move(slice)))>,
            canonical_index_t<decltype(tuple_detail::get<1>(std::move(slice)))>  //
            >                                                                    //
        {                                                                        //
         canonical_index<IndexType>(tuple_detail::get<0>(std::move(slice))),
         canonical_index<IndexType>(tuple_detail::get<1>(std::move(slice)))
        }
    );
    // parasoft-end-suppress AUTOSAR-A5_2_2-a-2
    // parasoft-end-suppress AUTOSAR-A18_9_2-a
  }
  // NOLINTEND(bugprone-move-forwarding-reference)

 public:
  // NOLINTBEGIN(readability-magic-numbers) : number is priority
  /// @brief convert a slice to canonical form
  /// @tparam Slice slice type
  /// @param slice slice value
  /// @pre @c slice is representable as a value of @c IndexType if @c Slice is
  ///   index-like
  /// @return
  ///   * @c full_extent_t if @c Slice is convertible to @c full_extent_t; otherwise
  ///   * canonical-index of @c slice if @c Slice is convertible to @c IndexType; otherwise
  ///   * a specialization of @c extent_slice
  template <class Slice>
  constexpr auto operator()(Slice&& slice) const noexcept(  //
      noexcept(impl(priority_tag<5>{}, std::declval<Slice&&>()))
  ) -> decltype(impl(priority_tag<5>{}, std::declval<Slice&&>())) {
    static_assert(
        is_submdspan_slice_v<IndexType, Slice>,
        "each type in 'SliceSpecifiers' must be a 'submdspan' slice type"
    );

    static_assert(std::is_rvalue_reference<Slice&&>::value, "'Slice&&' must be an rvalue reference");
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a "'slice' is always an rvalue
    // reference by construction in this implementation detail function"
    // NOLINTNEXTLINE(bugprone-move-forwarding-reference)
    return impl(priority_tag<5>{}, std::move(slice));
    // parasoft-end-suppress AUTOSAR-A18_9_2-a
  }
  // NOLINTEND(readability-magic-numbers)
};

/// @brief function object that transforms a submdspan slice to canonical form
/// @tparam IndexType index type used during canonicalization
///
/// Transforms a submdspan slice @c s to the canonical slice value for
/// @c IndexType:
/// * if @c s is convertible to @c full_extent_t, then @c full_extent
/// * else if @c Slice is convertible to @c IndexType, then
///   <c> canonical_index<IndexType>(), Slice> </c>
/// * else if @c s is a specialization of @c extent_slice, then an
///   @c extent_slice where the offset, extent, and stride members have been
///   canonicalized with @c canonical_index<IndexType>
/// * else if @c s is a specialization of @c range_slice, then an
///   @c extent_slice where the offset and extent are deduced from first and last
///   of the @c range_slice,
/// * else if @c s is a pair-like where each value is convertible to
///   @c IndexType, then is interpreted as a @c range_slice with first and last
///   equal to <c> get<0>(s) </c> and <c> get<1>(s) </c> and then converted to
///   an @c extent_slice using the same as above.
/// All other slice values are ill-formed.
template <class IndexType>
extern constexpr auto canonical_slice = canonical_slice_fn<IndexType>{};

/// @brief obtain the canonical slice type for a submdspan slice
/// @tparam IndexType index type used during canonicalization
/// @tparam Slice submdspan slice type to transform
///
/// Transforms @c Slice to its canonical form:
/// * if @c Slice is convertible to @c full_extent_t, then @c full_extent_t
/// * else if @c Slice is convertible to @c IndexType, then
///   @c canonical_index_t<IndexType, Slice>
/// * else a specialization of @c extent_slice
template <class IndexType, class Slice>
using canonical_slice_t = decltype(canonical_slice<IndexType>(std::declval<Slice>()));

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CANONICAL_SLICE_HPP_
