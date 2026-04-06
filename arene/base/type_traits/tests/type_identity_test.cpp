// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/type_identity.hpp"

#include <type_traits>

#include <gtest/gtest.h>

namespace {
struct some_user_defined_type {};

/// @test `type_identity_t<T>` is just `T`
TEST(TypeIdentityTest, TypeIsTheSameAsParameter) {
  static_assert(std::is_same<int, arene::base::type_identity<int>::type>::value, "");
  static_assert(std::is_same<void, arene::base::type_identity<void>::type>::value, "");
  static_assert(
      std::is_same<some_user_defined_type, arene::base::type_identity<some_user_defined_type>::type>::value,
      ""
  );

  static_assert(std::is_same<int, arene::base::type_identity_t<int>>::value, "");
  static_assert(std::is_same<void*, arene::base::type_identity_t<void*>>::value, "");
  static_assert(std::is_same<some_user_defined_type, arene::base::type_identity_t<some_user_defined_type>>::value, "");
  static_assert(
      std::is_same<some_user_defined_type const, arene::base::type_identity_t<some_user_defined_type const>>::value,
      ""
  );
}

namespace {

template <typename T>
auto bit_and(T value, arene::base::type_identity_t<T> mask) -> T {
  static_assert(std::is_same<decltype(mask), T>::value, "");
  return value & mask;
}

}  // namespace

/// @test `type_identity_t` can be used to provide a non-deduced context for an argument to a function template, so the
/// type of a template parameter is not deduced from that argument
TEST(TypeIdentityTest, CallNotAmbiguousWhenPassedDifferentTypes) {
  // Pass different types. This would be ambiguous if both parameters
  // are deducable as T. But because we used type_identity_t<T> the second
  // argument is not deducable and so T will deduce to the type of the
  // first argument.
  auto result = bit_and(37U, 7);
  static_assert(std::is_same<decltype(result), decltype(37U)>::value, "");
  ASSERT_EQ(result, 5);
}

}  // namespace
