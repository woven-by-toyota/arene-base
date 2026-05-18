<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page utility utility: Low level utility facilities

<!-- markdownlint-enable MD041 -->

Arene Base provides backports of facilities from `<utility>` in newer C++
versions, along with other similar low-level facilities.

The public header is

```{.cpp}
#include "arene/base/utility.hpp"
```

The Bazel target is

```text
//:utility
```

## Utility Facilities {#utility-facilities}

The `utility` package provides the following basic facilities:

### Alignment {#alignment}

- `arene::base::is_aligned`: check if an integer or pointer is aligned to the
  specified power of 2.
- `arene::base::align_floor`: adjust an integer to the nearest multiple of the
  specified alignment that is less than or equal to the supplied value
- `arene::base::align_ceil`: adjust an integer to the nearest multiple of the
  specified alignment that is greater than or equal to the supplied value
- `arene::base::alignment_offset`: determine the number of bytes by which a
  pointer would need to be adjusted to be aligned to the specified value

### Bit Masks {#bit-masks}

- `arene::base::bit_mask`: create a mask value where the specified number of low
  bits are all 1

### Creating Const Reference To A Non-Const Object {#creating-const-reference-to-a-non-const-object}

- `arene::base::as_const(foo)` returns a `const` reference to `foo`. This is a
  back-port of
  [`std::as_const`](https://en.cppreference.com/w/cpp/utility/as_const)

### Forwarding A Reference Similarly To Another Type {#forwarding-a-reference-similarly-to-another-type}

`arene::base::forward_like` provides a backport of C++23's
[`std::forward_like`](https://en.cppreference.com/w/cpp/utility/forward_like).
This is useful in implementing _deducing this_ style helpers, where the const
and r/l-value reference qualifications of the `Self` parameter need to be
propagated to the type of other values.

### Tags For In-place Construction {#tags-for-in-place-construction}

- `arene::base::in_place`, `arene::base::in_place_index` and
  `arene::base::in_place_type` provide tags to indicate that types such as
  `arene::base::optional` should construct their stored value from the supplied
  arguments "in place", rather than attempting a copy or conversion.

### Creating A Range From An Iterator Pair {#creating-a-range-from-an-iterator-pair}

- `arene::base::make_subrange` creates a range object that can be used with
  range-based `for` loops from an iterator pair:

```{.cpp}
template<typename Iterator>
void foo(Iterator first, Iterator last) {
  for(auto& element: arene::base::make_subrange(first, last)) {
    do_stuff(element);
  }
}
```

### Swapping Values {#swapping-values}

`arene::base::swap` provides a `constexpr` version of
[`std::swap`](https://en.cppreference.com/w/cpp/algorithm/swap), which is also
implemented as a
[Customization Point Object](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html).
As specified in the linked paper, doing so allows a few of advantages:

- It avoids the user having to remember to do the "ADL two-step" of needing to
  do `using arene::base::swap;` before an unqualified `swap` call: Invoking
  `arene::base::swap(T&,T&)` will find ADL-specializations of `swap(T&, T&)`, or
  fall back to the default implementation if no customizations are found.
- It allows consistent concept enforcement. The `arene::base::is_swappable_v<T>`
  trait is checked up front, rather than a user seeing a potentially confusing
  failure if ADL lookup fails and the type is not default-swappable.
- It allows `swap` to be passed directly to algorithms as a function object.

### Converting An Enum To Its Underlying Value {#converting-an-enum-to-its-underlying-value}

- `arene::base::to_underlying` is a backport of
  [`std::to_underlying`](https://en.cppreference.com/w/cpp/utility/to_underlying),
  for casting an enumeration to its underlying type.

### Safe Integer Comparison {#safe-integer-comparison}

Backports of the
[C++20 safe integer comparison facilities](https://en.cppreference.com/w/cpp/utility/intcmp)
are provided. These perform integer comparisons without performing any implicit
conversions on the arguments, thereby avoiding issues such as negative values
implicitly wrapping around to large positive values. For example, using raw
comparison operators `0 < 5u == true` as expected, but `-1 < 5u == false`. Using
the equivalent safe comparison instead, `arene::base::cmp_less(0, 5u) == true`
and `arene::base::cmp_less(-1, 5u) == true` as one would expect.

The available functions are as follows:

|             utility              | corresponding operator | C++20 stdlib equivalent  |
| :------------------------------: | :--------------------: | :----------------------: |
|     `arene::base::cmp_equal`     |      `operator==`      |     `std::cmp_equal`     |
|   `arene::base::cmp_not_equal`   |      `operator!=`      |   `std::cmp_not_equal`   |
|     `arene::base::cmp_less`      |      `operator<`       |     `std::cmp_less`      |
|    `arene::base::cmp_greater`    |      `operator>`       |    `std::cmp_greater`    |
|  `arene::base::cmp_less_equal`   |      `operator<=`      |  `std::cmp_less_equal`   |
| `arene::base::cmp_greater_equal` |      `operator>=`      | `std::cmp_greater_equal` |
