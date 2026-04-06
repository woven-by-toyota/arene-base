// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_INTEGER_SEQUENCE_OPS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_INTEGER_SEQUENCE_OPS_HPP_

// IWYU pragma: private, include "arene/base/integer_sequences.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace integer_sequence_ops_detail {

/// @brief termination condition for @c arene::base::integer_sequence_cat
template <typename... Sequences>
struct integer_sequence_cat_impl {};

/// @brief Concatenating a single @c integer_sequence is the identity function.
/// @tparam T the type of the elements
/// @tparam Elements the elements in the sequence
template <typename T, T... Elements>
struct integer_sequence_cat_impl<std::integer_sequence<T, Elements...>> {
  /// @brief The input sequence
  using type = std::integer_sequence<T, Elements...>;
};

/// @brief Concatenates two @c std::integer_sequences.
///
/// @tparam T the type of the elements
/// @tparam SequenceElements1 the sequence of values in the first type sequence
/// @tparam SequenceElements2 the sequence of values in the second type sequence
template <typename T, T... SequenceElements1, T... SequenceElements2>
struct integer_sequence_cat_impl<
    std::integer_sequence<T, SequenceElements1...>,
    std::integer_sequence<T, SequenceElements2...>> {
  /// @brief An @c std::integer_sequence which contains all the elements in @c SequenceElements1 followed by all the
  /// elements in
  /// @c SequenceElements2
  using type = std::integer_sequence<T, SequenceElements1..., SequenceElements2...>;
};

/// @brief Concatenates a @c std::integer_sequence with multiple sequences.
/// @tparam Sequence1 the first sequence to concatenate
/// @tparam OtherSequences the other sequences to concatenate
template <typename Sequence1, typename... OtherSequences>
struct integer_sequence_cat_impl<Sequence1, OtherSequences...> {
  /// @brief An @c std::integer_sequence which contains all the elements in @c Sequence1, followed by the concatenation
  /// of all the elements from all the sequences in @c OtherSequences
  using type =
      typename integer_sequence_cat_impl<Sequence1, typename integer_sequence_cat_impl<OtherSequences...>::type>::type;
};

}  // namespace integer_sequence_ops_detail

/// @brief Produces a single @c std::integer_sequence which is the concatenation of multiple @c std::integer_sequence
/// @tparam Sequences the sequence of @c std::integer_sequence to concatenate
/// @pre Each element in @c Sequences must be an instantiation of @c std::integer_sequence.
/// @return std::integer_sequence A single sequence that contains the elements from all sequences in @c Sequences,
///          maintaining order of appearance.
template <typename... Sequences>
using integer_sequence_cat = typename integer_sequence_ops_detail::integer_sequence_cat_impl<Sequences...>::type;

/// @brief Trait providing the @c Index th element of a @c std::integer_sequence.
/// The @c value member holds the value. If @c Index is out of range, there is no @c value member.
/// @tparam Index the index of the element to retrieve
/// @tparam Sequence the sequence to retrieve the element from
/// @pre <c>Sequence</c> is an instantiation of @c std::integer_sequence
template <std::size_t Index, typename Sequence, bool = (Index < Sequence::size())>
class integer_sequence_element {};

namespace integer_sequence_ops_detail {

/// @brief Get the specified element of an integer sequence.
/// @tparam Index the index of the element to retrieve
/// @tparam T the type of the elements
/// @tparam Elements the elements in the sequence
/// @return the specified element
/// @pre @c Index must be less than the number of elements in the sequence
template <std::size_t Index, typename T, T... Elements>
constexpr auto get_integer_sequence_element(std::integer_sequence<T, Elements...>) noexcept -> T {
  static_assert(Index < sizeof...(Elements), "Index must be in range of the sequence");
  return *(arene::base::next(std::initializer_list<T>{Elements...}.begin(), static_cast<std::ptrdiff_t>(Index)));
}

}  // namespace integer_sequence_ops_detail

// @cond INTERNAL
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: Documented by reference"
/// @copydoc arene::base::integer_sequence_element<Index, Sequence, bool>
template <std::size_t Index, typename T, T... Elements>
class integer_sequence_element<Index, std::integer_sequence<T, Elements...>, true>
    : public std::integral_constant<
          T,
          integer_sequence_ops_detail::get_integer_sequence_element<Index>(std::integer_sequence<T, Elements...>{})> {};
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2
// @endcond

/// @brief The value of the @c Index th element of a @c std::integer_sequence.
/// @see arene::base::integer_sequence_element
/// @pre @c Sequence must be an instantiation of @c std::integer_sequence.
template <std::size_t Index, typename Sequence>
extern constexpr auto integer_sequence_element_v = integer_sequence_element<Index, Sequence>::value;

