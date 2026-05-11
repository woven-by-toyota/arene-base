// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file lexicographical_compare.hpp
/// @brief Provides an implementation of a backport of @c std::lexicographical_compare from C++20
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace lexicographical_compare_detail {

/// @brief Implementation class for performing lexicographical comparison across two iterator ranges based on a binary
/// predicate
class lexicographical_compare_fn {
 public:
  /// @brief Performa lexicographical comparison across two iterator ranges based on a binary predicate
  /// @tparam Itr1 The iterator type for the first range
  /// @tparam Itr2 The iterator type for the second range
  /// @tparam BinaryPredicate The type of the predicate
  /// @param first_itr The start of the first range
  /// @param first_end The end of the first range
  /// @param second_itr The start of the second range
  /// @param second_end The end of the second range
  /// @param cmp The predicate to use for comparison
  /// @return bool @c true if the first range is lexicographically before the second, given the predicate, @c false
  /// otherwise
  template <typename Itr1, typename Itr2, typename BinaryPredicate = std::less<>>
  ARENE_NODISCARD constexpr auto
  operator()(Itr1 first_itr, Itr1 first_end, Itr2 second_itr, Itr2 second_end, BinaryPredicate cmp = {}) const noexcept
      -> bool {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        is_invocable_r_v<bool, BinaryPredicate, decltype(*first_itr), decltype(*second_itr)>,
        "cmp must be invocable with the elements from the input sequences and return a boolean-convertible value"
    );
    static_assert(
        is_invocable_r_v<bool, BinaryPredicate, decltype(*second_itr), decltype(*first_itr)>,
        "cmp must be invocable with the elements from the input sequences and return a boolean-convertible value"
    );
    // parasoft-end-suppress CERT_C-PRE31-c-3
    bool first_at_end{first_itr == first_end};
    bool second_at_end{second_itr == second_end};
    while ((!first_at_end) && (!second_at_end)) {
      if (cmp(*first_itr, *second_itr)) {
        return true;
      }
      if (cmp(*second_itr, *first_itr)) {
        return false;
      }
      arene::base::advance(first_itr, 1);
      arene::base::advance(second_itr, 1);
      first_at_end = first_itr == first_end;
      second_at_end = second_itr == second_end;
    }
    return first_at_end && (!second_at_end);
  }
};

/// @brief Implementation class for performing lexicographical comparison across two iterator ranges based on a
/// three-way comparison predicate
class lexicographical_compare_three_way_fn {
 public:
  /// @brief Performa lexicographical comparison across two iterator ranges based on a binary predicate
  /// @tparam Itr1 The iterator type for the first range
  /// @tparam Itr2 The iterator type for the second range
  /// @tparam Comparator The type of the comparison predicate
  /// @param first_itr The start of the first range
  /// @param first_end The end of the first range
  /// @param second_itr The start of the second range
  /// @param second_end The end of the second range
  /// @param tw_comp The predicate to use for comparison
  /// @return strong_ordering @c strong_ordering::less if the first range is lexicographically ordered before the
  /// second, @c strong_ordering::equal if the two ranges are lexicographically equivalent, otherwise @c
  /// strong_ordering::greater to indicate that the first range is lexicographically ordered after the second, according
  /// to the supplied predicate
  template <typename Itr1, typename Itr2, typename Comparator = compare_three_way>
  ARENE_NODISCARD constexpr auto
  operator()(Itr1 first_itr, Itr1 first_end, Itr2 second_itr, Itr2 second_end, Comparator tw_comp = {}) const noexcept
      -> strong_ordering {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        is_invocable_r_v<strong_ordering, Comparator, decltype(*first_itr), decltype(*second_itr)>,
        "tw_comp must be invocable with the value_type of the intput iterators and return a strong_ordering"
    );
    // parasoft-end-suppress CERT_C-PRE31-c-3
    bool first_at_end{first_itr == first_end};
    bool second_at_end{second_itr == second_end};
    while ((!first_at_end) && (!second_at_end)) {
      auto const comp_r = tw_comp(*first_itr, *second_itr);
      if (comp_r != strong_ordering::equal) {
        return comp_r;
      }
      arene::base::advance(first_itr, 1);
      arene::base::advance(second_itr, 1);
      first_at_end = first_itr == first_end;
      second_at_end = second_itr == second_end;
    }
    if (first_at_end && (!second_at_end)) {
      return strong_ordering::less;
    }
    return first_at_end && second_at_end ? strong_ordering::equal : strong_ordering::greater;
  }
};

}  // namespace lexicographical_compare_detail

/// @def arene::base::lexicographical_compare
/// @copydoc arene::base::lexicographical_compare_detail::lexicographical_compare_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(lexicographical_compare_detail::lexicographical_compare_fn, lexicographical_compare);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

/// @def arene::base::lexicographical_compare_three_way
/// @copydoc arene::base::lexicographical_compare_detail::lexicographical_compare_three_way_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(
    lexicographical_compare_detail::lexicographical_compare_three_way_fn,
    lexicographical_compare_three_way
);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP_
