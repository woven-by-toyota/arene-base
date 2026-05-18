<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page conditional_tests Conditional Tests

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

When defining type-parameterized tests, it's common for individual test cases to
have stricter requirements on the type parameter than the actual subject of the
tests. For example, a container type may have equality operators if and only if
the value type also has equality operators. To get full test coverage these
conditional features need to be fully tested, but they do not compile for
certain values of the type parameter, and Google Test does not provide any
built-in way to conditionally exclude them. This header provides macros which
allow test cases to be conditionally included or excluded depending on the type
parameter.

## Macros {#macros}

The test macro `CONDITIONAL_TYPED_TEST_P` works like `TYPED_TEST_P` from Google
Test but with a third argument for a condition depending on `TypeParam`. The
third argument is variadic within the preprocessor so it should not be necessary
to parenthesize more complicated template conditions. Here is an example:

```cpp
// Declare the typed test suite as normal
TYPED_TEST_SUITE_P(MySuite);

// Unconditional test
TYPED_TEST_P(MySuite, TestCase1) {
  EXPECT_EQ(some_class<TypeParam>{}, 0);
}

// Conditional test
CONDITIONAL_TYPED_TEST_P(MySuite, TestCase2, std::is_default_constructible<TypeParam>::value) {
  EXPECT_EQ(some_class<TypeParam>::default_value, TypeParam{});
}

// Register the conditional tests exactly as you would with a normal unconditional test
REGISTER_TYPED_TEST_SUITE_P(MySuite, TestCase1, TestCase2);
```

The conditional type test will run with the given body when `TypeParam` is
default-constructible, and will run with an empty body when `TypeParam` is not
default-constructible. In the latter case, the test body is never instantiated,
so it's possible to write tests which fail to compile when the condition does
not hold.
