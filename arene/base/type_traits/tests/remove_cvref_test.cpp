// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/remove_cvref.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `remove_reference_t` is `T` when the argument is not a reference type, or if the argument is `T&` or `T&&`.
/// `const`-ness of `T` is preserved.
TEST(RemoveRefT, BasicChecks) {
  struct x {};
  static_assert(std::is_same<arene::base::remove_reference_t<int>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_reference_t<int const>, int const>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_reference_t<x>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_reference_t<x const>, x const>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_reference_t<int&>, int>::value, "Lvalue reference");
  static_assert(std::is_same<arene::base::remove_reference_t<int const&>, int const>::value, "Lvalue reference");
  static_assert(std::is_same<arene::base::remove_reference_t<x&>, x>::value, "Lvalue reference");
  static_assert(std::is_same<arene::base::remove_reference_t<x const&>, x const>::value, "Lvalue reference");
  static_assert(std::is_same<arene::base::remove_reference_t<int&&>, int>::value, "Rvalue reference");
  static_assert(std::is_same<arene::base::remove_reference_t<int const&&>, int const>::value, "Rvalue reference");
  static_assert(std::is_same<arene::base::remove_reference_t<x&&>, x>::value, "Rvalue reference");
  static_assert(std::is_same<arene::base::remove_reference_t<x const&&>, x const>::value, "Rvalue reference");
}

/// @test `remove_cv_t` is `T` when the argument `T`, `const T`, `volatile T` or `const volatile T`. References are
/// unaffected, so `remove_cv_t<const T&>` is `const T&`
TEST(RemoveCvT, BasicChecks) {
  struct x {};
  static_assert(std::is_same<arene::base::remove_cv_t<int>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<int const>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<int const volatile>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<int volatile>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<x>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<x const>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<x const volatile>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<x volatile>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cv_t<int&>, int&>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cv_t<int const&>, int const&>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cv_t<x&>, x&>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cv_t<x const&>, x const&>::value, "Reference");
}

/// @test `remove_cvref<T>::type` is the same as `remove_cv_t<remove_reference_t<T>>`
TEST(RemoveCvRef, BasicChecks) {
  struct x {};
  static_assert(std::is_same<arene::base::remove_cvref<int>::type, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<int const>::type, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<int const volatile>::type, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<int volatile>::type, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<x>::type, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<x const>::type, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<x const volatile>::type, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<x volatile>::type, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref<int&>::type, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref<int const&>::type, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref<int const volatile&>::type, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref<int volatile&>::type, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref<x&>::type, x>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref<x const&>::type, x>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref<x const volatile&>::type, x>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref<x volatile&>::type, x>::value, "Reference");
}

/// @test `remove_cvref_t<T>` is the same as `remove_cv_t<remove_reference_t<T>>`
TEST(RemoveCvRefT, BasicChecks) {
  struct x {};
  static_assert(std::is_same<arene::base::remove_cvref_t<int>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<int const>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<int const volatile>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<int volatile>, int>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x const>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x const volatile>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x volatile>, x>::value, "Non-reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<int&>, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<int const&>, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<int const volatile&>, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<int volatile&>, int>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x&>, x>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x const&>, x>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x const volatile&>, x>::value, "Reference");
  static_assert(std::is_same<arene::base::remove_cvref_t<x volatile&>, x>::value, "Reference");
}

}  // namespace
