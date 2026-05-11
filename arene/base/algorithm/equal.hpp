// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::equal"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file equal.hpp
/// @brief Provides implementation of a backport of C++20's version of std::equal
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_EQUAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_EQUAL_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace equal_detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

///
/// @brief Implementation helper for @c arene::base::equal .
/// @see arene::base::equal(first_start,first_end,second_start,cmp)
///
/// @tparam Itr1 The iterator type for the first range
/// @tparam Itr2 The iterator type for the second range
/// @tparam BinaryPredicate The predicate type to apply to check for equality
/// @param first_itr Iterator to the start of the first sequence
/// @param first_end Iterator to the end of the first sequence
/// @param second_itr Iterator to the start of the second sequence
/// @param cmp Functor to use to compare elements in the sequences. A return of @c true from this functor means
///            elements are equivalent. Will be called exactly once on each set of elements in the two sequences, and
///            at most @c distance(first_itr,first_end) times. If omitted then @c std::equal_to is used.
/// @return bool @c true if all elements in the sequence @c [first_itr,first_end) compare equivalent to the elements in
///         the sequence equivalent to @c [second_itr,second_itr+distance(first_start,first_end)) else @c false
/// @pre The iterator equivalent to @c second_itr+distance(first_itr,first_end) must be a valid iterator in the
///      sequence represented by @c second_itr , else behavior is undefined.
template <typename Itr1, typename Itr2, typename BinaryPredicate>
ARENE_NODISCARD constexpr auto do_equal(Itr1 first_itr, Itr1 first_end, Itr2 second_itr, BinaryPredicate cmp) -> bool {
  while (first_itr != first_end) {
    if (!cmp(*first_itr, *second_itr)) {
      return false;
    }
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "These are iterator types, so incrementing is ok"
    ++first_itr;
    ++second_itr;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
  }
  return true;
}

///
/// @brief Implementation helper for @c arene::base::equal .
/// @see arene::base::equal(first_start,first_end,second_start,second_end,cmp)
///
/// @tparam Itr1 The iterator type for the first range
/// @tparam Itr2 The iterator type for the second range
/// @tparam BinaryPredicate The predicate type to apply to check for equality
/// @param first_itr Iterator to the start of the first sequence
/// @param first_end Iterator to the end of the first sequence
/// @param second_itr Iterator to the start of the second sequence
/// @param second_end Iterator to the end of the second sequence
/// @param cmp Functor to use to compare elements in the sequences. A return of @c true from this functor means
///            elements are equivalent. Will be called exactly once on each set of elements in the two sequences, and
///            at most @c min(distance(first_itr,first_end),distance(second_itr,second_end)) times. If omitted then @c
///            std::equal_to is used.
/// @return bool @c true if the ranges are the same length, and all elements in the sequence @c [first_itr,first_end)
///            compare equivalent to the elements in the sequence equivalent to @c [second_itr,second_end) else @c false
template <typename Itr1, typename Itr2, typename BinaryPredicate>
ARENE_NODISCARD constexpr auto do_equal(
    std::input_iterator_tag,
    Itr1 first_itr,
    Itr1 first_end,
    std::input_iterator_tag,
    Itr2 second_itr,
    Itr2 second_end,
    BinaryPredicate cmp
) noexcept -> bool {
  while (first_itr != first_end && second_itr != second_end) {
    if (!cmp(*first_itr, *second_itr)) {
      return false;
    }
    ++first_itr;
    ++second_itr;
  }
  return first_itr == first_end && second_itr == second_end;
}

///
/// @brief Implementation helper for @c arene::base::equal . It specializes for random-access-iterators, allowing them
///        to short circuit if the sequences are of different size.
/// @see arene::base::equal(first_start,first_end,second_start,second_end,cmp)
///
/// @tparam Itr1 The iterator type for the first range
/// @tparam Itr2 The iterator type for the second range
/// @tparam BinaryPredicate The predicate type to apply to check for equality
/// @param first_itr Iterator to the start of the first sequence
/// @param first_end Iterator to the end of the first sequence
/// @param second_itr Iterator to the start of the second sequence
/// @param second_end Iterator to the end of the second sequence
/// @param cmp Functor to use to compare elements in the sequences. A return of @c true from this functor means
///            elements are equivalent. Will be called exactly once on each set of elements in the two sequences, and
///            at most @c min(distance(first_itr,first_end),distance(second_itr,second_end)) times. If omitted then @c
///            std::equal_to is used.
/// @return bool @c true if the ranges are the same length, and all elements in the sequence @c [first_itr,first_end)
///            compare equivalent to the elements in the sequence equivalent to @c [second_itr,second_end) else @c false
template <typename Itr1, typename Itr2, typename BinaryPredicate>
ARENE_NODISCARD constexpr auto do_equal(
    std::random_access_iterator_tag,
    Itr1 first_itr,
    Itr1 first_end,
    std::random_access_iterator_tag,
    Itr2 second_itr,
    Itr2 second_end,
    BinaryPredicate cmp
) noexcept -> bool {
  // We call the non-second-end consuming overload of do_equal here because we know the ranges are equal length and
  // this saves an iterator comparison per increment plus two iterator comparisons at end for equal sequence.
  return (::arene::base::distance(first_itr, first_end) == ::arene::base::distance(second_itr, second_end)) &&
         do_equal(first_itr, first_end, second_itr, std::move(cmp));
}

// parasoft-end-suppress

