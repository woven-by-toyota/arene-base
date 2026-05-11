// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PAIR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PAIR_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/unwrap_reference.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/integer_sequence.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_convertible.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_default_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"
#include "stdlib/include/stdlib_detail/swap.hpp"
#include "stdlib/include/stdlib_detail/tuple_element.hpp"
#include "stdlib/include/stdlib_detail/tuple_fwd.hpp"
#include "stdlib/include/stdlib_detail/tuple_size.hpp"

namespace std {

/// @brief Tag type for piecewise construction
struct piecewise_construct_t {};

/// @brief Tag constant for piecewise construction
constexpr piecewise_construct_t piecewise_construct{};

// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: delegating constructors would not reduce duplication"
/// @brief A simple data structure to hold two values.
/// @tparam T1 The type of the first value
/// @tparam T2 The type of the second value
template <typename T1, typename T2>
class pair {
  // parasoft-end-suppress AUTOSAR-A12_1_5-a
 public:
  /// @brief The type of the first value
  using first_type = T1;
  /// @brief The type of the second value
  using second_type = T2;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This is part of the C++14 standard API"
  /// @brief The first value
  first_type first;
  /// @brief The second value
  second_type second;
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

 private:
  /// @brief Check if the memberwise copy constructor should be explicit
  /// @tparam Arg1 The type of the initializer for first argument
  /// @tparam Arg2 The type of the initializer for second argument
  template <typename Arg1, typename Arg2>
  static constexpr auto
      memberwise_copy_constructor_is_explicit{!is_convertible_v<Arg1 const&, Arg1> || !is_convertible_v<Arg2 const&, Arg2>};

  /// @brief Check if the memberwise forward constructor should be explicit
  /// @tparam Arg1 The type of the initializer for first argument
  /// @tparam Arg2 The type of the initializer for second argument
  template <typename Arg1, typename Arg2>
  static constexpr auto
      memberwise_forward_constructor_is_explicit{!is_convertible_v<Arg1&&, first_type> || !is_convertible_v<Arg2&&, second_type>};

  /// @brief Check if the copy constructor should be explicit
  /// @tparam Arg1 The type of the initializer for first argument
  /// @tparam Arg2 The type of the initializer for second argument
  template <typename Arg1, typename Arg2>
  static constexpr auto
      copy_constructor_is_explicit{!is_convertible_v<Arg1 const&, first_type> || !is_convertible_v<Arg2 const&, second_type>};

  /// @brief Check if the move constructor should be explicit
  /// @tparam Arg1 The type of the initializer for first argument
  /// @tparam Arg2 The type of the initializer for second argument
  template <typename Arg1, typename Arg2>
  static constexpr auto
      move_constructor_is_explicit{!is_convertible_v<Arg1&&, first_type> || !is_convertible_v<Arg2&&, second_type>};

