// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ARRAY_ARRAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ARRAY_ARRAY_HPP_

// IWYU pragma: private, include "arene/base/array.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/algorithm/lexicographical_compare.hpp"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"

namespace arene {
namespace base {

namespace array_detail {
/// @brief If exceptions are enabled, throws out of range. Otherwise, is an invariant violation as it should not be
/// called.
/// @note We have to use this method rather than the mixin because array must be an aggregate type, and in C++14 that
/// means no base classes.
/// @throws std::out_of_range if exceptions are enabled.
ARENE_NORETURN void throw_out_of_range();
}  // namespace array_detail

// parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: arene::base::array members cannot hide extents members"
// parasoft-begin-suppress AUTOSAR-A12_1_1-b-2 "False positive: arene::base::array models std::array, which is an
// Aggregate, and has no constructors"
/// @brief A fixed-size array of T objects.
///
/// This is a backport of the @c constexpr @c std::array from C++17.
/// @tparam T The type of the element held by the array
/// @tparam N The number of elements in the array
template <typename T, std::size_t N>
class array {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Identifiers do not hide content in unrelated class span"
  /// @brief The type of each array element
  using value_type = T;
  /// @brief The type of a pointer to an array element
  using pointer = T*;
  /// @brief The type of a pointer to a const array element
  using const_pointer = T const*;
  /// @brief The type of a reference to an array element
  using reference = T&;
  /// @brief The type of a reference to a const array element
  using const_reference = T const&;
  /// @brief The type returned by @c size()
  using size_type = std::size_t;
  /// @brief The type for the distance between two iterators
  using difference_type = std::ptrdiff_t;

  /// @brief An iterator for the array
  using iterator = typename span<T, N>::iterator;
  /// @brief An iterator for the array that treats the elements as @c const
  using const_iterator = typename span<T const, N>::iterator;

