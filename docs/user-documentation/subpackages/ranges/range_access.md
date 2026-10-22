<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page ranges_range_access ranges/range_access: Range Access

<!-- markdownlint-enable MD041 -->

The `ranges` library provides several facilities for _range access_ in a generic
manner.

# `ranges::begin` and `ranges::end` {#subpackages-ranges-range-access-ranges-begin-and-end}

`arene::base::ranges::begin` returns an iterator to the beginning of a range.
`arene::base::ranges::end` returns an iterator to the end of a range, usually
the position one past the last element. Together, they provide a common way to
iterate over containers and user-defined ranges.

Both facilities are callable range-access objects. The usual generic pattern is
to obtain both accessors once and then iterate until the beginning iterator
equals the end iterator:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/range_access_examples.cpp generic_range_access
<!-- prettier-ignore-end -->

The input range must be either an _lvalue_, or
`arene::base::enable_borrowed_range_v<Rng>` must be `true` for the range type,
otherwise they do not participate in overload resolution. In addition, they are
_Customization Point Objects_ (CPOs) which determine the appropriate way to
access the range based on the following priority order:

1. If the range is a c-style array, the accessors return a pointer to the first
   and one-past-the-last element.
2. If `T::begin()`/`T::end()` member functions are available, they are used.
3. If ADL-locatable `begin(T)`/`end(T)` functions are available, they are used.

If none of the above are available, the accessors do not participate in overload
resolution. The following sections provide examples of leveraging the CPO for
user-defined types.

## Accessing containers {#subpackages-ranges-range-access-accessing-containers}

The accessors can be used uniformly with different standard container types. The
following example uses both a fixed-size `arene::base::array` and a dynamically
sized `std::vector`:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/range_access_examples.cpp container_usage
<!-- prettier-ignore-end -->

## User-defined ranges with member functions {#subpackages-ranges-range-access-user-defined-ranges-with-member-functions}

A user-defined range can provide `begin()` and `end()` member functions. When
these member functions are available, `arene::base::ranges::begin` and
`arene::base::ranges::end` use them automatically:

\snippet docs/examples/subpackages/ranges/range_access_examples.cpp member_range

The range can then be passed to generic code using the range accessors:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/range_access_examples.cpp member_range_usage
<!-- prettier-ignore-end -->

## User-defined ranges with ADL functions {#subpackages-ranges-range-access-user-defined-ranges-with-adl-functions}

If a user-defined type cannot provide member functions, it can instead provide
free `begin()` and `end()` functions in the type's associated namespace. The
range accessors find these functions through argument-dependent lookup (ADL).
The free functions should describe the same sequence and return a beginning and
end iterator that can be used together:

\snippet docs/examples/subpackages/ranges/range_access_examples.cpp adl_range

The ADL-customized range is used in the same way as a container or a range with
member functions:

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/range_access_examples.cpp adl_range_usage
<!-- prettier-ignore-end -->

When both member functions and ADL functions are available, the member functions
take precedence for `arene::base::ranges::begin` and `arene::base::ranges::end`.

## `ranges::size` {#subpackages-ranges-range-access-ranges-size}

`arene::base::ranges::size` returns the number of elements in a range. It is a
backport of [`std::ranges::size`](https://en.cppreference.com/w/cpp/ranges/size)
and, like `ranges::begin` and `ranges::end`, it is a _Customization Point
Object_ (CPO) that selects the appropriate way to obtain the size based on the
following priority order:

1. If a constant-time `T::size()` member function is available and the type is
   not opted out (see below), it is used.
2. Otherwise, if an ADL-locatable `size(T)` function is available and the type
   is not opted out, it is used.
3. Otherwise, if `end(T) - begin(T)` is well-formed and the iterator/sentinel
   pair models a _sized sentinel_ (subtractable in both directions, yielding the
   iterator's difference type), the size is computed as `end(T) - begin(T)`,
   converted to the unsigned counterpart of the iterator's difference type
   (matching `std::ranges::size`).

If none of the above are available, `ranges::size` does not participate in
overload resolution.

`size` is intended to be a constant-time operation. Callers should assume it
runs in constant time; for ranges whose size can only be obtained in linear
time, use `arene::base::distance` instead, which disclaims that guarantee.

Because `size` assumes the member/ADL `size()` path is constant time, a type
whose `size()` is only linear time should opt out of it. This is normally the
responsibility of the _container author_, who knows the operation's complexity:
specialize `arene::base::ranges::disable_sized_range` for the type so that
`size` ignores its member/ADL `size()` (see \ref
subpackages-ranges-type-traits-disable-sized-range). A _user_ of a third-party
type can specialize it too when the author has not, but the author is best
placed to make that guarantee correct for every use.

The following example shows a range whose size is reported by a constant-time
member function, and a range whose size is instead computed from
`begin()`/`end()`:

\snippet docs/examples/subpackages/ranges/range_access_examples.cpp member_size

<!-- prettier-ignore-start -->
\snippet docs/examples/subpackages/ranges/range_access_examples.cpp begin_end_size
<!-- prettier-ignore-end -->

When both describe the same elements, the two paths report the same size:

\snippet docs/examples/subpackages/ranges/range_access_examples.cpp query_size
