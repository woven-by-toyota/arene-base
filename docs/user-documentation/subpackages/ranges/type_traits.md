<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page ranges_type_traits ranges/type_traits: Range Type Traits

<!-- markdownlint-enable MD041 -->

The `ranges/type_traits` subpackage provides C++14-compatible type traits for
properties described by concepts in `std::ranges`.

The public export header is:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/type_traits_examples.cpp include_header
<!-- prettier-ignore-end -->

# `enable_borrowed_range` {#subpackages-ranges-type-traits-enable-borrowed-range}

`arene::base::ranges::enable_borrowed_range<Rng>` is an opt-in trait and a
backport of
[`std::ranges::enable_borrowed_range`](https://en.cppreference.com/w/cpp/ranges/borrowed_range).
It indicates that iterators obtained from a range remain valid after the range
object is destroyed. The primary template defaults to `false`.

The trait should be specialized to be `true` only for a range type that has this
lifetime property. The trait records this property; it does not provide the
range's iterator operations.

The following example defines an owning range and a range that refers to storage
owned elsewhere:

\snippet docs/examples/subpackages/ranges/type_traits_examples.cpp range_types

Only the range whose iterators do not depend on the lifetime of the range object
is opted in:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/type_traits_examples.cpp specialize_borrowed_range
<!-- prettier-ignore-end -->

The class-trait and variable-template forms can then be queried at compile time:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/type_traits_examples.cpp query_borrowed_range
<!-- prettier-ignore-end -->

Note that users should rarely query `enabled_borrowed_range` directly. Instead
prefer `arene::base::is_borrowed_range<Rng>`.

The trait is used by `arene::base::ranges::begin` and `arene::base::ranges::end`
to determine whether range access is permitted for an rvalue; see \ref
ranges_range_access .

## `disable_sized_range` {#subpackages-ranges-type-traits-disable-sized-range}

`arene::base::ranges::disable_sized_range<Rng>` is an opt-out trait and a
backport of
[`std::ranges::disable_sized_range`](https://en.cppreference.com/w/cpp/ranges/sized_range).
It indicates that a range type's member or ADL `size()` is not a constant-time
operation and so must not be used to compute the range's size. It is a class
trait whose primary template derives from `std::false_type`.

Unlike the standard variable-template customization point, `disable_sized_range`
is a struct because C++14 lacks inline variables, which makes explicit
specialization of a variable template across translation units error-prone.

The trait should be specialized to derive from `std::true_type` only for a range
type whose `size()` cannot be assumed to run in constant time, for example one
that must count the elements of a singly-linked list:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/disable_sized_range_examples.cpp slow_size
<!-- prettier-ignore-end -->

Such a range is opted out so that its non-constant-time `size()` is disregarded:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/disable_sized_range_examples.cpp specialize_disable_sized_range
<!-- prettier-ignore-end -->

The variable template `arene::base::ranges::disable_sized_range_v<Rng>` provides
the same query as `disable_sized_range<Rng>::value` in a concise form, and can
be queried at compile time:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/disable_sized_range_examples.cpp query_disable_sized_range
<!-- prettier-ignore-end -->

## `is_range` {#subpackages-ranges-type-traits-is-range}

`arene::base::ranges::is_range<Rng>` is a compile-time query for whether `Rng`
is a range suitable for use in range algorithms/views/etc. It is a backport of
[`std::ranges::range`](https://en.cppreference.com/w/cpp/ranges/range). As with
the range access functions, the cv- and reference-qualification of `Rng` is
significant. For example, an lvalue reference to a non-borrowed range can
satisfy `is_range`, while the unqualified range type does not because it would
be accessed as an rvalue.

The trait has a static member `value` which is equal to `true` if and only if:

1. `arene::base::ranges::begin(std::declval<Rng>())` is well-formed.
2. `arene::base::ranges::end(std::declval<Rng>())` is well-formed.

The variable template `arene::base::ranges::is_range_v<Rng>` provides the same
query as `is_range<Rng>::value` in a concise form.

The following examples show valid standard-library and user-defined ranges, as
well as types which do not satisfy the complete range requirements:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/type_traits_examples.cpp is_range_usage
<!-- prettier-ignore-end -->

## `is_borrowed_range` {#subpackages-ranges-type-traits-is-borrowed-range}

`arene::base::ranges::is_borrowed_range<Rng>` is a compile-time query for
whether `Rng` is a `range` whose iterators's lifetime is not tied to the
lifetime of the variable it is obtained from. It is a backport of
[`std::ranges::borrowed_range`](https://en.cppreference.com/w/cpp/ranges/borrowed_range).

Most commonly this is used to assert in a generic context that it is safe to
return an iterator from a function obtained from an input range to that
function. For example, the range-algorithm form of `find()` is only valid for
borrowed ranges.

A type `Rng` is a borrowed range if it satisfies
`arene::base::ranges::is_range<Rng>` and one or both of the following are true:

- `arene::base::ranges::enable_borrowed_range<arene::base::remove_cvref_t<Rng>>`
  is specialized to `true` for the type.
- `Rng` is an lvalue reference.

The variable template `arene::base::ranges::is_borrowed_range_v<Rng>` provides
the same query as `arene::base::ranges::is_borrowed_range<Rng>::value` in a
concise form.

The following examples show use of the trait to determine whether a type is a
borrowed range:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/type_traits_examples.cpp is_borrowed_range_usage
<!-- prettier-ignore-end -->

## `borrowed_iterator_t` {#subpackages-ranges-type-traits-borrowed-iterator-t}

`arene::base::ranges::borrowed_iterator_t<Rng>` is a type alias that
conditionally selects between a ranges iterator or
`arene::base::ranges::dangling` based on whether `Rng` is a borrowed range. If
`Rng` satisfies `arene::base::ranges::is_borrowed_range_v<Rng>`, the type alias
resolves to `arene::base::ranges::iterator_t<Rng>`. Otherwise, it resolves to
`arene::base::ranges::dangling`. This is useful for writing generic range
algorithms that must always match and return an iterator, even when the input
range may not be a borrowed range.
