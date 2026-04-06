// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_DEFAULT_CONSTRUCTIBLE_PROPERTIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_DEFAULT_CONSTRUCTIBLE_PROPERTIES_HPP_

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/terminate.hpp"
#include "arene/base/type_info/type_name_string.hpp"  // IWYU pragma: keep
#include "testlibs/utilities/constexpr_traits.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <iostream>
#endif

namespace arene {
namespace base {
namespace testing {
namespace detail {

/// @brief Assert that two default constructed instances are equal
/// @tparam T The type to check
///
/// The default implementation for types that are not default constructible does not assert.
template <class T, arene::base::constraints<std::enable_if_t<!std::is_default_constructible<T>::value>> = nullptr>
constexpr auto assert_default_constructed_instances_are_equal() -> void {}

/// @brief Assert that two default constructed instances are equal
/// @tparam T The type to check
///
/// For types that are not default constructible in a constant expression, the equality is checked once at runtime then
/// cached.
template <
    class T,
    arene::base::constraints<
        std::enable_if_t<std::is_default_constructible<T>::value>,
        std::enable_if_t<!::testing::is_constexpr_default_constructible_v<T>>> = nullptr>
auto assert_default_constructed_instances_are_equal() -> void {
  if (!(T{} == T{})) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    std::cerr << "ERROR: test provided a default constructible value(" << arene::base::type_name_v<T>.c_str() << ")"
              << " but two default constructed instances do not compare equal\n";
#endif
    std::terminate();
  }
}

/// @brief Assert that two default constructed instances are equal
/// @tparam T The type to check
///
/// For types that are default constructible in a constant expression, the equality is checked in a static assertion.
template <
    class T,
    arene::base::constraints<
        std::enable_if_t<std::is_default_constructible<T>::value>,
        std::enable_if_t<::testing::is_constexpr_default_constructible_v<T>>> = nullptr>
constexpr auto assert_default_constructed_instances_are_equal() -> void {
  // Note: If the condition is directly placed in the static_assert, gcc8 will always execute it, even when this
  // overload is not chosen. Putting the conditional result in a variable avoids this issue.
  constexpr bool are_equal = T{} == T{};
  static_assert(
      are_equal,
      "test provided a default constructible value, but two default constructed instances do not compare equal"
  );
}

/// @brief Assert that a copy of a default constructed instance equals the original
/// @tparam T The type to check
///
/// The default implementation for types that are not default constructible or copy constructible does not assert.
template <
    class T,
    arene::base::constraints<
        std::enable_if_t<!std::is_default_constructible<T>::value || !std::is_copy_constructible<T>::value>> = nullptr>
constexpr auto assert_copied_value_equals_original() -> void {}

/// @brief Assert that a copy of a default constructed instance equals the original
/// @tparam T The type to check
///
/// For types that are not default constructible or copy constructible in a constant expression, the equality is checked
/// once at runtime and cached.
template <
    class T,
    arene::base::constraints<
        std::enable_if_t<std::is_default_constructible<T>::value>,
        std::enable_if_t<std::is_copy_constructible<T>::value>,
        std::enable_if_t<!::testing::is_constexpr_default_and_copy_constructible_v<T>>> = nullptr>
auto assert_copied_value_equals_original() -> void {
  auto const original = T{};
  auto const copy = original;
  if (!(original == copy)) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    std::cerr << "ERROR: test provided a default constructible value(" << arene::base::type_name_v<T>.c_str() << ")"
              << " but a copy of the default constructed instance does not compare equal to the original\n";
#endif
    std::terminate();
  }
}

/// @brief Assert that a copy of a default constructed instance equals the original
/// @tparam T The type to check
///
/// For types that are default constructible and copy constructible in a constant expression, the equality is checked in
/// a static assertion.
template <
    class T,
    arene::base::constraints<
        std::enable_if_t<std::is_default_constructible<T>::value>,
        std::enable_if_t<std::is_copy_constructible<T>::value>,
        std::enable_if_t<::testing::is_constexpr_default_and_copy_constructible_v<T>>> = nullptr>
constexpr auto assert_copied_value_equals_original() -> void {
  constexpr auto original = T{};
  constexpr auto copy = original;

  // Note: If the condition is directly placed in the static_assert, gcc8 will always execute it, even when this
  // overload is not chosen. Putting the conditional result in a variable avoids this issue.
  constexpr auto are_equal = original == copy;
  static_assert(
      are_equal,
      "test provided a default constructible value, but a copy of the default constructed instance does not compare "
      "equal to the original"
  );
}
}  // namespace detail

/// @brief Assert default constructible types have the expected properties
/// @tparam T The type to check
///
/// For types that are default constructible, two default constructed instances must compare equal.
/// For types that are also copy constructible, a copy of a default constructed instance must compare equal to the
/// original.
template <class T>
constexpr auto assert_default_constructible_properties() -> void {
  detail::assert_default_constructed_instances_are_equal<T>();
  detail::assert_copied_value_equals_original<T>();
}

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_TESTING_DEFAULT_CONSTRUCTIBLE_PROPERTIES_HPP_