/// @brief A class for callable objects that implement equality checks for ranges
class equal_fn {
 public:
  ///
  /// @brief Compares a sequence with an equally sized sequence
  ///
  /// @tparam Itr1 Must model at least @c InputIterator
  /// @tparam Itr2 Must model at least @c InputIterator
  /// @param first_start Iterator to the start of the first sequence
  /// @param first_end Iterator to the end of the first sequence
  /// @param second_start Iterator to the start of the second sequence
  /// @return bool @c true if all elements in the sequence @c [first_start,first_end) compare equivalent to the elements
  ///         in the sequence @c [second_start,second_start+distance(first_start,first_end)) else @c false
  ///
  /// @pre The iterator equivalent to @c second_start+distance(first_start,first_end) must be a valid iterator in the
  ///      sequence represented by @c second_start , else behavior is undefined.
  template <typename Itr1, typename Itr2>
  ARENE_NODISCARD constexpr auto operator()(Itr1 first_start, Itr1 first_end, Itr2 second_start) const noexcept
      -> bool {
    return (*this)(first_start, first_end, second_start, std::equal_to<>{});
  }
  ///
  /// @brief Compares a sequence with an equally sized sequence using a given comparator.
  ///
  /// @tparam Itr1 Must model at least @c InputIterator
  /// @tparam Itr2 Must model at least @c InputIterator
  /// @tparam BinaryPredicate Must satisfy
  ///         <c>is_invocable_r_v<bool, BinaryPredicate, decltype(*first_start), decltype(*first_start)></c>
  /// @param first_start Iterator to the start of the first sequence
  /// @param first_end Iterator to the end of the first sequence
  /// @param second_start Iterator to the start of the second sequence
  /// @param cmp Functor to use to compare elements in the sequences. A return of @c true from this functor means
  ///            elements are equivalent. Will be called exactly once on each set of elements in the two sequences, and
  ///            at most @c distance(first_end,first_start) times.
  /// @return bool @c true if all elements in the sequence @c [first_start,first_end) compare equivalent to the elements
  ///         in the sequence @c [second_start,second_start+distance(first_start,first_end)) else @c false
  ///
  /// @pre The iterator equivalent to @c second_start+distance(first_start,first_end) must be a valid iterator in the
  ///      sequence represented by @c second_start , else behavior is undefined.
  template <typename Itr1, typename Itr2, typename BinaryPredicate>
  ARENE_NODISCARD constexpr auto operator()(Itr1 first_start, Itr1 first_end, Itr2 second_start, BinaryPredicate cmp)
      const noexcept -> bool {
    return do_equal(first_start, first_end, second_start, std::move(cmp));
  }

  ///
  /// @brief Compares a sequence with a potentially differently sized sequence
  ///
  /// @tparam Itr1 Must model at least @c InputIterator
  /// @tparam Itr2 Must model at least @c InputIterator
  /// @param first_start Iterator to the start of the first sequence
  /// @param first_end Iterator to the end of the first sequence
  /// @param second_start Iterator to the start of the second sequence
  /// @param second_end Iterator to the end of the second sequence
  /// @return bool @c true if the ranges are the same length, and all elements in the sequence @c [first_itr,first_end)
  ///            compare equivalent to the elements in the sequence equivalent to @c [second_itr,second_end) else @c
  ///            false
  template <typename Itr1, typename Itr2>
  ARENE_NODISCARD constexpr auto operator()(Itr1 first_start, Itr1 first_end, Itr2 second_start, Itr2 second_end)
      const noexcept -> bool {
    return (*this)(first_start, first_end, second_start, second_end, std::equal_to<>{});
  }
  ///
  /// @brief Compares a sequence with a potentially differently sized sequence using a given comparator.
  ///
  /// @tparam Itr1 Must model at least @c InputIterator
  /// @tparam Itr2 Must model at least @c InputIterator
  /// @tparam BinaryPredicate Must satisfy
  ///         <c>is_invocable_r_v<bool, BinaryPredicate, decltype(*first_start), decltype(*first_start)></c>
  /// @param first_start Iterator to the start of the first sequence
  /// @param first_end Iterator to the end of the first sequence
  /// @param second_start Iterator to the start of the second sequence
  /// @param second_end Iterator to the end of the second sequence
  /// @param cmp Functor to use to compare elements in the sequences. A return of @c true from this functor means
  ///            elements are equivalent. Will be called exactly once on each set of elements in the two sequences, and
  ///            at most @c std::min(distance(first_start,first_end),distance(second_first,second_end) times.
  /// @return bool @c true if the ranges are the same length, and all elements in the sequence @c [first_itr,first_end)
  ///            compare equivalent to the elements in the sequence equivalent to @c [second_itr,second_end) else @c
  ///            false
  template <typename Itr1, typename Itr2, typename BinaryPredicate>
  ARENE_NODISCARD constexpr auto
  operator()(Itr1 first_start, Itr1 first_end, Itr2 second_start, Itr2 second_end, BinaryPredicate cmp) const noexcept
      -> bool {
    return do_equal(
        typename std::iterator_traits<Itr1>::iterator_category{},
        first_start,
        first_end,
        typename std::iterator_traits<Itr2>::iterator_category{},
        second_start,
        second_end,
        std::move(cmp)
    );
  }
};

}  // namespace equal_detail
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(equal_detail::equal_fn, equal);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene
// parasoft-end-suppress AUTOSAR-A2_8_1-a-2
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_EQUAL_HPP_
