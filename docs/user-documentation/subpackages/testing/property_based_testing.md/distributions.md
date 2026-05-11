<!-- Copyright 2026, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page distributions Typed Distributions

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

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

## Bitwise Uniform Distribution {#bitwise-uniform-distribution}

The bitwise uniform distribution
`arene::base::testing::bitwise_uniform_distribution<T>` is a template which will
return uniformly-distributed values of type `T` when given a
`UniformRandomBitGenerator` such as `prng_xoshiro` or one of the standard PRNGs
from `<random>`.

```cpp
auto rng = arene::base::testing::prng_xoshiro{std::uint64_t{42}};
auto dist = arene::base::testing::bitwise_uniform_distribution<std::uint16_t>{};

std::uint16_t const value = dist(rng);
```

Because the resulting values are **bitwise uniform** rather than **value-wise
uniform**, if the mapping between bit patterns of `T` and semantic values of `T`
is not uniform, then the generated values will not be _semantically_ uniform.
For example, this is the case for floating point types: a random bit pattern
converted to IEEE floating point is more likely to be near zero than far from
it, and more likely to be NaN than any other particular value.

Unlike any of the standard distributions, `bitwise_uniform_distribution` is
`consexpr-compatible`.

## Limitations {#limitations}

- `bitwise_uniform_distribution` currently only supports bit generators whose
  `min()` is zero and `max()` is the maximum value of their `result_type`.
- `bitwise_uniform_distribution` currently does not support setting a minimum or
  maximum value to generate: it only generates across the whole range of the
  value type.
- `bitwise_uniform_distribution` is currently limited to test code only.
