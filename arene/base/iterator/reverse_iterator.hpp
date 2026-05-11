// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file reverse_iterator.hpp
/// @brief Provides a backport of the constexpr implementation of @c std::reverse_iterator .
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_REVERSE_ITERATOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_REVERSE_ITERATOR_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/prev.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/arithmetic_traits.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include "arene/base/iterator/detail/std_reverse_iterator_enabled.hpp"
#else
#include "arene/base/iterator/detail/std_reverse_iterator_disabled.hpp"
#endif

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

namespace reverse_iterator_detail {

// parasoft-begin-suppress AUTOSAR-A5_0_3-a "This abstraction is generic and
// does not restrict the value type of the base iterator"

/// @brief Base class for @c reverse_iterator providing operations that do not depend on pointer-ness of underlying
///        iterator.
template <typename Itr>
class reverse_iterator_base {
 public:
  static_assert(is_bidirectional_iterator_v<Itr>, "Must be at least a bidirectional iterator to be reversed.");

  /// @brief Equivalent to @c value_type for the underlying iterator.
  using value_type = typename std::iterator_traits<Itr>::value_type;
  /// @brief Equivalent to @c reference for the underlying iterator.
  using reference = typename std::iterator_traits<Itr>::reference;
  /// @brief Equivalent to @c pointer for the underlying iterator.
  using pointer = typename std::iterator_traits<Itr>::pointer;
  /// @brief Equivalent to @c difference_type for the underlying iterator.
  using difference_type = typename std::iterator_traits<Itr>::difference_type;
  /// @brief Equivalent to @c iterator_category for the underlying iterator.
  using iterator_category = typename std::iterator_traits<Itr>::iterator_category;
  /// @brief The type of the wrapped iterator
  using iterator_type = Itr;

 protected:
  // This weird declaration order is needed due to a bug in noexcept evaluation in GCC<10, where it incorrectly requires
  // named referenced in noexcept(noexcept()) specifiers to have been declared before being referenced if they do not
  // depend on a template parameter.

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "Protected data members permitted by M11-0-1 Permit #1 v1.0.0"
  /// @brief The underlying forward iterator
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes) detemplitized base class with no invariants.
  Itr itr_;
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: Uses this"
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "False positive: does return a value"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "False positive: does return a value"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "False positive: does return a value"
  /// @brief Gets the iterator needed for dereferencing in a reversed view based on forward iterators.
  ///
  /// @return Itr Iterator to the position _before_ the current position.
  ARENE_NODISCARD constexpr auto itr_for_access() const noexcept(noexcept(::arene::base::prev(itr_))) -> iterator_type {
    return ::arene::base::prev(itr_);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  /// @brief Default ctor
  ///
  /// @post The iterator is constructed as if via @c Itr{}
  constexpr reverse_iterator_base() noexcept = default;

  /// @brief Copy ctor
  /// @param other The iterator being copied
  constexpr reverse_iterator_base(reverse_iterator_base const& other) noexcept = default;

  /// @brief Move ctor
  /// @param other The iterator being moved
  constexpr reverse_iterator_base(reverse_iterator_base&& other) noexcept = default;

  /// @brief Copy assignment
  /// @param other The iterator being copied
  /// @return *this
  constexpr auto operator=(reverse_iterator_base const& other) noexcept -> reverse_iterator_base& = default;

  /// @brief Move assignment
  /// @param other The iterator being moved
  /// @return *this
  constexpr auto operator=(reverse_iterator_base&& other) noexcept -> reverse_iterator_base& = default;

  /// @brief default destructor
  ~reverse_iterator_base() = default;

  /// @brief Construct from a non-reversed iterator
  ///
  /// @param iter The iterator to construct the reversed iterator from
  /// @post The iterator points to the location of @c iter in a reversed view.
  constexpr explicit reverse_iterator_base(Itr iter) noexcept(std::is_nothrow_copy_constructible<Itr>::value)
      : itr_(iter) {}

 public:
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "False positive: does return a value"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "False positive: does return a value"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "False positive: does return a value"
  /// @brief Dereferences the iterator
  ///
  /// @return reference A reference to the element at the position of the iterator
  ARENE_NODISCARD constexpr auto operator*() const noexcept(noexcept(
      // weird noexcept style is needed due to a bug in GCC8 for evaluation of noexcept in constexpr.
      *std::declval<reverse_iterator_base const&>().itr_for_access()
  )) -> reference {
    return *itr_for_access();
  }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2

  /// @brief Unwraps the iterator
  ///
  /// @return Itr A copy of the underlying iterator.
  ARENE_NODISCARD constexpr auto base() const noexcept(std::is_nothrow_copy_constructible<Itr>::value)
      -> iterator_type {
    return itr_;
  }
};

// parasoft-end-suppress AUTOSAR-A5_0_3-a

}  // namespace reverse_iterator_detail

