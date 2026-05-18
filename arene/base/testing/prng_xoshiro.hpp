// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PRNG_XOSHIRO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PRNG_XOSHIRO_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief pseudorandom number generator implementing the xoshiro256++ algorithm
///
/// xoshiro256++ is a general-purpose PRNG designed by David Blackman and
/// Sebastiano Vigna. It offers sub-nanosecond speed, a 256-bit state space
/// large enough for parallel applications, and passes all known statistical
/// test suites.
///
/// The generator satisfies the C++ UniformRandomBitGenerator named
/// requirement, so it can be used with standard library distributions
/// and algorithms that accept a URBG.
///
/// A single 64-bit seed is expanded into the full 256-bit internal state
/// using the SplitMix64 generator, as recommended by the xoshiro authors.
/// This ensures the state is never all-zero (the only disallowed state)
/// for any seed value.
///
/// @see https://prng.di.unimi.it/
/// @see https://prng.di.unimi.it/xoshiro256plusplus.c
class prng_xoshiro {
  /// @brief internal state of the generator
  array<std::uint64_t, 4> state_{};

  /// @brief bitwise rotate a value to the left
  /// @tparam N number of bit positions to rotate
  /// @param arg value to rotate.
  /// @return rotated value
  template <std::uint8_t N>
  static constexpr auto rotate_left(std::uint64_t arg) -> std::uint64_t {
    constexpr auto upper_limit = 64U;
    static_assert(N < upper_limit, "rotation amount must be less than 64");

    return N == 0U ? arg : ((arg << N) | (arg >> (upper_limit - N)));
  }

  /// @brief 64-bit PRNG used to expand a single seed into the four 64-bit state
  ///   words required by xoshiro256++
  /// @see https://prng.di.unimi.it/splitmix64.c
  struct splitmix64 {
    /// @brief splimix64 state
    std::uint64_t state{};

    /// @brief advance the generator by one step and return the
    ///   scrambled output
    /// @return next 64-bit pseudorandom value
    constexpr auto next() noexcept -> std::uint64_t {
      // fixed constants specified by the algorithm description
      // NOLINTBEGIN(readability-magic-numbers)
      auto mixed = (state += std::uint64_t{0x9e3779b97f4a7c15ULL});
      mixed = (mixed ^ (mixed >> 30U)) * std::uint64_t{0xbf58476d1ce4e5b9ULL};
      mixed = (mixed ^ (mixed >> 27U)) * std::uint64_t{0x94d049bb133111ebULL};
      return mixed ^ (mixed >> 31U);
      // NOLINTEND(readability-magic-numbers)
    }
  };

 public:
  /// @brief seed argument type
  using seed_type = std::uint64_t;

  /// @brief prng result type
  using result_type = std::uint64_t;

  /// @brief construct the generator with an explicit seed
  /// @param initial_seed initial seed value
  constexpr explicit prng_xoshiro(seed_type initial_seed) noexcept {
    auto seeder = splitmix64{initial_seed};
    for (auto& state : state_) {
      state = seeder.next();
    }
  }

  /// @brief construct the generator with a seed of zero
  constexpr prng_xoshiro() noexcept
      : prng_xoshiro{seed_type{}} {}

  /// @brief generate the next pseudorandom value and advance the
  ///   internal state
  /// @return a uniformly distributed 64-bit pseudorandom value
  ///   in the range [min(), max()]
  /// @see https://prng.di.unimi.it/xoshiro256plusplus.c
  constexpr auto operator()() noexcept -> result_type {
    // fixed constants specified by the algorithm description
    // NOLINTBEGIN(readability-magic-numbers)
    auto const result = rotate_left<23>(state_[0] + state_[3]) + state_[0];

    auto const shifted = state_[1] << 17U;

    state_[2] ^= state_[0];
    state_[3] ^= state_[1];
    state_[1] ^= state_[2];
    state_[0] ^= state_[3];

    state_[2] ^= shifted;

    state_[3] = rotate_left<45>(state_[3]);
    // NOLINTEND(readability-magic-numbers)

    return result;
  }

  /// @brief smallest value that @c operator() can return
  /// @return zero
  static constexpr auto min() noexcept -> result_type { return {}; }

  /// @brief largest value that @c operator() can return
  /// @return ~min()
  static constexpr auto max() noexcept -> result_type { return ~min(); }
};

}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PRNG_XOSHIRO_HPP_
