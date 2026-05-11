// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @brief Assert @c remove_const<Input>::type equals the @c ExpectedResult
/// @tparam Input The input type to remove const from
/// @tparam ExpectedResult The expect result of removing const from the @c Input type
template <typename Input, typename ExpectedResult>
constexpr auto static_assert_remove_const() noexcept -> void {
  testing::StaticAssertTypeEq<typename std::remove_const<Input>::type, ExpectedResult>();
  testing::StaticAssertTypeEq<std::remove_const_t<Input>, ExpectedResult>();
}

/// @test `remove_const::type` is `T` when the argument is `T` or `const T`. References
/// are unaffected, so `remove_const<const T&>::type` is `const T&`. Only the outer qualifier of pointers is affected,
/// so `remove_const<T const * const>::type` is `T const *`.
TEST(RemoveConst, BasicChecks) {
  // Value types
  static_assert_remove_const<int, int>();
  static_assert_remove_const<int const, int>();
  static_assert_remove_const<int const volatile, int volatile>();
  static_assert_remove_const<int volatile, int volatile>();

  // User-defined value types
  struct x {};
  static_assert_remove_const<x, x>();
  static_assert_remove_const<x const, x>();
  static_assert_remove_const<x const volatile, x volatile>();
  static_assert_remove_const<x volatile, x volatile>();

  // l-value reference types
  static_assert_remove_const<int&, int&>();
  static_assert_remove_const<int const&, int const&>();
  static_assert_remove_const<x&, x&>();
  static_assert_remove_const<x const&, x const&>();

  // r-value reference types
  static_assert_remove_const<int&&, int&&>();
  static_assert_remove_const<int const&&, int const&&>();
  static_assert_remove_const<x&&, x&&>();
  static_assert_remove_const<x const&&, x const&&>();

  // const pointer types
  static_assert_remove_const<int*, int*>();
  static_assert_remove_const<int const*, int const*>();
  static_assert_remove_const<int* const, int*>();
  static_assert_remove_const<int const* const, int const*>();
  static_assert_remove_const<x*, x*>();
  static_assert_remove_const<x const*, x const*>();
  static_assert_remove_const<x* const, x*>();
  static_assert_remove_const<x const* const, x const*>();

  // volatile pointer types
  static_assert_remove_const<int*, int*>();
  static_assert_remove_const<int volatile*, int volatile*>();
  static_assert_remove_const<int* volatile, int* volatile>();
  static_assert_remove_const<int volatile* volatile, int volatile* volatile>();
  static_assert_remove_const<x*, x*>();
  static_assert_remove_const<x volatile*, x volatile*>();
  static_assert_remove_const<x* volatile, x* volatile>();
  static_assert_remove_const<x volatile* volatile, x volatile* volatile>();

  // const-volatile pointer types
  static_assert_remove_const<int*, int*>();
  static_assert_remove_const<int const volatile*, int const volatile*>();
  static_assert_remove_const<int* const volatile, int* volatile>();
  static_assert_remove_const<int const volatile* const volatile, int const volatile* volatile>();
  static_assert_remove_const<x*, x*>();
  static_assert_remove_const<x const volatile*, x const volatile*>();
  static_assert_remove_const<x* const volatile, x* volatile>();
  static_assert_remove_const<x const volatile* const volatile, x const volatile* volatile>();
}

}  // namespace
