<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

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

# Introduction {#subpackages-math-introduction}

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
  floating point value is a NaN or not. It is a customization point object; see
  [Customization point objects](#subpackages-math-float-property-cpos).
- `arene::base::isinf` is equivalent to `std::isinf`, and checks if a given
  floating point value is infinite (positive or negative) or not. It is a
  customization point object; see
  [Customization point objects](#subpackages-math-float-property-cpos).
- `arene::base::isfinite` is equivalent to `std::isfinite`, and checks if a
  given floating point value is finite (not infinite or NaN) or not. It is a
  customization point object; see
  [Customization point objects](#subpackages-math-float-property-cpos).
- `arene::base::isnormal` is equivalent to `std::isnormal`, and checks if a
  given floating point value is normal (not zero, subnormal, infinite, or NaN)
  or not. It is a customization point object; see
  [Customization point objects](#subpackages-math-float-property-cpos).
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
- `arene::base::complex_l1_norm` returns the L1 norm of a single
  possibly-complex number. For complex numbers, this is the sum of the absolute
  values of the real and imaginary parts; for real numbers it is simply the
  absolute value.

All these operations are marked `constexpr`, so they can be used in constant
expressions.

The `math` sub-package also provides the value of π as a `constexpr` value:

- `arene::base::numbers::pi_v<Type>` is the value of π as the specified `Type`.
  `Type` must be `float` or `double`.
- `arene::base::numbers::pi` is the value of π as a `double`.

## ISO/IEC 60559 conformance {#subpackages-math-isoiec-60559-conformance}

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

## Extensible math functions {#subpackages-math-extensible-math-functions}

Some of Arene Base's math functions are implemented as user-extensible
customization point objects following the general logic of the C++ standard
proposal [P4188](https://wg21.link/P4188). By filling in the corresponding
functions for a user-defined type, as described below, such types can be made to
work with Arene Base's extensible function objects.

### The arene::base::math namespace {#subpackages-math-the-arene-base-math-namespace}

Depending on their internal logic for selecting user-defined overloads, some
extensible math function objects are in the `arene::base::math` namespace.
Function objects in this namespace implement the following customization logic:

1. If the argument type has a _member function_ of the same name as the function
   object, then calling the function object returns the result of this member
   function.
1. If the argument type does not have a member function of the same name as the
   function object but there is a _free function_ that can be found by an
   unqualified call with the same name, then calling the function object returns
   the result of this free function.
1. If neither a member function or free function of the same name is found, then
   calling the function object applies some fallback behaviour. This may be a
   generic implementation, a `std` function, or a compilation error.

The following table shows the extensible math functions which are currently
available in the `arene::base::math` namespace, along with their fallback
behaviour when no customized implementation is present:

| Function | Fallback behaviour                                                     |
| -------- | ---------------------------------------------------------------------- |
| `abs`    | `arene::base::abs` for built-in arithmetic types, otherwise error      |
| `imag`   | Returns `numbers::zero_v<T>` (i.e. treats the argument as purely real) |
| `real`   | Returns the argument (i.e. treats the argument as purely real)         |

### Customization point objects {#subpackages-math-float-property-cpos}

The four floating-point predicates `arene::base::isnan`, `arene::base::isinf`,
`arene::base::isfinite`, and `arene::base::isnormal` are implemented as
customization point objects (CPOs). For arithmetic floating-point types
(`float`, `double`) they evaluate the predicate directly. For other types they
dispatch through argument-dependent lookup to an unqualified `isnan` / `isinf` /
`isfinite` / `isnormal` found in the argument's associated namespaces, allowing
user-defined floating-point-like types to participate.

A user-supplied overload must return `bool`. This is enforced at compile time: a
non-`bool` return triggers a `static_assert`. The CPO itself is not invocable
for types that provide no matching overload, so the predicates can be used in
SFINAE contexts (e.g. with `arene::base::is_invocable_v`).

Example: a wrapper type that participates in the predicates via hidden friends.

```{.cpp}
struct my_float {
  double v;

  friend constexpr auto isnan(my_float x) noexcept -> bool {
    return arene::base::isnan(x.v);
  }
  friend constexpr auto isinf(my_float x) noexcept -> bool {
    return arene::base::isinf(x.v);
  }
  friend constexpr auto isfinite(my_float x) noexcept -> bool {
    return arene::base::isfinite(x.v);
  }
  friend constexpr auto isnormal(my_float x) noexcept -> bool {
    return arene::base::isnormal(x.v);
  }
};

static_assert(arene::base::isnan(my_float{std::numeric_limits<double>::quiet_NaN()}), "");
static_assert(arene::base::isfinite(my_float{1.0}), "");
```

Despite being CPOs, these are not in the `math` namespace because they only make
sense for floating point arguments and are thus not fully generic.

### Complex number decomposition {#subpackages-math-complex-number-decomposition}

The following additional functions are generic customization point objects which
support user-defined math types, and are not floating-point specific:

- `arene::base::math::real` returns the real part of a complex number by calling
  its member or free function `real` implementation; if the type does not
  provide one, then it is assumed to be inherently real and the passed value is
  returned to the caller
- `arene::base::math::imag` returns the imaginary part of a complex number by
  calling its member or free function `imag` implementation; if the type does
  not provide one, then it is assumed to be inherently real and an appropriately
  typed 0 is returned to the caller. Note that this last case may require
  specializing `arene::base::numbers::zero` for the argument type; because the
  `zero_v` accessor binds a reference to `zero<T>::value`, such a specialization
  must also provide a namespace-scope (out-of-line) definition of `value`.

For both of these function objects, the logic for selecting an overload is like
this:

1. If the argument type has **both** a member function `real` and a member
   function `imag`, then both `arene::base::math::real` and
   `arene::base::math::imag` use the corresponding member functions of the
   argument
1. If the argument type does not have **both** `real` and `imag` member
   functions but does have **both** `real` and `imag` free function overloads,
   then both `arene::base::math::real` and `arene::base::math::imag` use the
   corresponding free functions
1. If argument type neither has **both** `real` and `imag` member functions nor
   has **both** `real` and `imag` free function overloads, then the type is
   treated as real

A type trait `arene::base::math::is_complex_v<T>` is provided to check if a
given type would be treated as complex or as real by Arene Base.

## Examples {#subpackages-math-examples}

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