  /// @brief An iterator for iteration through the array in reverse order.
  using reverse_iterator = typename span<T, N>::reverse_iterator;
  /// @brief An iterator for iteration through the array in reverse order that treats the elements as @c const.
  using const_reverse_iterator = typename span<T, N>::const_reverse_iterator;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief The number of elements in the array, which is always @c N
  /// @return The size of the array, N
  static constexpr std::integral_constant<std::size_t, N> size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-end-suppress AUTOSAR-A2_10_1-e "False positive: Identifiers do not hide content in unrelated class span"

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'max_size' does not hide anything"
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief The maximum number of elements in the array, which is always @c N
  /// @return The size of the array, N
  static constexpr std::integral_constant<std::size_t, N> max_size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'empty' does not hide anything"
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief Check if the array is empty, always @c false
  /// @return false
  static constexpr std::integral_constant<bool, false> empty{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Obtain a pointer to the first element of the array.
  /// @return a pointer to the first element of the array.
  constexpr auto data() noexcept -> T* { return values_; }

  /// @brief Obtain a pointer to the first element of the array.
  /// @return a pointer to the first element of the array, const-qualified.
  constexpr auto data() const noexcept -> T const* { return values_; }

  /// @brief Obtain an iterator to the first element of the array.
  /// @return an @c iterator that references the first element of the array.
  constexpr auto begin() noexcept -> iterator { return span<T, N>{values_}.begin(); }

  /// @brief Obtain an iterator to the one-passed-the-end of the array.
  /// @return an @c iterator that references one-past the last element of the array.
  constexpr auto end() noexcept -> iterator { return span<T, N>{values_}.end(); }

  /// @brief Obtain an iterator to the first element of the array.
  /// @return a @c const_iterator that references the first element of the array.
  constexpr auto begin() const noexcept -> const_iterator { return span<T const, N>{values_}.begin(); }

  /// @brief Obtain an iterator to the one-passed-the-end of the array.
  /// @return a @c const_iterator that references one past the last element of the array.
  constexpr auto end() const noexcept -> const_iterator { return span<T const, N>{values_}.end(); }

  /// @brief Obtain a reverse_iterator to the last element of the array.
  /// @return a @c reverse_iterator that references the last element of the array.
  constexpr auto rbegin() noexcept -> reverse_iterator { return span<T, N>{values_}.rbegin(); }

  /// @brief Obtain a @c reverse_iterator that references one-before the first element of the array.
  /// @return a @c reverse_iterator that references one-before the first element of the array.
  constexpr auto rend() noexcept -> reverse_iterator { return span<T, N>{values_}.rend(); }

  /// @brief Obtain a reverse_iterator to the last element of the array.
  /// @return a @c const_reverse_iterator that references the last element of the array.
  constexpr auto rbegin() const noexcept -> const_reverse_iterator { return span<T const, N>{values_}.rbegin(); }

  /// @brief Obtai a @c const_reverse_iterator that references one before the first element of the array.
  /// @return a @c const_reverse_iterator that references one before the first element of the array.
  constexpr auto rend() const noexcept -> const_reverse_iterator { return span<T const, N>{values_}.rend(); }

  /// @brief Obtain a @c const_iterator that references the first element of the array.
  /// @return a @c const_iterator that references the first element of the array.
  constexpr auto cbegin() const noexcept -> const_iterator { return span<T const, N>{values_}.begin(); }

  /// @brief Obtain a @c const_iterator that references one past the last element of the array.
  /// @return a @c const_iterator that references one past the last element of the array.
  constexpr auto cend() const noexcept -> const_iterator { return span<T const, N>{values_}.end(); }

  /// @brief Obtain a @c const_reverse_iterator that references the last element of the array.
  /// @return a @c const_reverse_iterator that references the last element of the array.
  constexpr auto crbegin() const noexcept -> const_reverse_iterator { return span<T const, N>{values_}.rbegin(); }

  /// @brief Obtain a @c const_reverse_iterator that references one before the first element of the array.
  /// @return a @c const_reverse_iterator that references one before the first element of the array.
  constexpr auto crend() const noexcept -> const_reverse_iterator { return span<T const, N>{values_}.rend(); }

  /// @brief Get a reference to the n'th element of the array.
  ///
  /// @param index The index of the element to access.
  /// @pre @c index Must be less than @c N or else @c ARENE_PRECONDITION violation.
  /// @return A reference to that element
  constexpr auto operator[](std::size_t index) noexcept -> T& {
    ARENE_PRECONDITION(index < size());
    return values_[index];
  }
  /// @brief Get a reference to the n'th element of the array.
  ///
  /// @param index The index of the element to access.
  /// @pre @c index Must be less than @c N or else @c ARENE_PRECONDITION violation.
  /// @return A reference to that element
  constexpr auto operator[](std::size_t index) const noexcept -> T const& {
    ARENE_PRECONDITION(index < size());
    return values_[index];
  }

  /// @brief Get a reference to the n'th element of the array, or throw.
  ///
  /// @tparam AreExceptionsEnabled SFINAE flag to disable this method if exceptions are not enabled. Should not be
  /// specified by the user.
  /// @param index The index of the element to access.
  /// @return A reference to that element
  /// @throws std::out_of_range If index is not less than @c N
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  constexpr auto at(std::size_t index) -> T& {
    if (index >= size()) {
      array_detail::throw_out_of_range();
    }
    return values_[index];
  }
  /// @brief Get a reference to the n'th element of the array, or throw.
  ///
  /// @tparam AreExceptionsEnabled Defaulted SFINAE flag to disable this method if exceptions are not enabled; should
  /// not be specified by user.
  /// @param index The index of the element to access.
  /// @return A reference to that element
  /// @throws std::out_of_range If index is not less than @c N
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  constexpr auto at(std::size_t index) const -> T const& {
    if (index >= size()) {
      array_detail::throw_out_of_range();
    }
    return values_[index];
  }

  /// @brief Get a reference to the first element of the array.
  ///
  /// @return A reference to the first element
  constexpr auto front() noexcept -> T& { return (*this)[0]; }

  /// @brief Get a const-reference to the first element of the array.
  ///
  /// @return A reference to the first element
  constexpr auto front() const noexcept -> T const& { return (*this)[0]; }

  /// @brief Get a const-reference to the last element of the array.
  ///
  /// @return A reference to the last element
  constexpr auto back() noexcept -> T& { return (*this)[N - 1U]; }

  /// @brief Get a const-reference to the last element of the array.
  ///
  /// @return A reference to the last element
  constexpr auto back() const noexcept -> T const& { return (*this)[N - 1U]; }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Equality compare all elements in the array.
  /// @param lhs one of the arrays to compare
  /// @param rhs the other array to compare
  /// @return true if all elements in the arrays are equal.
  /// @return false if any elements in the arrays are not equal, or the sizes of the arrays are different.
  /// @{
  template <typename U = T, constraints<std::enable_if_t<is_equality_comparable_v<U>>> = nullptr>
  friend constexpr auto operator==(array const& lhs, array const& rhs) noexcept -> bool {
    return arene::base::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }
  template <std::size_t UN, typename U = T, constraints<std::enable_if_t<is_equality_comparable_v<U>>> = nullptr>
  friend constexpr auto operator==(array const&, array<T, UN> const&) noexcept -> bool {
    return false;
  }
  template <
      typename U = T,
      constraints<
          std::enable_if_t<!is_equality_comparable_v<U>>,
          std::enable_if_t<compare_three_way_supported_v<U>>,
          std::enable_if_t<!has_fast_inequality_check_v<U>>> = nullptr>
  friend constexpr auto operator==(array const& lhs, array const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) == arene::base::strong_ordering::equal;
  }
  template <
      typename U = T,
      constraints<
          std::enable_if_t<!is_equality_comparable_v<U>>,
          std::enable_if_t<compare_three_way_supported_v<U>>,
          std::enable_if_t<has_fast_inequality_check_v<U>>> = nullptr>
  friend constexpr auto operator==(array const& lhs, array const& rhs) noexcept -> bool {
    return (array::fast_inequality_check(lhs, rhs) == inequality_heuristic::may_be_equal_or_not_equal) &&
           (array::three_way_compare(lhs, rhs) == arene::base::strong_ordering::equal);
  }
  template <
      std::size_t UN,
      typename U = T,
      constraints<std::enable_if_t<!is_equality_comparable_v<U>>, std::enable_if_t<compare_three_way_supported_v<U>>> =
          nullptr>
  friend constexpr auto operator==(array const&, array<T, UN> const&) noexcept -> bool {
    return false;
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Inequality compare all elements in the array.
  /// @param lhs one of the arrays to compare
  /// @param rhs the other array to compare
  /// @return true if any elements in the arrays are not equal, or the sizes of the arrays are different.
  /// @return false if all elements in the arrays are equal.
  template <std::size_t UN, typename U = T>
  friend constexpr auto operator!=(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'three_way_compare' does not hide anything"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Imparts a lexicographic ordering of two arrays.
  /// @param lhs one of the arrays to compare
  /// @param rhs the other array to compare
  /// @return arene::base::strong_ordering::less if the first element which compares unequal between
  ///         @c rhs and @c lhs also compares less-than, or if @c lhs is a strict prefix of @c rhs.
  /// @return arene::base::strong_ordering::equal if all elements compare equal.
  /// @return arene::base::strong_ordering::greater if the first element which compares unequal between
  ///         @c rhs and @c lhs also compares greater, or if @c rhs is a strict prefix of @c lhs.
  /// @{
  template <std::size_t UN, typename U = T, constraints<std::enable_if_t<compare_three_way_supported_v<U>>> = nullptr>
  static constexpr auto three_way_compare(array const& lhs, array<T, UN> const& rhs) noexcept
      -> arene::base::strong_ordering {
    return lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }
  template <typename U = T, constraints<std::enable_if_t<compare_three_way_supported_v<U>>> = nullptr>
  static constexpr auto three_way_compare(array const&, array<T, 0> const&) noexcept -> arene::base::strong_ordering {
    // This overload shouldn't need to exist, but there is a bug in GCC8 where a for-loop over 0 increments results in
    // the containing function no longer being constexpr.
    return arene::base::strong_ordering::greater;
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'fast_inequality_check' does not hide anything"
  /// @brief Quickly checks two arrays for inequality to see if a more costly full ordering check is needed
  /// @param lhs one of the arrays to compare
  /// @param rhs the other array to compare
  /// @return @c arene::base::inequality_heuristic::definitely_not_equal if the two arrays have different sizes or if
  ///         any pair of their elements compare @c definitely_not_equal
  /// @return @c arene::base::inequality_heuristic::may_be_equal_or_not_equal otherwise
  template <std::size_t UN, typename U = T, constraints<std::enable_if_t<has_fast_inequality_check_v<U>>> = nullptr>
  static constexpr auto fast_inequality_check(array const& lhs, array<T, UN> const& rhs) noexcept
      -> arene::base::inequality_heuristic {
    if (size() != rhs.size()) {
      return arene::base::inequality_heuristic::definitely_not_equal;
    }

    for (size_type idx{}; idx < size(); ++idx) {
      if (U::fast_inequality_check(lhs[idx], rhs[idx]) == arene::base::inequality_heuristic::definitely_not_equal) {
        return arene::base::inequality_heuristic::definitely_not_equal;
      }
    }

    return arene::base::inequality_heuristic::may_be_equal_or_not_equal;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Imparts a lexicographic ordering of two arrays.
  /// @param lhs one of the arrays to compare
  /// @param rhs the other array to compare
  /// @see arene::base::array<T,N>::three_way_compare .
  /// @{
  template <std::size_t UN>
  friend constexpr auto operator<(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) == arene::base::strong_ordering::less;
  }
  template <std::size_t UN>
  friend constexpr auto operator<=(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) != arene::base::strong_ordering::greater;
  }
  template <std::size_t UN>
  friend constexpr auto operator>(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) == arene::base::strong_ordering::greater;
  }
  template <std::size_t UN>
  friend constexpr auto operator>=(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) != arene::base::strong_ordering::less;
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-M0_1_8-a-2 "False positive: updates the 'values_' member"
  /// @brief Set every element in the array to the provided value
  /// @param val The value to use
  template <
      typename U = T,
      constraints<std::enable_if_t<std::is_same<U, T>::value>, std::enable_if_t<std::is_copy_assignable<U>::value>> =
          nullptr>
  constexpr void fill(T const& val) noexcept(std::is_nothrow_copy_assignable<T>::value) {
    // parasoft-begin-suppress AUTOSAR-A18_1_1-a-2 "This class abstracts over the C array so users don't have to"
    for (T& elem : values_) {
      elem = val;
    }
    // parasoft-end-suppress AUTOSAR-A18_1_1-a-2
  }
  // parasoft-end-suppress AUTOSAR-M0_1_8-a-2

  ///
  /// @brief swaps all the elements this array and another.
  ///
  /// @tparam U the type of the elements in the array. Must satisfy @c is_swappable_v .
  /// @param other the array to swap elements with
  /// @post The elements in this array and @c other are exchanged 1:1. Relative order is maintained.
  ///
  template <
      typename U = T,
      constraints<std::enable_if_t<std::is_same<U, T>::value>, std::enable_if_t<is_swappable_v<U>>> = nullptr>
  constexpr void swap(array& other) noexcept(is_nothrow_swappable_v<U>) {
    iterator rhs_iter{other.begin()};
    for (auto& lhs_elem : *this) {
      ::arene::base::swap(lhs_elem, *rhs_iter);
      ++rhs_iter;
    }
  }

  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Friend swap operators permitted by A11-3-1 Permit #2 v1.0.0"
  ///
  /// @brief swaps all the elements between two arrays.
  ///
  /// @tparam U the type of the elements in the array. Must satisfy @c is_swappable_v .
  /// @param lhs the left hand array to swap.
  /// @param rhs the right hand array to swap.
  /// @post The elements in @c lhs and @c rhs are exchanged 1:1. Relative order is maintained.
  ///
  template <
      typename U = T,
      constraints<std::enable_if_t<std::is_same<U, T>::value>, std::enable_if_t<is_swappable_v<U>>> = nullptr>
  friend constexpr void swap(array& lhs, array& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "For arene::base::array to match the behaviour of std::array, values_
  // must be public"
  // parasoft-begin-suppress AUTOSAR-A18_1_1-a-2 "arene::base::array uses a C-style array so everywhere else can use
  // arene::base::array instead"
  /// @brief The actual elements of the @c array object. This must be a public member
  /// to allow aggregate initialization, but users should treat it as private,
  /// and only use the documented public interface.
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays,private-member-variables-in-classes,misc-non-private-member-variables-in-classes,readability-identifier-naming)
  T values_[N];
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-end-suppress AUTOSAR-A18_1_1-a-2
};
// parasoft-end-suppress AUTOSAR-A12_1_1-b-2
// parasoft-end-suppress AUTOSAR-A2_10_1-e

template <typename T, std::size_t N>
constexpr std::integral_constant<std::size_t, N> array<T, N>::size;

template <typename T, std::size_t N>
constexpr std::integral_constant<std::size_t, N> array<T, N>::max_size;

template <typename T, std::size_t N>
constexpr std::integral_constant<bool, false> array<T, N>::empty;

// parasoft-begin-suppress AUTOSAR-A14_7_2-a-2 "False positive: This *is* the same file as the primary template declared
// above"
/// @brief Specialize for zero-sized arrays as 'T array[0]' is not standards compliant.
/// @see arene::base::array
template <typename T>
class array<T, 0> {
 public:
  /// @brief The type of each array element
  using value_type = T;
  /// @brief The type of a pointer to an array element
  using pointer = T*;
  /// @brief The type of a pointer to a const array element
  using const_pointer = T const*;
  /// @brief The type of a reference to an array element
  using reference = T&;
  /// @brief The type of a reference to a const array element
  using const_reference = T const&;
  /// @brief The type returned by @c size()
  using size_type = std::size_t;
  /// @brief The type for the distance between two iterators
  using difference_type = std::ptrdiff_t;

