// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_UNIQUE_TEST_VALUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_UNIQUE_TEST_VALUE_HPP_

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/inline_container/testing/customization.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/terminate.hpp"
#include "arene/base/type_info/type_name_string.hpp"  // IWYU pragma: keep
#include "arene/base/type_traits/comparison_traits.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <iostream>
#endif

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "exact equality is intended for floating point as we are testing uniqueness");

namespace arene {
namespace base {
namespace testing {
namespace detail {

/// @brief memoized check that test values are unique
/// @tparam T the type to return the test values of
/// @tparam N the index of the last test value to check
///
template <class T, std::size_t N, class = void>
struct assert_test_values_are_unique;

/// @brief specifies if unique checks are @c noexcept
/// @tparam T the type to return the test values of
///
template <class T, class = void>
constexpr extern bool is_test_value_noexcept_usable_v = false;

/// @brief specifies if unique checks are @c noexcept
/// @tparam T the type to return the test values of
///
/// Specialization if @c test_value<T> is defined and equality comparable.
///
template <class T>
constexpr extern bool is_test_value_noexcept_usable_v<  //
    T,
    std::enable_if_t<is_equality_comparable_v<decltype(test_value<T>(0U))>>> =
    noexcept(test_value<T>(0U) == test_value<T>(0U));

/// @brief no-op function object
///
struct no_op {
  /// @brief does nothing
  ///
  /// Used as the base case for checking that test values are unique.
  ///
  constexpr auto operator()() const noexcept -> void {}
};

/// @brief checks if values are unique
/// @return @c true if values are unique, otherwise @c false
///
/// Determines if <tt> test_value<T>(0), ..., test_value<T>(n) </tt> are unique. That is,
/// * <tt> test_value<T>(i) == test_value<T>(j) </tt> is @c <tt> i == j </tt>, for <tt> i, j </tt> in <tt>[0, ...,
/// N]</tt>
///
template <class T, std::size_t N>
constexpr auto are_test_values_unique() noexcept(is_test_value_noexcept_usable_v<T>) -> bool {
  using assert_unique_for_smaller_n = std::conditional_t<  //
      N == 0U,
      no_op,
      assert_test_values_are_unique<T, N - 1U>>;
  assert_unique_for_smaller_n{}();

  auto const self_equal = test_value<T>(N) == test_value<T>(N);

  auto equal_to_any_previous = false;
  for (auto i = std::size_t{}; i < N; ++i) {
    if (test_value<T>(i) == test_value<T>(N)) {
      equal_to_any_previous = true;
      break;
    }
  }

  return self_equal && !equal_to_any_previous;
  ;
}

/// @brief memoized check that test values are unique
/// @tparam T the type to return the test values of
/// @tparam N the index of the last test value to check
///
/// Specialization when uniqueness cannot be determined at compile-time.
///
template <class T, std::size_t N, class>
struct assert_test_values_are_unique  //
{
  /// @brief checks if values are unique
  /// @return @c true if values are unique
  ///
  /// If <tt> test_value<T>(0), ..., test_value<T>(n) </tt> are not unique,
  /// displays a message to @c std::cerr and calls @c std::terminate.
  ///
  auto operator()() const noexcept(is_test_value_noexcept_usable_v<T>) -> void  //
  {
    static auto const unique = [] {
      if (!are_test_values_unique<T, N>()) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
        std::cerr                                           //
            << "ERROR: test requested unique values but "   //
            << "'arene::base::testing::test_values<"        //
            << arene::base::type_name_v<T>.c_str() << ">("  //
            << N                                            //
            << ")'"                                         //
            << " duplicates an earlier test value\n";
#endif
        std::terminate();
      }

      return nullptr;
    }();

    std::ignore = unique;
  }
};

/// @brief memoized check that test values are unique
/// @tparam T the type to return the test values of
/// @tparam N the index of the last test value to check
///
/// Specialization when uniqueness can be determined at compile-time.
///
template <class T, std::size_t N>
struct assert_test_values_are_unique<
    T,
    N,
    std::enable_if_t<(std::ignore = (test_value<T>(0U) == test_value<T>(0U)), true)>>  //
{
  /// @brief determines if values are unique
  static constexpr bool value = are_test_values_unique<T, N>();

  /// @brief checks if values are unique
  /// @return @c true if values are unique
  ///
  /// If <tt> test_value<T>(0), ..., test_value<T>(n) </tt> are not unique,
  /// triggers a @c static_assert
  ///
  constexpr auto operator()() const noexcept -> void  //
  {
    static_assert(value, "test requested N unique values but 'arene::base::testing::test_values' produces duplicates");
  }
};

}  // namespace detail

/// @brief obtain the nth unique @c arene::base::testing::test_value
/// @tparam T the type to return the test values of
/// @tparam N the index of the test value to obtain
/// @return @c arene::base::testing::test_value<T>(n)
///
/// Obtains @c test_value<T>(n) and ensures that the values
/// <tt> test_value<T>(0), ..., test_value<T>(n) </tt> are unique.
///
/// If the values are not unique and this can be determined at compile-time,
/// this function fails to compile.
///
/// If the values are not unique and this cannot be determined at compile-time,
/// a call to this function calls @c std::terminate.
///
/// @note Requires `test_value<T>` to be regular invocable
///
template <class T, std::size_t N>
constexpr auto unique_test_value() noexcept(detail::is_test_value_noexcept_usable_v<T>) -> decltype(test_value<T>(N)) {
  static_assert(
      is_equality_comparable_v<decltype(test_value<T>(N))>,
      "if tests request a unique test value, the test value type must provide an equality operator"
  );
  detail::assert_test_values_are_unique<T, N>{}();
  return test_value<T>(N);
}

}  // namespace testing
}  // namespace base
}  // namespace arene

ARENE_IGNORE_END();

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_UNIQUE_TEST_VALUE_HPP_
