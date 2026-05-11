// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/as_const.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_volatile.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::substitution_succeeds;

struct example {
  bool value;
};

template <typename T>
class AsConstTest : public ::testing::Test {};

using types = ::testing::Types<int, std::uint32_t, example, int*, int const*, int volatile, int const volatile>;

TYPED_TEST_SUITE(AsConstTest, types, );

// trait to deduce the return type of calling arene::base::as_const(T)
template <typename T>
using as_const_returns_t = decltype(arene::base::as_const(std::declval<T>()));

/// @test @c arene::base::as_const<T&&>(value) is not a valid instantiation and does not participate in overload
/// resolution.
TYPED_TEST(AsConstTest, CannotBeInvokedOnRvalues) {
  STATIC_ASSERT_FALSE((substitution_succeeds<as_const_returns_t, TypeParam&&>));
}
/// @test @c arene::base::as_const<T&>(value) 's return type is <c>const T&</c>.
TYPED_TEST(AsConstTest, ReturnsConstQualifiedReference) {
  ::testing::StaticAssertTypeEq<as_const_returns_t<TypeParam&>, std::add_const_t<TypeParam>&>();
}
/// @test @c arene::base::as_const<T&>(value) maintains the @c volatile qualification of @c T .
TYPED_TEST(AsConstTest, MaintainsVolatileQualificationOfInput) {
  STATIC_ASSERT_EQ(
      std::is_volatile<TypeParam>::value,
      std::is_volatile<std::remove_reference_t<as_const_returns_t<TypeParam&>>>::value
  );
}
/// @test @c arene::base::as_const<T&>(value) returns a reference to @c value .
TYPED_TEST(AsConstTest, ReturnsTheInputValue) {
  TypeParam value{};
  ASSERT_EQ(&arene::base::as_const(value), &value);
}

}  // namespace