  /// @brief An iterator for the array
  using iterator = typename span<T, 0>::iterator;
  /// @brief An iterator for the array that treats the elements as @c const
  using const_iterator = typename span<T const, 0>::iterator;

  /// @brief An iterator for iteration through the array in reverse order.
  using reverse_iterator = typename span<T, 0>::reverse_iterator;
  /// @brief An iterator for iteration through the array in reverse order that treats the elements as @c const.
  using const_reverse_iterator = typename span<T, 0>::const_reverse_iterator;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief The number of elements in the array, which is always 0
  /// @return The size of the array, 0
  static constexpr std::integral_constant<std::size_t, 0> size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief The maximum number of elements in the array, which is always 0
  /// @return The size of the array, 0
  static constexpr std::integral_constant<std::size_t, 0> max_size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief Check if the array is empty, always @c true
  /// @return true
  static constexpr std::integral_constant<bool, true> empty{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a pointer to the first element of the array.
  /// @return A null pointer
  constexpr auto data() noexcept -> T* { return nullptr; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a pointer to the first element of the const array.
  /// @return A null pointer
  constexpr auto data() const noexcept -> T const* { return nullptr; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get an iterator that references the first element of the array.
  /// @return The iterator
  constexpr auto begin() noexcept -> iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get an iterator that references one-past the last element of the array.
  /// @return The iterator
  constexpr auto end() noexcept -> iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const_iterator that references the first element of the array.
  /// @return The iterator
  constexpr auto begin() const noexcept -> const_iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const_iterator that references one past the last element of the array.
  /// @return The iterator
  constexpr auto end() const noexcept -> const_iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get an reverse_iterator that references the last element of the array.
  /// @return The iterator
  constexpr auto rbegin() noexcept -> reverse_iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get an reverse_iterator that references one-before the first element of the array.
  /// @return The iterator
  constexpr auto rend() noexcept -> reverse_iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const_reverse_iterator that references the last element of the array.
  /// @return The iterator
  constexpr auto rbegin() const noexcept -> const_reverse_iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const_reverse_iterator that references one before the first element of the array.
  /// @return The iterator
  constexpr auto rend() const noexcept -> const_reverse_iterator { return {}; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  /// @brief Get a const_iterator that references the first element of the array.
  /// @return The iterator
  static constexpr auto cbegin() noexcept -> const_iterator { return {}; }

  /// @brief Get a const_iterator that references one past the last element of the array.
  /// @return The iterator
  static constexpr auto cend() noexcept -> const_iterator { return {}; }

  /// @brief Get a const_reverse_iterator that references the last element of the array.
  /// @return The iterator
  static constexpr auto crbegin() noexcept -> const_reverse_iterator { return {}; }

  /// @brief Get a const_reverse_iterator that references one before the first element of the array.
  /// @return The iterator
  static constexpr auto crend() noexcept -> const_reverse_iterator { return {}; }

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  /// @brief Get a reference to the index'th element of the array.
  /// @return Never returns
  ///
  /// @invariant Will always trigger @c ARENE_PRECONDITION
  /// @{
  ARENE_NORETURN constexpr auto operator[](std::size_t) noexcept -> T& {
    ARENE_PRECONDITION(!empty());
    ARENE_INVARIANT_UNREACHABLE("Precondition always fires");
  }
  ARENE_NORETURN constexpr auto operator[](std::size_t) const noexcept -> T const& {
    ARENE_PRECONDITION(!empty());
    ARENE_INVARIANT_UNREACHABLE("Precondition always fires");
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
  // parasoft-end-suppress CERT_C-EXP37-a-3
  // parasoft-end-suppress CERT_CPP-MSC52-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress AUTOSAR-A8-4-2-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress CERT_C-MSC37-a "False positive: these functions are marked NORETURN."
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: these functions are marked NORETURN."
  /// @brief Get a reference to the index'th element of the array.
  /// @tparam AreExceptionsEnabled Defaulted SFINAE flag to disable this method if exceptions are not enabled; should
  /// not be specified by user.
  /// @return Never returns
  ///
  /// @throws std::out_of_range unconditionally
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  ARENE_NORETURN constexpr auto at(std::size_t) -> T& {
    array_detail::throw_out_of_range();
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2

  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Get a const-reference to the index'th element of the array.
  /// @tparam AreExceptionsEnabled Defaulted SFINAE flag to disable this method if exceptions are not enabled; should
  /// not be specified by user.
  /// @return Never returns
  ///
  /// @throws std::out_of_range
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  ARENE_NORETURN constexpr auto at(std::size_t) const -> T const& {
    array_detail::throw_out_of_range();
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2
  // parasoft-begin-suppress CERT_C-MSC37-a
  // parasoft-begin-suppress CERT_CPP-MSC52-a

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const-reference to the first element of the array.
  ///
  /// @return A reference to the first element
  ARENE_NORETURN constexpr auto front() noexcept -> T& {
    ARENE_PRECONDITION(!empty());
    ARENE_INVARIANT_UNREACHABLE("Precondition always fires");
  }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress AUTOSAR-A8-4-2-a-2

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const-reference to the first element of the array.
  ///
  /// @return A reference to the first element
  ARENE_NORETURN constexpr auto front() const noexcept -> T const& {
    ARENE_PRECONDITION(!empty());
    ARENE_INVARIANT_UNREACHABLE("Precondition always fires");
  }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress AUTOSAR-A8-4-2-a-2

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const-reference to the last element of the array.
  ///
  /// @return A reference to the last element
  ARENE_NORETURN constexpr auto back() noexcept -> T& {
    ARENE_PRECONDITION(!empty());
    ARENE_INVARIANT_UNREACHABLE("Precondition always fires");
  }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress AUTOSAR-A8-4-2-a-2

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_PRECONDITION terminates, so does not need a return"
  // parasoft-begin-suppress AUTOSAR-M9_3_3-a-2 "False positive: 'static' would prevent overloading on 'const'"
  /// @brief Get a const-reference to the last element of the array.
  ///
  /// @return A reference to the last element
  ARENE_NORETURN constexpr auto back() const noexcept -> T const& {
    ARENE_PRECONDITION(!empty());
    ARENE_INVARIANT_UNREACHABLE("Precondition always fires");
  }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a-2
  // parasoft-end-suppress CERT_CPP-MSC52-a-2
  // parasoft-end-suppress CERT_C-MSC37-a-2
  // parasoft-end-suppress AUTOSAR-A8-4-2-a-2

  // parasoft-begin-suppress AUTOSAR-M0_1_8-b-2 "Required for consistency with API of primary template"
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Set every element in the array to the provided value. This is a noop for an empty array.
  static constexpr void fill(T const&) noexcept {}
  // parasoft-end-suppress CERT_C-EXP37-a-3
  // parasoft-end-suppress AUTOSAR-M0_1_8-b-2

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Swap all the elements with another array. This is a noop for an empty array.
  constexpr void swap(array&) noexcept {}
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Friend swap operators permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a public function, and doesn't need to be used
  // in the translation unit"
  /// @brief Swap all the elements with another array. This is a noop for an empty array.
  friend constexpr void swap(array&, array&) noexcept {}
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: All overloads are fully are documented"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "False positive: All overloads are fully are documented"
  /// @brief Compares two arrays lexicographically.
  /// @param lhs The left hand operand to the comparison.
  /// @param rhs The right hand operand to the comparison.
  /// @return The equivalent of @c compare_three_way{}(0U,UN)
  /// @{
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Friend three_way_compare permitted by A11-3-1 Permit #2 v1.0.0""
  template <std::size_t UN>
  static constexpr auto three_way_compare(array const&, array<T, UN> const&) noexcept -> arene::base::strong_ordering {
    return compare_three_way{}(0U, UN);
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  template <std::size_t UN>
  friend constexpr auto operator==(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) == arene::base::strong_ordering::equal;
  }
  template <std::size_t UN>
  friend constexpr auto operator!=(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  template <std::size_t UN>
  friend constexpr auto operator<(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) == arene::base::strong_ordering::less;
  }
  template <std::size_t UN>
  friend constexpr auto operator<=(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) != arene::base::strong_ordering::greater;
  }
  template <std::size_t UN>
  friend constexpr auto operator>(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) == arene::base::strong_ordering::greater;
  }
  template <std::size_t UN>
  friend constexpr auto operator>=(array const& lhs, array<T, UN> const& rhs) noexcept -> bool {
    return array::three_way_compare(lhs, rhs) != arene::base::strong_ordering::less;
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  /// @}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b-2
  // parasoft-end-suppress AUTOSAR-A2_7_3-a-2
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a-2

template <typename T>
constexpr std::integral_constant<std::size_t, 0> array<T, 0>::size;

template <typename T>
constexpr std::integral_constant<std::size_t, 0> array<T, 0>::max_size;

template <typename T>
constexpr std::integral_constant<bool, true> array<T, 0>::empty;

namespace array_detail {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Internal function to construct an @c array from a C-style array, using the
/// specified indices
/// @tparam T The type of the array elements
/// @tparam Size The number of array elements
/// @tparam Indices The indices to use for initialization
/// @param arr The source array
/// @return The constructed array
template <typename T, std::size_t Size, std::size_t... Indices>
constexpr auto to_array(
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    T (&arr)[Size],
    std::index_sequence<Indices...>
) noexcept(std::is_nothrow_constructible<base::remove_cvref_t<T>, T&>::value) -> array<base::remove_cvref_t<T>, Size> {
  return {{arr[Indices]...}};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Internal function to construct an @c array from an rvalue C-style array,
/// using the specified indices
/// @tparam T The type of the array elements
/// @tparam Size The number of array elements
/// @tparam Indices The indices to use for initialization
/// @param arr The source array
/// @return The constructed array
template <typename T, std::size_t Size, std::size_t... Indices>
constexpr auto to_array(
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    T (&&arr)[Size],
    std::index_sequence<Indices...>
) noexcept(std::is_nothrow_move_constructible<base::remove_cvref_t<T>>::value) -> array<base::remove_cvref_t<T>, Size> {
  return {{std::move(arr[Indices])...}};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
}  // namespace array_detail

/// @brief Function to construct an @c array from a C-style array, using the specified
/// indices
/// @tparam T The type of the array elements
/// @tparam Size The number of array elements
/// @param arr The source array
/// @return The constructed array
template <
    typename T,
    std::size_t Size,
    constraints<
        std::enable_if_t<!std::is_array<T>::value>,
        std::enable_if_t<std::is_constructible<base::remove_cvref_t<T>, T&>::value>> = nullptr>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto to_array(T (&arr)[Size]) noexcept(std::is_nothrow_constructible<base::remove_cvref_t<T>, T&>::value)
    -> array<base::remove_cvref_t<T>, Size> {
  return array_detail::to_array(arr, std::make_index_sequence<Size>());
}

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Internal function to construct an @c array from an rvalue C-style array,
/// using the specified indices
/// @tparam T The type of the array elements
/// @tparam Size The number of array elements
/// @param arr The source array
/// @return The constructed array
template <
    typename T,
    std::size_t Size,
    constraints<
        std::enable_if_t<!std::is_array<T>::value>,
        std::enable_if_t<std::is_move_constructible<base::remove_cvref_t<T>>::value>> = nullptr>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
constexpr auto to_array(T (&&arr)[Size]) noexcept(std::is_nothrow_move_constructible<base::remove_cvref_t<T>>::value)
    -> array<base::remove_cvref_t<T>, Size> {
  return array_detail::to_array(std::move(arr), std::make_index_sequence<Size>());
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief Get a reference to the element at the specified index in the array
///
/// @tparam I The index of the element to retrieve. Must satisfy @c I<N .
/// @tparam T The type of the array elements
/// @tparam N The number of array elements
/// @param arr The array to retrieve the element from
/// @return T& equivalent to @c arr[I] .
template <std::size_t I, typename T, std::size_t N, constraints<std::enable_if_t<(I < N)>> = nullptr>
constexpr auto get(array<T, N>& arr) noexcept -> T& {
  return arr[I];
}
/// @brief Get a reference to the element at the specified index in the array
///
/// @tparam I The index of the element to retrieve. Must satisfy @c I<N .
/// @tparam T The type of the array elements
/// @tparam N The number of array elements
/// @param arr The array to retrieve the element from
/// @return T const& equivalent to @c arr[I] .
template <std::size_t I, typename T, std::size_t N, constraints<std::enable_if_t<(I < N)>> = nullptr>
constexpr auto get(array<T, N> const& arr) noexcept -> T const& {
  return arr[I];
}
/// @brief Get a reference to the element at the specified index in the array
///
/// @tparam I The index of the element to retrieve. Must satisfy @c I<N .
/// @tparam T The type of the array elements
/// @tparam N The number of array elements
/// @param arr The array to retrieve the element from
/// @return T&& equivalent to @c std::move(arr[I]) .
template <std::size_t I, typename T, std::size_t N, constraints<std::enable_if_t<(I < N)>> = nullptr>
constexpr auto get(array<T, N>&& arr) noexcept -> T&& {
  return std::move(arr[I]);
}
/// @brief Get a reference to the element at the specified index in the array
///
/// @tparam I The index of the element to retrieve. Must satisfy @c I<N .
/// @tparam T The type of the array elements
/// @tparam N The number of array elements
/// @param arr The array to retrieve the element from
/// @return T const&& equivalent to @c std::move(arr[I]) .
template <std::size_t I, typename T, std::size_t N, constraints<std::enable_if_t<(I < N)>> = nullptr>
constexpr auto get(array<T, N> const&& arr) noexcept -> T const&& {
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a "Moving is required to maintain rvalue qualification"
  return std::move(arr[I]);
  // parasoft-end-suppress AUTOSAR-A18_9_3-a "Moving is required to maintain rvalue qualification"
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "False positive: specialization of standard templates is permitted"
// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "False positive: specialization of standard templates is permitted"
// parasoft-begin-suppress AUTOSAR-A11_0_2-a "False positive: inheritance from integral_constant is permitted"
namespace std {
/// @brief Specialization of @c std::tuple_size for @c arene::base::array
/// @tparam T The type of the array elements
/// @tparam N The number of array elements
template <typename T, std::size_t N>
class tuple_size<arene::base::array<T, N>> : public std::integral_constant<std::size_t, N> {};

/// @brief Specialization of @c std::tuple_element for @c arene::base::array
/// @tparam N The index of the element
/// @tparam T The type of the array elements
/// @tparam ArrN The number of array elements
template <std::size_t N, typename T, std::size_t ArrN>
class tuple_element<N, arene::base::array<T, ArrN>> {
  static_assert(N < ArrN, "Index out of bounds in std::tuple_element<arene::base::array>");

 public:
  /// @brief The type of the element at index @c N
  using type = T;
};

}  // namespace std
// parasoft-end-suppress AUTOSAR-A11_0_2-a "False positive: inheritance from integral_constant is permitted"
// parasoft-end-suppress AUTOSAR-A17_6_1-a-2 "False positive: specialization of standard templates is permitted"
// parasoft-end-suppress CERT_CPP-DCL58-a-2 "False positive: specialization of standard templates is permitted"

// parasoft-end-suppress AUTOSAR-A11_3_1-a-2
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ARRAY_ARRAY_HPP_
