// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/constraints/substitution_succeeds.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

namespace {

using arene::base::substitution_succeeds;

template <class T, class U>
constexpr bool is_same_v = std::is_same<T, U>::value;

// Types for testing
struct a;
struct b;

// Only succeeds if given no arguments
template <class... P>
using require_no_types = std::enable_if_t<sizeof...(P) == 0>;

// Only succeeds if given an "A"
template <class T>
using require_a = std::enable_if_t<is_same_v<T, a>>;

// Only succeeds if given an "A" and A "B", in that order
template <class T, class U>
using require_a_b = std::enable_if_t<is_same_v<T, a> && is_same_v<U, b>>;

// Only succeeds if "T" has a type member "Foo"
template <class T>
using require_member_foo = typename T::foo;

// Only succeeds if "T" and "U" are valid operands to binary operator +
template <class T, class U>
using require_addable = decltype(std::declval<T>() + std::declval<U>());

/// @test If `substitution_succeeds` is given a template that can only be instantiated without any template parameters,
/// then it only evaluates to `true` if there are no additional parameters to `substitution_succeeds`
TEST(SubstitutionSucceeds, NullarySourceTemplate) {
  EXPECT_TRUE(substitution_succeeds<require_no_types>);
  EXPECT_FALSE((substitution_succeeds<require_no_types, a>));
  EXPECT_FALSE((substitution_succeeds<require_no_types, b>));
  EXPECT_FALSE((substitution_succeeds<require_no_types, a, b>));
  EXPECT_FALSE((substitution_succeeds<require_no_types, b, a>));
}

/// @test If `substitution_succeeds` is given a template that can only be instantiated with one template parameter,
/// then it only evaluates to `true` if there is one additional parameter to `substitution_succeeds`, and that parameter
/// meets the constraints of the template.
TEST(SubstitutionSucceeds, UnarySourceTemplate) {
  EXPECT_FALSE(substitution_succeeds<require_a>);
  EXPECT_TRUE((substitution_succeeds<require_a, a>));
  EXPECT_FALSE((substitution_succeeds<require_a, b>));
  EXPECT_FALSE((substitution_succeeds<require_a, a, b>));
  EXPECT_FALSE((substitution_succeeds<require_a, b, a>));
}

/// @test If `substitution_succeeds` is given a template that can only be instantiated with two template parameters,
/// then it only evaluates to `true` if there are two additional parameters to `substitution_succeeds`, and those
/// parameters meet the constraints of the template.
TEST(SubstitutionSucceeds, BinarySourceTemplate) {
  EXPECT_FALSE(substitution_succeeds<require_a_b>);
  EXPECT_FALSE((substitution_succeeds<require_a_b, a>));
  EXPECT_FALSE((substitution_succeeds<require_a_b, b>));
  EXPECT_TRUE((substitution_succeeds<require_a_b, a, b>));
  EXPECT_FALSE((substitution_succeeds<require_a_b, b, a>));
}

struct type_with_member_foo {
  using foo = void;
};

/// @test `substitution_succeeds` works for templates that are constrained on the existence of a type member in the
/// parameter type
TEST(SubstitutionSucceeds, MemberCheck) {
  EXPECT_TRUE((substitution_succeeds<require_member_foo, type_with_member_foo>));
  EXPECT_FALSE((substitution_succeeds<require_member_foo, int>));
}

/// @test `substitution_succeeds` works for templates that are constrained on the existence of an operation on the
/// parameter types
TEST(SubstitutionSucceeds, AddCheck) {
  EXPECT_TRUE((substitution_succeeds<require_addable, int, int>));
  EXPECT_TRUE((substitution_succeeds<require_addable, int, int*>));
  EXPECT_TRUE((substitution_succeeds<require_addable, int*, int>));
  EXPECT_FALSE((substitution_succeeds<require_addable, int*, int*>));
}

}  // namespace