namespace integer_sequence_ops_detail {
/// @brief Check if an integer sequence contains a value.
/// @tparam T the type of the value to search for
/// @tparam Value the value to search for
/// @tparam Elements the elements in the sequence
/// @param value The value to search for
/// @return true if @c Value is in @c Elements
/// @return false otherwise.
template <typename T, T... Elements>
constexpr auto integer_sequence_contains(T value, std::integer_sequence<T, Elements...>) noexcept -> bool {
  for (T const element : {Elements...}) {
    if (element == value) {
      return true;
    }
  }
  return false;
}
/// @brief Check if an empty integer sequence contains a value.
/// @tparam T the type of the value to search for
/// @return false
template <typename T>
constexpr auto integer_sequence_contains(T, std::integer_sequence<T>) noexcept -> bool {
  return false;
}

}  // namespace integer_sequence_ops_detail

/// @brief Checks if the specified integer sequence contains the specified value.
/// @tparam Sequence the sequence to search
/// @tparam value the value to search for
/// @pre @c Sequence must be an instantiation of @c std::integer_sequence.
template <typename Sequence, typename Sequence::value_type Value>
extern constexpr bool integer_sequence_contains_v =
    integer_sequence_ops_detail::integer_sequence_contains(Value, Sequence{});

/// @brief Trait for the index of the first occurrence of a value in a @c std::integer_sequence.
///
/// The result is stored in the @c value member if it is found, otherwise the member @c value is not provided.
/// @tparam Sequence the sequence to search
/// @tparam value the value to search for
/// @pre @c Sequence must be an instantiation of @c std::integer_sequence.
template <typename Sequence, typename Sequence::value_type Value, bool = integer_sequence_contains_v<Sequence, Value>>
struct integer_sequence_index_of {};

namespace integer_sequence_ops_detail {

/// @brief Implementation helper for @c get_integer_sequence_index_of to detemplitize it and allow 100% branch coverage.
/// @tparam T the type of the value to search for
/// @param value the value to search for
/// @param elements the elements in the sequence
/// @return The index of the first instance of <c>value</c> in <c>elements</c>, else @c elements.size() if not found.
template <typename T>
constexpr auto get_integer_sequence_index_of_impl(T value, std::initializer_list<T> elements) noexcept -> std::size_t {
  std::size_t index{0U};
  for (T const element : elements) {
    if (element == value) {
      return index;
    }
    ++index;
  }
  return index;
}

/// @brief Return the index of a value in an integer sequence
/// @tparam T the type of the value to search for
/// @tparam Value the value to search for
/// @tparam Elements the elements in the sequence
/// @return The index of <c>Value</c> in <c>Elements</c>
/// @pre Value must be present in @c Elements else compilation will fail.
template <typename T, T Value, T... Elements>
constexpr auto get_integer_sequence_index_of(std::integer_sequence<T, Elements...>) noexcept -> std::size_t {
  constexpr std::size_t index{get_integer_sequence_index_of_impl(Value, {Elements...})};
  static_assert(index < sizeof...(Elements), "Value must be in input sequence!");
  return index;
}
}  // namespace integer_sequence_ops_detail

// @cond INTERNAL
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: Documented by reference"
/// @copydoc arene::base::integer_sequence_index_of<Sequence, Value, bool>
template <typename Sequence, typename Sequence::value_type Value>
class integer_sequence_index_of<Sequence, Value, true>
    : public std::integral_constant<
          std::size_t,
          integer_sequence_ops_detail::get_integer_sequence_index_of<typename Sequence::value_type, Value>(Sequence{}
          )> {};
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2
// @endcond

/// @brief The index of the first occurrence of a value in a @c std::integer_sequence.
/// @see arene::base::integer_sequence_index_of_v
/// @pre @c Sequence must be an instantiation of @c std::integer_sequence.
/// @pre @c value must occur in @c Sequence else compilation will fail.
template <typename Sequence, typename Sequence::value_type Value>
extern constexpr std::size_t integer_sequence_index_of_v = integer_sequence_index_of<Sequence, Value>::value;

namespace integer_sequence_ops_detail {
/// @brief Return the number of occurrences of a value in an integer sequence
/// @tparam T the type of the value to search for
/// @tparam Value the value to search for
/// @tparam Elements the elements in the sequence
/// @param value The element to search for
/// @return The number of occurrences of @c value in @c Elements
template <typename T, T... Elements>
constexpr auto get_integer_sequence_count_of(T value, std::integer_sequence<T, Elements...>) noexcept -> std::size_t {
  std::size_t count{0U};
  for (T const element : {Elements...}) {
    if (element == value) {
      ++count;
    }
  }
  return count;
}
/// @brief Return the number of occurrences of a value in an empty integer sequence
/// @tparam T the type of the value to search for
/// @return Zero
template <typename T>
constexpr auto get_integer_sequence_count_of(T, std::integer_sequence<T>) noexcept -> std::size_t {
  return 0U;
}

}  // namespace integer_sequence_ops_detail

