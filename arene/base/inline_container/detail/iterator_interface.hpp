// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file iterator_interface.hpp
/// @brief Helper type for defining iterators.
///
/// @note This is a temporary location. This needs to be moved to
/// //arene/base:iterator package to allow public use and it needs thorough
/// tests for that as well.
///
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_ITERATOR_INTERFACE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_ITERATOR_INTERFACE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace inline_container_detail {

// parasoft-begin-suppress AUTOSAR-A11_3_1-a "Passkey idiom permitted by A11-3-1 Permit #1"

/// @brief tag used to define basis functions for @c iterator_interface
///
/// This tag is used in defining basis functions that do not overlap with iterator
/// member functions. It allows fewer basis functions to be defined and avoids
/// the need for using-declarations in derived types to bring base class members
/// into scope (e.g. @c Derived::operator++() hides
/// @c iterator_interface::operator++(int)) without a using-declaration.
///
class iterator_interface_tag {
  // only allow iterator_interface to invoke basis functions
  template <class Derived>
  friend class iterator_interface;

  /// @brief default constructor
  ///
  explicit iterator_interface_tag() = default;
};

// parasoft-end-suppress AUTOSAR-A11_3_1-a

/// @brief determine if the derived type has a @c step_forward function
/// @tparam Derived the derived iterator type using this interface
///
template <typename Derived, typename = constraints<>>
constexpr bool has_step_forward_v{false};

/// @brief determine if the derived type has a @c step_forward function
/// @tparam Derived the derived iterator type using this interface
///
template <typename Derived>
constexpr bool has_step_forward_v<
    Derived,
    constraints<decltype(step_forward(std::declval<iterator_interface_tag>(), std::declval<Derived&>()))>>{true};

/// @brief determine if the derived type has a @c step_backward function
/// @tparam Derived the derived iterator type using this interface
///
template <typename Derived, typename = constraints<>>
constexpr bool has_step_backward_v{false};

/// @brief determine if the derived type has a @c step_backward function
/// @tparam Derived the derived iterator type using this interface
///
template <typename Derived>
constexpr bool has_step_backward_v<
    Derived,
    constraints<decltype(step_backward(std::declval<iterator_interface_tag>(), std::declval<Derived&>()))>>{true};

/// @brief determine if the derived type has an @c operator+= function
/// @tparam Derived the derived iterator type using this interface
///
template <typename Derived, typename = constraints<>>
constexpr bool has_plus_equals_v{false};

/// @brief determine if the derived type has an @c operator+= function
/// @tparam Derived the derived iterator type using this interface
///
template <typename Derived>
constexpr bool has_plus_equals_v<
    Derived,
    constraints<decltype(std::declval<Derived&>() += std::declval<typename Derived::difference_type const&>())>>{true};

/// @brief determine if += basis function exists and is @c noexcept
/// @tparam Derived the derived iterator type using this interface
///
template <class Derived, typename = constraints<>>
constexpr bool is_plus_equals_noexcept_v{false};

/// @brief determine if += basis function exists and is @c noexcept
/// @tparam Derived the derived iterator type using this interface
///
template <class Derived>
constexpr bool is_plus_equals_noexcept_v<Derived, constraints<std::enable_if_t<has_plus_equals_v<Derived>>>>{
    noexcept(std::declval<Derived&>() += std::declval<typename Derived::difference_type>())
};

// parasoft-begin-suppress AUTOSAR-A13_5_1-a "Non-const operator[] not needed because it would be identical"

// parasoft-begin-suppress AUTOSAR-A2_7_2-a-2 "False positive: no commented-out code, this is documentation"
/// @brief helper class to define an iterator
/// @tparam Derived derived type providing iterator basis functions
///
/// CRTP helper class to define functions necessary for an iterator.
///
/// The basis functions required from the derived type depend on the iterator
/// category.
///
/// InputIterator: not implemented
///
/// OutputIterator: not implemented
///
/// ForwardIterator: not implemented
///
/// BidirectionalIterator:
/// * <tt> auto operator*() -> reference </tt>
/// * <tt> friend auto operator==(Derived const&, Derived const&) -> bool </tt>
/// * <tt> friend auto step_forward(inline_container_detail::iterator_interface_tag, Derived&) -> void </tt>
/// * <tt> friend auto step_backward(inline_container_detail::iterator_interface_tag, Derived&) -> void </tt>
///
/// RandomAccessIterator:
/// * <tt> auto operator*() -> reference </tt>
/// * <tt> auto operator+=(difference_type) -> Derived& </tt>
/// * <tt> friend auto operator-(Derived const&, Derived const&) -> difference_type </tt>
/// Optionally, these will be used if present, or synthesized from <tt> operator+= </tt> if not:
/// * <tt> friend auto step_forward(inline_container_detail::iterator_interface_tag, Derived&) -> void </tt>
/// * <tt> friend auto step_backward(inline_container_detail::iterator_interface_tag, Derived&) -> void </tt>
/// Optionally, these will be used if present, or synthesized from <tt> operator- </tt> if not:
/// * <tt> friend auto operator==(Derived const&, Derived const&) -> bool </tt>
/// * <tt> friend auto operator<(Derived const&, Derived const&) -> bool </tt>
/// If the above are provided then the other comparisons will be synthesized from them, not from <tt> operator- </tt>.
///
/// The derived class must also provide all iterator member types.
///
// parasoft-end-suppress AUTOSAR-A2_7_2-a-2
template <class Derived>
class iterator_interface {
  /// @brief helper to obtain the iterator category
  /// @tparam T iterator type
  ///
  template <class T>
  using category_t = typename T::iterator_category;

  /// @brief helper to enable a function for bidirectional-or-better iterators
  /// @tparam Self template parameter intended to always be @c Derived
  /// @tparam R return value
  ///
  template <class Self, class R = void>
  using enable_if_at_least_bidirectional_t =
      std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, category_t<Self>>::value, R>;

  /// @brief helper to enable a function for random-access-or-better iterators
  /// @tparam Self template parameter intended to always be @c Derived
  /// @tparam R return value
  ///
  template <class Self, class R = void>
  using enable_if_at_least_random_access_t =
      std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, category_t<Self>>::value, R>;

  /// @brief obtain a reference to the derived type
  /// @return reference to the derived type
  ///
  constexpr auto derived() noexcept -> Derived& { return static_cast<Derived&>(*this); }

  /// @brief obtain a reference to the derived type
  /// @return reference to the derived type
  ///
  constexpr auto derived() const noexcept -> Derived const& { return static_cast<Derived const&>(*this); }

  /// @brief determine if forward basis function is @c noexcept
  /// @tparam Self template parameter intended to always be @c Derived
  ///
  template <class Self>
  static constexpr bool is_step_forward_noexcept_v{
      noexcept(step_forward(iterator_interface_tag{}, std::declval<Self&>()))
  };

  /// @brief determine if backward basis function is @c noexcept
  /// @tparam Self template parameter intended to always be @c Derived
  ///
  template <class Self>
  static constexpr bool is_step_backward_noexcept_v{
      noexcept(step_backward(iterator_interface_tag{}, std::declval<Self&>()))
  };

 protected:
  /// @brief default destructor
  ///
  ~iterator_interface() = default;

  /// @brief default constructor
  ///
  iterator_interface() = default;

  /// @brief default copy constructor
  ///
  iterator_interface(iterator_interface const&) = default;

  /// @brief default copy assignment operator
  ///
  auto operator=(iterator_interface const&) -> iterator_interface& = default;

  /// @brief default move constructor
  ///
  iterator_interface(iterator_interface&&) = default;

  /// @brief default move assignment operator
  ///
  auto operator=(iterator_interface&&) -> iterator_interface& = default;

 public:
  /// @brief dereference
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @pre @c *this is dereferenceable
  /// @return pointer to the indirect value
  ///
  template <class Self = Derived, constraints<enable_if_at_least_bidirectional_t<Self>> = nullptr>
  constexpr auto operator->() const noexcept(noexcept(*std::declval<Self&>())) -> typename Self::pointer {
    return &*derived();
  }

  /// @brief preincrement
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @pre @c *this is dereferenceable
  /// @return @c *this after incrementing
  ///
  template <
      class Self = Derived,
      constraints<enable_if_at_least_bidirectional_t<Self>, std::enable_if_t<has_step_forward_v<Self>>> = nullptr>
  constexpr auto operator++() noexcept(is_step_forward_noexcept_v<Self>) -> Self& {
    step_forward(iterator_interface_tag{}, derived());
    return derived();
  }

  /// @brief preincrement
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @pre @c *this is dereferenceable
  /// @return @c *this after incrementing
  ///
  template <
      class Self = Derived,
      constraints<enable_if_at_least_bidirectional_t<Self>, std::enable_if_t<!has_step_forward_v<Self>>> = nullptr>
  constexpr auto operator++() noexcept(is_plus_equals_noexcept_v<Self>) -> Self& {
    static_assert(
        has_plus_equals_v<Self>,
        "Iterator types using iterator_interface must define step_forward, operator+=, or both"
    );
    return derived() += typename Self::difference_type{1};
  }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "False positive: postincrement requires an int parameter"

  /// @brief postincrement
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @pre @c *this is dereferenceable
  /// @return copy of @c *this before incrementing
  ///
  template <class Self = Derived, constraints<enable_if_at_least_bidirectional_t<Self>> = nullptr>
  constexpr auto operator++(int
  ) noexcept(noexcept(++std::declval<Self&>()) && std::is_nothrow_copy_constructible<Self>::value) -> Self {
    auto tmp = derived();
    ++derived();
    return tmp;
  }

  // parasoft-end-suppress AUTOSAR-A3_9_1-b

  /// @brief negative shift
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @param difference signed distance by which to shift this iterator
  /// @pre <tt> difference != std::numeric_limits<difference_type>::lowest() </tt>, otherwise precondition violation
  /// @pre Applying @c --itr @c diff times (if <c>diff > 0</c>) or applying @c ++itr @c -diff times (if <c>diff < 0</c>)
  /// results in a valid iterator, otherwise the behaviour is undefined
  /// @return reference to *this after it's been shifted by the given amount
  ///
  template <class Self = Derived, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  constexpr auto operator-=(typename Self::difference_type const& difference) noexcept(is_plus_equals_noexcept_v<Self>)
      -> Self& {
    // difference_type is required to be a signed integer type by 24.2.1:1, and these are assumed to be 2's complement.
    // The expression "-difference" is well-formed as long as "difference" is not the lowest possible value.
    using diff_type = typename Self::difference_type;
    ARENE_PRECONDITION(difference != std::numeric_limits<diff_type>::lowest());
    return derived() += -difference;
  }

  /// @brief predecrement
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @pre @c *this is decrementable
  ///   (there exists an iterator @c i such that <tt> *this == ++i </tt>)
  /// @post @c *this is dereferenceable
  /// @return @c *this after decrementing
  ///
  template <
      class Self = Derived,
      constraints<enable_if_at_least_bidirectional_t<Self>, std::enable_if_t<has_step_backward_v<Self>>> = nullptr>
  constexpr auto operator--() noexcept(is_step_backward_noexcept_v<Self>) -> Self& {
    step_backward(iterator_interface_tag{}, derived());
    return derived();
  }

  /// @brief predecrement
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @pre @c *this is decrementable
  ///   (there exists an iterator @c i such that <tt> *this == ++i </tt>)
  /// @post @c *this is dereferenceable
  /// @return @c *this after decrementing
  ///
  template <
      class Self = Derived,
      constraints<enable_if_at_least_bidirectional_t<Self>, std::enable_if_t<!has_step_backward_v<Self>>> = nullptr>
  constexpr auto operator--() noexcept(is_plus_equals_noexcept_v<Self>) -> Self& {
    static_assert(
        has_plus_equals_v<Self>,
        "Iterator types using iterator_interface must define step_backward, operator+=, or both"
    );
    return derived() += typename Self::difference_type{-1};
  }

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "False positive: postdecrement requires an int parameter"

  /// @brief postdecrement
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @pre @c *this is decrementable
  ///   (there exists an iterator @c i such that <tt> *this == ++i </tt>)
  /// @post for returned iterator @c r
  ///   * @c r is dereferencable
  ///   * <tt> --(++r) == r </tt>
  /// @return copy of @c *this before decrementing
  ///
  template <class Self = Derived, constraints<enable_if_at_least_bidirectional_t<Self>> = nullptr>
  constexpr auto operator--(int
  ) noexcept(noexcept(--std::declval<Self&>()) && std::is_nothrow_copy_constructible<Self>::value) -> Self {
    auto tmp = derived();
    --derived();
    return tmp;
  }

  // parasoft-end-suppress AUTOSAR-A3_9_1-b

  /// @brief square bracket dereference
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @param difference signed distance by which to shift this iterator
  /// @pre <c>*this + difference</c> is dereferenceable, otherwise the behaviour is undefined
  /// @return reference to the element pointed to be <c>*this + difference</c>
  ///
  template <class Self = Derived, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  constexpr auto operator[](typename Self::difference_type const& difference) const
      noexcept(is_plus_equals_noexcept_v<Self>&& noexcept(*std::declval<Self&>())) -> typename Self::reference {
    return *(derived() + difference);
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  /// @brief binary @c + with an iterator on the left and a difference on the right
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @param itr the iterator used as a starting point for the shift
  /// @param diff the signed difference by which to shift the starting iterator
  /// @return @c itr shifted forward by @c diff
  /// @pre Applying @c ++itr @c diff times (if <c>diff > 0</c>) or applying @c --itr @c -diff times (if <c>diff < 0</c>)
  /// results in a valid iterator, otherwise the behaviour is undefined
  ///
  template <class Self = Derived, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto
  operator+(Derived itr, typename Self::difference_type diff) noexcept(is_plus_equals_noexcept_v<Self>) -> Self {
    itr += diff;
    return itr;
  }

  /// @brief binary @c + with a difference on the left and an iterator on the right
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @param diff the signed difference by which to shift the starting iterator
  /// @param itr the iterator used as a starting point for the shift
  /// @return @c itr shifted forward by @c diff
  /// @pre Applying @c ++itr @c diff times (if <c>diff > 0</c>) or applying @c --itr @c -diff times (if <c>diff < 0</c>)
  /// results in a valid iterator, otherwise the behaviour is undefined
  ///
  template <class Self = Derived, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto operator+(typename Self::difference_type diff, Derived itr) noexcept(
      is_plus_equals_noexcept_v<Self> && std::is_nothrow_move_constructible<Self>::value
  ) -> Self {
    return std::move(itr) + diff;
  }

  /// @brief binary @c - with an iterator on the left and a difference on the right
  /// @tparam Self defaulted parameter to allow SFINAE
  /// @param itr the iterator used as a starting point for the shift
  /// @param diff the signed difference by which to shift the starting iterator
  /// @return @c itr shifted backward by @c diff
  /// @pre <tt> difference != std::numeric_limits<difference_type>::lowest() </tt>, otherwise precondition violation
  /// @pre Applying @c --itr @c diff times (if <c>diff > 0</c>) or applying @c ++itr @c -diff times (if <c>diff < 0</c>)
  /// results in a valid iterator, otherwise the behaviour is undefined
  ///
  template <class Self = Derived, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto
  operator-(Derived itr, typename Self::difference_type diff) noexcept(is_plus_equals_noexcept_v<Self>) -> Self {
    itr -= diff;
    return itr;
  }

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b "Parameters of all comparisons do resolve to the same type eventually"

  /// @brief synthesized equality, used for random-access iterators that don't define their own equality
  /// @tparam Self parameter to allow SFINAE
  /// @param lhs left argument
  /// @param rhs right argument
  /// @return <tt> (rhs - lhs) == 0 </tt>
  ///
  template <class Self, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto operator==(Self const& lhs, Derived const& rhs) noexcept(noexcept(rhs - lhs)) -> bool {
    return (rhs - lhs) == typename Self::difference_type{0};
  }

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
  // parasoft-begin-suppress AUTOSAR-A13_2_3-a "False positive: 'operator!=' does return 'bool'"

  /// @brief inequality
  /// @tparam Self parameter to allow SFINAE
  /// @param lhs left argument
  /// @param rhs right argument
  /// @return <tt> !(lhs == rhs) </tt>
  ///
  template <class Self>
  friend constexpr auto operator!=(Self const& lhs, Derived const& rhs) noexcept(noexcept(lhs == rhs)) -> bool {
    return !(lhs == rhs);
  }

  // parasoft-end-suppress AUTOSAR-M3_3_2-a
  // parasoft-end-suppress AUTOSAR-A13_2_3-a

  /// @brief synthesized less-than, used for random-access iterators that don't define their own less-than
  /// @tparam Self parameter to allow SFINAE
  /// @param lhs left argument
  /// @param rhs right argument
  /// @return <tt> (rhs - lhs) > 0 </tt>
  /// @pre It is possible to reach @c lhs from @c rhs by repeatedly applying @c operator-- or <c>operator++</c>,
  /// otherwise the behaviour is undefined
  ///
  template <class Self, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto operator<(Self const& lhs, Derived const& rhs) noexcept(noexcept(rhs - lhs)) -> bool {
    return (rhs - lhs) > typename Self::difference_type{0};
  }

  /// @brief less than or equal
  /// @tparam Self parameter to allow SFINAE
  /// @param lhs left argument
  /// @param rhs right argument
  /// @return <tt> !(rhs < lhs) </tt>
  /// @pre It is possible to reach @c lhs from @c rhs by repeatedly applying @c operator-- or <c>operator++</c>,
  /// otherwise the behaviour is undefined
  ///
  template <class Self, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto operator<=(Self const& lhs, Derived const& rhs) noexcept(noexcept(!(rhs < lhs))) -> bool {
    return !(rhs < lhs);
  }

  /// @brief greater than
  /// @tparam Self parameter to allow SFINAE
  /// @param lhs left argument
  /// @param rhs right argument
  /// @return <tt> rhs < lhs </tt>
  /// @pre It is possible to reach @c lhs from @c rhs by repeatedly applying @c operator-- or <c>operator++</c>,
  /// otherwise the behaviour is undefined
  ///
  template <class Self, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto operator>(Self const& lhs, Derived const& rhs) noexcept(noexcept(rhs < lhs)) -> bool {
    return rhs < lhs;
  }

  /// @brief greater than or equal
  /// @tparam Self parameter to allow SFINAE
  /// @param lhs left argument
  /// @param rhs right argument
  /// @return <tt> !(lhs < rhs) </tt>
  /// @pre It is possible to reach @c lhs from @c rhs by repeatedly applying @c operator-- or <c>operator++</c>,
  /// otherwise the behaviour is undefined
  ///
  template <class Self, constraints<enable_if_at_least_random_access_t<Self>> = nullptr>
  friend constexpr auto operator>=(Self const& lhs, Derived const& rhs) noexcept(noexcept(!(lhs < rhs))) -> bool {
    return !(lhs < rhs);
  }

  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A13_5_5-b
};

// parasoft-end-suppress AUTOSAR-A13_5_1-a

}  // namespace inline_container_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_ITERATOR_INTERFACE_HPP_
