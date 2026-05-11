// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_ITERATOR_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

// IWYU pragma: private, include <iterator>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints.hpp"
#include "arene/base/iterator.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_default_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_reference.hpp"
#include "stdlib/include/stdlib_detail/iterator_traits.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

namespace std {
// parasoft-begin-suppress AUTOSAR-A13_5_1-a "This is implemented as specified by the C++ Standard"
/// @brief An iterator adaptor that wraps another iterator such that dereferencing the wrapped iterator yields an rvalue
/// reference to the source value
/// @tparam Iterator the type of the underlying iterator
template <typename Iterator>
class move_iterator {
  static_assert(
      arene::base::is_input_iterator_v<Iterator>,
      "The underlying iterator must be at least an input iterator"
  );

  /// @brief The stored value of the underlying iterator
  Iterator current_;

 public:
  // parasoft-begin-suppress AUTOSAR-M2_10_1 "False positive: These declarations don't hide anything"
  // parasoft-begin-suppress AUTOSAR-A2_10_1 "False positive: These declarations don't hide anything"
  /// @brief The type of the underlying iterator
  using iterator_type = Iterator;
  /// @brief The difference type of the iterator
  using difference_type = typename iterator_traits<Iterator>::difference_type;
  /// @brief iterator pointer type
  using pointer = Iterator;
  /// @brief iterator value type
  using value_type = typename iterator_traits<Iterator>::value_type;
  /// @brief iterator category tag type
  using iterator_category = typename iterator_traits<Iterator>::iterator_category;
  /// @brief iterator reference type
  using reference = conditional_t<
      is_reference_v<typename iterator_traits<Iterator>::reference>,
      remove_reference_t<typename iterator_traits<Iterator>::reference>&&,
      typename iterator_traits<Iterator>::reference>;
  // parasoft-end-suppress AUTOSAR-A2_10_1
  // parasoft-end-suppress AUTOSAR-M2_10_1

  /// @brief Default construct the iterator, holding a default-constructed underlying iterator. Only available if @c
  /// Iterator is default-constructible.
  /// @tparam SfinaeIterator Dummy type to ensure underlying iterator is default-constructible
  template <
      typename SfinaeIterator = Iterator,
      arene::base::constraints<enable_if_t<is_default_constructible_v<SfinaeIterator>>> = nullptr>
  move_iterator() noexcept(is_nothrow_default_constructible_v<Iterator>)
      : current_{} {}

  /// @brief Construct an iterator holding the specified value of the underlying iterator.
  /// @param iter The iterator to store
  explicit move_iterator(Iterator iter) noexcept(is_nothrow_copy_constructible_v<Iterator>)
      : current_(iter) {}

  /// @brief Default copy constructor
  move_iterator(move_iterator const&) = default;
  /// @brief Default move constructor
  move_iterator(move_iterator&&) = default;
  /// @brief Default copy assignment operator
  auto operator=(move_iterator const&) -> move_iterator& = default;
  /// @brief Default move assignment operator
  auto operator=(move_iterator&&) -> move_iterator& = default;

  /// @brief Default destructor
  ~move_iterator() = default;

  /// @brief Conversion constructor from another move iterator with a different type of underlying iterator. The stored
  /// iterator is constructed from the underlying iterator of the source move iterator. Requires that the source
  /// underlying iterator is convertible to @c Iterator
  /// @tparam OtherIterator The type of the underlying iterator of the source iterator
  /// @param source The source iterator
  template <
      typename OtherIterator,
      arene::base::constraints<enable_if_t<is_convertible_v<OtherIterator, Iterator>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  move_iterator(move_iterator<OtherIterator> const& source)
      : current_(source.base()) {}

  /// @brief Conversion assignment from another move iterator with a different type of underlying iterator. The stored
  /// iterator is assigned from the underlying iterator of the source move iterator. Requires that the source
  /// underlying iterator can be assigned to @c Iterator
  /// @tparam OtherIterator The type of the underlying iterator of the source iterator
  /// @param source The source iterator
  /// @return A reference to @c *this
  template <
      typename OtherIterator,
      arene::base::constraints<enable_if_t<is_convertible_v<OtherIterator, Iterator>>> = nullptr>
  auto operator=(move_iterator<OtherIterator> const& source) -> move_iterator& {
    current_ = source.base();
    return *this;
  }

  /// @brief Retrieve the underlying iterator
  /// @return A copy of the stored underlying iterator
  auto base() const -> iterator_type { return current_; }

  // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C style cast is used"
  /// @brief Dereference the iterator
  /// @return An rvalue reference to the referred to value, if the underlying iterator returns a reference, or a copy of
  /// the referenced value otherwise
  auto operator*() const -> reference { return static_cast<reference>(*current_); }
  // parasoft-end-suppress AUTOSAR-A5_2_2-a

  /// @brief Arrow dereference operator for accessing the referenced value
  /// @return A copy of the stored underlying iterator
  auto operator->() const -> pointer { return current_; }

  /// @brief Preincrement operator. Increments the stored iterator
  /// @return A reference to @c *this
  auto operator++() -> move_iterator& {
    arene::base::advance(current_, 1);
    return *this;
  }
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  /// @brief Postincrement operator. Increments the stored iterator
  /// @return A copy of @c *this prior to the increment
  auto operator++(int) -> move_iterator {
    move_iterator temp{*this};
    arene::base::advance(current_, 1);
    return temp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  /// @brief Predecrement operator. Decrements the stored iterator
  /// @return A reference to @c *this
  /// @pre @c Iterator must be at least a bidirectional iterator
  auto operator--() -> move_iterator& {
    static_assert(
        arene::base::is_bidirectional_iterator_v<Iterator>,
        "The underlying iterator must be at least a bidirectional iterator to decrement"
    );
    arene::base::advance(current_, -1);
    return *this;
  }
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postdecrement requires an int parameter"
  /// @brief Postdecrement operator. Decrements the stored iterator
  /// @return A copy of @c *this prior to the decrement
  /// @pre @c Iterator must be at least a bidirectional iterator
  auto operator--(int) -> move_iterator {
    static_assert(
        arene::base::is_bidirectional_iterator_v<Iterator>,
        "The underlying iterator must be at least a bidirectional iterator to decrement"
    );
    move_iterator temp{*this};
    arene::base::advance(current_, -1);
    return temp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  // parasoft-begin-suppress "AUTOSAR-M5_17_1-a" "Overloaded operators have consistent semantics"

  /// @brief Obtain a new iterator referencing the value from moving the current iterator forwards by a specified amount
  /// @param delta The amount to move
  /// @return A new iterator referencing the new value
  /// @pre @c Iterator must be a random access iterator
  auto operator+(difference_type delta) const -> move_iterator {
    static_assert(
        arene::base::is_random_access_iterator_v<Iterator>,
        "The underlying iterator must be at least a random access iterator for random access operations"
    );
    move_iterator temp{*this};
    temp += delta;
    return temp;
  }

  /// @brief Move the current iterator forwards by a specified amount
  /// @param delta The amount to move
  /// @return A reference to @c *this
  /// @pre @c Iterator must be a random access iterator
  auto operator+=(difference_type delta) -> move_iterator& {
    static_assert(
        arene::base::is_random_access_iterator_v<Iterator>,
        "The underlying iterator must be at least a random access iterator for random access operations"
    );
    arene::base::advance(current_, delta);
    return *this;
  }

  /// @brief Obtain a new iterator referencing the value from moving the current iterator backwards by a specified
  /// amount
  /// @param delta The amount to move
  /// @return A new iterator referencing the new value
  /// @pre @c Iterator must be a random access iterator
  auto operator-(difference_type delta) const -> move_iterator {
    static_assert(
        arene::base::is_random_access_iterator_v<Iterator>,
        "The underlying iterator must be at least a random access iterator for random access operations"
    );
    move_iterator temp{*this};
    temp -= delta;
    return temp;
  }

  /// @brief Move the current iterator backwards by a specified amount
  /// @param delta The amount to move
  /// @return A reference to @c *this
  /// @pre @c Iterator must be a random access iterator
  auto operator-=(difference_type delta) -> move_iterator& {
    static_assert(
        arene::base::is_random_access_iterator_v<Iterator>,
        "The underlying iterator must be at least a random access iterator for random access operations"
    );
    arene::base::advance(current_, -delta);
    return *this;
  }

  // parasoft-end-suppress "AUTOSAR-M5_17_1-a"

  /// @brief Access the value at a specified offset from the current iterator
  /// @param delta The offset to the new position
  /// @return An rvalue reference to the specified value
  /// @pre @c Iterator must be a random access iterator
  auto operator[](difference_type delta) const -> reference {
    static_assert(
        arene::base::is_random_access_iterator_v<Iterator>,
        "The underlying iterator must be at least a random access iterator for random access operations"
    );
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "This is an iterator type, so indexing is OK"
    return static_cast<reference>(current_[delta]);
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
  }
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a

// parasoft-begin-suppress AUTOSAR-A13_5_5-b "This is implemented as specified by the C++ Standard"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Check if two iterators are equal
/// @tparam Iterator1 The type of the first iterator
/// @tparam Iterator2 The type of the second iterator
/// @param lhs The first iterator
/// @param rhs The second iterator
/// @return The value @c true if the iterators are equal, and @c false otherwise
template <typename Iterator1, typename Iterator2>
auto operator==(move_iterator<Iterator1> const& lhs, move_iterator<Iterator2> const& rhs) -> bool {
  return lhs.base() == rhs.base();
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Check if two iterators are not equal
/// @tparam Iterator1 The type of the first iterator
/// @tparam Iterator2 The type of the second iterator
/// @param lhs The first iterator
/// @param rhs The second iterator
/// @return The value @c true if the iterators are not equal, and @c false otherwise
template <typename Iterator1, typename Iterator2>
auto operator!=(move_iterator<Iterator1> const& lhs, move_iterator<Iterator2> const& rhs) -> bool {
  return !(lhs == rhs);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Check if one iterator is less than another
/// @tparam Iterator1 The type of the first iterator
/// @tparam Iterator2 The type of the second iterator
/// @param lhs The first iterator
/// @param rhs The second iterator
/// @return The value @c true if the first iterator is less than the second, and @c false otherwise
template <typename Iterator1, typename Iterator2>
auto operator<(move_iterator<Iterator1> const& lhs, move_iterator<Iterator2> const& rhs) -> bool {
  return lhs.base() < rhs.base();
}

/// @brief Check if one iterator is less than or equal to another
/// @tparam Iterator1 The type of the first iterator
/// @tparam Iterator2 The type of the second iterator
/// @param lhs The first iterator
/// @param rhs The second iterator
/// @return The value @c true if the first iterator is less than or equal to the second, and @c false otherwise
template <typename Iterator1, typename Iterator2>
auto operator<=(move_iterator<Iterator1> const& lhs, move_iterator<Iterator2> const& rhs) -> bool {
  return !(rhs < lhs);
}

/// @brief Check if one iterator is greater than another
/// @tparam Iterator1 The type of the first iterator
/// @tparam Iterator2 The type of the second iterator
/// @param lhs The first iterator
/// @param rhs The second iterator
/// @return The value @c true if the first iterator is greater than the second, and @c false otherwise
template <typename Iterator1, typename Iterator2>
auto operator>(move_iterator<Iterator1> const& lhs, move_iterator<Iterator2> const& rhs) -> bool {
  return rhs < lhs;
}

/// @brief Check if one iterator is greater than or equal to another
/// @tparam Iterator1 The type of the first iterator
/// @tparam Iterator2 The type of the second iterator
/// @param lhs The first iterator
/// @param rhs The second iterator
/// @return The value @c true if the first iterator is greater than or equal to the second, and @c false otherwise
template <typename Iterator1, typename Iterator2>
auto operator>=(move_iterator<Iterator1> const& lhs, move_iterator<Iterator2> const& rhs) -> bool {
  return !(lhs < rhs);
}

// parasoft-end-suppress AUTOSAR-A13_5_5-b

// parasoft-begin-suppress AUTOSAR-M5_0_15-a "This is an iterator type, so arithmetic is OK"
/// @brief Obtain the distance between two iterators
/// @tparam Iterator1 The type of the first iterator
/// @tparam Iterator2 The type of the second iterator
/// @param lhs The first iterator
/// @param rhs The second iterator
/// @return The difference between the iterators
template <typename Iterator1, typename Iterator2>
auto operator-(move_iterator<Iterator1> const& lhs, move_iterator<Iterator2> const& rhs)
    -> decltype(lhs.base() - rhs.base()) {
  return lhs.base() - rhs.base();
}
// parasoft-end-suppress AUTOSAR-M5_0_15-a

/// @brief Obtain a new iterator referencing the value from moving the specified iterator forwards by a specified amount
/// @param delta The amount to move
/// @param iter The iterator to start from
/// @return A new iterator referencing the new value
/// @pre @c Iterator must be a random access iterator
template <typename Iterator>
auto operator+(typename move_iterator<Iterator>::difference_type delta, move_iterator<Iterator> const& iter)
    -> move_iterator<Iterator> {
  return iter + delta;
}

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Create a @c move_iterator with the specified underlying iterator
/// @tparam Iterator The type of the underlying iterator
/// @param iter The value to use for the underlying iterator
/// @return The new @c move_iterator
template <typename Iterator>
auto make_move_iterator(Iterator iter) -> std::move_iterator<Iterator> {
  return std::move_iterator<Iterator>{iter};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_ITERATOR_HPP_
