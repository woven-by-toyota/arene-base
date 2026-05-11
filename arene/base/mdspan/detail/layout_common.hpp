// parasoft-begin-suppress AUTOSAR-A2_8_1-a "The check_bypasser class is common to all layouts."

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_LAYOUT_COMMON_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_LAYOUT_COMMON_HPP_

// IWYU pragma: private, include "arene/base/mdspan/layout.hpp"
// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/checked_math.hpp"
#include "arene/base/mdspan/detail/strided_mapping_base.hpp"  // IWYU pragma: keep
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

/// @brief A strided data layout, i.e. incrementing any dimension moves the output by that dimension's constant stride
struct layout_stride {
  /// @brief A mapping from a logical pack of indices into a single flat physical output index
  /// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
  template <class Extents>
  class mapping;
};

/// @brief A strided data layout in which the leftmost stride is the largest and the rightmost stride is the smallest
struct layout_right {
  /// @brief A mapping from a logical pack of indices into a single flat physical output index
  /// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
  template <class Extents>
  class mapping;
};

/// @brief A strided data layout in which the leftmost stride is the smallest and the rightmost stride is the largest
struct layout_left {
  /// @brief A mapping from a logical pack of indices into a single flat physical output index
  /// @tparam The extents of the space used for this mapping; must be a specialization of @c extents
  template <class Extents>
  class mapping;
};

namespace layout_detail {

/// @brief Get a set of strides with the smallest stride on the right and the largest on the left
/// @tparam Extents The type of the extents to use, deduced from @c extents
/// @param extents An extents object
/// @return An array of strides; the rightmost is @c 1 and each entry is the product of the extents to its right
/// @pre Each stride entry will fit in @c Extents::index_type without wrapping, else @c ARENE_PRECONDITION violation
template <typename Extents, constraints<std::enable_if_t<Extents::rank() != 0U>> = nullptr>
constexpr auto right_strides(Extents const& extents) noexcept
    -> ::arene::base::array<typename Extents::index_type, Extents::rank()> {
  using index_type = typename Extents::index_type;
  using rank_type = typename Extents::rank_type;

  ::arene::base::array<index_type, Extents::rank()> strides{};
  mdspan_detail::checked_math_result<index_type> current_stride{index_type{1}};
  strides[Extents::rank() - rank_type{1}] = current_stride.value;
  for (rank_type dim{Extents::rank() - rank_type{1}}; dim > rank_type{}; --dim) {
    current_stride = mdspan_detail::checked_multiplies<index_type>(current_stride, extents.extent(dim));
    if (current_stride.value == index_type{}) {
      // The standard doesn't allow strides to be 0, which can be naively computed if an extent is 0.
      // If any extents are 0, trying to use this mapping will always go out of bounds so the value doesn't matter.
      current_stride.value = index_type{1};
    }
    strides[dim - rank_type{1}] = current_stride.value;
  }
  ARENE_PRECONDITION(!current_stride.overflowed);

  return strides;
}

/// @brief Get a set of strides with the smallest stride on the right and the largest on the left
/// @note This special overload for <c>rank == 0</c> exists entirely to make it easier to measure test coverage.
/// @tparam Extents The type of the extents to use, deduced from the first parameter
/// @return An array of strides; the rightmost is @c 1 and each entry is the product of the extents to its right
template <typename Extents, constraints<std::enable_if_t<Extents::rank() == 0U>> = nullptr>
constexpr auto right_strides(Extents const&) noexcept
    -> ::arene::base::array<typename Extents::index_type, Extents::rank()> {
  return {};
}

// ----- Constraints and precondition checks for various constructors -----

/// @brief Cast one extents type to another, checking along the way that the product of the extents can be represented
/// @tparam ToExtents The output type of the cast
/// @tparam FromExtents The input type of the cast
/// @param from_extents A set of extents to cast to @c ToExtents
/// @pre The product of all the extents in @c from_extents can be represented as a <c>ToExtents::index_type</c>, else
/// @c ARENE_PRECONDITION violation
/// @return @c from_extents represented as a @c to_extents
template <
    typename ToExtents,
    typename FromExtents,
    constraints<std::enable_if_t<!std::is_convertible<FromExtents, ToExtents>::value>> = nullptr>
constexpr auto extents_cast(FromExtents&& from_extents) noexcept -> ToExtents {
  ToExtents const output{std::forward<FromExtents>(from_extents)};

  // Checking this for overflow in the *destination* index type; if it overflows, we crash.
  precondition_not_overflowed(extent_product(output));
  return output;
}

/// @brief Cast one extents type to another, checking along the way that the product of the extents can be represented
/// @tparam ToExtents The output type of the cast
/// @tparam FromExtents The input type of the cast
/// @param from_extents A set of extents to cast to @c ToExtents
/// @pre The product of all the extents in @c from_extents can be represented as a <c>ToExtents::index_type</c>, else
/// @c ARENE_PRECONDITION violation
/// @return @c from_extents represented as a @c to_extents
template <
    typename ToExtents,
    typename FromExtents,
    constraints<std::enable_if_t<std::is_convertible<FromExtents, ToExtents>::value>> = nullptr>
constexpr auto extents_cast(FromExtents&& from_extents) noexcept -> ToExtents {
  return std::forward<FromExtents>(from_extents);
}

/// @brief Check that two sets of strides are equal to each other, crashing with a precondition violation if not
/// @tparam LeftIndex The @c index_type of the @c mapping from which @c left was taken
/// @tparam RightIndex The @c index_type of the @c mapping from which @c right was taken
/// @param left The left list of strides to compare
/// @param right The right list of strides to compare
/// @pre The elements of @c left and @c right are equal to each other, else @c ARENE_PRECONDITION violation
template <typename LeftIndex, typename RightIndex>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) These parameters are symmetric; swapping them changes nothing
constexpr auto precondition_strides_equal(span<LeftIndex const> left, span<RightIndex const> right) noexcept -> void {
  ARENE_PRECONDITION(equal(left.begin(), left.end(), right.begin(), right.end()));
}

/// @brief A passkey class used to bypass construction precondition checks; only usable from provided layout types
class check_bypasser {
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "friend declaration is used for passkey, not for exposing private data"
  template <class Extents>
  friend class layout_stride::mapping;
  template <class Extents>
  friend class layout_left::mapping;
  template <class Extents>
  friend class layout_right::mapping;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  /// @brief Construct a check_bypasser (private so only callable from friend classes)
  explicit constexpr check_bypasser() noexcept = default;

 public:
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: Rule does not require all parameters to be named"
  /// @brief Move a check_bypasser
  constexpr check_bypasser(check_bypasser&&) noexcept = default;
  /// @brief Move assign a check_bypasser
  constexpr auto operator=(check_bypasser&&) noexcept -> check_bypasser& = default;

  /// @brief Copy a check_bypasser
  constexpr check_bypasser(check_bypasser const&) noexcept = delete;
  /// @brief Copy assign a check_bypasser
  constexpr auto operator=(check_bypasser const&) noexcept -> check_bypasser& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Destroy a check_bypasser
  ~check_bypasser() = default;
};

}  // namespace layout_detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"
// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_LAYOUT_COMMON_HPP_
