<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compare compare: Three-Way-Comparison and Operator Definitions.

<!-- markdownlint-enable MD041 -->

Arene Base attempts to provide some of the benefits of the C++20 `<=>`
(spaceship) operator to C++14 and C++17 code by providing types and templates to
eliminate much of the boiler plate.

When writing a _value_ type, you often need to write comparison operators, to
answer the question "is value `a` greater or less than value `b`?" As there are
canonical definitions of `operator<=`, `operator>`, and `operator>=` which can
all be implemented in terms of `operator<`, this often leads to a lot of
boilerplate implementation with minimal, and easy to get wrong, changes between
them. When these canonical definitions are not used, it is very easy to violate
the mathematical assumptions of ordering they are intended to represent,
particularly as a structure changes over time.

In C++20, the `<=>` (spaceship) operator was introduced in order to help
simplify this: you only need to provide `operator<=>` and the compiler will
automatically define the remainder of the operations for you. You can also,
optionally, provide `operator==`, if there is a performance benefit to doing so.

The public header for the Arene Base comparison facilities is

```{.cpp}
#include "arene/base/compare.hpp"
```

The Bazel target is

```text
//:compare
```

## Strong Ordering and Weak Ordering {#strong-ordering-and-weak-ordering}

A type and comparison form a _strong ordering_ if any two values of the type are
either equal, or one is "before" the other given the specified comparison.
Strings compared lexicographically thus form a strong ordering: `"abc"` is after
`"abb"` and before `"abd"`, and in Unicode or ASCII, `"Abc"` is before `"abc"`.
On the other hand, strings compared in a case-insensitive manner do **not** form
a strong ordering, since `"Abc"` and `"abc"` are not _equal_, but _equivalent_:
neither is before the other, but they are not identical. Case-insensitive
comparison is thus a _weak ordering_.

Formally, a strong ordering requires that `!(a < b) && !(b < a)` implies
`a == b`, whereas a weak ordering only requires that `!(a < b) && !(b < a)`
implies `a is equivalent to b`, and `a` may still not be equal to `b`.

For most types, you want a _strong ordering_, so this is what the Arene Base
facilities assist with.

### The `arene::base::strong_ordering` type {#the-arene-base-strong_ordering-type}

The C++20 spaceship operator returns either an instance of
`std::strong_ordering` or `std::weak_ordering` depending whether it's a strong
or weak ordering. Arene Base only supports strong orderings, since that's what
is required for most types.

`arene::base::strong_ordering` is based on the `std::strong_ordering` type from
C++20. It's an `enum class` with three members:
`arene::base::strong_ordering::less`, `arene::base::strong_ordering::equal` and
`arene::base::strong_ordering::greater`, that can be used to represent the
result of a comparison.

## The comparison support facilities {#the-comparison-support-facilities}

The Arene Base comparison support facilities consist of:

- \subpage three-way-compare
- \subpage operator-inheritance

For the best experience providing comparison operators for a class type, that
class type should provide a <!-- Comment to force a line wrap ............ -->
\ref three-way-compare "three-way comparison function" for comparing instances
of that class with instances of other types with which it should be comparable.
If equality is faster to check than ordering (e.g. strings with different
lengths cannot be equal) then an equality operator should also be provided, with
the class type being provided as the left-hand argument. The remaining
comparison operators are then provided by deriving from the appropriate base
class. Please see the linked documentation for details.

## Known limitations {#known-limitations}

The \ref operator-inheritance "comparison support templates" only support
comparison from instances of the type being defined to instances of specific
other types. The other type may be an instantiation of a template, but it can't
be generic: you can't define operators for comparison against "all
instantiations of `foo<>`" or "any type convertible to `X`". For those cases,
you are forced to write the comparison operators by hand. For a given class,
some comparison operators might be provided via the comparison support
templates, while others are written manually.

### Canonical implementations {#canonical-implementations}

For a class `my_class`, which should be comparable to all instances of the
`other_template` class template, the canonical implementations of comparison
operators based on \ref three-way-compare "three-way comparisons" look like
this:

```{.cpp}
class my_class{
  // implementation

  template<typename T>
  ARENE_NODISCARD static arene::base::strong_ordering three_way_compare(
    const my_class& lhs, const other_template<T>& rhs) noexcept {
    // Real comparison
  }

  // Comparisons for my_class OP other_template<T>
  template<typename T>
  ARENE_NODISCARD friend bool operator<(const my_class& lhs, const other_template<T>& rhs) noexcept {
    return my_class::three_way_compare(lhs, rhs) == arene::base::strong_ordering::less;
  }

  template<typename T>
  ARENE_NODISCARD friend bool operator>(const my_class& lhs, const other_template<T>& rhs) noexcept {
    return my_class::three_way_compare(lhs, rhs) == arene::base::strong_ordering::greater;
  }

  template<typename T>
  ARENE_NODISCARD friend bool operator==(const my_class& lhs, const other_template<T>& rhs) noexcept {
    return my_class::three_way_compare(lhs, rhs) == arene::base::strong_ordering::equal;
  }

  template<typename T>
  ARENE_NODISCARD friend bool operator!=(const my_class& lhs, const other_template<T>& rhs) noexcept {
    return my_class::three_way_compare(lhs, rhs) != arene::base::strong_ordering::equal;
  }

  template<typename T>
  ARENE_NODISCARD friend bool operator<=(const my_class& lhs, const other_template<T>& rhs) noexcept {
    return my_class::three_way_compare(lhs, rhs) != arene::base::strong_ordering::greater;
  }

  template<typename T>
  ARENE_NODISCARD friend bool operator>=(const my_class& lhs, const other_template<T>& rhs) noexcept {
    return my_class::three_way_compare(lhs, rhs) != arene::base::strong_ordering::less;
  }

  // Comparisons for other_template<T> OP my_class
  template<typename T>
  ARENE_NODISCARD friend bool operator<(const other_template<T>& lhs, const my_class& rhs) noexcept {
    return rhs > lhs;
  }
  template<typename T>
  ARENE_NODISCARD friend bool operator>(const other_template<T>& lhs, const my_class& rhs) noexcept {
    return rhs < lhs;
  }
  template<typename T>
  ARENE_NODISCARD friend bool operator==(const other_template<T>& lhs, const my_class& rhs) noexcept {
    return rhs == lhs;
  }
  template<typename T>
  ARENE_NODISCARD friend bool operator!=(const other_template<T>& lhs, const my_class& rhs) noexcept {
    return rhs != lhs;
  }
  template<typename T>
  ARENE_NODISCARD friend bool operator<=(const other_template<T>& lhs, const my_class& rhs) noexcept {
    return rhs >= lhs;
  }
  template<typename T>
  ARENE_NODISCARD friend bool operator>=(const other_template<T>& lhs, const my_class& rhs) noexcept {
    return rhs <= lhs;
  }
};
```

If instead of `other_template<T>`, the type to be compared was "any type that
such that `my_comparable_check<T>::value` is `true`, then the signatures could
be changed to be something like:

```{.cpp}
template<typename T, arene::base::constraints<std::enable_if_t<my_comparable_check<T>::value>> = nullptr>
ARENE_NODISCARD friend bool operator<(const my_class& lhs, const T& rhs) noexcept;
```

This can easily be extended to cover multiple constraints.
