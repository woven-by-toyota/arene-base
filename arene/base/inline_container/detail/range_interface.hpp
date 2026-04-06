// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_RANGE_INTERFACE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_RANGE_INTERFACE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

namespace arene {
namespace base {
namespace inline_container {
namespace detail {

/// @brief helper class to define common range operations
/// @tparam Derived derived type providing range basis functions
///
/// CRTP helper class to define functions necessary for defining a range. Similar to @c std::ranges::view_interface.
///
/// The derived type is required to provide the following typedefs:
/// * @c iterator
/// * @c const_iterator
///
/// The derived type is required to implement the following basis functions:
/// * <tt> auto begin() const -> const_iterator_t </tt>
/// * <tt> auto end() const -> const_iterator_t </tt>
///
/// The derived type can optionally implement mutable variants:
/// * <tt> auto begin() -> iterator_t </tt>
/// * <tt> auto end() -> iterator_t </tt>
///
/// This interface will unconditionally provide:
/// * <tt> auto cbegin() const -> const_iterator_t </tt>
/// * <tt> auto cend() const -> const_iterator_t </tt>
/// * <tt> auto empty() const -> bool </tt>
///
/// If the iterator type satisfies the requirements for @c std::bidirectional_iterator:
/// * <tt> auto rbegin() const -> arene::base::reverse_iterator<const_iterator_t> </tt>
/// * <tt> auto rend() const -> arene::base::reverse_iterator<const_iterator_t> </tt>
/// * <tt> auto crbegin() const -> arene::base::reverse_iterator<const_iterator_t> </tt>
/// * <tt> auto crend() const -> arene::base::reverse_iterator<const_iterator_t> </tt>
/// * <tt> auto front() const -> const_reference_t </tt>
/// * <tt> auto back() const -> const_reference_t </tt>
///
/// If the iterator type satisfies the requirements for @c std::bidirectional_iterator and provides mutable iterators:
/// * <tt> auto rbegin() -> arene::base::reverse_iterator<iterator_t> </tt>
/// * <tt> auto rend() -> arene::base::reverse_iterator<iterator_t> </tt>
/// * <tt> auto front() -> reference_t </tt>
/// * <tt> auto back() -> reference_t </tt>
///
/// If the iterator type satisfies the requirements for @c std::random_access_iterator:
/// * <tt> auto operator[](difference_type) const -> const_reference_t </tt>
///
/// If the iterator type satisfies the requirements for @c std::random_access_iterator and provides mutable iterators:
/// * <tt> auto operator[](difference_type) -> reference_t </tt>
///
/// Note: This class can be extended to provide additional common member functions, such as <c>size</c>, etc.
template <class Derived>
class range_interface {
  // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
  /// @brief obtain a reference to the derived type
  /// @return a reference to the derived type
  constexpr auto derived() noexcept -> Derived& { return static_cast<Derived&>(*this); }

  /// @brief obtain a const reference to the derived type
  /// @return a const reference to the derived type
  constexpr auto derived() const noexcept -> Derived const& { return static_cast<Derived const&>(*this); }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

  /// @brief helper to obtain the type of the iterator provided by the derived type
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using iterator_t = decltype(std::declval<D2>().begin());

  /// @brief helper to obtain the type of the const iterator provided by the derived type
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using const_iterator_t = typename D2::const_iterator;

  /// @brief helper to obtain the type of the reference provided by the derived type
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using reference_t = decltype(*std::declval<iterator_t<D2>>());

  /// @brief helper to obtain the type of the const reference provided by the derived type
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using const_reference_t = decltype(*std::declval<const_iterator_t<D2>>());

  /// @brief helper to obtain the type of the reverse iterator to provide
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using reverse_iterator_t = ::arene::base::reverse_iterator<iterator_t<D2>>;

  /// @brief helper to obtain the type of the const reverse iterator to provide
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using const_reverse_iterator_t = ::arene::base::reverse_iterator<const_iterator_t<D2>>;

  /// @brief helper to obtain the iterator category
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using category_t = typename std::iterator_traits<iterator_t<D2>>::iterator_category;

  /// @brief helper to obtain the iterator difference type
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2>
  using difference_t = typename std::iterator_traits<iterator_t<D2>>::difference_type;

  /// @brief helper to enable a function for ranges with bidirectional iterators
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  /// @tparam R return value
  template <class D2>
  using enable_if_bidirectional_t =
      std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, category_t<D2>>::value>;

