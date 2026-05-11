#include "arene/base/testing/run_property.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

// IWYU pragma: no_include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/testing/prng_xoshiro.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_config.hpp"
#include "arene/base/testing/property_error.hpp"
// IWYU pragma: no_include "arene/base/testing/property_summary.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

using arene::base::testing::property_config;
using arene::base::testing::property_error;
using arene::base::testing::run_property;
using error_code = property_error::error_code;

// the property test macros will generate callables with a specific prng argument type
using prng_type = arene::base::testing::prng_xoshiro&;

/// @test @c run_property uses the seed supplied via @c property_config and
/// propagates it
TEST(TestingRunProperty, UsesConfigSeed) {
  for (auto seed : std::initializer_list<std::uint64_t>{0U, 1U, 2U, 42U}) {
    auto const summary = run_property([](prng_type&) { ARENE_PROP_ASSERT(true); }, {1U, 0U, seed});

    ASSERT_EQ(summary.successes, 1U);
    ASSERT_EQ(summary.discards, 0U);
    ASSERT_EQ(summary.failed, false);
    ASSERT_EQ(summary.seed, seed);
    ASSERT_EQ(summary.first_failure.has_value(), false);
  }
}

/// @test @c run_property can be used with a callable that specifies multiple arguments
/// @note How those arguments are created is currently unspecified
TEST(TestingRunProperty, UsableWithCallableThatHasArguments) {
  std::ignore = run_property(  //
      [](prng_type&, int, double, char) { ARENE_PROP_ASSERT(true); },
      {0U, 0U, 0U}
  );
}

/// @test @c run_property reports failure if a trial fails
TEST(TestingRunProperty, ObservesFailure) {
  constexpr auto config = property_config{100U, 0U, 42U};

  for (auto const passing_trials : std::initializer_list<std::size_t>{0, 1, 2, 3, 4, 5}) {
    auto i = passing_trials;
    auto const summary = run_property(
        [&i](prng_type&) {
          auto const j = i--;
          ARENE_PROP_ASSERT(j);
        },
        config
    );

    ASSERT_EQ(summary.successes, passing_trials);
    ASSERT_EQ(summary.discards, 0U);
    ASSERT_EQ(summary.failed, true);
    ASSERT_EQ(summary.seed, config.seed);
    ASSERT_EQ(summary.first_failure.has_value(), true);
  }
}

/// @test @c run_property specifies the first property assertion that fails
TEST(TestingRunProperty, FirstFailingPropertyAssertion) {
  constexpr auto config = property_config{1U, 0U, 42U};

  auto const summary = run_property(
      [](prng_type&) {
        ARENE_PROP_ASSERT(true);
        ARENE_PROP_ASSERT(false);
        ARENE_PROP_ASSERT(false);
      },
      config
  );

  ASSERT_EQ(summary.successes, 0U);
  ASSERT_EQ(summary.discards, 0U);
  ASSERT_EQ(summary.failed, true);
  ASSERT_EQ(summary.seed, config.seed);

  ASSERT_EQ((*summary.first_failure).error, error_code::failure);
  ASSERT_EQ((*summary.first_failure).location.line, 80U);
}

/// @test @c run_property reports failure if max_discards are reached before required_successes
TEST(TestingRunProperty, MaxDiscardsReached) {
  for (auto const discard_limit : std::initializer_list<std::size_t>{0, 1, 2, 3, 4, 5}) {
    auto const config = property_config{100U, discard_limit, 42U};

    auto i = 0U;
    auto const discard_if_odd = [&i](prng_type&) {
      ARENE_PROP_PRE(i++ % 2U == 0U);
      ARENE_PROP_ASSERT(true);
    };

    auto const summary = run_property(discard_if_odd, config);

    ASSERT_EQ(summary.successes, i / 2U);
    ASSERT_EQ(summary.discards, config.max_discards + 1U);
    ASSERT_EQ(summary.failed, true);
    ASSERT_EQ(summary.seed, config.seed);
    ASSERT_EQ(summary.first_failure.has_value(), false);
  }
}

/// @test @c run_property reports success if required_successes are reached before max_discards
TEST(TestingRunProperty, RequiredSuccessesReached) {
  for (auto const required_successes : std::initializer_list<std::size_t>{0, 1, 2, 3, 4, 5}) {
    auto const config = property_config{required_successes, 100U, 42U};

    auto i = 0U;
    auto const discard_if_odd = [&i](prng_type&) {
      ARENE_PROP_PRE(i++ % 2U == 0U);
      ARENE_PROP_ASSERT(true);
    };

    auto const summary = run_property(discard_if_odd, config);

    EXPECT_EQ(summary.successes, config.required_successes);
    EXPECT_EQ(summary.discards, i / 2U);
    EXPECT_EQ(summary.failed, false);
    EXPECT_EQ(summary.seed, config.seed);
    EXPECT_EQ(summary.first_failure.has_value(), false);
  }
}

}  // namespace

// NOLINTEND(readability-identifier-length)
