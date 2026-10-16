// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/testing/property_config.hpp"
#include "arene/base/testing/tests/property_config_env_helpers.hpp"

// NOLINTBEGIN(concurrency-mt-unsafe)
namespace {

using arene::base::testing::tests::clear_property_env;
using arene::base::testing::tests::setenv;

constexpr auto required_successes_env = arene::base::testing::required_successes_env_name;
constexpr auto max_discards_env = arene::base::testing::max_discards_env_name;
constexpr auto seed_env = arene::base::testing::seed_env_name;

// 2^64 = 18446744073709551616, one past the uint64_t maximum.
constexpr auto* one_past_uint64_max = "18446744073709551616";

/// @brief Fixture that clears all three property-config env vars before each
/// test and restores their prior values after.
///
/// The clear/restore is done in @c SetUp / @c TearDown rather than via a data
/// member so that it runs per-test on minitest, which constructs the fixture
/// instance once at program start.
class ReadPropertyConfigDeath : public ::testing::Test {
  arene::base::optional<clear_property_env> clear_env_{};

 protected:
  auto SetUp() -> void override { clear_env_.emplace(); }
  auto TearDown() -> void override { clear_env_.reset(); }

  /// @brief Set @p env_var to @p value and assert that @c read_property_config aborts with a precondition violation.
  ///
  /// Defined as a fixture member so that @c ASSERT_DEATH expands inside a member function (required by minitest, which
  /// expands to @c this->flush_output() ). Under googletest the macro body does not reference @c this , so clang-tidy
  /// would suggest making this method static but that would break the minitest expansion.
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  auto expect_aborts_for(arene::base::null_terminated_string_view env_var, char const* value) noexcept -> void {
    std::ignore = setenv(env_var, value);

    ASSERT_DEATH(std::ignore = arene::base::testing::read_property_config(), "Precondition");
  }
};

/// @test an empty value for any of the property-config env vars violates the parser's non-empty precondition.
TEST_F(ReadPropertyConfigDeath, EmptyValueAborts) {
  expect_aborts_for(required_successes_env, "");
  expect_aborts_for(max_discards_env, "");
  expect_aborts_for(seed_env, "");
}

/// @test an alphabetic value for any of the property-config env vars violates the parser's digit-range precondition.
TEST_F(ReadPropertyConfigDeath, AlphaValueAborts) {
  expect_aborts_for(required_successes_env, "abc");
  expect_aborts_for(max_discards_env, "abc");
  expect_aborts_for(seed_env, "abc");
}

/// @test a value with a leading space for any of the property-config env vars violates the parser's digit-range
/// precondition.
TEST_F(ReadPropertyConfigDeath, LeadingSpaceValueAborts) {
  expect_aborts_for(required_successes_env, " 10");
  expect_aborts_for(max_discards_env, " 10");
  expect_aborts_for(seed_env, " 10");
}

/// @test a value with trailing non-digit characters for any of the property-config env vars violates the parser's
/// digit-range precondition.
TEST_F(ReadPropertyConfigDeath, TrailingGarbageValueAborts) {
  expect_aborts_for(required_successes_env, "12x");
  expect_aborts_for(max_discards_env, "12x");
  expect_aborts_for(seed_env, "12x");
}

/// @test a value with a leading minus sign for any of the property-config env vars violates the parser's digit-range
/// precondition.
TEST_F(ReadPropertyConfigDeath, LeadingMinusValueAborts) {
  expect_aborts_for(required_successes_env, "-1");
  expect_aborts_for(max_discards_env, "-1");
  expect_aborts_for(seed_env, "-1");
}

/// @test a value with a leading zero (other than the literal @c "0") for any of the property-config env vars violates
/// the parser's no-leading-zero precondition.
TEST_F(ReadPropertyConfigDeath, LeadingZeroValueAborts) {
  expect_aborts_for(required_successes_env, "00");
  expect_aborts_for(max_discards_env, "0042");
  expect_aborts_for(seed_env, "0000042");
}

/// @test a value strictly greater than @c std::numeric_limits<std::uint64_t>::max() for any of the property-config env
/// vars violates the parser's overflow precondition.
TEST_F(ReadPropertyConfigDeath, OverflowingValueAborts) {
  expect_aborts_for(required_successes_env, one_past_uint64_max);
  expect_aborts_for(max_discards_env, one_past_uint64_max);
  expect_aborts_for(seed_env, one_past_uint64_max);
}

}  // namespace
// NOLINTEND(concurrency-mt-unsafe)