// parasoft-begin-suppress AUTOSAR-A5_0_3-a "This abstraction is generic and
// does not restrict the value type of the base iterator"

// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: the ordering base class is empty"
// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: only const overload provided"
/// @brief A backport of the @c constexpr @c std::reverse_iterator from C++17
///
/// @tparam Itr The type of the wrapped_iterator to wrap. Must satisfy @c is_bidirectional_iterator_v .
///
/// Specialization for when the iterator is a non-pointer type.
template <typename Itr, bool = std::is_pointer<Itr>::value>
class reverse_iterator final
    : public reverse_iterator_detail::reverse_iterator_base<Itr>
    , generic_ordering_from_three_way_compare<reverse_iterator<Itr>> {
  /// @brief Internal type alias for the implementation base class
  using reviter_base = reverse_iterator_detail::reverse_iterator_base<Itr>;

  /// @brief Internal type alias for the implementation base class
  using ordering_base = generic_ordering_from_three_way_compare<reverse_iterator<Itr>>;

 public:
  // alias in typedefs
  using typename reviter_base::difference_type;
  using typename reviter_base::iterator_category;
  using typename reviter_base::pointer;
  using typename reviter_base::reference;
  using typename reviter_base::value_type;

  /// @brief Default construct
  constexpr reverse_iterator() = default;

  /// @brief Copy ctor
  /// @param other The iterator being copied
  constexpr reverse_iterator(reverse_iterator const& other) noexcept = default;

  /// @brief Move ctor
  /// @param other The iterator being moved
  constexpr reverse_iterator(reverse_iterator&& other) noexcept = default;

  /// @brief Copy assignment
  /// @param other The iterator being copied
  /// @return *this
  constexpr auto operator=(reverse_iterator const& other) noexcept -> reverse_iterator& = default;

  /// @brief Move assignment
  /// @param other The iterator being moved
  /// @return *this
  constexpr auto operator=(reverse_iterator&& other) noexcept -> reverse_iterator& = default;

  /// @brief default destructor
  ~reverse_iterator() = default;

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a-2 "False positive: 'iter' is used"
  /// @brief Construct from a non-reversed iterator
  ///
  /// @param iter The iterator to construct the reversed iterator from
  /// @post The iterator points to the location of @c iter in a reversed view.
  constexpr explicit reverse_iterator(Itr iter) noexcept(std::is_nothrow_copy_constructible<Itr>::value)
      : reviter_base(iter),
        ordering_base{} {}
  // parasoft-end-suppress AUTOSAR-A0_1_4-a-2

  /// @brief Conversion-construct from a reversed iterator of compatible type
  ///
  /// @tparam U The type of the iterator to convert from. Must satisfy <c>std::is_convertible<const U&, Itr></c>.
  /// @param iter The iterator to construct the reversed iterator from
  /// @post The iterator points to the location of @c iter in a reversed view.
  template <
      typename U,
      constraints<
          std::enable_if_t<!std::is_same<U, Itr>::value>,
          std::enable_if_t<std::is_convertible<U const&, Itr>::value>> = nullptr>
  constexpr explicit reverse_iterator(reverse_iterator<U> const& iter
  ) noexcept(std::is_nothrow_constructible<Itr, U const&>::value&& noexcept(iter.base()))
      : reviter_base(iter.base()),
        ordering_base{} {}

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a-2 "False positive: 'iter' is used"
  /// @brief Conversion-construct from a std::reverse_iterator
  ///
  /// @param iter The iterator to construct the reversed iterator from
  /// @post The iterator points to the location of @c iter in a reversed view.
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Need implicit conversion for drop-in with std::reverse_iterator
  constexpr reverse_iterator(reverse_iterator_detail::std_reverse_iterator<Itr> const& iter
  ) noexcept(std::is_nothrow_constructible<reviter_base, Itr>::value&& noexcept(iter.base()))
      : reviter_base(iter.base()),
        ordering_base{} {}
  // parasoft-end-suppress AUTOSAR-A0_1_4-a-2

  // parasoft-begin-suppress AUTOSAR-A13_2_3-a "False positive: Not a relational operator"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: A value is always returned"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: A value is always returned"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: A value is always returned"
  // parasoft-begin-suppress AUTOSAR-A13_5_2-a "Implicit conversion is needed for interoperability between types"
  /// @brief Conversion operator to a std::reverse_iterator
  ///
  /// @return std::reverse_itr<Itr> An instance of @c std::reverse_iterator<Itr> as if constructed from @c base() .
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Need implicit conversion for drop-in with std::reverse_iterator
  constexpr operator reverse_iterator_detail::std_reverse_iterator<Itr>() const
      noexcept(std::is_nothrow_constructible<reverse_iterator_detail::std_reverse_iterator<Itr>, Itr>::value&& noexcept(
          std::declval<reverse_iterator>().base()
      )) {
    return reverse_iterator_detail::std_reverse_iterator<Itr>{this->base()};
  }
  // parasoft-end-suppress AUTOSAR-A13_5_2-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress CERT_CPP-MSC52-a
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
  // parasoft-end-suppress AUTOSAR-A13_5_2-a

  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "False positive: does return a value"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "False positive: does return a value"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "False positive: does return a value"
  /// @brief Dereferences the iterator
  ///
  /// @return pointer A pointer to the element at the position of the iterator
  ARENE_NODISCARD constexpr auto operator->() const
      noexcept(noexcept(std::declval<reverse_iterator const&>().itr_for_access().operator->())) -> pointer {
    return this->itr_for_access().operator->();
  }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_4-a-2 "False positive: 'delta' is used"
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "False positive: does return a value"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "False positive: does return a value"
  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "False positive: does return a value"
  /// @brief Index-and-dereference the iterator
  /// @param delta The offest of the element to access
  ///
  /// @pre @c *this+delta is within the underlying range
  /// @return reference A reference to the element at the position equivalent to @c *this+delta .
  ARENE_NODISCARD constexpr auto operator[](difference_type const delta) const noexcept -> reference {
    return *(*this + delta);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_4-a-2

  /// @brief Increments the iterator
  ///
  /// @return reverse_iterator& The iterator post-modification
  /// @post The iterator is advanced by one position
  constexpr auto operator++() noexcept(noexcept(--std::declval<Itr&>())) -> reverse_iterator& {
    --(this->itr_);
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  /// @brief Increments the iterator
  ///
  /// @return reverse_iterator A copy of the iterator pre-modification
  /// @post The iterator is advanced by one position
  constexpr auto operator++(int
  ) noexcept(std::is_nothrow_copy_constructible<reverse_iterator>::value&& noexcept(++std::declval<reverse_iterator&>())
  ) -> reverse_iterator {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  /// @brief Decremented the iterator
  ///
  /// @return reverse_iterator& The iterator post-modification
  /// @post The iterator is decremented by one position
  constexpr auto operator--() noexcept(noexcept(++std::declval<Itr&>())) -> reverse_iterator& {
    ++(this->itr_);
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  /// @brief Decrements the iterator
  ///
  /// @return reverse_iterator A copy of the iterator pre-modification
  /// @post The iterator is decremented by one position
  constexpr auto operator--(int
  ) noexcept(std::is_nothrow_copy_constructible<reverse_iterator>::value&& noexcept(--std::declval<reverse_iterator&>())
  ) -> reverse_iterator {
    auto tmp = *this;
    --(*this);
    return tmp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Add an offset to the iterator
  ///
  /// @param iter The iterator to add the offset to.
  /// @param delta The offset to add.
  /// @return reverse_iterator The iterator equivalent to advancing @c iter by @c delta positions.
  ARENE_NODISCARD friend constexpr auto operator+(
      reverse_iterator const& iter,
      difference_type const delta
  ) noexcept(std::is_nothrow_copy_constructible<Itr>::value && is_nothrow_subtractable_v<Itr const&, difference_type const>)
      -> reverse_iterator {
    return reverse_iterator{iter.itr_ - delta};
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Add an offset to the iterator
  ///
  /// @param iter The iterator to add the offset to.
  /// @param delta The offset to add.
  /// @return reverse_iterator The iterator equivalent to advancing @c iter by @c delta positions.
  ARENE_NODISCARD friend constexpr auto operator+(
      difference_type const delta,
      reverse_iterator const& iter
  ) noexcept(std::is_nothrow_copy_constructible<Itr>::value && is_nothrow_subtractable_v<Itr const&, difference_type const>)
      -> reverse_iterator {
    return iter + delta;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Subtract an offset from the iterator
  ///
  /// @param iter The iterator to subtract the offset from.
  /// @param delta The offset to subtract.
  /// @return reverse_iterator The iterator equivalent to decrementing @c iter by @c delta positions.
  ARENE_NODISCARD friend constexpr auto operator-(
      reverse_iterator const& iter,
      difference_type const delta
  ) noexcept(std::is_nothrow_copy_constructible<Itr>::value && is_nothrow_addable_v<Itr const&, difference_type const>)
      -> reverse_iterator {
    return reverse_iterator{iter.itr_ + delta};
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Compute the difference between two iterators
  ///
  /// @param lhs The left-hand iterator operand.
  /// @param rhs The right-hand iterator operand.
  /// @return difference_type The number of positions between the two iterators.
  ARENE_NODISCARD friend constexpr auto operator-(
      reverse_iterator const lhs,
      reverse_iterator const rhs
  ) noexcept(is_nothrow_subtractable_v<Itr const&>) -> difference_type {
    return rhs.itr_ - lhs.itr_;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  /// @brief Increments the iterator by an offset
  ///
  /// @param delta The offset to add.
  /// @return reverse_iterator The iterator post-modification
  /// @post The iterator is incremented by @c delta positions.
  constexpr auto operator+=(difference_type const delta) noexcept(noexcept(std::declval<Itr&>() -= delta))
      -> reverse_iterator& {
    this->itr_ -= delta;
    return *this;
  }

  /// @brief Increments the iterator by an offset
  ///
  /// @param delta The offset to subtract.
  /// @return reverse_iterator The iterator post-modification
  /// @post The iterator is decremented by @c delta positions.
  constexpr auto operator-=(difference_type const delta) noexcept(noexcept(std::declval<Itr&>() += delta))
      -> reverse_iterator& {
    this->itr_ += delta;
    return *this;
  }

  /// @brief Compare two random access iterators for relative ordering
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c compare_three_way_supported_v<U,Itr>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  ///
  /// @return strong_ordering::less If @c lhs is an iterator to a position before @c rhs in the sequence.
  /// @return strong_ordering::equal If @c lhs is an iterator to a position equivalent to @c rhs in the sequence.
  /// @return strong_ordering::greater If @c lhs is an iterator to a position after @c rhs in the sequence.
  template <typename U, constraints<std::enable_if_t<compare_three_way_supported_v<U, Itr>>> = nullptr>
  ARENE_NODISCARD static constexpr auto
  three_way_compare(reverse_iterator const& lhs, reverse_iterator<U> const& rhs) noexcept -> strong_ordering {
    return compare_three_way{}(rhs.itr_, lhs.itr_);
  }

  /// @brief Compare two random access iterators for relative ordering
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c compare_three_way_supported_v<U,Itr>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  ///
  /// @return strong_ordering::less If @c lhs is an iterator to a position before @c rhs in the sequence.
  /// @return strong_ordering::equal If @c lhs is an iterator to a position equivalent to @c rhs in the sequence.
  /// @return strong_ordering::greater If @c lhs is an iterator to a position after @c rhs in the sequence.
  template <typename U, constraints<std::enable_if_t<compare_three_way_supported_v<U, Itr>>> = nullptr>
  ARENE_NODISCARD static constexpr auto
  three_way_compare(reverse_iterator const& lhs, reverse_iterator_detail::std_reverse_iterator<U> const& rhs) noexcept
      -> strong_ordering {
    return compare_three_way{}(rhs.base(), lhs.itr_);
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Compare two bidirectional iterators for equality
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c is_equality_comparable<Itr,U>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  /// @return true if the iterators point to the same position.
  /// @return false if the iterators point to different positions.
  template <
      typename U,
      constraints<
          std::enable_if_t<!compare_three_way_supported_v<Itr, U>>,
          std::enable_if_t<is_equality_comparable_v<Itr, U>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(reverse_iterator const& lhs, reverse_iterator<U> const& rhs) noexcept
      -> bool {
    return lhs.itr_ == rhs.itr_;
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  /// @brief Compare two bidirectional iterators for equality
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c is_equality_comparable<Itr,U>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  /// @return true if the iterators point to the same position.
  /// @return false if the iterators point to different positions.
  template <
      typename U,
      constraints<
          std::enable_if_t<!compare_three_way_supported_v<Itr, U>>,
          std::enable_if_t<is_equality_comparable_v<Itr, U>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator==(
      reverse_iterator const& lhs,
      reverse_iterator_detail::std_reverse_iterator<U> const& rhs
  ) noexcept(noexcept(std::declval<Itr>() == std::declval<U>()) && noexcept(rhs.base())) -> bool {
    return lhs.itr_ == rhs.base();
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Compare two bidirectional iterators for inequality
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c is_equality_comparable<Itr,U>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  /// @return true if the iterators point to the different positions.
  /// @return false if the iterators point to the same position.
  template <
      typename U,
      constraints<
          std::enable_if_t<!compare_three_way_supported_v<Itr, U>>,
          std::enable_if_t<is_inequality_comparable_v<Itr, U>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto operator!=(reverse_iterator const& lhs, reverse_iterator<U> const& rhs) noexcept
      -> bool {
    return lhs.itr_ != rhs.itr_;
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Compare two bidirectional iterators for inequality
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c is_equality_comparable<Itr,U>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  /// @return true if the iterators point to the different positions.
  /// @return false if the iterators point to the same position.
  template <
      typename U,
      constraints<
          std::enable_if_t<!compare_three_way_supported_v<Itr, U>>,
          std::enable_if_t<is_inequality_comparable_v<Itr, U>>> = nullptr>
  ARENE_NODISCARD friend constexpr auto
  operator!=(reverse_iterator const& lhs, reverse_iterator_detail::std_reverse_iterator<U> const& rhs) noexcept
      -> bool {
    return lhs.itr_ != rhs.base();
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2

// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: the ordering base class is empty"
// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: only const overload provided"
/// @brief A backport of the @c constexpr @c std::reverse_iterator from C++17
///
/// @tparam Itr The type of the wrapped_iterator to wrap. Must satisfy @c is_bidirectional_iterator_v .
///
/// Specialization for when the iterator is a pointer type.
template <typename Itr>
class reverse_iterator<Itr, true> final
    : public reverse_iterator_detail::reverse_iterator_base<Itr>
    , generic_ordering_from_three_way_compare<reverse_iterator<Itr>> {
  /// @brief Internal type alias for the implementation base class
  using reviter_base = reverse_iterator_detail::reverse_iterator_base<Itr>;
  /// @brief Internal type alias for the implementation base class
  using ordering_base = generic_ordering_from_three_way_compare<reverse_iterator<Itr>>;

 public:
  // alias in typedefs
  using typename reviter_base::difference_type;
  using typename reviter_base::iterator_category;
  using typename reviter_base::pointer;
  using typename reviter_base::reference;
  using typename reviter_base::value_type;

  /// @brief Default construct
  constexpr reverse_iterator() = default;

  /// @brief Copy ctor
  /// @param other The iterator being copied
  constexpr reverse_iterator(reverse_iterator const& other) noexcept = default;

  /// @brief Move ctor
  /// @param other The iterator being moved
  constexpr reverse_iterator(reverse_iterator&& other) noexcept = default;

  /// @brief Copy assignment
  /// @param other The iterator being copied
  /// @return *this
  constexpr auto operator=(reverse_iterator const& other) noexcept -> reverse_iterator& = default;

  /// @brief Move assignment
  /// @param other The iterator being moved
  /// @return *this
  constexpr auto operator=(reverse_iterator&& other) noexcept -> reverse_iterator& = default;

  /// @brief default destructor
  ~reverse_iterator() = default;

  /// @brief Construct from a non-reversed iterator
  ///
  /// @param iter The iterator to construct the reversed iterator from
  /// @post The iterator points to the location of @c iter in a reversed view.
  constexpr explicit reverse_iterator(Itr iter) noexcept(std::is_nothrow_copy_constructible<Itr>::value)
      : reviter_base(iter),
        ordering_base{} {}

  /// @brief Conversion-construct from a reversed iterator of compatible type
  ///
  /// @tparam U The type of the iterator to convert from. Must satisfy <c>std::is_convertible<const U&, Itr></c>.
  /// @param iter The iterator to construct the reversed iterator from
  /// @post The iterator points to the location of @c iter in a reversed view.
  template <
      typename U,
      constraints<
          std::enable_if_t<!std::is_same<U, Itr>::value>,
          std::enable_if_t<std::is_convertible<U const&, Itr>::value>> = nullptr>
  constexpr explicit reverse_iterator(reverse_iterator<U> const& iter
  ) noexcept(std::is_nothrow_constructible<Itr, U const&>::value&& noexcept(iter.base()))
      : reviter_base(iter.base()),
        ordering_base{} {}

  /// @brief Conversion-construct from a std::reverse_iterator
  ///
  /// @param iter The iterator to construct the reversed iterator from
  /// @post The iterator points to the location of @c iter in a reversed view.
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Need implicit conversion for drop-in with std::reverse_iterator
  constexpr reverse_iterator(reverse_iterator_detail::std_reverse_iterator<Itr> const& iter
  ) noexcept(std::is_nothrow_constructible<reviter_base, Itr>::value&& noexcept(iter.base()))
      : reviter_base(iter.base()),
        ordering_base{} {}

  // parasoft-begin-suppress AUTOSAR-A13_2_3-a "False positive: Not a relational operator"
  // parasoft-begin-suppress AUTOSAR-A13_5_2-a "Implicit conversion is needed for interoperability between types"
  /// @brief Conversion operator to a std::reverse_iterator
  ///
  /// @return std::reverse_itr<Itr> An instance of @c std::reverse_iterator<Itr> as if constructed from @c base() .
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Need implicit conversion for drop-in with std::reverse_iterator
  constexpr operator reverse_iterator_detail::std_reverse_iterator<Itr>() const
      noexcept(std::is_nothrow_constructible<reverse_iterator_detail::std_reverse_iterator<Itr>, Itr>::value&& noexcept(
          std::declval<reverse_iterator>().base()
      )) {
    return reverse_iterator_detail::std_reverse_iterator<Itr>{this->base()};
  }
  // parasoft-end-suppress AUTOSAR-A13_2_3-a
  // parasoft-end-suppress AUTOSAR-A13_5_2-a

  /// @brief Dereferences the iterator
  ///
  /// @return pointer A pointer to the element at the position of the iterator
  ARENE_NODISCARD constexpr auto operator->() const noexcept -> pointer { return this->itr_for_access(); }

  /// @brief Index-and-dereference the iterator
  ///
  /// @param delta The index at which to dereference the iterator
  ///
  /// @return reference A reference to the element at the position equivalent to @c *this+delta .

  ARENE_NODISCARD constexpr auto operator[](difference_type const delta) const noexcept -> reference {
    return *(*this + delta);
  }

  /// @brief Increments the iterator
  ///
  /// @return reverse_iterator& The iterator post-modification
  /// @post The iterator is advanced by one position
  constexpr auto operator++() noexcept -> reverse_iterator& {
    *this += 1;
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  /// @brief Increments the iterator
  ///
  /// @return reverse_iterator A copy of the iterator pre-modification
  /// @post The iterator is advanced by one position
  constexpr auto operator++(int) noexcept -> reverse_iterator {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  /// @brief Decremented the iterator
  ///
  /// @return reverse_iterator& The iterator post-modification
  /// @post The iterator is decremented by one position
  constexpr auto operator--() noexcept -> reverse_iterator& {
    *this -= 1;
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  /// @brief Decrements the iterator
  ///
  /// @return reverse_iterator A copy of the iterator pre-modification
  /// @post The iterator is decremented by one position
  constexpr auto operator--(int) noexcept -> reverse_iterator {
    auto tmp = *this;
    --(*this);
    return tmp;
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-A7_1_1-a-2 "False positive: iter is modified"
  /// @brief Add an offset to the iterator
  ///
  /// @param iter The iterator to add the offset to.
  /// @param delta The offset to add.
  /// @return reverse_iterator The iterator equivalent to advancing @c iter by @c delta positions.
  ARENE_NODISCARD friend constexpr auto operator+(reverse_iterator iter, difference_type const delta) noexcept
      -> reverse_iterator {
    iter += delta;
    return iter;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A7_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Add an offset to the iterator
  ///
  /// @param iter The iterator to add the offset to.
  /// @param delta The offset to add.
  /// @return reverse_iterator The iterator equivalent to advancing @c iter by @c delta positions.
  ARENE_NODISCARD friend constexpr auto operator+(difference_type const delta, reverse_iterator const& iter) noexcept
      -> reverse_iterator {
    return iter + delta;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A7_1_1-a-2 "False positive: iter is modified"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Subtract an offset from the iterator
  ///
  /// @param iter The iterator to subtract the offset from.
  /// @param delta The offset to subtract.
  /// @return reverse_iterator The iterator equivalent to decrementing @c iter by @c delta positions.
  ARENE_NODISCARD friend constexpr auto operator-(reverse_iterator iter, difference_type const delta) noexcept
      -> reverse_iterator {
    iter -= delta;
    return iter;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A7_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: Function is namespace scope and used in other
  // translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Compute the difference between two iterators
  ///
  /// @param lhs The left-hand iterator operand.
  /// @param rhs The right-hand iterator operand.
  /// @return difference_type The number of positions between the two iterators.
  ARENE_NODISCARD friend constexpr auto operator-(reverse_iterator const lhs, reverse_iterator const rhs) noexcept
      -> difference_type {
    return ::arene::base::distance(lhs.itr_, rhs.itr_);
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  /// @brief Increments the iterator by an offset
  ///
  /// @param delta The offset to add.
  /// @return reverse_iterator The iterator post-modification
  /// @post The iterator is incremented by @c delta positions.
  constexpr auto operator+=(difference_type const delta) noexcept -> reverse_iterator& {
    this->itr_ = ::arene::base::prev(this->itr_, delta);
    return *this;
  }

  /// @brief Increments the iterator by an offset
  ///
  /// @param delta The offset to subtract.
  /// @return reverse_iterator The iterator post-modification
  /// @post The iterator is decremented by @c delta positions.
  constexpr auto operator-=(difference_type const delta) noexcept -> reverse_iterator& {
    this->itr_ = ::arene::base::next(this->itr_, delta);
    return *this;
  }

  /// @brief Compare two random access iterators for relative ordering
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c compare_three_way_supported_v<U,Itr>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  ///
  /// @return strong_ordering::less If @c lhs is an iterator to a position before @c rhs in the sequence.
  /// @return strong_ordering::equal If @c lhs is an iterator to a position equivalent to @c rhs in the sequence.
  /// @return strong_ordering::greater If @c lhs is an iterator to a position after @c rhs in the sequence.
  template <typename U, constraints<std::enable_if_t<compare_three_way_supported_v<U, Itr>>> = nullptr>
  ARENE_NODISCARD static constexpr auto
  three_way_compare(reverse_iterator const& lhs, reverse_iterator<U> const& rhs) noexcept -> strong_ordering {
    return compare_three_way{}(rhs.itr_, lhs.itr_);
  }

  /// @brief Compare two random access iterators for relative ordering
  ///
  /// @tparam U The type of the iterator to compare against. Must satisfy @c compare_three_way_supported_v<U,Itr>
  /// @param lhs The first iterator.
  /// @param rhs The second iterator.
  ///
  /// @return strong_ordering::less If @c lhs is an iterator to a position before @c rhs in the sequence.
  /// @return strong_ordering::equal If @c lhs is an iterator to a position equivalent to @c rhs in the sequence.
  /// @return strong_ordering::greater If @c lhs is an iterator to a position after @c rhs in the sequence.
  template <typename U, constraints<std::enable_if_t<compare_three_way_supported_v<U, Itr>>> = nullptr>
  ARENE_NODISCARD static constexpr auto
  three_way_compare(reverse_iterator const& lhs, reverse_iterator_detail::std_reverse_iterator<U> const& rhs) noexcept
      -> strong_ordering {
    return compare_three_way{}(rhs.base(), lhs.itr_);
  }
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2

// parasoft-end-suppress AUTOSAR-A5_0_3-a

/// @brief Factory function for creating a @c reverse_iterator with deduced template arguments pre C++17.
///
/// @tparam Itr The type of the wrapped_iterator to wrap. Must satisfy @c is_bidirectional_iterator_v .
/// @param iter The iterator to construct from.
/// @return reverse_iterator<Itr> Equivalent to @c reverse_iterator<Itr>{std::forward<Itr>(iter)} .
template <typename Itr, constraints<std::enable_if_t<is_bidirectional_iterator_v<Itr>>> = nullptr>
constexpr auto make_reverse_iterator(Itr&& iter
) noexcept(noexcept(std::is_nothrow_constructible<reverse_iterator<Itr>, Itr&&>::value)) -> reverse_iterator<Itr> {
  return reverse_iterator<Itr>{std::forward<Itr>(iter)};
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_REVERSE_ITERATOR_HPP_
