<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page static_assertions Static Assertion Framework

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

Arene Base provides a unit testing support library, compatible with gtest, which
provides `STATIC_ASSERT_XXX` macros which mirror their non-static counterparts
in a 1:1 manner. This allows writing compile-time tests which look and feel like
their runtime counterparts, with a couple of
[known limitations](#known-limitations). These macros are only for use within
test code; non-test code should continue to use static assertions.

## Background {#background}

When writing C++ code which can be evaluated at compile time such as a type
trait, or `constexpr` function/method, it is often desirable to write unit test
cases which:

1. Unconditionally evaluate at compile time.
1. Cause compilation to fail if the unit test fails, pushing validation to
   earlier in the iteration cycle.

Such tests are particularly important for `constexpr` annotated APIs: It is not
a compilation error to use `constexpr`-incompatible logic in a `constexpr`
annotated function; without a compile-time evaluated test developers might
accidentally write logic they believe is `constexpr` but which is not.
[GoogleTest](https://github.com/google/googletest/) (hereafter gtest) is the
standard unit testing framework in use throughout the Arene ecosystem, as well
as in many non-Arene projects. Unfortunately, gtest lacks any official support
for compile-time test cases: all of its `ASSERT_XXX` macros are evaluated
exclusively at runtime, and it has no alternative facilities for writing static
test cases. There are other major unit testing frameworks such as
[Catch2](https://github.com/catchorg/Catch2) which do support static assertions,
but changing testing frameworks to get such functionality is inconvenient for
most projects.

As a result, most developers write such test cases by implementing their own
assertions using C++ `static_assert` declarations. A real example from
arene_base:

```cpp
static_assert(arene::base::any_of(&x[0], &x[1]), "Any of 1 element is that element");
```

There are several drawbacks of this approach:

- Because they don’t look like regular gtest macros, they may confuse new
  maintainers.

- The error messages are written by the test developer, potentially making them
  inconsistent with similar operations.
- The static assertion is more burdensome to write than a regular `ASSERT_XXX`.

- There is no runtime-evaluated component.

- The test coverage tools in use at Woven do not currently account for coverage
  in a purely compile-time evaluated content. This means that test developers
  have to write the test case twice; once with `static_assert` and once with a
  regular gtest `ASSERT_XXX`, to properly measure coverage.

### Provided Assertions {#provided-assertions}

The following set of assertions are provided by the framework:

|            Assertion            |  Category  |        Meaning        |
| :-----------------------------: | :--------: | :-------------------: |
|  `STATIC_ASSERT_EQ(lhs, rhs)`   |  Equality  |      lhs == rhs       |
|  `STATIC_ASSERT_NE(lhs, rhs)`   |  Equality  |      lhs != rhs       |
|  `STATIC_ASSERT_LT(lhs, rhs)`   | Relational |       lhs < rhs       |
|  `STATIC_ASSERT_LE(lhs, rhs)`   | Relational |      lhs <= rhs       |
|  `STATIC_ASSERT_GT(lhs, rhs)`   | Relational |       lhs > rhs       |
|  `STATIC_ASSERT_GE(lhs, rhs)`   | Relational |      lhs >= rhs       |
|   `STATIC_ASSERT_TRUE(value)`   |  Boolean   |     value == true     |
|  `STATIC_ASSERT_FALSE(value)`   |  Boolean   |    value == false     |
| `STATIC_ASSERT_STREQ(lhs, rhs)` |  C-String  | strcmp(lhs, rhs) == 0 |
| `STATIC_ASSERT_STRNE(lhs, rhs)` |  C-String  | strcmp(lhs, rhs) != 0 |

`STATIC_ASSERT_STREQ` and `STATIC_ASSERT_STRNE` are intended to be used for
c-strings only, resolving the ambiguity on how to interpret `char*`. For other
string types, prefer `STATIC_ASSERT_EQ` and `STATIC_ASSERT_NE`. The
case-insensitive string comparison assertions `ASSERT_STRCASEEQ` and
`ASSERT_STRCASENE` do not currently have an analog in the framework. Users can
fall back to the runtime versions of these assertions if they are needed.

The floating point specific equality assertions gtest provides are omitted
because implementing them as `constexpr` in a C++14 context is difficult,
involving the re-implementation of complex facilities for determining “units in
the last place.” Users should fall back to the normal gtest `ASSERT_FLOAT_EQ`
and `ASSERT_DOUBLE_EQ` assertions for more accurate floating point comparison if
needed.

## Known Limitations {#known-limitations}

### Inputs Must Be Constant Expressions {#inputs-must-be-constant-expressions}

A limitation of any simple _wrapper_ approach to compile-time unit tests using
`static_assert` inside a runtime testing framework like gtest is that ultimately
the input to the assertion must be a constant expression, and the test bodies
are not inherently `const`ant expressions. This can introduce hurdles to writing
compile-time-evaluated unit tests which are both correct and meaningful if the
test requires more complexity than a simple inline call of the method under
test. Such situations are common when attempting to validate classes with
mutable state. Consider the following example for testing the behavior of the
`push()` method of a `constexpr` compatible container:

```cpp
TEST(ConstexprContainer, PushPlacesTheElementAtFront)
{
 constexpr_container<int> container;
 for (auto value : {1, 2, 3, 4})
 {
   container.push(value);
   ASSERT_EQ(container.front(), value);
 }
}
```

One cannot simply place `constexpr` in front of the type declaration and replace
`ASSERT_EQ` with `STATIC_ASSERT_EQ` to turn this test into a compile time
evaluated test; when applied to a variable declaration, `constexpr` implies
`const`, and this test needs to call non-`const` methods of
`ConstexprContainer`.

A workaround to this problem is to add a layer of indirection via a `constexpr`
function, as within the body of that function the container can be declared
normally and thus not be `const`. Ideally, this could be done inside the test
body using a lambda. Unfortunately, C++14 `constexpr` support is more
restrictive than later C++ versions, and lambda expressions are not `constexpr`.
This leaves external free functions, which must return the value to be asserted
on. Following these guidelines, the push test could be rewritten to:

```cpp
constexpr auto push_places_the_element_at_front(
   std::initializer_list<int> initial_values, int value) {
 constexpr_container<int> container;
 for (const auto initial_value : initial_values) {
   container.push(initial_value);
 }
 container.push(value);
 return container.front();
}

TEST(ConstexprContainer, PushPlacesTheElementAtFront) {
 constexpr int value1 = 1;
 STATIC_ASSERT_EQ(push_places_the_element_at_front({}, value1), value1);
 constexpr int value2 = 2;
 STATIC_ASSERT_EQ(push_places_the_element_at_front({value1}, value2), value2);
 constexpr int value3 = 3;
 STATIC_ASSERT_EQ(push_places_the_element_at_front({value1, value2}, value3), value3);
 constexpr int value4 = 4;
 STATIC_ASSERT_EQ(push_places_the_element_at_front({value1, value2, value3}, value4), value4);
}
```

This change in shape accomplishes the goal of failing at compile time if
`push()` does not place the element at head, while maintaining the property of
testing across a robust set of initial states. However its readability has been
harmed in a couple of ways:

1. A portion of the body of the test is no longer located inside the TEST
   declaration, meaning a future maintainer has to look in multiple places to
   see what the assertion actually does.
1. To maintain the state accumulation aspect of the test, the indirection
   function needs to now consume the state to put the system in before the
   actual assertion of interest runs. This results in additional error-prone
   boilerplate, increasing the likelihood of oracle problems.

### Printing Values On Failure {#printing-values-on-failure}

Because ultimately the assertion macros are simply wrapping `static_assert`, and
until C++26 `static_assert`'s message must be a string-literal, it's not
possible for the assertions to print the _value_ of the inputs that caused the
failure.

For situations where this makes debugging failure cases difficult, there is a
compile time configuration option, `ARENE_GTEST_STATIC_ASSERTIONS`, which if set
to `0` decays the assertions back to their regular `ASSERT_XXX` form. This
should be set on the commandline of your compiler invocation; for bazel this
would look like this:

```bash
bazel test //foo/bar/... --cxxopt="-DARENE_GTEST_STATIC_ASSERTIONS=0"
```

## Conditionally Static Assertions {#conditionally-static-assertions}

In addition to the assertions above which always perform both a runtime
assertion and a static assertion, Arene Base also provides a set of assertion
macros which are always checked at runtime and may or may not be checked
statically depending on the characters of the type `TypeParam`. In
type-parameterized tests such as those declared with `TYPED_TEST_P`, `TypeParam`
is set by Google Test to be the type with which the test has been parameterized.
By using these conditional macros, you can write tests which are asserted
statically only for instantiations where `TypeParam` supports doing so, while
always being asserted at runtime.

The following conditionally-static assertion macros are available:

|             Assertion             |  Category  |    Meaning     |
| :-------------------------------: | :--------: | :------------: |
| `COND_STATIC_ASSERT_EQ(lhs, rhs)` |  Equality  |   lhs == rhs   |
| `COND_STATIC_ASSERT_NE(lhs, rhs)` |  Equality  |   lhs != rhs   |
| `COND_STATIC_ASSERT_LT(lhs, rhs)` | Relational |   lhs < rhs    |
| `COND_STATIC_ASSERT_LE(lhs, rhs)` | Relational |   lhs <= rhs   |
| `COND_STATIC_ASSERT_GT(lhs, rhs)` | Relational |   lhs > rhs    |
| `COND_STATIC_ASSERT_GE(lhs, rhs)` | Relational |   lhs >= rhs   |
| `COND_STATIC_ASSERT_TRUE(value)`  |  Boolean   | value == true  |
| `COND_STATIC_ASSERT_FALSE(value)` |  Boolean   | value == false |

These generate static assertions when `TestFixture::constexpr_compatible` is
true, and generate dynamic assertions in any case. **Note that this means that
to use these assertions, you will need to define a static variable**
`constexpr_compatible` **in the enclosing test fixture.**

The same caveats noted above for always-on static assertions also apply to the
conditional ones: notably, the assertion body must not refer to non-constexpr
variables, and no dynamic error message is available. More useful error messages
can be obtained by disabling static assertions using the same
`ARENE_GTEST_STATIC_ASSERTIONS` mechanism described above for the unconditional
`STATIC_ASSERT` macros.
