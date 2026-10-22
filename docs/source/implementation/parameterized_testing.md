<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# Type-parameterized Testing {#type-parameterized-testing}

It can be helpful to write tests that are parameterized by type, so that users
of Arene Base can instantiate them for the types they plan to actually use and
verify that 100% test coverage is available for these concrete instantiations of
Arene Base's library types. An example of this can be found in the tests for
`inline_vector`.

It's generally easy to do this for basic test cases, but GoogleTest uses macros
extensively and the behavior of these macros can not directly interact with the
type system, so it can be difficult to deal with edge cases. This is especially
true in C++14 because of the lack of `constexpr if`. This document contains some
advice for developing type-parameterized tests in Arene Base.

## Basics of Type-parameterized Testing {#basics-of-type-parameterized-testing}

When writing tests, all support for type parameterization in GoogleTest will
need to go through `TYPED_*_P` macros. A minimal setup on the test-author side
is the following; all of these macro invocations are required, and they must be
in a header so that users can include them in their code:

```cpp
// arene/base/some_subpackage/header_containing_tests.hpp

#include <gtest/gtest.h>
#include "arene/base/testing/gtest.hpp"

namespace arene {
namespace base {
namespace testing {

template <typename T>
class MySuiteTest : public ::testing::Test {
  // This is a fixture accessible from all test cases using TestFixture::something
};
// The Death Tests use the same fixture as the "normal" tests.
// To improve test report readability, it should be defined as a distinct type.
template <typename T>
class InlineVectorDeathTest : public InlineVectorTest<T> {};

// Declare the two test suites for GoogleTest; test cases will all be added to one of these.
TYPED_TEST_SUITE_P(MySuiteTest);
TYPED_TEST_SUITE_P(MySuiteDeathTest);

/// @test A description of what the test does
TYPED_TEST_P(MySuiteTest, SomeTestCaseName) { ASSERT_TRUE(some_condition); }

/// @test A description of what the test does
TYPED_TEST_P(MySuiteTest, SomeTestCaseName2) {
  // This will assert the condition at runtime as well as at compile-time
  STATIC_ASSERT_TRUE(some_condition_that_is_always_constexpr());
  // This will assert the condition at runtime and assert it at compile-time for literal types
  COND_STATIC_ASSERT_TRUE(some_condition_that_is_only_constexpr_for_literal_types());
}

/// @test A description of what the death test does; death tests must go in the DeathTest suite
TYPED_TEST_P(MySuiteDeathTest, SomeTestCaseName3) { ASSERT_DEATH(some_illegal_call()); }

/// @test A description of what the constexpr test does
CONDITIONAL_TYPED_TEST_P(MySuiteTest, SomeTestCaseName4, MySuiteTest<TypeParam>::constexpr_compatible) {
  // This test case is conditional on MySuiteTest<TypeParam>::constexpr_compatible so it's
  // safe to instantiate constexpr values in the test body.
  constexpr TypeParam value{};
  // Since you can be sure TypeParam is a literal type, unconditional STATIC_ASSERT is OK
  STATIC_ASSERT_EQ(value, some_constexpr_function<TypeParam>());
}

// After declaring all tests in a suite, register it with GoogleTest.
// If you forget to register some of the tests in a suite, GoogleTest emits a runtime error.
REGISTER_TYPED_TEST_SUITE_P(
    MySuiteTest,
    SomeTestCaseName,
    SomeTestCaseName2,
    SomeTestCaseName4  // conditional tests are registered just like normal ones
);

REGISTER_TYPED_TEST_SUITE_P(
    MySuiteDeathTest,
    SomeTestCaseName3
);

} // namespace testing
} // namespace base
} // namespace arene
```

To use the Arene Base test parameterization support macros, you need to declare
both of the test suites described above: `*Test`, `*DeathTest`. If you don't
actually have anything you want to put in one of them, you can register a suite
without any test cases.

Once a test suite has been declared and registered, a user can instantiate it
using the `ARENE_INSTANTIATE_TESTS` macro:

