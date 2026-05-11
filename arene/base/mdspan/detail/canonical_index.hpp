// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines canonical_index"

// Copyright 2026, Toyota Motor Corporation
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CANONICAL_INDEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CANONICAL_INDEX_HPP_

#include "arene/base/mdspan/detail/is_representable_as.hpp"
#include "arene/base/mdspan/detail/representable_cast.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/is_integral_constant_like.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief convert an index-like object to canonical form: an instance of
/// @c IndexType or @c integral_constant<IndexType, N>
/// @tparam IndexType index type
template <class IndexType>
class canonical_index_fn {
  /// @brief helper for the integral-constant-like case
  /// @tparam Slice integral-constant-like slice type
  /// @return @c std::integral_constant<IndexType, Slice::value>
  template <class Slice>
  static constexpr auto impl(std::true_type, Slice const&) noexcept
      -> std::integral_constant<IndexType, IndexType(Slice::value)> {
    static_assert(
        mdspan_detail::is_representable_as<IndexType>(Slice::value),
        "value of integral-constant-like must be representable as the extents' index type"
    );
    return {};
  }

  /// @brief helper for the non integral-constant-like case
  /// @tparam Slice index-like slice type
  /// @param slice index-like value
  /// @pre @c slice is representable as a value of @c IndexType
  /// @return @c IndexType
  template <class Slice>
  static constexpr auto impl(std::false_type, Slice&& slice) noexcept(  //
      std::is_nothrow_constructible<IndexType, Slice&&>::value
  ) -> IndexType {
    static_assert(std::is_rvalue_reference<Slice&&>::value, "'Slice&&' must be an rvalue reference");
    // parasoft-begin-suppress AUTOSAR-A18_9_2-a "'slice' is always an rvalue
    // reference by construction in this implementation detail function"
    // NOLINTNEXTLINE(bugprone-move-forwarding-reference)
    return mdspan_detail::representable_cast<IndexType>(std::move(slice));
    // parasoft-end-suppress AUTOSAR-A18_9_2-a
  }

 public:
  /// @brief convert an index-like slice to canonical form
  /// @tparam Slice index-like slice type
  /// @param slice index-like value
  /// @pre @c slice is representable as a value of @c IndexType
  /// @return @c std::integral_constant<IndexType, Slice::value> if @c Slice is
  ///   integral-constant-like, otherwise @c IndexType(std::move(slice))
  template <class Slice>
  constexpr auto operator()(Slice slice) const noexcept(  //
      noexcept(impl(is_integral_constant_like<Slice>{}, std::declval<Slice&&>()))
  ) -> decltype(impl(is_integral_constant_like<Slice>{}, std::declval<Slice&&>())) {
    return impl(is_integral_constant_like<Slice>{}, std::move(slice));
  }
};

/// @brief function object that transforms an index-like slice to canonical form
/// @tparam IndexType index type used during canonicalization
///
/// Transforms an index-like submdspan slice @c s to the canonical index value
/// for @c IndexType:
/// * if @c s is integral-constant-like, then this is equivalent to
///   <c> integral_constant<IndexType, static_cast<IndexType>(s)>{} </c>
/// * else <c> static_cast<IndexType>(s) </c>
template <class IndexType>
extern constexpr auto canonical_index = canonical_index_fn<IndexType>{};

/// @brief obtain the canonical index type for an index-like slice
/// @tparam IndexType index type to use during canonicalization
/// @tparam Slice index-like submdspan slice type to transform
///
/// Transforms @c Slice to the canonical index type for @c IndexType:
/// @c IndexType or @c integral_constant<IndexType, N>
template <class IndexType, class Slice>
using canonical_index_t = decltype(canonical_index<IndexType>(std::declval<Slice>()));

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CANONICAL_INDEX_HPP_
