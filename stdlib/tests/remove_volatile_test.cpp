// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @brief Assert @c remove_volatile<Input>::type equals the @c ExpectedResult
/// @tparam Input The input type to remove volatile from
/// @tparam ExpectedResult The expect result of removing volatile from the @c Input type
template <typename Input, typename ExpectedResult>
constexpr auto static_assert_remove_volatile() noexcept -> void {
  testing::StaticAssertTypeEq<typename std::remove_volatile<Input>::type, ExpectedResult>();
  testing::StaticAssertTypeEq<std::remove_volatile_t<Input>, ExpectedResult>();
}

/// @test `remove_volatile::type` is `T` when the argument is `T`, or `volatile T`. References
/// are unaffected, so `remove_volatile<volatile T&>::type` is `volatile T&`. Only the outer qualifier of pointers is
/// affected, so `remove_volatile<T volatile * volatile>::type` is `T volatile *`.
TEST(RemoveVolatile, BasicChecks) {
  // Value types
  static_assert_remove_volatile<int, int>();
  static_assert_remove_volatile<int const, int const>();
  static_assert_remove_volatile<int const volatile, int const>();
  static_assert_remove_volatile<int volatile, int>();

  // User-defined value types
  struct x {};
  static_assert_remove_volatile<x, x>();
  static_assert_remove_volatile<x const, x const>();
  static_assert_remove_volatile<x const volatile, x const>();
  static_assert_remove_volatile<x volatile, x>();

  // l-value reference types
  static_assert_remove_volatile<int&, int&>();
  static_assert_remove_volatile<int volatile&, int volatile&>();
  static_assert_remove_volatile<x&, x&>();
  static_assert_remove_volatile<x volatile&, x volatile&>();

  // r-value reference types
  static_assert_remove_volatile<int&&, int&&>();
  static_assert_remove_volatile<int volatile&&, int volatile&&>();
  static_assert_remove_volatile<x&&, x&&>();
  static_assert_remove_volatile<x volatile&&, x volatile&&>();

  // const pointer types
  static_assert_remove_volatile<int*, int*>();
  static_assert_remove_volatile<int const*, int const*>();
  static_assert_remove_volatile<int* const, int* const>();
  static_assert_remove_volatile<int const* const, int const* const>();
  static_assert_remove_volatile<x*, x*>();
  static_assert_remove_volatile<x const*, x const*>();
  static_assert_remove_volatile<x* const, x* const>();
  static_assert_remove_volatile<x const* const, x const* const>();

  // volatile pointer types
  static_assert_remove_volatile<int*, int*>();
  static_assert_remove_volatile<int volatile*, int volatile*>();
  static_assert_remove_volatile<int* volatile, int*>();
  static_assert_remove_volatile<int volatile* volatile, int volatile*>();
  static_assert_remove_volatile<x*, x*>();
  static_assert_remove_volatile<x volatile*, x volatile*>();
  static_assert_remove_volatile<x* volatile, x*>();
  static_assert_remove_volatile<x volatile* volatile, x volatile*>();

  // const-volatile pointer types
  static_assert_remove_volatile<int*, int*>();
  static_assert_remove_volatile<int const volatile*, int const volatile*>();
  static_assert_remove_volatile<int* const volatile, int* const>();
  static_assert_remove_volatile<int const volatile* const volatile, int const volatile* const>();
  static_assert_remove_volatile<x*, x*>();
  static_assert_remove_volatile<x const volatile*, x const volatile*>();
  static_assert_remove_volatile<x* const volatile, x* const>();
  static_assert_remove_volatile<x const volatile* const volatile, x const volatile* const>();
}

}  // namespace