/// @brief Get the number of occurrences of a value in the provided sequence.
/// The result is stored in the <c>value</c> member.
/// @tparam Sequence the sequence to search
/// @tparam value the value to search for
/// @pre @c Sequence must be an instantiation of @c std::integer_sequence.
template <typename Sequence, typename Sequence::value_type Value>
class integer_sequence_count_of
    : public std::integral_constant<
          std::size_t,
          integer_sequence_ops_detail::get_integer_sequence_count_of(Value, Sequence{})> {};

/// @brief Get the number of occurrences of a value in the provided sequence.
/// @tparam Sequence the sequence to search
/// @tparam value the value to search for
/// @pre @c Sequence must be an instantiation of @c std::integer_sequence.
template <typename Sequence, typename Sequence::value_type Value>
extern constexpr std::size_t integer_sequence_count_of_v = integer_sequence_count_of<Sequence, Value>::value;

namespace integer_sequence_ops_detail {
/// @brief Create a new integer sequence with the unique values from the provided
/// integer sequence. The values in the resultant sequence are in the order of
/// their first occurrence in the provided sequence.
/// The resultant sequence is returned in the <c>type</c> member type.
/// @tparam Sequence the provided integer sequence
/// @pre <c>Sequence</c> must be an instantiation of
/// <c>std::integer_sequence</c>
template <typename Sequence>
class integer_sequence_unique_elements_impl;

/// @brief Create a new integer sequence with the unique values from the provided
/// integer sequence. The values in the resultant sequence are in the order of
/// their first occurrence in the provided sequence.
/// The resultant sequence is returned in the <c>type</c> member type.
/// @tparam T the type of the integer sequence elements
template <typename T>
class integer_sequence_unique_elements_impl<std::integer_sequence<T>> {
 public:
  /// @brief The resulting sequence
  using type = std::integer_sequence<T>;
};

/// @brief Merge two integer sequences together to produce a result with no duplicates.
/// The resultant sequence is returned in the <c>type</c> member type.
/// @tparam UniqueSequence The first sequence
/// @tparam Sequence2 The second sequence
/// @pre <c>UniqueSequence</c> and <c>Sequence2</c> must both be instantiations
/// of <c>std::integer_sequence</c> with the same element type.
/// <c>UniqueSequence</c> must not have any duplicate values.
template <typename UniqueSequence, typename Sequence2>
class integer_sequence_merge_unique_elements_impl;

/// @brief Merge two integer sequences together to produce a result with no duplicates.
/// The resultant sequence is returned in the <c>type</c> member type.
/// @tparam UniqueSequence The first sequence
/// @tparam T The type of elements in the second sequence
/// @pre <c>UniqueSequence</c> must both be an instantiation of
/// <c>std::integer_sequence</c> with an element type of <c>T</c>.
/// <c>UniqueSequence</c> must not have any duplicate values
template <typename UniqueSequence, typename T>
class integer_sequence_merge_unique_elements_impl<UniqueSequence, std::integer_sequence<T>> {
 public:
  /// @brief The resulting sequence
  using type = UniqueSequence;
};

/// @brief Merge two integer sequences together to produce a result with no duplicates.
/// The resultant sequence is returned in the <c>type</c> member type.
/// @tparam T The type of elements in the sequences
/// @tparam UniqueElements The elements in the first sequence
/// @tparam FirstElement The first element in the second sequence
/// @tparam OtherElements The other elements in the second sequence
/// @pre <c>UniqueElements</c> must not have any duplicate values
template <typename T, T... UniqueElements, T FirstElement, T... OtherElements>
class integer_sequence_merge_unique_elements_impl<
    std::integer_sequence<T, UniqueElements...>,
    std::integer_sequence<T, FirstElement, OtherElements...>> {
 private:
  /// @brief The current sequence of unique elements
  using unique_sequence = std::integer_sequence<T, UniqueElements...>;
  /// @brief A sequence of the remaining unprocessed elements from the second sequence
  using remainder_sequence = std::integer_sequence<T, OtherElements...>;

 public:
  /// @brief The resulting sequence
  using type = typename std::conditional_t<
      integer_sequence_contains_v<unique_sequence, FirstElement>,
      integer_sequence_merge_unique_elements_impl<unique_sequence, remainder_sequence>,
      integer_sequence_merge_unique_elements_impl<
          std::integer_sequence<T, UniqueElements..., FirstElement>,
          remainder_sequence>>::type;
};

}  // namespace integer_sequence_ops_detail

/// @brief Create a new integer sequence with the unique values from the provided
/// integer sequence. The values in the resultant sequence are in the order of
/// their first occurrence in the provided sequence.
/// @tparam Sequence the provided integer sequence
/// @pre <c>Sequence</c> must be an instantiation of
/// <c>std::integer_sequence</c>
template <typename Sequence>
using integer_sequence_unique_elements = typename integer_sequence_ops_detail::
    integer_sequence_merge_unique_elements_impl<std::integer_sequence<typename Sequence::value_type>, Sequence>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_INTEGER_SEQUENCE_OPS_HPP_
