<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page math math: Facilities For Mathematical Operations

<!-- markdownlint-enable MD041 -->

The `math` sub-package provides a set of mathematical functions which are marked
`constexpr` so they can be used in constant expression contexts, such as
template parameters and array bounds.

These include facilities for obtaining powers of 2, or checking if a number is a
power of 2, obtaining the absolute value of a number, finding the greatest
common divisor of two integers, and finding the integral part of the
logarithm-base-2 of an integer.

The public header is

```{.cpp}
#include "arene/base/math.hpp"
```

The Bazel target is

```text
//:math
```

## Introduction {#introduction}

When writing code that operates on template parameters and other compile-time
constants, it is often desirable to use mathematical functions. Unfortunately,
the standard library doesn't provide all the operations that might be desired,
and those that are provided are often not marked `constexpr` so cannot be used
in constant expressions anyway.

The Arene Base `math` sub-package provides some functions to ease this
difficulty:

- `arene::base::abs` is equivalent to `std::abs`, and provides the absolute
  value of an integral or floating point value.
- `arene::base::abs_diff` returns the absolute difference between two integral
  or floating point values. It is intended as a safe alternative to
  `arene::base::abs(a-b)`.
- `arene::base::copysign` is equivalent to `std::copysign`, and returns a
  floating point value with the magnitude of the first operand and the sign of
  the second operand.
- `arene::base::isnan` is equivalent to `std::isnan`, and checks if a given
  floating point value is a NaN or not.
- `arene::base::isinf` is equivalent to `std::isinf`, and checks if a given
  floating point value is infinite (positive or negative) or not.
- `arene::base::isfinite` is equivalent to `std::isfinite`, and checks if a
  given floating point value is finite (not infinite or NaN) or not.
- `arene::base::isnormal` is equivalent to `std::isnormal`, and checks if a
  given floating point value is normal (not zero, subnormal, infinite, or NaN)
  or not.
- `arene::base::signbit` is equivalent to `std::signbit`, and checks if a given
  floating point value is negative (i.e. its sign bit is 1) or not.
- `arene::base::is_power_of_2` checks if an integer is a power of 2
- `arene::base::power_of_2` provides an integer that is the specified power
  of 2.
- `arene::base::make_power_of_2_sequence` returns a `std::integer_sequence`
  holding the values of `2^N` for all values of `N` in the specified range.
- `arene::base::gcd` returns the greatest common divisor of two integral values
  of the same type
- `arene::base::lcm` returns the least common multiple of two integral values of
  the same type
- `arene::base::log2` returns the integral portion of the base-2 logarithm of
  the supplied integral value.

All these operations are marked `constexpr`, so they can be used in constant
expressions.

The `math` sub-package also provides the value of π as a `constexpr` value:

- `arene::base::numbers::pi_v<Type>` is the value of π as the specified `Type`.
  `Type` must be `float` or `double`.
- `arene::base::numbers::pi` is the value of π as a `double`.

### ISO/IEC 60559 conformance {#isoiec-60559-conformance}

For the functions described above which have clear `std` equivalents, we tried
to implement the functions as closely as possible to their specifications from
the ISO/IEC 60559 standard (also known as IEEE 754). However, in the case of
signaling NaN, we were not able to follow the standard:

- `arene::base::abs` on platforms with no `__builtin_fabs` may cause signaling
  NaN to emit a floating point exception, and will always return a positive
  quiet NaN
- `arene::base::isnan`, `arene::base::isinf`, `arene::base::isfinite`, and
  `arene::base::isnormal` may cause signaling NaN to emit a floating point
  exception

Note that these "floating point exceptions" are the exceptions described in
ISO/IEC 60559 and are normally hardware-based. They are not C++ exceptions; no
functions in the math subpackage will emit C++ exceptions. Floating point
exceptions can be checked and cleared using the standard header `<cfenv>`.

## Examples {#examples}

A class template that takes a size parameter might require that it is a power of
2, in order for certain operations to be optimal:

```{.cpp}
template<std::size_t Size>
class foo {
  static_assert(arene::base::is_power_of_2(Size),"Size must be a power of 2");
// ...
};
```

Reducing a fraction to its simplest equivalent form requires dividing the
numerator and denominator by their Greatest Common Divisor:

```{.cpp}
std::pair<std::size_t, std::size_t> reduce_fraction(
  std::pair<std::size_t, std::size_t> input) {
  const auto divisor = arene::base::gcd(input.first, input.second);
  return {input.first / divisor, input.second / divisor);
}
```
