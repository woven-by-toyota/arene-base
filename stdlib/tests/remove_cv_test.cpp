// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @brief Assert @c remove_cv<Input>::type equals the @c ExpectedResult
/// @tparam Input The input type to remove const from
/// @tparam ExpectedResult The expect result of removing const and volatile from the @c Input type
template <typename Input, typename ExpectedResult>
constexpr auto static_assert_remove_cv() noexcept -> void {
  testing::StaticAssertTypeEq<typename std::remove_cv<Input>::type, ExpectedResult>();
  testing::StaticAssertTypeEq<std::remove_cv_t<Input>, ExpectedResult>();

  testing::StaticAssertTypeEq<std::remove_cv_t<Input>, std::remove_const_t<std::remove_volatile_t<Input>>>();
  testing::StaticAssertTypeEq<std::remove_cv_t<Input>, std::remove_volatile_t<std::remove_const_t<Input>>>();
}

/// @test `remove_cv::type` is `T` when the argument is `T`, `const T`, `volatile T` or `const volatile T`. References
/// are unaffected, so `remove_cv<const T&>::type` is `const T&`. Only the outer qualifier of pointers is affected, so
/// `remove_cv<T const * const>::type` is `T const *`.
TEST(RemoveCV, BasicChecks) {
  // Value types
  static_assert_remove_cv<int, int>();
  static_assert_remove_cv<int const, int>();
  static_assert_remove_cv<int volatile, int>();
  static_assert_remove_cv<int const volatile, int>();

  // User-defined value types
  struct x {};
  static_assert_remove_cv<x, x>();
  static_assert_remove_cv<x const, x>();
  static_assert_remove_cv<x volatile, x>();
  static_assert_remove_cv<x const volatile, x>();

  // l-value reference types
  static_assert_remove_cv<int&, int&>();
  static_assert_remove_cv<int const&, int const&>();
  static_assert_remove_cv<int volatile&, int volatile&>();
  static_assert_remove_cv<int const volatile&, int const volatile&>();
  static_assert_remove_cv<x&, x&>();
  static_assert_remove_cv<x const&, x const&>();
  static_assert_remove_cv<x volatile&, x volatile&>();
  static_assert_remove_cv<x const volatile&, x const volatile&>();

  // r-value reference types
  static_assert_remove_cv<int&&, int&&>();
  static_assert_remove_cv<int const&&, int const&&>();
  static_assert_remove_cv<int volatile&&, int volatile&&>();
  static_assert_remove_cv<int const volatile&&, int const volatile&&>();
  static_assert_remove_cv<x&&, x&&>();
  static_assert_remove_cv<x const&&, x const&&>();
  static_assert_remove_cv<x volatile&&, x volatile&&>();
  static_assert_remove_cv<x const volatile&&, x const volatile&&>();

  // const pointer types
  static_assert_remove_cv<int*, int*>();
  static_assert_remove_cv<int const*, int const*>();
  static_assert_remove_cv<int* const, int*>();
  static_assert_remove_cv<int const* const, int const*>();
  static_assert_remove_cv<x*, x*>();
  static_assert_remove_cv<x const*, x const*>();
  static_assert_remove_cv<x* const, x*>();
  static_assert_remove_cv<x const* const, x const*>();

  // volatile pointer types
  static_assert_remove_cv<int*, int*>();
  static_assert_remove_cv<int volatile*, int volatile*>();
  static_assert_remove_cv<int* volatile, int*>();
  static_assert_remove_cv<int volatile* volatile, int volatile*>();
  static_assert_remove_cv<x*, x*>();
  static_assert_remove_cv<x volatile*, x volatile*>();
  static_assert_remove_cv<x* volatile, x*>();
  static_assert_remove_cv<x volatile* volatile, x volatile*>();

  // const-volatile pointer types
  static_assert_remove_cv<int*, int*>();
  static_assert_remove_cv<int const volatile*, int const volatile*>();
  static_assert_remove_cv<int* const volatile, int*>();
  static_assert_remove_cv<int const volatile* const volatile, int const volatile*>();
  static_assert_remove_cv<x*, x*>();
  static_assert_remove_cv<x const volatile*, x const volatile*>();
  static_assert_remove_cv<x* const volatile, x*>();
  static_assert_remove_cv<x const volatile* const volatile, x const volatile*>();
}

}  // namespace