 public:
  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "False positive: Not equivalent, ensures members are value-initialized"
  /// @brief Default construct a @c pair, with the @c first and @c second members value-initialized.
  ///
  /// This is disabled if either @c T1 or @c T2 is not default-constructible
  /// @tparam CheckType1 Template parameter used for checking constraints on @c T1
  /// @tparam CheckType2 Template parameter used for checking constraints on @c T2
  template <
      typename CheckType1 = T1,
      typename CheckType2 = T2,
      arene::base::constraints<
          enable_if_t<is_default_constructible_v<CheckType1>>,
          enable_if_t<is_default_constructible_v<CheckType2>>> = nullptr>
  constexpr pair() noexcept(is_nothrow_default_constructible_v<T1> && is_nothrow_default_constructible_v<T2>)
      : first{},
        second{} {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "This is part of the C++14 standard API"
  /// @brief Construct a @c pair from specified values for the two data members.
  ///
  /// This is disabled if either @c T1 or @c T2 is not copy-constructible
  /// @tparam CheckType1 Template parameter used for checking constraints on @c T1
  /// @tparam CheckType2 Template parameter used for checking constraints on @c T2
  /// @param val1 The value to use to initialize @c first
  /// @param val2 The value to use to initialize @c second
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<const first_type&, first_type>::value </tt>
  /// is false or <tt> is_convertible<const second_type&, second_type>::value </tt> is false.
  template <
      typename CheckType1 = T1,
      typename CheckType2 = T2,
      arene::base::constraints<
          enable_if_t<is_copy_constructible_v<CheckType1>>,
          enable_if_t<is_copy_constructible_v<CheckType2>>,
          enable_if_t<memberwise_copy_constructor_is_explicit<CheckType1, CheckType2>>> = nullptr>
  explicit constexpr pair(
      first_type const& val1,
      second_type const& val2
  ) noexcept(is_nothrow_copy_constructible_v<T1> && is_nothrow_copy_constructible_v<T2>)
      : first(val1),
        second(val2) {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "This is part of the C++14 standard API"
  /// @brief Construct a @c pair from specified values for the two data members.
  ///
  /// This is disabled if either @c T1 or @c T2 is not copy-constructible
  /// @tparam CheckType1 Template parameter used for checking constraints on @c T1
  /// @tparam CheckType2 Template parameter used for checking constraints on @c T2
  /// @param val1 The value to use to initialize @c first
  /// @param val2 The value to use to initialize @c second
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<const first_type&, first_type>::value </tt>
  /// is false or <tt> is_convertible<const second_type&, second_type>::value </tt> is false.
  template <
      typename CheckType1 = T1,
      typename CheckType2 = T2,
      arene::base::constraints<
          enable_if_t<is_copy_constructible_v<CheckType1>>,
          enable_if_t<is_copy_constructible_v<CheckType2>>,
          enable_if_t<!memberwise_copy_constructor_is_explicit<CheckType1, CheckType2>>> = nullptr>
  constexpr pair(
      first_type const& val1,
      second_type const& val2
  ) noexcept(is_nothrow_copy_constructible_v<T1> && is_nothrow_copy_constructible_v<T2>)
      : first(val1),
        second(val2) {}
  // parasoft-end-suppress AUTOSAR-A13_3_1-a

  /// @brief Construct a @c pair from specified values for the two data members using perfect forwarding.
  ///
  /// This is disabled if either @c T1 or @c T2 is not constructible from the corresponding initializer.
  /// @tparam Arg1 The type of the initializer for @c first
  /// @tparam Arg2 The type of the initializer for @c second
  /// @param val1 The value to use to initialize @c first
  /// @param val2 The value to use to initialize @c second
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<Arg1&&, first_type>::value </tt> is false or
  /// <tt> is_convertible<Arg2&&, second_type>::value </tt> is false.
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<is_constructible_v<T1, Arg1&&>>,
          enable_if_t<is_constructible_v<T2, Arg2&&>>,
          enable_if_t<memberwise_forward_constructor_is_explicit<Arg1, Arg2>>> = nullptr>
  explicit constexpr pair(
      Arg1&& val1,
      Arg2&& val2
  ) noexcept(is_nothrow_constructible_v<T1, Arg1&&> && is_nothrow_constructible_v<T2, Arg2&&>)
      : first(std::forward<Arg1>(val1)),
        second(std::forward<Arg2>(val2)) {}

  /// @brief Construct a @c pair from specified values for the two data members using perfect forwarding.
  ///
  /// This is disabled if either @c T1 or @c T2 is not constructible from the corresponding initializer.
  /// @tparam Arg1 The type of the initializer for @c first
  /// @tparam Arg2 The type of the initializer for @c second
  /// @param val1 The value to use to initialize @c first
  /// @param val2 The value to use to initialize @c second
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<Arg1&&, first_type>::value </tt> is false or
  /// <tt> is_convertible<Arg2&&, second_type>::value </tt> is false.
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<is_constructible_v<T1, Arg1&&>>,
          enable_if_t<is_constructible_v<T2, Arg2&&>>,
          enable_if_t<!memberwise_forward_constructor_is_explicit<Arg1, Arg2>>> = nullptr>
  constexpr pair(
      Arg1&& val1,
      Arg2&& val2
  ) noexcept(is_nothrow_constructible_v<T1, Arg1&&> && is_nothrow_constructible_v<T2, Arg2&&>)
      : first(std::forward<Arg1>(val1)),
        second(std::forward<Arg2>(val2)) {}

  /// @brief Copy a @c pair from another pair with potentially different value types
  ///
  /// This is disabled if either @c T1 or @c T2 is not constructible from the corresponding initializer.
  /// @tparam Arg1 The type of @c first in the source pair
  /// @tparam Arg2 The type of @c second in the source pair
  /// @param other The pair to initialize from
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<const Arg1&, first_type>::value </tt> is
  /// false or <tt> is_convertible<const Arg2&, second_type>::value </tt> is false.
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<is_constructible_v<T1, Arg1 const&>>,
          enable_if_t<is_constructible_v<T2, Arg2 const&>>,
          enable_if_t<copy_constructor_is_explicit<Arg1, Arg2>>> = nullptr>
  explicit constexpr pair(pair<Arg1, Arg2> const& other
  ) noexcept(is_nothrow_constructible_v<T1, Arg1 const&> && is_nothrow_constructible_v<T2, Arg2 const&>)
      : pair(other.first, other.second) {}

  /// @brief Copy a @c pair from another pair with potentially different value types
  ///
  /// This is disabled if either @c T1 or @c T2 is not constructible from the corresponding initializer.
  /// @tparam Arg1 The type of @c first in the source pair
  /// @tparam Arg2 The type of @c second in the source pair
  /// @param other The pair to initialize from
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<const Arg1&, first_type>::value </tt> is
  /// false or <tt> is_convertible<const Arg2&, second_type>::value </tt> is false.
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<is_constructible_v<T1, Arg1 const&>>,
          enable_if_t<is_constructible_v<T2, Arg2 const&>>,
          enable_if_t<!copy_constructor_is_explicit<Arg1, Arg2>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr pair(pair<Arg1, Arg2> const& other
  ) noexcept(is_nothrow_constructible_v<T1, Arg1 const&> && is_nothrow_constructible_v<T2, Arg2 const&>)
      : pair(other.first, other.second) {}

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A18_9_2-a "False positive: Forwarding fields"
  /// @brief Move-construct a @c pair from another pair with potentially different value types
  ///
  /// This is disabled if either @c T1 or @c T2 is not constructible from the corresponding initializer.
  /// @tparam Arg1 The type of @c first in the source pair
  /// @tparam Arg2 The type of @c second in the source pair
  /// @param other The pair to initialize from
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<Arg1&&, first_type>::value </tt> is false or
  /// <tt> is_convertible<Arg2&&, second_type>::value </tt> is false.
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<is_constructible_v<T1, Arg1&&>>,
          enable_if_t<is_constructible_v<T2, Arg2&&>>,
          enable_if_t<move_constructor_is_explicit<Arg1, Arg2>>> = nullptr>
  explicit constexpr pair(pair<Arg1, Arg2>&& other
  ) noexcept(is_nothrow_constructible_v<T1, Arg1&&> && is_nothrow_constructible_v<T2, Arg2&&>)
      : first(std::forward<Arg1>(other.first)),
        second(std::forward<Arg2>(other.second)) {}
  // parasoft-end-suppress AUTOSAR-A18_9_2-a
  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A18_9_2-a "False positive: Forwarding fields"
  /// @brief Move-construct a @c pair from another pair with potentially different value types
  ///
  /// This is disabled if either @c T1 or @c T2 is not constructible from the corresponding initializer.
  /// @tparam Arg1 The type of @c first in the source pair
  /// @tparam Arg2 The type of @c second in the source pair
  /// @param other The pair to initialize from
  ///
  /// @note The constructor is explicit if and only if <tt> is_convertible<Arg1&&, first_type>::value </tt> is false or
  /// <tt> is_convertible<Arg2&&, second_type>::value </tt> is false.
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<is_constructible_v<T1, Arg1&&>>,
          enable_if_t<is_constructible_v<T2, Arg2&&>>,
          enable_if_t<!move_constructor_is_explicit<Arg1, Arg2>>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr pair(pair<Arg1, Arg2>&& other
  ) noexcept(is_nothrow_constructible_v<T1, Arg1&&> && is_nothrow_constructible_v<T2, Arg2&&>)
      : first(std::forward<Arg1>(other.first)),
        second(std::forward<Arg2>(other.second)) {}
  // parasoft-end-suppress AUTOSAR-A18_9_2-a
  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

 private:
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: The parameters are used"
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b "False positive: The parameters are documented with @brief"
  // parasoft-begin-suppress AUTOSAR-A18_9_2-a "The type stored in the tuple is defined to be forwarded."
  /// @brief Helper constructor for piecewise construction
  /// @tparam Args1 The types of arguments in the tuple for constructing @c first
  /// @tparam Args2 The types of arguments in the tuple for constructing @c second
  /// @tparam I1 Indices for the first tuple arguments
  /// @tparam I2 Indices for the second tuple arguments
  /// @param first_args Tuple containing arguments to construct @c first
  /// @param second_args Tuple containing arguments to construct @c second
  template <typename... Args1, typename... Args2, size_t... I1, size_t... I2>
  pair(tuple<Args1...>& first_args,index_sequence<I1...>, tuple<Args2...>& second_args, //
                 index_sequence<I2...>)              //
    noexcept(is_nothrow_constructible_v<T1, Args1&&...> && is_nothrow_constructible_v<T2, Args2&&...>)
      : first{std::forward<Args1>(get<I1>(first_args))...},
        second{std::forward<Args2>(get<I2>(second_args))...} {}
  // parasoft-end-suppress AUTOSAR-A18_9_2-a
  // parasoft-end-suppress AUTOSAR-A2_7_3-b
  // parasoft-end-suppress AUTOSAR-A0_1_4-a

 public:
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief Construct a @c pair using piecewise construction from tuples of arguments
  ///
  /// This constructor allows constructing each element from a tuple of arguments using perfect forwarding.
  /// This is disabled if either @c T1 is not constructible from @c Args1&&... or @c T2 is not constructible from @c
  /// Args2&&...
  /// @tparam Args1 The types of arguments in the tuple for constructing @c first
  /// @tparam Args2 The types of arguments in the tuple for constructing @c second
  /// @param first_args Tuple containing arguments to construct @c first
  /// @param second_args Tuple containing arguments to construct @c second
  template <
      typename... Args1,
      typename... Args2,
      arene::base::constraints<
          enable_if_t<is_constructible_v<T1, Args1&&...>>,
          enable_if_t<is_constructible_v<T2, Args2&&...>>> = nullptr>
  pair(
      piecewise_construct_t,
      tuple<Args1...> first_args,
      tuple<Args2...> second_args
  ) noexcept(is_nothrow_constructible_v<T1, Args1&&...> && is_nothrow_constructible_v<T2, Args2&&...>)
      : pair{first_args, index_sequence_for<Args1...>{}, second_args, index_sequence_for<Args2...>{}} {}
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Copy-assign a @c pair from another pair with different value types
  ///
  /// This is disabled if either @c T1 or @c T2 is not assignable from the corresponding initializer.
  /// @tparam Arg1 The type of @c first in the source pair
  /// @tparam Arg2 The type of @c second in the source pair
  /// @param other The pair to copy from
  /// @return @c *this
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<!is_same_v<T1, Arg1> || !is_same_v<T2, Arg2>>,
          enable_if_t<is_assignable_v<T1&, Arg1 const&>>,
          enable_if_t<is_assignable_v<T2&, Arg2 const&>>> = nullptr>
  auto operator=(pair<Arg1, Arg2> const& other
  ) noexcept(is_nothrow_assignable_v<T1&, Arg1 const&> && is_nothrow_assignable_v<T2&, Arg2 const&>) -> pair& {
    first = other.first;
    second = other.second;
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: Fields are forwarded"
  // parasoft-begin-suppress AUTOSAR-A18_9_2-a "False positive: Forwarding fields"
  /// @brief Move-assign a @c pair from another pair with different value types
  ///
  /// This is disabled if either @c T1 or @c T2 is not assignable from the corresponding initializer.
  /// @tparam Arg1 The type of @c first in the source pair
  /// @tparam Arg2 The type of @c second in the source pair
  /// @param other The pair to move from
  /// @return @c *this
  template <
      typename Arg1,
      typename Arg2,
      arene::base::constraints<
          enable_if_t<!is_same_v<T1, Arg1> || !is_same_v<T2, Arg2>>,
          enable_if_t<is_assignable_v<T1&, Arg1&&>>,
          enable_if_t<is_assignable_v<T2&, Arg2&&>>> = nullptr>
  auto operator=(pair<Arg1, Arg2>&& other
  ) noexcept(is_nothrow_assignable_v<T1&, Arg1&&> && is_nothrow_assignable_v<T2&, Arg2&&>) -> pair& {
    first = std::forward<Arg1>(other.first);
    second = std::forward<Arg2>(other.second);
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A18_9_2-a
  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a
  // parasoft-end-suppress AUTOSAR-A12_8_4-a

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'swap' does not hide anything"
  /// @brief Swap a @c pair with another instance of the same type. Swaps @c first with @c other.first and swaps @c
  /// second with @c other.second
  ///
  /// This is disabled if either @c T1 or @c T2 is not swappable
  /// @param other The pair to swap with
  template <
      typename CheckType1 = T1,
      typename CheckType2 = T2,
      arene::base::constraints<
          enable_if_t<arene::base::is_swappable_v<CheckType1>>,
          enable_if_t<arene::base::is_swappable_v<CheckType2>>> = nullptr>
  void swap(pair& other
  ) noexcept(arene::base::is_nothrow_swappable_v<CheckType1> && arene::base::is_nothrow_swappable_v<CheckType2>) {
    using std::swap;
    swap(first, other.first);
    swap(second, other.second);
  }
};
// parasoft-end-suppress AUTOSAR-A2_10_1-d

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: Inline function for use in multiple translation units"
/// @brief Swap a @c pair with another instance of the same type. Swaps @c lhs.first with @c rhs.first and swaps @c
/// lhs.second with @c rhs.second
///
/// This is disabled if either @c T1 or @c T2 is not swappable
/// @param lhs The first pair to swap
/// @param rhs The second pair to swap
template <
    typename T1,
    typename T2,
    arene::base::constraints<
        enable_if_t<arene::base::is_swappable_v<T1>>,
        enable_if_t<arene::base::is_swappable_v<T2>>> = nullptr>
void swap(
    pair<T1, T2>& lhs,
    pair<T1, T2>& rhs
) noexcept(arene::base::is_nothrow_swappable_v<T1> && arene::base::is_nothrow_swappable_v<T2>) {
  lhs.swap(rhs);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: Inline function for use in multiple translation units"
/// @brief Compare two pairs to see if the first is less than the second. Uses lexicographical ordering based on the
/// less-than comparisons of the members.
/// @tparam T1 The type of the first data member of the pairs being compared
/// @tparam T2 The type of the second data member of the pairs being compared
/// @param lhs The first pair to compare
/// @param rhs The second pair to compare
/// @return @c true if @c lhs is less than @c rhs, @c false otherwise
/// @pre @c T1 and @c T2 must have less than operators that provide a strict weak ordering
template <typename T1, typename T2>
constexpr auto operator<(
    pair<T1, T2> const& lhs,
    pair<T1, T2> const& rhs
) noexcept(arene::base::is_nothrow_less_than_comparable_v<T1> && arene::base::is_nothrow_less_than_comparable_v<T2>)
    -> bool {
  return (lhs.first < rhs.first) || ((!(rhs.first < lhs.first)) && (lhs.second < rhs.second));
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: Inline function for use in multiple translation units"
/// @brief Compare two pairs to see if the first is greater than the second. Uses lexicographical ordering based on
/// the less-than comparisons of the members.
/// @tparam T1 The type of the first data member of the pairs being compared
/// @tparam T2 The type of the second data member of the pairs being compared
/// @param lhs The first pair to compare
/// @param rhs The second pair to compare
/// @return @c true if @c lhs is greater than @c rhs, @c false otherwise
/// @pre @c T1 and @c T2 must have less than operators that provide a strict weak ordering
template <typename T1, typename T2>
constexpr auto operator>(
    pair<T1, T2> const& lhs,
    pair<T1, T2> const& rhs
) noexcept(arene::base::is_nothrow_less_than_comparable_v<T1> && arene::base::is_nothrow_less_than_comparable_v<T2>)
    -> bool {
  return rhs < lhs;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: Inline function for use in multiple translation units"
/// @brief Compare two pairs to see if the first is less than or equal to the second. Uses lexicographical ordering
/// based on the less-than comparisons of the members.
/// @tparam T1 The type of the first data member of the pairs being compared
/// @tparam T2 The type of the second data member of the pairs being compared
/// @param lhs The first pair to compare
/// @param rhs The second pair to compare
/// @return @c true if @c lhs is less than or equal to @c rhs, @c false otherwise
/// @pre @c T1 and @c T2 must have less than operators that provide a strict weak ordering
template <typename T1, typename T2>
constexpr auto operator<=(
    pair<T1, T2> const& lhs,
    pair<T1, T2> const& rhs
) noexcept(arene::base::is_nothrow_less_than_comparable_v<T1> && arene::base::is_nothrow_less_than_comparable_v<T2>)
    -> bool {
  return !(rhs < lhs);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: Inline function for use in multiple translation units"
/// @brief Compare two pairs to see if the first is greater than or equal to the second. Uses lexicographical ordering
/// based on the less-than comparisons of the members.
/// @tparam T1 The type of the first data member of the pairs being compared
/// @tparam T2 The type of the second data member of the pairs being compared
/// @param lhs The first pair to compare
/// @param rhs The second pair to compare
/// @return @c true if @c lhs is greater than or equal to @c rhs, @c false otherwise
/// @pre @c T1 and @c T2 must have less than operators that provide a strict weak ordering
template <typename T1, typename T2>
constexpr auto operator>=(
    pair<T1, T2> const& lhs,
    pair<T1, T2> const& rhs
) noexcept(arene::base::is_nothrow_less_than_comparable_v<T1> && arene::base::is_nothrow_less_than_comparable_v<T2>)
    -> bool {
  return !(lhs < rhs);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: Inline function for use in multiple translation units"
/// @brief Compare two pairs to see if they are equal, by checking if both members of the first are equal to the
/// corresponding members of the second using the equality comparison operators of the members.
/// @tparam T1 The type of the first data member of the pairs being compared
/// @tparam T2 The type of the second data member of the pairs being compared
/// @param lhs The first pair to compare
/// @param rhs The second pair to compare
/// @return @c true if @c lhs is equal to @c rhs, @c false otherwise
/// @pre @c T1 and @c T2 must have equality operators that provide an equivalency comparison
template <typename T1, typename T2>
constexpr auto operator==(
    pair<T1, T2> const& lhs,
    pair<T1, T2> const& rhs
) noexcept(arene::base::is_nothrow_equality_comparable_v<T1> && arene::base::is_nothrow_equality_comparable_v<T2>)
    -> bool {
  return (lhs.first == rhs.first) && (lhs.second == rhs.second);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Conditionally noexcept based on underlying comparison"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: Inline function for use in multiple translation units"
/// @brief Compare two pairs to see if the first is not equal to the second. Uses the equality comparison operators of
/// the members.
/// @tparam T1 The type of the first data member of the pairs being compared
/// @tparam T2 The type of the second data member of the pairs being compared
/// @param lhs The first pair to compare
/// @param rhs The second pair to compare
/// @return @c true if @c lhs is not equal to @c rhs, @c false otherwise
/// @pre @c T1 and @c T2 must have equality operators that provide an equivalency comparison
template <typename T1, typename T2>
constexpr auto operator!=(
    pair<T1, T2> const& lhs,
    pair<T1, T2> const& rhs
) noexcept(arene::base::is_nothrow_equality_comparable_v<T1> && arene::base::is_nothrow_equality_comparable_v<T2>)
    -> bool {
  return !(lhs == rhs);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a
// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

namespace pair_detail {

/// @brief Internal helper to deduce the expected @c std::pair type
/// @tparam T1 The first argument type given to @c std::make_pair
/// @tparam T2 The second argument type given to @c std::make_pair
template <class T1, class T2>
using deduced_pair_type_t = pair<::arene::base::unwrap_ref_decay_t<T1>, ::arene::base::unwrap_ref_decay_t<T2>>;

}  // namespace pair_detail

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: The parameters are used"
// parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: This function does return a value"
// parasoft-begin-suppress CERT_C-MSC37-a "False positive: This function does return a value"
// parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: This function does return a value"
/// @brief Creates a @c std::pair object, deducing the target type from the types of arguments
/// @tparam T1 The first argument type
/// @tparam T2 The second argument type
/// @param first The first argument
/// @param second The second argument
///
/// Given types @c std::decay<T1>::type as @c U1 and @c std::decay<T2>::type as @c U2, the types @c V1 and @c V2 are
/// defined as follows:
///
/// If @c U1 is @c std::reference_wrapper<X>, @c V1 is @c X&; otherwise @c V1 is @c U1.
/// If @c U2 is @c std::reference_wrapper<Y>, @c V2 is @c Y&; otherwise @c V2 is @c U2.
/// @return <tt> std::pair<V1, V2>(std::forward<T1>(x), std::forward<T2>(y)) </tt>
template <class T1, class T2>
constexpr auto make_pair(
    T1&& first,
    T2&& second
) noexcept(is_nothrow_constructible_v<pair_detail::deduced_pair_type_t<T1, T2>, T1, T2>)
    -> pair_detail::deduced_pair_type_t<T1, T2> {
  return pair_detail::deduced_pair_type_t<T1, T2>(std::forward<T1>(first), std::forward<T2>(second));
}
// parasoft-end-suppress CERT_CPP-MSC52-a
// parasoft-end-suppress CERT_C-MSC37-a
// parasoft-end-suppress AUTOSAR-A8_4_2-a
// parasoft-end-suppress AUTOSAR-A0_1_4-a
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

namespace pair_detail {

// parasoft-begin-suppress AUTOSAR-A7_5_1-a "Temporary objects are handled by an overload that forwards the reference
// category, not the const-ref overload"

/// @brief access an element of a @c pair
/// @tparam Pair specialization of @c pair
/// @param values lvalue or rvalue reference to a @c pair
/// @return reference to the first element in @c values
///
template <class Pair>
constexpr auto get_impl(integral_constant<size_t, 0>, Pair&& values) noexcept
    -> decltype((std::forward<Pair>(values).first)) {
  return std::forward<Pair>(values).first;
}

/// @brief access an element of a @c pair
/// @tparam Pair specialization of @c pair
/// @param values lvalue or rvalue reference to a @c pair
/// @return reference to the second element in @c values
///
template <class Pair>
constexpr auto get_impl(integral_constant<size_t, 1>, Pair&& values) noexcept
    -> decltype((std::forward<Pair>(values).second)) {
  return std::forward<Pair>(values).second;
}

// parasoft-end-suppress AUTOSAR-A7_5_1-a

}  // namespace pair_detail

/// @brief access an element of a @c pair
/// @tparam I index of the element to access
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> I >= 2 </tt>
template <size_t I, class T1, class T2>
constexpr auto get(pair<T1, T2>& values) noexcept -> tuple_element_t<I, pair<T1, T2>>& {
  return pair_detail::get_impl(integral_constant<size_t, I>{}, values);
}

/// @brief access an element of a @c pair
/// @tparam I index of the element to access
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> I >= 2 </tt>
template <size_t I, class T1, class T2>
constexpr auto get(pair<T1, T2> const& values) noexcept -> tuple_element_t<I, pair<T1, T2>> const& {
  return pair_detail::get_impl(integral_constant<size_t, I>{}, values);
}

/// @brief access an element of a @c pair
/// @tparam I index of the element to access
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> I >= 2 </tt>
template <size_t I, class T1, class T2>
constexpr auto get(pair<T1, T2>&& values) noexcept -> tuple_element_t<I, pair<T1, T2>>&& {
  return pair_detail::get_impl(integral_constant<size_t, I>{}, std::move(values));
}

// parasoft-begin-suppress AUTOSAR-A18_9_3-a "We want to ensure the qualifier is preserved"
/// @brief access an element of a @c pair
/// @tparam I index of the element to access
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> I >= 2 </tt>
template <size_t I, class T1, class T2>
constexpr auto get(pair<T1, T2> const&& values) noexcept -> tuple_element_t<I, pair<T1, T2>> const&& {
  return pair_detail::get_impl(integral_constant<size_t, I>{}, std::move(values));
}
// parasoft-end-suppress AUTOSAR-A18_9_3-a

/// @brief access an element of a @c pair
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T1, class T2>
constexpr auto get(pair<T1, T2>& values) noexcept -> T1& {
  return values.first;
}

// parasoft-begin-suppress AUTOSAR-A7_5_1-a "Temporary objects are handled by an overload that forwards the reference
// category, not the const-ref overload"

/// @brief access an element of a @c pair
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T1, class T2>
constexpr auto get(pair<T1, T2> const& values) noexcept -> T1 const& {
  return values.first;
}

// parasoft-end-suppress AUTOSAR-A7_5_1-a

/// @brief access an element of a @c pair
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T1, class T2>
constexpr auto get(pair<T1, T2>&& values) noexcept -> T1&& {
  return std::move(values.first);
}

// parasoft-begin-suppress AUTOSAR-A18_9_3-a "We want to ensure the qualifier is preserved"
/// @brief access an element of a @c pair
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T1, class T2>
constexpr auto get(pair<T1, T2> const&& values) noexcept -> T1 const&& {
  return std::move(values.first);
}
// parasoft-end-suppress AUTOSAR-A18_9_3-a

/// @brief access an element of a @c pair
/// @tparam T1 second pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T2, class T1>
constexpr auto get(pair<T1, T2>& values) noexcept -> T2& {
  return values.second;
}

// parasoft-begin-suppress AUTOSAR-A7_5_1-a "Temporary objects are handled by an overload that forwards the reference
// category, not the const-ref overload"

/// @brief access an element of a @c pair
/// @tparam T1 second pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T2, class T1>
constexpr auto get(pair<T1, T2> const& values) noexcept -> T2 const& {
  return values.second;
}

// parasoft-end-suppress AUTOSAR-A7_5_1-a

/// @brief access an element of a @c pair
/// @tparam T1 second pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T2, class T1>
constexpr auto get(pair<T1, T2>&& values) noexcept -> T2&& {
  return std::move(values.second);
}

// parasoft-begin-suppress AUTOSAR-A18_9_3-a "We want to ensure the qualifier is preserved"
/// @brief access an element of a @c pair
/// @tparam T1 second pair type
/// @tparam T2 second pair type
/// @param values reference to a @c pair
/// @return reference to a value in @c values
/// @note ill-formed if <tt> std::is_same<T1, T2>::value </tt> is <tt> true </tt>
template <class T2, class T1>
constexpr auto get(pair<T1, T2> const&& values) noexcept -> T2 const&& {
  return std::move(values.second);
}
// parasoft-end-suppress AUTOSAR-A18_9_3-a

// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief obtain the first type of a @c pair
/// @tparam T1 first pair type
/// @tparam T2 second pair type
template <class T1, class T2>
class tuple_element<0, pair<T1, T2>> {
 public:
  /// @brief type at index 0
  using type = T1;
};

/// @brief obtain the second type of a @c pair
/// @tparam T1 first pair type
/// @tparam T2 second pair type
template <class T1, class T2>
class tuple_element<1, pair<T1, T2>> {
 public:
  /// @brief type at index 1
  using type = T2;
};

/// @brief obtain the size of a @c pair
/// @tparam T1 first pair type
/// @tparam T2 second pair type
/// @note This is always 2
template <class T1, class T2>
class tuple_size<pair<T1, T2>> : public integral_constant<size_t, 2> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_PAIR_HPP_
