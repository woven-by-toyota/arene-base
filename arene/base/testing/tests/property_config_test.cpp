// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/property_config.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/testing/tests/property_config_env_helpers.hpp"

// NOLINTBEGIN(concurrency-mt-unsafe)
namespace {

using arene::base::testing::tests::clear_property_env;
using arene::base::testing::tests::setenv;

constexpr auto required_successes_env = arene::base::testing::required_successes_env_name;
constexpr auto max_discards_env = arene::base::testing::max_discards_env_name;
constexpr auto seed_env = arene::base::testing::seed_env_name;

constexpr auto default_required_successes = std::size_t{5000};
constexpr auto default_max_discards = std::size_t{100U};
constexpr auto default_seed = std::uint64_t{0U};

/// @brief Fixture that clears all three property-config env vars before each
/// test and restores their prior values after.
///
/// The clear/restore is done in @c SetUp / @c TearDown rather than via a data
/// member so that it runs per-test on minitest, which constructs the fixture
/// instance once at program start.
class ReadPropertyConfig : public ::testing::Test {
  arene::base::optional<clear_property_env> clear_env_{};

 protected:
  auto SetUp() -> void override { clear_env_.emplace(); }
  auto TearDown() -> void override { clear_env_.reset(); }
};

/// @test all three environment variables unset yields the documented defaults.
TEST_F(ReadPropertyConfig, ReturnsDefaultsWhenAllUnset) {
  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.required_successes, default_required_successes);
  EXPECT_EQ(config.max_discards, default_max_discards);
  EXPECT_EQ(config.seed, default_seed);
}

/// @test @c ARENE_PROP_PARAM_REQUIRED_SUCCESSES overrides the default.
TEST_F(ReadPropertyConfig, RequiredSuccessesFromEnv) {
  std::ignore = setenv(required_successes_env, "5001");

  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.required_successes, 5001U);
  EXPECT_EQ(config.max_discards, default_max_discards);
  EXPECT_EQ(config.seed, default_seed);
}

/// @test @c ARENE_PROP_PARAM_MAX_DISCARDS overrides the default.
TEST_F(ReadPropertyConfig, MaxDiscardsFromEnv) {
  std::ignore = setenv(max_discards_env, "7");

  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.required_successes, default_required_successes);
  EXPECT_EQ(config.max_discards, 7U);
  EXPECT_EQ(config.seed, default_seed);
}

/// @test @c ARENE_PROP_PARAM_SEED overrides the default.
TEST_F(ReadPropertyConfig, SeedFromEnv) {
  std::ignore = setenv(seed_env, "42");

  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.required_successes, default_required_successes);
  EXPECT_EQ(config.max_discards, default_max_discards);
  EXPECT_EQ(config.seed, 42U);
}

/// @test all three environment variables can be set together.
TEST_F(ReadPropertyConfig, AllThreeSetTogether) {
  std::ignore = setenv(required_successes_env, "2000");
  std::ignore = setenv(max_discards_env, "150");
  std::ignore = setenv(seed_env, "123456789");

  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.required_successes, 2000U);
  EXPECT_EQ(config.max_discards, 150U);
  EXPECT_EQ(config.seed, 123456789U);
}

/// @test the seed parser accepts the full @c std::uint64_t range.
TEST_F(ReadPropertyConfig, SeedAcceptsMaxUint64) {
  std::ignore = setenv(seed_env, "18446744073709551615");

  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.seed, std::numeric_limits<std::uint64_t>::max());
}

/// @test the literal value @c "0" is a valid (non-leading-zero) input.
TEST_F(ReadPropertyConfig, ZeroAccepted) {
  std::ignore = setenv(seed_env, "0");

  auto const config = arene::base::testing::read_property_config();

  EXPECT_EQ(config.seed, 0U);
}

}  // namespace
// NOLINTEND(concurrency-mt-unsafe)
