<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page constexpr_test Constexpr Test Macros

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

Arene Base provides a unit testing support library, compatible with gtest, which
provides `CONSTEXPR_TEST` and `CONSTEXPR_TYPED_TEST` macros which mirror the
`TEST` and `TYPED_TEST` macros defined by GoogleTest. These macros provide an
alternative approach to using `STATIC_ASSERT_XXX`macros when compile-time tests.
Additionally, this support library provides `CONSTEXPR_ASSERT`, an assertion
macro that replaces ones from GoogleTest.

The constexpr test macros invoke the function body at compile-time. Function
bodies are also invoked at runtime to allow generation of coverage.

## Usage {#usage}

Here is a simple example of changing a runtime evaluated test using `TEST` to a
compile-time test using `CONSTEXPR_TEST`.

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

```cpp
CONSTEXPR_TEST(ConstexprContainer, PushPlacesTheElementAtFront)
{
  constexpr_container<int> container;
  for (auto value: {1, 2, 3, 4})
  {
    container.push(value);
    CONSTEXPR_ASSERT(container.front() == value);
  }
}
```

GoogleTest assertion macros (e.g. `ASSERT_EQ`) cannot be used within a
`CONSTEXPR_TEST`. Instead, a single `CONSTEXPR_ASSERT` assertion macro is
available.

Use of `CONSTEXPR_TYPED_TEST` is analogous to `CONSTEXPR_TEST`.

```cpp
CONSTEXPR_TYPED_TEST(ConstexprContainer, PushPlacesTheElementAtFront)
{
  constexpr_container<TypeParam> container;
  for (auto value: {1, 2, 3, 4})
  {
    container.push(value);
    CONSTEXPR_ASSERT(container.front() == value);
  }
}
```

## Limitations {#limitations}

As the test bodies of `CONSTEXPR_TEST` and `CONSTEXPR_TYPED_TEST` must be
invocable at compile-time, they cannot refer to non-`constexpr` values. This
includes the `this` pointer to the test fixture.
