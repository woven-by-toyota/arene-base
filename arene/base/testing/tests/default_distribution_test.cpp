// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/default_distribution.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

// IWYU pragma: no_include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/testing/tests/sequenced_mock_prng.hpp"

namespace {

using arene::base::testing::bitwise_uniform_distribution;
using arene::base::testing::default_distribution;
using arene::base::testing::tests::sequenced_mock_prng;

/// @brief A user-defined type used to exercise the @c default_distribution customization point.
struct wrapped_int {
  int value;
};

/// @brief A user-defined distribution constraining generated @c wrapped_int values to the range [0, 100).
struct bounded_tagged_distribution {
  /// @brief The value type produced by this distribution.
  using result_type = wrapped_int;

  /// @brief Draw a @c wrapped_int whose value is in [0, 100).
  /// @tparam PRNG A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return A @c wrapped_int with @c value in the range [0, 100)
  template <typename PRNG>
  constexpr auto operator()(PRNG& prng) const noexcept(noexcept(prng())) -> result_type {
    return wrapped_int{static_cast<int>(prng() % 100U)};
  }
};

}  // namespace

// Override the default generation strategy for wrapped_int with the bounded distribution.
//
// A qualified declarator-id is used so the enclosing arene::base::testing namespace need not be reopened.
#ifndef ARENE_DOC_GENERATION_RUNNING
template <>
constexpr auto arene::base::testing::default_distribution<wrapped_int> = bounded_tagged_distribution{};
#endif

namespace {

/// @test The primary @c default_distribution yields the same values a @c bitwise_uniform_distribution would.
TEST(DefaultDistribution, DefaultsToBitwiseUniform) {
  auto default_prng = sequenced_mock_prng<std::uint64_t>{1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U};
  auto bitwise_prng = sequenced_mock_prng<std::uint64_t>{1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U};

  auto const bitwise = bitwise_uniform_distribution<std::uint32_t>{};

  for (std::size_t i{}; i < 8U; ++i) {
    EXPECT_EQ(default_distribution<std::uint32_t>(default_prng), bitwise(bitwise_prng));
  }
}

/// @test A user specialization of @c default_distribution is selected instead of the primary template.
TEST(DefaultDistribution, UserSpecializationSelected) {
  auto prng = sequenced_mock_prng<std::uint64_t>{0U, 42U, 250U, 999U};

  for (std::size_t i{}; i < 4U; ++i) {
    auto const generated = default_distribution<wrapped_int>(prng);
    ASSERT_GE(generated.value, 0);
    ASSERT_LT(generated.value, 100);
  }
}

/// @test A property macro auto-generates a parameter using the user's @c default_distribution specialization.
ARENE_PROPERTY(DefaultDistributionProperty, UsesUserSpecialization, (wrapped_int value)) {
  ARENE_PROP_ASSERT(value.value >= 0);
  ARENE_PROP_ASSERT(value.value < 100);
}

}  // namespace
