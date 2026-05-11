// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test `remove_reference<T>::type` is `T` when the argument is not a reference type, or if the argument is `T&` or
/// `T&&`. `const`-ness of `T` is preserved.
TEST(RemoveRef, BasicChecks) {
  struct x {};
  static_assert(std::is_same<std::remove_reference<int>::type, int>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference<int const>::type, int const>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference<x>::type, x>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference<x const>::type, x const>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference<int&>::type, int>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference<int const&>::type, int const>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference<x&>::type, x>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference<x const&>::type, x const>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference<int&&>::type, int>::value, "Rvalue reference");
  static_assert(std::is_same<std::remove_reference<int const&&>::type, int const>::value, "Rvalue reference");
  static_assert(std::is_same<std::remove_reference<x&&>::type, x>::value, "Rvalue reference");
  static_assert(std::is_same<std::remove_reference<x const&&>::type, x const>::value, "Rvalue reference");
}

/// @test `remove_reference_t` is `T` when the argument is not a reference type, or if the argument is `T&` or `T&&`.
/// `const`-ness of `T` is preserved.
TEST(RemoveRefT, BasicChecks) {
  struct x {};
  static_assert(std::is_same<std::remove_reference_t<int>, int>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference_t<int const>, int const>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference_t<x>, x>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference_t<x const>, x const>::value, "Non-reference");
  static_assert(std::is_same<std::remove_reference_t<int&>, int>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference_t<int const&>, int const>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference_t<x&>, x>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference_t<x const&>, x const>::value, "Lvalue reference");
  static_assert(std::is_same<std::remove_reference_t<int&&>, int>::value, "Rvalue reference");
  static_assert(std::is_same<std::remove_reference_t<int const&&>, int const>::value, "Rvalue reference");
  static_assert(std::is_same<std::remove_reference_t<x&&>, x>::value, "Rvalue reference");
  static_assert(std::is_same<std::remove_reference_t<x const&&>, x const>::value, "Rvalue reference");
}

}  // namespace
