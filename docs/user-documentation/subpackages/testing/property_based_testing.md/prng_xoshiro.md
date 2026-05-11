<!-- Copyright 2026, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page prng_xoshiro Pseudorandom Number Generator (xoshiro256++)

<!-- markdownlint-enable MD041 -->

## Introduction {#introduction}

`prng_xoshiro` is a constexpr-friendly pseudorandom number generator
implementing the xoshiro256++ algorithm, designed by David Blackman and
Sebastiano Vigna. It is intended for use in property-based tests that need
reproducible pseudorandom sequences.

The generator satisfies the C++ `UniformRandomBitGenerator` named requirement,
so it can be used with standard library distributions and algorithms that accept
a URBG.

## Usage {#usage}

### Basic Usage

Construct a generator with a seed and call it to produce 64-bit pseudorandom
values:

```cpp
auto rng = arene::base::testing::prng_xoshiro{std::uint64_t{42}};

auto const first = rng();
auto const second = rng();
```

### Use With Standard Distributions

The generator satisfies `UniformRandomBitGenerator`, so it works with standard
library distributions:

```cpp
auto rng = arene::base::testing::prng_xoshiro{std::uint64_t{99}};
auto dist = std::uniform_int_distribution<int>{0, 100};

auto const value = dist(rng);
```

### Use With Arene Base's Distributions

Arene Base also provides its own typed distributions which can be used in a
similar way to the standard ones, with some additional features and limitations:

```cpp
auto rng = arene::base::testing::prng_xoshiro{std::uint64_t{99}};
auto dist = arene::base::testing::bitwise_uniform_distribution<int>{};

auto const value = dist(rng);
```

See \ref distributions for details on Arene Base's distributions.

## Seeding {#seeding}

A single 64-bit seed is expanded into the full 256-bit internal state using the
SplitMix64 generator. This ensures the internal state is never all-zero (the
only disallowed state) for any seed value.

Different seeds produce different sequences. The same seed always produces the
same sequence across all platforms.

## Limitations {#limitations}

- `prng_xoshiro` is **not** cryptographically secure. It must not be used for
  security-sensitive purposes.
- This PRNG is currently limited to test code only.

## References {#references}

- [xoshiro / xoroshiro generators](https://prng.di.unimi.it/)
- [Reference xoshiro256++ implementation](https://prng.di.unimi.it/xoshiro256plusplus.c)
- [Reference SplitMix64 implementation](https://prng.di.unimi.it/splitmix64.c)
