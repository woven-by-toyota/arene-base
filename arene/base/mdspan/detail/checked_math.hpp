// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CHECKED_MATH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CHECKED_MATH_HPP_

#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/detail/is_nonnegative.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief result from a checked math operation
/// @tparam Integer The type of the underlying value representation
template <typename Integer>
struct checked_math_result {
  static_assert(std::is_integral<Integer>::value, "only integer types are supported");

  /// @brief The value resulting from the math operation, may be any value if overflowed
  Integer value{};
  /// @brief A flag indicating whether or not the math operation overflowed at some point
  bool overflowed{};
};

// parasoft-begin-suppress AUTOSAR-A13_5_5-b "Mixed comparisons permitted by A13-5-5 Permit #1"
/// @brief check if a math result is equal to an integer
/// @tparam Integer The type of the underlying value representation
/// @param left left argument
/// @param right right argument
/// @return @c true if @c left has not overflowed and @c left.value is equal to
///   @c right, otherwise @c false
/// @note This is primarily defined as a free-function to avoid the need for
///   many cpptest suppression statements.
template <typename Integer>
constexpr auto operator==(checked_math_result<Integer> left, Integer right) noexcept -> bool {
  return (!left.overflowed) && (left.value == right);
}
// parasoft-end-suppress AUTOSAR-A13_5_5-b

/// @brief function object performing a checked multiplication
/// @tparam Integer The type of the underlying value representation
template <typename Integer>
class checked_multiplies_fn {
 public:
  static_assert(std::is_integral<Integer>::value, "only integer types are supported");

  /// @brief Perform a checked multiplication as part of a chain, preventing and flagging overflow along the way
  /// @param left The left value to multiply; may already have overflowed
  /// @param right The right value to multiply; may already have overflowed
  /// @pre left and right are non-negative if not overflowed
  /// @note handling the negative value case is not necessary for @c submdspan
  /// @return if no overlow, then <c>{left*right, false}</c>;
  ///   else if either argument is equal to @c Integer{}, <c>{0, false}</c>;
  ///   else <c>{X, true}</c>, where @c X is an unspecified value
  constexpr auto operator()(checked_math_result<Integer> left, checked_math_result<Integer> right) const noexcept
      -> checked_math_result<Integer> {
    if ((left == Integer{}) || (right == Integer{})) {
      // Multiplying by 0 means no overflow in the total product even if there previously was one.
      return {};
    }

    if (left.overflowed || right.overflowed) {
      return {Integer{}, true};
    }

    ARENE_PRECONDITION(is_nonnegative(left.value));
    ARENE_PRECONDITION(is_nonnegative(right.value));

    if (right.value > static_cast<Integer>(std::numeric_limits<Integer>::max() / left.value)) {
      return {Integer{}, true};
    }

    return {static_cast<Integer>(left.value * right.value), false};
  }

  /// @brief Perform a checked multiplication as part of a chain, preventing and flagging overflow along the way
  /// @tparam T1 left argument type
  /// @tparam T2 right argument type
  /// @param left The left value to multiply; may already have overflowed
  /// @param right The right value to multiply; may already have overflowed
  /// @pre left and right are non-negative if not overflowed
  /// @note handling the negative value case is not necessary for @c submdspan
  /// @return if no overlow, then <c>{left*right, false}</c>;
  ///   else if either argument is equal to @c Integer{}, <c>{0, false}</c>;
  ///   else <c>{X, true}</c>, where @c X is an unspecified value
  /// @note Mandates <br>
  /// * @c T1 is convertible to @ checked_math_result<Integer>
  /// * @c T2 is convertible to @ checked_math_result<Integer>
  template <typename T1, typename T2>
  constexpr auto operator()(T1 left, T2 right) const noexcept -> checked_math_result<Integer> {
    return (*this)(checked_math_result<Integer>{left}, checked_math_result<Integer>{right});
  }
};

/// @def arene::base::mdspan_detail::checked_multiplies
/// @copydoc arene::base::mdspan_detail::checked_multiplies_fn::operator()
template <class IndexType>
extern constexpr auto checked_multiplies = checked_multiplies_fn<IndexType>{};

/// @brief function object performing a checked addition
/// @tparam Integer The type of the underlying value representation
template <typename Integer>
class checked_plus_fn {
 public:
  static_assert(std::is_integral<Integer>::value, "only integer types are supported");

  /// @brief Perform a checked addition as part of a chain, preventing and flagging overflow along the way
  /// @param left The left value to add; may already have overflowed
  /// @param right The right value to add ; may already have overflowed
  /// @pre left and right are non-negative if not overflowed
  /// @note handling the negative value case is not necessary for @c submdspan
  /// @return if no overlow, then <c>{left+right, false}</c>;
  ///   else <c>{X, true}</c>, where @c X is an unspecified value
  constexpr auto operator()(checked_math_result<Integer> left, checked_math_result<Integer> right) const noexcept
      -> checked_math_result<Integer> {
    if (left.overflowed || right.overflowed) {
      return {Integer{}, true};
    }

    ARENE_PRECONDITION(is_nonnegative(left.value));
    ARENE_PRECONDITION(is_nonnegative(right.value));

    if (left.value > static_cast<Integer>(std::numeric_limits<Integer>::max() - right.value)) {
      return {Integer{}, true};
    }

    return {static_cast<Integer>(left.value + right.value), false};
  }

  /// @brief Perform a checked addition as part of a chain, preventing and flagging overflow along the way
  /// @tparam T1 left argument type
  /// @tparam T2 right argument type
  /// @param left The left value to add; may already have overflowed
  /// @param right The right value to add ; may already have overflowed
  /// @pre left and right are non-negative if not overflowed
  /// @note handling the negative value case is not necessary for @c submdspan
  /// @return if no overlow, then <c>{left+right, false}</c>;
  ///   else <c>{X, true}</c>, where @c X is an unspecified value
  /// @note Mandates <br>
  /// * @c T1 is convertible to @ checked_math_result<Integer>
  /// * @c T2 is convertible to @ checked_math_result<Integer>
  template <typename T1, typename T2>
  constexpr auto operator()(T1 left, T2 right) const noexcept -> checked_math_result<Integer> {
    return (*this)(checked_math_result<Integer>{left}, checked_math_result<Integer>{right});
  }
};

/// @def arene::base::mdspan_detail::checked_plus
/// @copydoc arene::base::mdspan_detail::checked_plus_fn::operator()
template <class IndexType>
extern constexpr auto checked_plus = checked_plus_fn<IndexType>{};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_CHECKED_MATH_HPP_