  /// @brief helper to enable a function for ranges with random access iterators
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  /// @tparam R return value
  template <class D2>
  using enable_if_random_access_t =
      std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, category_t<D2>>::value>;

  /// @brief helper to check if the derived provides mutable iterators
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  ///
  /// If the derived type is a const-view type, it may only provide const iterators.
  template <class D2>
  using enable_if_provides_mutable_iterator =
      std::enable_if_t<!std::is_same<iterator_t<D2>, const_iterator_t<D2>>::value>;

 protected:
  /// @brief Construct a 'range_interface' CRTP helper type.
  ///
  /// Intended to be invoked as:
  /// ~~~{.cpp}
  /// range_interface{this}
  /// ~~~
  ///
  /// This constructor exists to prevent use by a type that does _not_ define
  /// 'range_interface' as a CRTP base class or to detect mismatch between the actual derived type and the @c Derived
  /// template parameter.
  ///
  template <class Actual, class Expected = Derived>
  explicit constexpr range_interface(Actual*) noexcept {
    static_assert(
        std::is_same<Actual, Expected>::value,
        "'Derived' type mismatch.\n"  //
        "Please verify that 'Derived' is defined correctly when using 'range_interface' as a CRTP base."
    );
  }

 public:
  /// @brief check whether the range is empty
  /// @return @c true if the range is empty, otherwise @c false
  constexpr auto empty() const
      noexcept(noexcept(std::declval<Derived const&>().begin() == std::declval<Derived const&>().end())) -> bool {
    return derived().begin() == derived().end();
  }

  /// @brief return a const iterator to the beginning
  /// @return a const iterator to the beginning
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2 = Derived>
  constexpr auto cbegin() const noexcept(noexcept(std::declval<Derived const&>().begin())) -> const_iterator_t<D2> {
    return derived().begin();
  }

  /// @brief return a const iterator to the end
  /// @return a const iterator to the end
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2 = Derived>
  constexpr auto cend() const noexcept(noexcept(std::declval<Derived const&>().end())) -> const_iterator_t<D2> {
    return derived().end();
  }

  /// @brief return a reverse iterator to the beginning
  /// @return a reverse iterator to the beginning
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <
      class D2 = Derived,
      class = constraints<enable_if_bidirectional_t<D2>, enable_if_provides_mutable_iterator<D2>>>
  constexpr auto rbegin() noexcept(noexcept(reverse_iterator_t<D2&>(std::declval<Derived&>().end())))
      -> reverse_iterator_t<D2&> {
    return reverse_iterator_t<D2&>(derived().end());
  }

  /// @brief return a const reverse iterator to the beginning
  /// @return a const reverse iterator to the beginning
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2 = Derived, class = constraints<enable_if_bidirectional_t<D2>>>
  constexpr auto rbegin() const noexcept(noexcept(reverse_iterator_t<D2 const&>(std::declval<Derived const&>().end())))
      -> reverse_iterator_t<D2 const&> {
    return reverse_iterator_t<D2 const&>(derived().end());
  }

  /// @brief return a reverse iterator to the end
  /// @return a reverse iterator to the end
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <
      class D2 = Derived,
      class = constraints<enable_if_bidirectional_t<D2>, enable_if_provides_mutable_iterator<D2>>>
  constexpr auto rend() noexcept(noexcept(reverse_iterator_t<D2&>(std::declval<Derived&>().begin())))
      -> reverse_iterator_t<D2&> {
    return reverse_iterator_t<D2&>(derived().begin());
  }

  /// @brief return a const reverse iterator to the end
  /// @return a const reverse iterator to the end
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2 = Derived, class = constraints<enable_if_bidirectional_t<D2>>>
  constexpr auto rend() const noexcept(noexcept(reverse_iterator_t<D2 const&>(std::declval<Derived const&>().begin())))
      -> reverse_iterator_t<D2 const&> {
    return reverse_iterator_t<D2 const&>(derived().begin());
  }

  /// @brief return a const reverse iterator to the beginning
  /// @return a const reverse iterator to the beginning
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2 = Derived, class = constraints<enable_if_bidirectional_t<D2>>>
  constexpr auto crbegin() const noexcept(noexcept(const_reverse_iterator_t<D2>(std::declval<Derived const&>().end())))
      -> const_reverse_iterator_t<D2> {
    return const_reverse_iterator_t<D2>(derived().end());
  }

