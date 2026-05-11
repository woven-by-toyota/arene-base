// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/constraints/constraints.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

namespace {

using arene::base::constraints;
using arene::base::substitution_succeeds;
using std::enable_if_t;

template <class T, class U>
constexpr bool is_same_v = std::is_same<T, U>::value;

// Types for testing
struct a;
struct b;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Function templates and corresponding aliases used as test cases
//
// Function template with empty set of constraints (always succeed)
template <class... P, constraints<> = nullptr>
void empty_constraints_fun() {}
//
// Alias of empty_constraints_fun's type for use with @c substitution_succeeds
template <class... P>
using empty_constraints = decltype(empty_constraints_fun<P...>);
//
////////////////////////////////////////////////////////////////////////////////
//
// Function template with a single constraint, checking @c T is type @c a
template <class T, constraints<enable_if_t<is_same_v<T, a>>> = nullptr>
void is_a_fun();
///
// Alias of is_a_fun's type for use with @c substitution_succeeds
template <class T>
using is_a = decltype(is_a_fun<T>);
//
////////////////////////////////////////////////////////////////////////////////
//
// Function template with two Constraints, checking @c T is @c a and @c U is @c
// b
template <class T, class U, constraints<enable_if_t<is_same_v<T, a>>, enable_if_t<is_same_v<U, b>>> = nullptr>
void is_a_b_fun();
//
// Alias of is_a_b_fun's type for use with @c substitution_succeeds
template <class T, class U>
using is_a_b = decltype(is_a_b_fun<T, U>);
//
////////////////////////////////////////////////////////////////////////////////
//
// Overloads with mutually-exclusive constraints
// (This just makes sure they aren't considered redeclarations)
template <class T, constraints<enable_if_t<is_same_v<T, a>>> = nullptr>
void a_or_b_fun() {}
//
template <class T, constraints<enable_if_t<is_same_v<T, b>>> = nullptr>
void a_or_b_fun() {}
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @test Empty constraints do not restrict template instantiations
TEST(Constraints, EmptyConstraints) {
  empty_constraints_fun<>();
  empty_constraints_fun<a>();
  empty_constraints_fun<b>();
  empty_constraints_fun<a, b>();
  empty_constraints_fun<b, a>();
  // TODO: This test fails on gcc 8.3, and it should not. It was previously commented out due to eMOC
  // incompatibility, so needs to be investigated further.
  // EXPECT_TRUE(substitution_succeeds<empty_constraints>);
  EXPECT_TRUE((substitution_succeeds<empty_constraints, a>));
  EXPECT_TRUE((substitution_succeeds<empty_constraints, b>));
  EXPECT_TRUE((substitution_succeeds<empty_constraints, a, b>));
  EXPECT_TRUE((substitution_succeeds<empty_constraints, b, a>));
}

/// @test When the constraint is not satisfied, a substitution failure occurs.
TEST(Constraints, PreventsInstantiationIfConstraintIsFalse) {
  EXPECT_FALSE(substitution_succeeds<is_a>);
  EXPECT_TRUE((substitution_succeeds<is_a, a>));
  EXPECT_FALSE((substitution_succeeds<is_a, b>));
  EXPECT_FALSE((substitution_succeeds<is_a, a, b>));
  EXPECT_FALSE((substitution_succeeds<is_a, b, a>));
}

/// @test Given multiple requirements, constraints produces a substitution failure if any individual requirement is not
/// met.
TEST(Constraints, WithMultipleRequirementsIsNotSatisfiedIfAnyRequirementIsNotSatisfied) {
  EXPECT_FALSE(substitution_succeeds<is_a_b>);
  EXPECT_FALSE((substitution_succeeds<is_a_b, a>));
  EXPECT_FALSE((substitution_succeeds<is_a_b, b>));
  EXPECT_TRUE((substitution_succeeds<is_a_b, a, b>));
  EXPECT_FALSE((substitution_succeeds<is_a_b, b, a>));
}

// Test that we can use Constraints<> to specialise classes
template <typename T, typename = constraints<>>
struct has_foo : std::integral_constant<bool, false> {};

template <typename T>
struct has_foo<T, constraints<decltype(T::foo())>> : std::integral_constant<bool, true> {};

struct this_class_has_foo {
  static void foo();
};

struct this_class_does_not_have_foo {};

/// @test Can use a trailing template type parameter defaulted to `constraints<>` to allow specializations to specify
/// constraints
TEST(Contraints, CanUseContraintsToSpecializeClasses) {
  static_assert(has_foo<this_class_has_foo>::value, "Class that meets the constraint should use the specialization");
  static_assert(
      !has_foo<this_class_does_not_have_foo>::value,
      "Class that does not meet the constraint should use the primary template"
  );
}

}  // namespace