```cpp
#include <gtest/gtest.h>
#include "arene/base/testing/gtest.hpp"
#include "arene/base/some_subpackage/header_containing_tests.hpp"

// This type list can be renamed, but is used in the instantiation below this
using TestTypes = ::testing::Types<std::int32_t, double, std::string, my_class>;

// "UserApp" here refers to the user's instantiation of the tests and can be renamed.
// Unlike GoogleTest's INSTANTIATE_TYPED_TEST_SUITE_P, there is no trailing comma.
ARENE_INSTANTIATE_TESTS(UserApp, MySuite, TestTypes);
```

There should be an internal instantiation of the tests inside a `.cpp` test file
in Arene Base to make sure that the parameterized tests are working correctly.
By convention, this instantiation is called `Internal` in the place of `UserApp`
above.

## Difficulty Points {#difficulty-points}

### Constexpr Tests {#constexpr-tests}

GoogleTest has quite poor support for compile-time programming. Test fixtures
inherit from the base `::testing::Test` class and each individual test case then
inherits from its test fixture, which is instantiated as part of the test. The
test technically becomes a member function definition which is then run from an
instance of a virtually-derived class. This prevents the test case from being a
`constexpr` function, and indeed all of the assertion macros contain `goto`s, so
they can not be used in `constexpr` functions at all. As such, supporting
compile-time logic in tests is difficult, especially when done conditionally.

The simplest case is when test logic is manifestly compile-time and can never
fail to be so; an example of this is testing exception specifications. These can
use the `STATIC_ASSERT_*` macros in the `testing` subpackage to check the
condition statically and again at runtime for coverage purposes.

The next-simplest case is when local test logic **may** be compile-time for
certain types, but can not be for others. An example of this is an assertion
which locally uses a `constexpr` template function that constructs an instance
of the `TypeParam`. Since the `TypeParam` may or may not have a `constexpr`
constructor, this assertion can be static for some types but not others. In this
case, you can use the `COND_STATIC_ASSERT_*` macros in the same subpackage to
implement the assertions, but you'll need to make sure that the assertions are
generally self-contained without any `constexpr TypeParam` intantiations leading
up to them, since these can fail to compile for some `TypeParam`s. Like the
unconditional `STATIC_ASSERT_*` macros, these conditional ones devolve into
normal `ASSERT_*` macros when `ARENE_GTEST_STATIC_ASSERTIONS` is set to `0`.

**If your test suite uses any of the `COND_STATIC_ASSERT` macros, you will need
to define a member variable** `static constexpr bool constexpr_compatible`
**inside your test fixture which should depend on the type parameter.** The
conditional assertion macros use this variable to determine whether to run the
static part of the assertion or not. For example, you might define this to be
`true` if `TypeParam` is trivially constructible and trivially copy assignable.

Sometimes you have a test case which only makes sense for `TypeParam`s which
support compile-time logic, for example, a test with some roundabout logic to
check that an already-tested runtime function is also available at compile time.
These tests can be made conditional on `constexpr` compatibility of the type
parameter by using the `CONDITIONAL_TYPED_TEST_P` macro to declare them instead
of `TYPED_TEST_P`, and putting a condition which ensures the test body can be
executed in `constexpr`; it will probably make the most sense to refer to the
`constexpr_compatible` variable used by the `COND_STATIC_ASSERT` macros. Note
that GoogleTest's `TestFixture` alias is not visible from the scope of the
condition in `CONDITIONAL_TYPED_TEST_P`, so you'll need to spell out
`MyTestSuite<TypeParam>` explicitly with your suite's name.

### Pathological Types {#pathological-types}

Oftentimes generic types have logic which only works for certain type
parameters, for example a container which is movable if and only if the element
type is movable. In these cases, it's convenient to test the generic type by
defining a pathological parameter type (e.g. one with no move constructors) and
confirming that the generic type behaves as expected. Unfortunately, these
pathological types often break the more generic test cases, where a lot of logic
requires a certain baseline level of functionality.

Currently, Arene Base does not have any generic tools to deal with these
pathological types, so your best bet is to avoid instantiating the parameterized
tests with them, and create a separate non-parameterized test suite where you
perform the pathological type tests. We hope to introduce better tools for these
cases soon.
