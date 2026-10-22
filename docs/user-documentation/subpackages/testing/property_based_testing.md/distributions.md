<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page distributions Typed Distributions

<!-- markdownlint-enable MD041 -->

# Introduction {#subpackages-testing-property-based-testing-md-distributions-introduction}

As with the standard library `<random>` header, in order to generate a typed
value, it's necessary to apply a _distribution_ on top of a random bit generator
like \ref prng_xoshiro . If you have access to the standard distributions such
as `std::uniform_int_distribution` they will work fine with this facility, but
Arene Base also provides its own distributions with different features such as
`constexpr` compatibility.

However, while the Arene Base distributions' design and role are conceptually
similar to those of the standard distributions, the former do **not** satisfy
the standard named requirement `RandomNumberDistribution` and thus will not be
suitable as a drop-in replacement for them in all situations.

## Bitwise Uniform Distribution {#subpackages-testing-property-based-testing-md-distributions-bitwise-uniform-distribution}

The bitwise uniform distribution
`arene::base::testing::bitwise_uniform_distribution<T>` is a template which will
return uniformly-distributed values of type `T` when given a
`UniformRandomBitGenerator` such as `prng_xoshiro` or one of the standard PRNGs
from `<random>`.

```cpp
auto rng = arene::base::testing::prng_xoshiro{std::uint64_t{42}};
auto int_dist = arene::base::testing::bitwise_uniform_distribution<std::uint16_t>{};
auto float_dist = arene::base::testing::bitwise_uniform_distribution<float>{};

std::uint16_t const int_value = int_dist(rng);
float const float_value = float_dist(rng);
```

Because the resulting values are **bitwise uniform** rather than **value-wise
uniform**, if the mapping between bit patterns of `T` and semantic values of `T`
is not uniform, then the generated values will not be _semantically_ uniform.
For example, this is the case for floating point types: a random bit pattern
converted to IEEE floating point is more likely to be near zero than far from
it, and more likely to be NaN than any other particular value.

Unlike any of the standard distributions, `bitwise_uniform_distribution` is
`constexpr`-compatible for integral types. For floating-point types it is
`constexpr`-compatible only on toolchains that provide `__builtin_bit_cast`; use
`ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)` to check.

## Default Distribution {#subpackages-testing-property-based-testing-md-distributions-default-distribution}

When a property test macro (such as `ARENE_PROPERTY`) auto-generates a parameter
of type `T`, the runtime consults the `default_distribution<T>` customization
point to decide how to generate the value. By default,
`arene::base::testing::default_distribution<T>` is a default-constructed
`bitwise_uniform_distribution<T>`.

To change the default generation strategy for your own type, specialize
`default_distribution` for that type. For example, to constrain a type's
generated values to a specific distribution:

```cpp
template <>
constexpr auto arene::base::testing::default_distribution<my_type> = my_type_distribution{};
```

Any object satisfying the distribution interface (a call operator accepting a
`UniformRandomBitGenerator` reference and returning a value of the target type)
may be used. Property bodies that need a non-default distribution for a given
parameter can instead invoke a distribution explicitly using the `prng` object
made available inside the body.

## Limitations {#subpackages-testing-property-based-testing-md-distributions-limitations}

- `bitwise_uniform_distribution` currently only supports bit generators whose
  `min()` is zero and `max()` is the maximum value of their `result_type`.
- `bitwise_uniform_distribution` currently does not support setting a minimum or
  maximum value to generate: it only generates across the whole range of the
  value type.
- `bitwise_uniform_distribution` is currently limited to test code only.
