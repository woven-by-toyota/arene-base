// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file wrapped_iterator.hpp
/// @brief Provides a simple wrapped_iterator wrapper around another iterator-like thing.
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_WRAPPED_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_WRAPPED_ITERATOR_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace detail {

// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: only const overload is provided"

/// @brief A simple wrapper around an "iterator-like" thing which requires a passkey to construct.
///
/// Generally used to wrap pointers as iterators. It also injects @c ARENE_PRECONDITION checks on operations that the
/// iterator is not default constructed.
/// @tparam Itr The type of the wrapped_iterator to wrap. Must be compatible with @c std::iterator_traits
/// @tparam Passkey A passkey type to require for construction
template <typename Itr, typename Passkey>
class wrapped_iterator : public generic_ordering_from_three_way_compare<wrapped_iterator<Itr, Passkey>> {
  /// @brief The type of the base class for defining operators
  using operator_base = generic_ordering_from_three_way_compare<wrapped_iterator<Itr, Passkey>>;
  static_assert(is_iterator_v<Itr>, "Can only wrap iterators.");

  template <typename OtherIterator, typename OtherPasskey>
  static constexpr bool is_compatible_iterator{
      std::is_convertible<OtherIterator const&, Itr>::value && std::is_convertible<OtherPasskey const&, Passkey>::value
  };

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: these type aliases do not hide anything"
  /// @brief The value type of the element pointed to be the iterator.
  using value_type = typename std::iterator_traits<Itr>::value_type;
  /// @brief The type of a reference to an element pointed to be the iterator.
  using reference = typename std::iterator_traits<Itr>::reference;
  /// @brief The type of a pointer to an element pointed to be the iterator.
  using pointer = typename std::iterator_traits<Itr>::pointer;
  /// @brief The type of a difference between two pointers
  using difference_type = typename std::iterator_traits<Itr>::difference_type;
  /// @brief The iterator_category of the iterator.
  using iterator_category = typename std::iterator_traits<Itr>::iterator_category;
  /// @brief The type of the underlying iterator.
  using iterator_type = Itr;
  /// @brief The type of the passkey needed to construct the iterator.
  using passkey_type = Passkey;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Default ctor
  /// @post The iterator is constructed as if via @c iterator_type{}
  constexpr wrapped_iterator() noexcept = default;

  // parasoft-begin-suppress AUTOSAR-A5_0_3-a "This abstraction is generic does not restrict the value type of a
  // pointer"
  /// @brief Initialize the iterator from a given iterator.
  ///
  /// @param iter The iterator to construct from
  /// @post @c base()==iter .
  constexpr wrapped_iterator(passkey_type, iterator_type iter) noexcept
      : operator_base(),
        itr_(std::move(iter)) {}
  // parasoft-end-suppress AUTOSAR-A5_0_3-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Converting constructor from a compatible iterator
  ///
  /// @tparam OtherIterator The type of the other underlying iterator. Must satisfy <c>std::is_convertible_to<const
  /// OtherIterator&, Itr></c>.
  /// @tparam OtherPasskey The type of the other passkey. Must satisfy <c>std::is_convertible_to<const OtherPasskey&,
  /// Passkey></c>.
  /// @param iter The iterator to construct from
  /// @post The iterator is constructed as if via copying @c iter .
  template <
      typename OtherIterator,
      typename OtherPasskey,
      constraints<
          std::enable_if_t<!std::is_same<OtherIterator, Itr>::value || !std::is_same<OtherPasskey, Passkey>::value>,
          std::enable_if_t<is_compatible_iterator<OtherIterator, OtherPasskey>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Need to allow implicit conversion between compatible iterators.
  constexpr wrapped_iterator(wrapped_iterator<OtherIterator, OtherPasskey> const& iter) noexcept
      : wrapped_iterator(passkey_type{}, iter.base()) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief Dereferences the iterator
  ///
  /// @return Equivalent to calling @c operator*() on the underlying iterator .
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  constexpr auto operator*() const noexcept -> reference {
    ARENE_PRECONDITION(itr_ != iterator_type{});
    return *itr_;
  }

  /// @brief Dereferences the iterator
  ///
  /// @tparam ThisItr SFINAE dummy param.
  /// @return Equivalent to calling @c operator->() on the underlying iterator
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  template <typename ThisItr = Itr, constraints<std::enable_if_t<!std::is_pointer<ThisItr>::value>> = nullptr>
  constexpr auto operator->() const noexcept -> pointer {
    ARENE_PRECONDITION(itr_ != iterator_type{});
    return itr_.operator->();
  }
  /// @brief Dereferences the iterator
  ///
  /// @tparam ThisItr SFINAE dummy param.
  /// @return The underlying pointer
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  template <typename ThisItr = Itr, constraints<std::enable_if_t<std::is_pointer<ThisItr>::value>> = nullptr>
  constexpr auto operator->() const noexcept -> pointer {
    ARENE_PRECONDITION(itr_ != iterator_type{});
    return itr_;
  }

  /// @brief Index-and-dereference the iterator
  ///
  /// @param delta The index from the current iterator to dereference
  /// @return The result of dereferencing @c *this+delta
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.

  constexpr auto operator[](difference_type const delta) const noexcept -> reference { return *(*this + delta); }

  /// @brief Increments the iterator
  ///
  /// @return wrapped_iterator& @c *this post increment.
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  constexpr auto operator++() noexcept -> wrapped_iterator& { return (*this += 1); }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  /// @brief Increments the iterator
  ///
  /// @return wrapped_iterator A copy of @c *this pre increment
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  constexpr auto operator++(int) noexcept -> wrapped_iterator {
    auto tmp = *this;
    *this += 1;
    return tmp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  /// @brief Decrements the iterator
  ///
  /// @return wrapped_iterator& @c *this post decrement.
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  constexpr auto operator--() noexcept -> wrapped_iterator& { return (*this -= 1); }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postdecrement requires an int parameter"
  /// @brief Decrements the iterator
  ///
  /// @return wrapped_iterator A copy of @c *this pre decrement.
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  constexpr auto operator--(int) noexcept -> wrapped_iterator {
    auto tmp = *this;
    *this -= 1;
    return tmp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A7_1_1-a-2 "False positive: iter is modified"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Add an offset to the iterator
  ///
  /// @param iter The iterator to increment.
  /// @param delta The number of positions to increment the iterator by.
  /// @return wrapped_iterator An iterator which represents @c lhs incremented by @c delta positions.
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  friend constexpr auto operator+(wrapped_iterator iter, difference_type const delta) noexcept -> wrapped_iterator {
    iter += delta;
    return iter;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A7_1_1-a-2 "False positive: iter is modified"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Add an offset to the iterator
  ///
  /// @param iter The iterator to increment.
  /// @param delta The number of positions to increment the iterator by.
  /// @return wrapped_iterator An iterator which represents @c lhs incremented by @c delta positions.
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  friend constexpr auto operator+(difference_type const delta, wrapped_iterator iter) noexcept -> wrapped_iterator {
    return iter + delta;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A7_1_1-a-2 "False positive: iter is modified"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Subtract an offset from the iterator
  ///
  /// @param lhs The iterator to decrement.
  /// @param delta The number of positions to decrement the iterator by.
  /// @return wrapped_iterator An iterator which represents @c lhs decremented by @c delta positions.
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  friend constexpr auto operator-(wrapped_iterator lhs, difference_type const delta) noexcept -> wrapped_iterator {
    lhs -= delta;
    return lhs;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Compute the difference in positions between two iterators
  ///
  /// @tparam OtherIterator The type of the other iterator. Must satisfy
  ///         @c std::is_convertible<OtherIterator,iterator_type>
  /// @param lhs The left hand iterator operand.
  /// @param rhs The right hand iterator operand.
  /// @return difference_type Equivalent to @c distance<Itr>(rhs,lhs) .
  /// @pre @c lhs and @c rhs are either both non-default-constructed iterators, or both default-constructed iterators,
  ///      else @c ARENE_PRECONDITION violation.
  /// @pre @c lhs and @c rhs both point to the same, valid sequence in memory else behavior is undefined.
  template <
      typename OtherIterator,
      typename OtherPasskey,
      constraints<std::enable_if_t<is_compatible_iterator<OtherIterator, OtherPasskey>>> = nullptr>
  friend constexpr auto
  operator-(wrapped_iterator const& lhs, wrapped_iterator<OtherIterator, OtherPasskey> const& rhs) noexcept
      -> difference_type {
    ARENE_PRECONDITION(
        (lhs.base() != iterator_type{}) ==
        (rhs.base() != typename wrapped_iterator<OtherIterator, OtherPasskey>::iterator_type{})
    );
    return ::arene::base::distance<iterator_type>(rhs.base(), lhs.base());
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  /// @brief Increments the iterator by an offset
  ///
  /// @param delta The number of positions to increment the iterator by.
  /// @return wrapped_iterator& @c *this post-increment .
  /// @pre <tt> (delta == 0) || (*this != iterator_type{}) </tt> else @c ARENE_PRECONDITION violation.
  constexpr auto operator+=(difference_type const delta) noexcept -> wrapped_iterator& {
    ARENE_PRECONDITION((delta == difference_type{}) || (itr_ != iterator_type{}));
    arene::base::advance(itr_, delta);
    return *this;
  }

  /// @brief Decrements the iterator by an offset
  ///
  /// @param delta The number of positions to decrement the iterator by.
  /// @return wrapped_iterator& @c *this post-decrement .
  /// @pre @c *this!=iterator_type{} else @c ARENE_PRECONDITION violation.
  constexpr auto operator-=(difference_type const delta) noexcept -> wrapped_iterator& {
    ARENE_PRECONDITION(itr_ != iterator_type{});
    ARENE_PRECONDITION(delta != std::numeric_limits<difference_type>::min());
    arene::base::advance(itr_, -delta);
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'three_way_compare' does not hide anything"

  /// @brief Compare two iterators for relative ordering
  ///
  /// This is a total ordering across all iterators of the same type
  ///
  /// @tparam OtherIterator The type of the other iterator to compare against. Must satisfy
  ///         @c compare_three_way_supported_v<Itr,OtherItr>
  /// @tparam OtherPasskey The type of the other passkey to compare against. Must satisfy
  ///         @c std::is_convertible<OtherPasskey,passkey_type>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  ///
  /// @return strong_ordering::less If @c lhs references a position before @c rhs .
  /// @return strong_ordering::equal If @c lhs references a position equivalent to @c rhs .
  /// @return strong_ordering::greater If @c lhs references a position after @c rhs .
  template <
      typename OtherIterator,
      typename OtherPasskey,
      constraints<
          std::enable_if_t<compare_three_way_supported_v<Itr, OtherIterator>>,
          std::enable_if_t<is_compatible_iterator<OtherIterator, OtherPasskey>>> = nullptr>
  ARENE_NODISCARD static constexpr auto
  three_way_compare(wrapped_iterator const& lhs, wrapped_iterator<OtherIterator, OtherPasskey> const& rhs) noexcept
      -> strong_ordering {
    return compare_three_way{}(lhs.base(), rhs.base());
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A5_0_3-a "This abstraction is generic does not restrict the value type of a
  // pointer"
  /// @brief Unwraps the iterator
  ///
  /// @return iterator_type A copy of the underlying iterator.
  ARENE_NODISCARD constexpr auto base() const noexcept -> iterator_type { return itr_; }
  // parasoft-end-suppress AUTOSAR-A5_0_3-a

 private:
  // parasoft-begin-suppress AUTOSAR-A5_0_3-a "This abstraction is generic does not restrict the value type of a
  // pointer"
  /// @brief The wrapped_iterator to the current element
  iterator_type itr_{};
  // parasoft-end-suppress AUTOSAR-A5_0_3-a
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2

}  // namespace detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress CERT_C-EXP37-a-3

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_WRAPPED_ITERATOR_HPP_
