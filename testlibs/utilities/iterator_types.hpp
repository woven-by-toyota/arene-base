// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_ITERATOR_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_ITERATOR_TYPES_HPP_

#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/utility/make_subrange.hpp"
#include "testlibs/utilities/throws_on.hpp"

namespace testing {

/// @brief demote an iterator to the given iterator category
///
/// A @c demoted_iterator wraps another iterator but only exposes the minimum operations necessary to satisfy the given
/// iterator category.
/// @tparam Iterator The base iterator type
/// @tparam Category The iterator category to emulate
template <class Iterator, class Category = typename std::iterator_traits<Iterator>::iterator_category>
class demoted_iterator {
  Iterator base_{};

  template <class C>
  using enable_if_bidirectional_t = std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, C>::value>;

  template <class C>
  using enable_if_random_access_t = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, C>::value>;

 public:
  using base_type = Iterator;

  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using iterator_category = Category;

  demoted_iterator() = default;
  constexpr explicit demoted_iterator(base_type iter)
      : base_{std::move(iter)} {}

  constexpr auto base() noexcept -> base_type& { return base_; }
  constexpr auto base() const noexcept -> base_type const& { return base_; }

  constexpr auto operator*() const noexcept(noexcept(*std::declval<base_type&>())) -> reference { return *base(); }
  constexpr auto operator->() const noexcept(std::is_nothrow_constructible<pointer>::value) -> pointer {
    return &*base();
  }
  constexpr auto operator++() noexcept(noexcept(++std::declval<base_type&>())) -> demoted_iterator& {
    ++base();
    return *this;
  }
  constexpr auto operator++(int
  ) noexcept(noexcept(++std::declval<base_type&>()) && std::is_nothrow_copy_assignable<base_type&>::value)
      -> demoted_iterator {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  friend constexpr auto operator==(demoted_iterator lhs, demoted_iterator rhs) noexcept(
      noexcept(std::declval<base_type&>() == std::declval<base_type&>())
  ) -> bool {
    return lhs.base() == rhs.base();
  }
  friend constexpr auto operator!=(demoted_iterator lhs, demoted_iterator rhs) noexcept(
      noexcept(std::declval<base_type&>() != std::declval<base_type&>())
  ) -> bool {
    return lhs.base() != rhs.base();
  }

  template <class C = Category, class = enable_if_bidirectional_t<C>>
  constexpr auto operator--() noexcept(noexcept(--std::declval<base_type&>())) -> demoted_iterator& {
    --base();
    return *this;
  }
  template <class C = Category, class = enable_if_bidirectional_t<C>>
  constexpr auto operator--(int
  ) noexcept(noexcept(--std::declval<base_type&>()) && std::is_nothrow_copy_assignable<base_type&>::value)
      -> demoted_iterator {
    auto tmp = *this;
    --*this;
    return tmp;
  }

  template <class C = Category, class = enable_if_random_access_t<C>>
  constexpr auto operator+=(difference_type n
  ) noexcept(noexcept(std::declval<base_type&>() += std::declval<difference_type&>())) -> demoted_iterator& {
    base() += n;
    return *this;
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator+(demoted_iterator itr, difference_type n) noexcept(
      noexcept(std::declval<base_type&>() += std::declval<difference_type&>())
  ) -> demoted_iterator {
    return itr += n;
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator+(difference_type n, demoted_iterator itr) noexcept(
      noexcept(std::declval<base_type&>() += std::declval<difference_type&>())
  ) -> demoted_iterator {
    return itr += n;
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  constexpr auto operator-=(difference_type n
  ) noexcept(noexcept(std::declval<base_type&>() -= std::declval<difference_type&>())) -> demoted_iterator& {
    base() -= n;
    return *this;
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator-(demoted_iterator itr, difference_type n) noexcept(
      noexcept(std::declval<base_type&>() -= std::declval<difference_type&>())
  ) -> demoted_iterator {
    return itr -= n;
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator-(demoted_iterator lhs, demoted_iterator rhs) noexcept(
      noexcept(std::declval<base_type&>() - std::declval<base_type&>())
  ) -> difference_type {
    return lhs.base() - rhs.base();
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  constexpr auto operator[](difference_type n) const noexcept(
      noexcept(std::declval<base_type&>() + std::declval<difference_type&>()) && noexcept(*std::declval<base_type&>())
  ) -> reference {
    return *(base() + n);
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator<(demoted_iterator lhs, demoted_iterator rhs) noexcept(
      noexcept(std::declval<base_type&>() < std::declval<base_type&>())
  ) -> bool {
    return lhs.base() < rhs.base();
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator>(demoted_iterator lhs, demoted_iterator rhs) noexcept(
      noexcept(std::declval<base_type&>() > std::declval<base_type&>())
  ) -> bool {
    return lhs.base() > rhs.base();
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator<=(demoted_iterator lhs, demoted_iterator rhs) noexcept(
      noexcept(std::declval<base_type&>() <= std::declval<base_type&>())
  ) -> bool {
    return lhs.base() <= rhs.base();
  }
  template <class C = Category, class = enable_if_random_access_t<C>>
  friend constexpr auto operator>=(demoted_iterator lhs, demoted_iterator rhs) noexcept(
      noexcept(std::declval<base_type&>() >= std::declval<base_type&>())
  ) -> bool {
    return lhs.base() >= rhs.base();
  }
};

/// @brief Demote an iterator to the given iterator category
/// @tparam Category The iterator category to emulate
/// @tparam Iterator The base iterator type
///
/// Create an iterator with operations restrited to the given iterator category.
///
template <class Category, class Iterator>
constexpr auto make_demoted_iterator(Iterator iter) {
  return demoted_iterator<Iterator, Category>{std::move(iter)};
}

/// @brief Demote a range to the given iterator category
///
/// Create a view of a range that only provides iterators of the given iterator category.
/// @tparam Category The iterator category to emulate
/// @tparam Iterator The type of iterator used to define the range
template <class Category, class Iterator>
constexpr auto demoted_view(Category, Iterator begin, Iterator end) {
  using iterator_type = demoted_iterator<Iterator, Category>;
  return arene::base::make_subrange(iterator_type{begin}, iterator_type{end});
}

/// @brief Demote a range to the given iterator category
///
/// Create a view of a range that only provides iterators of the given iterator category.
/// @tparam Category The iterator category to emulate
/// @tparam Range The type of the range
/// @{
template <
    class Category,
    class Range,
    std::enable_if_t<std::is_array<std::remove_reference_t<Range>>::value, std::nullptr_t> = nullptr>
constexpr auto demoted_view(Category category, Range&& range) {
  return demoted_view(category, std::begin(range), std::end(range));
}
template <
    class Category,
    class Range,
    std::enable_if_t<!std::is_array<std::remove_reference_t<Range>>::value, std::nullptr_t> = nullptr>
constexpr auto demoted_view(Category category, Range&& range) {
  return demoted_view(category, range.begin(), range.end());
}
/// @}

/// @brief an incomplete iterator type used for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
/// @tparam Category iterator category type
/// @tparam Value iterator value type
///
/// @note member functions for bidirectional iterator and stronger not yet implemented
///
template <throws_on Spec, class Category, class Value>
// NOLINTNEXTLINE(hicpp-special-member-functions)
struct noexcept_configurable_iterator {
  using value_type = Value;
  using reference = value_type&;
  using pointer = value_type*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = Category;

  noexcept_configurable_iterator() = default;
  noexcept_configurable_iterator(noexcept_configurable_iterator const&) noexcept(
      !contains<Spec>(throws_on::copy_construct)
  );

  auto operator=(noexcept_configurable_iterator const&) noexcept(!contains<Spec>(throws_on::copy_assign))
      -> noexcept_configurable_iterator&;

  auto operator*() const noexcept(!contains<Spec>(throws_on::dereference)) -> reference;
  auto operator->() const noexcept(!contains<Spec>(throws_on::arrow)) -> pointer;
  auto operator++() noexcept(!contains<Spec>(throws_on::pre_increment)) -> noexcept_configurable_iterator&;
  auto operator++(int) noexcept(!contains<Spec>(throws_on::post_increment)) -> noexcept_configurable_iterator;

  auto operator==(noexcept_configurable_iterator const&) const noexcept(!contains<Spec>(throws_on::equal)) -> bool;
  auto operator!=(noexcept_configurable_iterator const&) const noexcept(!contains<Spec>(throws_on::not_equal)) -> bool;

  // bidirectional operators
  // @{
  auto operator--() noexcept(!contains<Spec>(throws_on::pre_decrement)) -> noexcept_configurable_iterator&;
  auto operator--(int) noexcept(!contains<Spec>(throws_on::post_decrement)) -> noexcept_configurable_iterator;

  // random access operators
  // @{
  auto operator+=(difference_type) noexcept(!contains<Spec>(throws_on::plus_equals_difference))
      -> noexcept_configurable_iterator&;
  auto operator+(difference_type) const noexcept(!contains<Spec>(throws_on::plus_difference))
      -> noexcept_configurable_iterator;
  friend auto operator+(difference_type, noexcept_configurable_iterator const&) noexcept(
      !contains<Spec>(throws_on::difference_plus)
  ) -> noexcept_configurable_iterator {
    return {};
  }
  auto operator-=(difference_type) noexcept(!contains<Spec>(throws_on::minus_equals_difference))
      -> noexcept_configurable_iterator&;
  auto operator-(difference_type) const noexcept(!contains<Spec>(throws_on::minus_difference))
      -> noexcept_configurable_iterator;
  friend auto operator-(noexcept_configurable_iterator const&, noexcept_configurable_iterator const&) noexcept(
      !contains<Spec>(throws_on::minus_self)
  ) -> difference_type {
    return {};
  }
  auto operator[](difference_type) const noexcept(!contains<Spec>(throws_on::index)) -> reference;

  auto operator<(noexcept_configurable_iterator const&) const noexcept(!contains<Spec>(throws_on::less)) -> bool;
  auto operator>(noexcept_configurable_iterator const&) const noexcept(!contains<Spec>(throws_on::greater)) -> bool;
  auto operator<=(noexcept_configurable_iterator const&) const noexcept(!contains<Spec>(throws_on::less_equal)) -> bool;
  auto operator>=(noexcept_configurable_iterator const&) const noexcept(!contains<Spec>(throws_on::greater_equal))
      -> bool;
  // @}
};

/// @brief an incomplete forward iterator type used for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
/// @tparam Value iterator value type
///
template <throws_on Spec, class Value = int>
using noexcept_configurable_input_iterator = noexcept_configurable_iterator<Spec, std::input_iterator_tag, Value>;

/// @brief an incomplete forward iterator type used for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
/// @tparam Value iterator value type
///
template <throws_on Spec, class Value = int>
using noexcept_configurable_forward_iterator = noexcept_configurable_iterator<Spec, std::forward_iterator_tag, Value>;

/// @brief an incomplete random_access iterator type used for noexcept testing
/// @tparam Spec determines the operations that are *not* @c noexcept
/// @tparam Value iterator value type
///
template <throws_on Spec, class Value = int>
using noexcept_configurable_random_access_iterator =
    noexcept_configurable_iterator<Spec, std::random_access_iterator_tag, Value>;

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_ITERATOR_TYPES_HPP_
