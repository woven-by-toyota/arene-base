// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_INDEX_PERMUTATION_RANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_INDEX_PERMUTATION_RANGE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/any_of.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/monostate/monostate.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/type_manipulation/consume_values.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace detail {

/// @brief Increment @c value , up to the specified @c max_value , if the incoming @c carry flag is set, and leaves @c
/// value unchanged otherwise. Leaves @c carry set to @c true if the result would be equal to @c max_value, in which
/// case @c value is set to zero. Otherwise sets @c carry to @c false.
/// @tparam IndexType the type of the index being incremented
/// @param value A reference to the value to update
/// @param max_value The maximum value permitted
/// @param carry A reference to the carry flag
/// @return A @c monostate object, to allow use with @c consume_values
template <typename IndexType>
constexpr auto increment_extent_with_carry(IndexType& value, IndexType const max_value, bool& carry) noexcept
    -> monostate {
  if (carry) {
    if ((max_value == IndexType{}) || (value == static_cast<IndexType>(max_value - IndexType{1}))) {
      value = IndexType{};
    } else {
      carry = false;
      ++value;
    }
  }
  return {};
}

// parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameters 'indices' and 'extents' are used"
/// @brief Move the supplied array of @c indices to the next valid value given the @c extents.
/// @tparam IndexType The type of the indices
/// @tparam Dimensions A list of indices up to the rank of the extents
/// @param indices The array of indices to update
/// @param extents The array of extent values representing the max values for each index
/// @return @c true if there was a valid next permutation, @c false otherwise
template <typename IndexType, std::size_t... Dimensions>
constexpr auto
next_extent_permutation(array<IndexType, sizeof...(Dimensions)>& indices, array<IndexType, sizeof...(Dimensions)> const& extents, std::index_sequence<Dimensions...>) noexcept
    -> bool {
  bool carry{true};
  consume_values({increment_extent_with_carry(indices[Dimensions], extents[Dimensions], carry)...});
  return !carry;
}
// parasoft-end-suppress AUTOSAR-A0_1_4-a

/// @brief A class to provide a range of all the permutations of a set of
///   indices with the specified maximum extents
/// @tparam IndexType The type of the indices
/// @tparam Rank The number of indices
///
/// This is a single-pass input range. It does @b not provide a multipass
/// guarantee: all iterators obtained from the same range share the range's
/// internal index state, so incrementing any iterator advances that shared
/// state. As a consequence, only one traversal of a given range object is
/// valid, and iterators to the same range are not independent.
///
/// @attention The array supplied to the constructor is referenced by pointer.
/// It must outlive the range, and it must @b not be modified while the range
/// (or any of its iterators) is in use. Modifying the referenced array
/// invalidates all outstanding iterators and results in undefined behaviour.
template <typename IndexType, std::size_t Rank>
class index_permutation_range {
  /// @brief A pointer to the array of max extents
  array<IndexType, Rank> const* extents_;
  /// @brief The current set of indices
  array<IndexType, Rank> current_indices_;

  /// @brief Advance to the next permutation if there is one.
  /// @return @c true if there is a next permutation, or @c false if there are no more valid permutations
  constexpr auto next() noexcept -> bool {
    return next_extent_permutation(current_indices_, *extents_, std::make_index_sequence<Rank>());
  }

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: standard iterator type aliases do not hide anything"
  /// @brief The iterator class for this range
  class iterator {
    /// @brief A pointer to the range being iterated over
    index_permutation_range* range_;

   public:
    /// @brief The element type of the range
    using value_type = array<IndexType, Rank>;
    /// @brief A reference to the element type
    using reference = value_type const&;
    /// @brief A pointer to the element type
    using pointer = value_type const*;
    /// @brief Iterator distance type
    using difference_type = std::ptrdiff_t;
    /// @brief The iterator category
    using iterator_category = std::input_iterator_tag;

    /// @brief Construct an "end" iterator
    constexpr iterator() noexcept
        : iterator(nullptr) {}

    // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
    /// @brief Construct an interator for the specified range
    /// @param range The range being iterated over
    constexpr explicit iterator(index_permutation_range* const range) noexcept
        : range_(range) {}
    // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "False positive: A11-3-1 allows friend for comparisons"
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
    // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function used in other TUs"
    /// @brief Check if two iterators are equal
    /// @param lhs The first iterator being compared
    /// @param rhs The second iterator being compared
    /// @return @c true if both iterators refer to the same range, or both are "end" iterators, @c false otherwise
    friend constexpr auto operator==(iterator const lhs, iterator const rhs) noexcept -> bool {
      return (lhs.range_ == rhs.range_);
    }
    // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
    // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
    // parasoft-end-suppress AUTOSAR-A11_3_1-a

    // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "False positive: A11-3-1 allows friend for comparisons"
    // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function used in other TUs"
    /// @brief Check if two iterators are not equal
    /// @param lhs The first iterator being compared
    /// @param rhs The second iterator being compared
    /// @return @c !(lhs==rhs)
    friend constexpr auto operator!=(iterator const lhs, iterator const rhs) noexcept -> bool { return !(lhs == rhs); }
    // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
    // parasoft-end-suppress AUTOSAR-A11_3_1-a
    // parasoft-end-suppress AUTOSAR-A7_1_5-a-2

    /// @brief Dereference the iterator
    /// @return A reference to the current index permutation
    /// @pre The iterator must not be an "end" iterator
    constexpr auto operator*() const noexcept -> reference {
      ARENE_PRECONDITION(range_ != nullptr);
      return range_->current_indices_;
    }
    /// @brief Dereference the iterator
    /// @return A pointer to the current index permutation
    /// @pre The iterator must not be an "end" iterator
    constexpr auto operator->() const noexcept -> pointer { return &*(*this); }

    /// @brief Increment the iterator
    /// @return A reference to @c *this
    /// @pre The iterator must not be an "end" iterator
    constexpr auto operator++() noexcept -> iterator& {
      ARENE_PRECONDITION(range_ != nullptr);
      if (!range_->next()) {
        range_ = nullptr;
      }
      return *this;
    }

    // parasoft-begin-suppress AUTOSAR-A3_9_1-b "False positive: postincrement requires an int parameter"
    // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
    /// @brief Increment the iterator
    /// @return A copy of @c *this prior to the increment
    /// @pre The iterator must not be an "end" iterator
    constexpr auto operator++(int) noexcept -> iterator {
      // parasoft-end-suppress CERT_C-EXP37-a-3
      // parasoft-end-suppress AUTOSAR-A3_9_1-b
      iterator temp(*this);
      ++(*this);
      return temp;
    }
  };
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief Construct a range for permuting indices up to the specified maximum values
  /// @param max_extents The maximum values for each index
  constexpr explicit index_permutation_range(array<IndexType, Rank> const& max_extents) noexcept
      : extents_(&max_extents),
        current_indices_{} {}

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'begin' does not hide anything"
  /// @brief Obtain an iterator to the current state of the range
  /// @return An @c iterator for @c *this
  constexpr auto begin() noexcept -> iterator {
    auto const is_empty_range = (Rank == 0U) || any_of(*extents_, bind_back(std::equal_to<>{}, IndexType{}));
    return is_empty_range ? end() : iterator{this};
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'end' does not hide anything"
  /// @brief Obtain an "end" iterator
  /// @return An @c iterator that is an "end" iterator
  static constexpr auto end() noexcept -> iterator { return iterator{}; }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
};

/// @brief function objet to construct an index permutation range
class make_index_permutation_range_fn {
 public:
  /// @brief Construct a range for permuting indices up to the specified maximum values
  /// @tparam IndexType The type of each index
  /// @tparam Rank The number of indices
  /// @param extents The maximum values for each index
  /// @return An @c index_permutation_range for permuting indices up to the specified maximum values
  /// @pre @c extents must outlive the returned @c index_permutation_range
  /// @pre values in @c extents must be non-negative
  /// @attention Any modifycation of @c extents invalidates all outstanding iterators
  template <typename IndexType, std::size_t Rank>
  constexpr auto operator()(array<IndexType, Rank> const& extents) const noexcept
      -> index_permutation_range<IndexType, Rank> {
    static_assert(std::is_integral<IndexType>::value, "'IndexType' must be an integer type");
    return index_permutation_range<IndexType, Rank>{extents};
  }

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief Deleted overload to prevent binding the range to a temporary
  /// @tparam IndexType The type of each index
  /// @tparam Rank The number of indices
  template <typename IndexType, std::size_t Rank>
  auto operator()(array<IndexType, Rank> const&&) const -> void = delete;
  // parasoft-end-suppress CERT_C-EXP37-a
};

/// @def arene::base::detail::make_index_permutation_range
/// @copydoc arene::base::detail::make_index_permutation_range_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(detail::make_index_permutation_range_fn, make_index_permutation_range);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_INDEX_PERMUTATION_RANGE_HPP_