  /// @brief return a const reverse iterator to the end
  /// @return a const reverse iterator to the end
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  template <class D2 = Derived, class = constraints<enable_if_bidirectional_t<D2>>>
  constexpr auto crend() const noexcept(noexcept(const_reverse_iterator_t<D2>(std::declval<Derived const&>().begin())))
      -> const_reverse_iterator_t<D2> {
    return const_reverse_iterator_t<D2>(derived().begin());
  }

  /// @brief return a reference to the first element of the range, the one referred to by @c begin
  /// @return a reference to the element
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  /// @pre <c>!empty()</c>, otherwise ARENE_PRECONDITION violation
  template <
      class D2 = Derived,
      class = constraints<enable_if_bidirectional_t<D2>, enable_if_provides_mutable_iterator<D2>>>
  constexpr auto front() noexcept(noexcept(*std::declval<Derived&>().begin())) -> reference_t<D2> {
    ARENE_PRECONDITION(!empty());
    return *derived().begin();
  }

  /// @brief return a reference to the first element of the range, the one referred to by @c begin
  /// @return a reference to the element
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  /// @pre <c>!empty()</c>, otherwise ARENE_PRECONDITION violation
  template <class D2 = Derived, class = constraints<enable_if_bidirectional_t<D2>>>
  constexpr auto front() const noexcept(noexcept(*std::declval<Derived const&>().begin())) -> const_reference_t<D2> {
    ARENE_PRECONDITION(!empty());
    return *derived().begin();
  }

  /// @brief return a reference to the last element of the range, the one referred to by @c rbegin
  /// @return a reference to the element
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  /// @pre <c>!empty()</c>, otherwise ARENE_PRECONDITION violation
  template <
      class D2 = Derived,
      class = constraints<enable_if_bidirectional_t<D2>, enable_if_provides_mutable_iterator<D2>>>
  constexpr auto back() noexcept(noexcept(*std::declval<Derived&>().begin())) -> reference_t<D2> {
    ARENE_PRECONDITION(!empty());
    return *derived().rbegin();
  }

  /// @brief return a reference to the last element of the range, the one referred to by @c rbegin
  /// @return a reference to the element
  /// @tparam D2 the derived type providing the basis functions, intended to always be Derived
  /// @pre <c>!empty()</c>, otherwise ARENE_PRECONDITION violation
  template <class D2 = Derived, class = constraints<enable_if_bidirectional_t<D2>>>
  constexpr auto back() const noexcept(noexcept(*std::declval<Derived const&>().begin())) -> const_reference_t<D2> {
    ARENE_PRECONDITION(!empty());
    return *derived().rbegin();
  }

  /// @brief return a @c const_reference to the element at a particular position within a random-access range
  /// @tparam D2 the derived type providing the basis functions, intended to always be @c Derived
  /// @param diff the offset from the beginning of the range
  /// @return a @c const_reference to the element returned by <c>*(begin() + diff)</c>
  /// @pre see the preconditions of @c const_iterator type's <c>operator[]</c>; most likely <c>begin() + diff</c> must
  /// be dereferenceable or else the behaviour is undefined, but different iterator types may have different behaviour.
  template <class D2 = Derived, class = constraints<enable_if_random_access_t<D2>>>
  constexpr auto operator[](difference_t<D2> diff) const noexcept(noexcept(std::declval<Derived&>().cbegin()[diff]))
      -> const_reference_t<D2> {
    return derived().begin()[diff];
  }

  /// @brief return a @c reference to the element at a particular position within a random-access range
  /// @tparam D2 the derived type providing the basis functions, intended to always be @c Derived
  /// @param diff the offset from the beginning of the range
  /// @return a @c reference to the element returned by <c>*(begin() + diff)</c>
  /// @pre see the preconditions of @c iterator type's <c>operator[]</c>; most likely <c>begin() + diff</c> must be
  /// dereferenceable or else the behaviour is undefined, but different iterator types may have different behaviour.
  template <
      class D2 = Derived,
      class = constraints<enable_if_random_access_t<D2>, enable_if_provides_mutable_iterator<D2>>>
  constexpr auto operator[](difference_t<D2> diff) noexcept(noexcept(std::declval<Derived&>().begin()[diff]))
      -> reference_t<D2> {
    return derived().begin()[diff];
  }
};

}  // namespace detail
}  // namespace inline_container
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_RANGE_INTERFACE_HPP_
