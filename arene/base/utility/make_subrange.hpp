// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::make_subrange"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_MAKE_SUBRANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_MAKE_SUBRANGE_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace make_subrange_detail {

///
/// @brief Implementation of a simple range view around an iterator and a sentinel. Morally similar to @c std::subrange.
///
/// @tparam Iterator The type of the position iterator for the range
/// @tparam Sentinel The type of the sentinel for the range
///
template <typename Iterator, typename Sentinel>
class iterator_range {
 public:
  /// @brief The iterator type of the range
  using iterator_type = remove_cvref_t<Iterator>;
  /// @brief The sentinel type of the range
  using sentinel_type = remove_cvref_t<Sentinel>;
  /// @brief The value type of the range
  using value_type = typename std::iterator_traits<iterator_type>::value_type;

  /// @brief Construct an iterator_range from an iterator/sentinel pair
  ///
  /// @param itr The itr
  /// @param sent The sent
  constexpr iterator_range(iterator_type itr, sentinel_type sent) noexcept(
      std::is_nothrow_move_constructible<iterator_type>::value &&
      std::is_nothrow_move_constructible<sentinel_type>::value
  )
      : iterator_(std::move(itr)),
        sentinel_(std::move(sent)) {}

  /// @brief Obtain the iterator for the start of the range
  /// @return The iterator
  constexpr auto begin() const noexcept(std::is_nothrow_copy_constructible<iterator_type>::value) -> iterator_type {
    return iterator_;
  }
  /// @brief Obtain the sentinel for the end of the range
  /// @return The sentinel
  constexpr auto end() const noexcept(std::is_nothrow_copy_constructible<sentinel_type>::value) -> sentinel_type {
    return sentinel_;
  }

 private:
  /// @brief The iterator representing the start of the subrange
  iterator_type iterator_;
  /// @brief The sentinel representing the end of the subrange
  sentinel_type sentinel_;
};
}  // namespace make_subrange_detail

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Combine an iterator and sentinel into a range type that can be used with range-based @c for loops.
/// @tparam Iterator The type of the position iterator for the range
/// @tparam Sentinel The type of the sentinel for the range
/// @param iterator The iterator
/// @param sentinel The sentinel
/// @return A range object for the iterator/sentinel pair
template <
    typename Iterator,
    typename Sentinel,
    constraints<std::enable_if_t<denotes_range_v<Iterator, Sentinel>>> = nullptr>
constexpr auto make_subrange(Iterator iterator, Sentinel sentinel) noexcept(
    std::is_nothrow_move_constructible<Iterator>::value &&  //
    std::is_nothrow_move_constructible<Sentinel>::value
) -> make_subrange_detail::iterator_range<Iterator, Sentinel> {
  return make_subrange_detail::iterator_range<Iterator, Sentinel>{std::move(iterator), std::move(sentinel)};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_MAKE_SUBRANGE_HPP_
