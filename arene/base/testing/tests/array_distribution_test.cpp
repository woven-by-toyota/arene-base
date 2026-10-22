// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/array_distribution.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/tests/sequenced_mock_prng.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::testing::array_distribution;
using arene::base::testing::bitwise_uniform_distribution;
using arene::base::testing::tests::sequenced_mock_prng;

/// @brief A distribution returning each PRNG draw plus a fixed offset
class offset_distribution {
 public:
  using result_type = std::uint32_t;

  constexpr offset_distribution() noexcept = default;

  explicit constexpr offset_distribution(result_type offset) noexcept
      : offset_{offset} {}

  template <class PRNG>
  constexpr auto operator()(PRNG& prng) const noexcept -> result_type {
    return static_cast<result_type>(prng() + offset_);
  }

 private:
  result_type offset_{};
};

/// @brief An element type whose default constructor is not noexcept, but whose other special members are
struct throwing_default_value {
  throwing_default_value() noexcept(false) { static_cast<void>(0); }
};

/// @brief An element type whose move constructor is not noexcept, but whose other special members are
using throwing_move_value = ::testing::configurable_value<std::int32_t, ::testing::throws_on::move_construct>;

/// @brief An element type whose move assignment is not noexcept, but whose other special members are
using throwing_move_assign_value = ::testing::configurable_value<std::int32_t, ::testing::throws_on::move_assign>;

/// @brief A distribution whose draw is noexcept and which produces the given element type
/// @tparam T The element type produced
template <class T>
struct noexcept_distribution_of {
  using result_type = T;

  template <class PRNG>
  auto operator()(PRNG&) const noexcept -> result_type;
};

/// @brief A distribution whose draw is not noexcept and which produces the given element type
/// @tparam T The element type produced
template <class T>
struct throwing_distribution_of {
  using result_type = T;

  template <class PRNG>
  auto operator()(PRNG&) const -> result_type;
};

/// @brief A distribution with no default constructor
struct non_default_constructible_distribution {
  using result_type = std::uint32_t;

  explicit non_default_constructible_distribution(int);

  template <class PRNG>
  auto operator()(PRNG&) const -> result_type;
};

/// @test Drawing an array is noexcept when the wrapped draw and the element special members are noexcept
TEST(ArrayDistributionNoexcept, TrueIfDrawAndElementSpecialMembersAreNoexcept) {
  using distribution_type = array_distribution<bitwise_uniform_distribution<std::uint32_t>, 3U>;

  STATIC_ASSERT_TRUE(
      noexcept(std::declval<distribution_type const&>()(std::declval<sequenced_mock_prng<std::uint32_t>&>()))
  );
}

/// @test Drawing an array is not noexcept when the wrapped distribution's draw is not noexcept
TEST(ArrayDistributionNoexcept, FalseIfDrawIsNotNoexcept) {
  using distribution_type = array_distribution<throwing_distribution_of<std::uint32_t>, 3U>;

  STATIC_ASSERT_FALSE(
      noexcept(std::declval<distribution_type const&>()(std::declval<sequenced_mock_prng<std::uint32_t>&>()))
  );
}

/// @test Drawing an array is not noexcept when the element default constructor is not noexcept
TEST(ArrayDistributionNoexcept, FalseIfElementDefaultConstructionIsNotNoexcept) {
  using distribution_type = array_distribution<noexcept_distribution_of<throwing_default_value>, 3U>;

  STATIC_ASSERT_FALSE(
      noexcept(std::declval<distribution_type const&>()(std::declval<sequenced_mock_prng<std::uint32_t>&>()))
  );
}

/// @test Drawing an array is not noexcept when the element move constructor is not noexcept
TEST(ArrayDistributionNoexcept, FalseIfElementMoveConstructionIsNotNoexcept) {
  using distribution_type = array_distribution<noexcept_distribution_of<throwing_move_value>, 3U>;

  STATIC_ASSERT_FALSE(
      noexcept(std::declval<distribution_type const&>()(std::declval<sequenced_mock_prng<std::uint32_t>&>()))
  );
}

/// @test Drawing an array is not noexcept when the element move assignment is not noexcept
TEST(ArrayDistributionNoexcept, FalseIfElementMoveAssignmentIsNotNoexcept) {
  using distribution_type = array_distribution<noexcept_distribution_of<throwing_move_assign_value>, 3U>;

  STATIC_ASSERT_FALSE(
      noexcept(std::declval<distribution_type const&>()(std::declval<sequenced_mock_prng<std::uint32_t>&>()))
  );
}

/// @test The adaptor is default constructible only when the wrapped distribution is
TEST(ArrayDistributionTest, DefaultConstructibleOnlyIfWrappedDistributionIs) {
  STATIC_ASSERT_TRUE(std::is_default_constructible<array_distribution<offset_distribution, 2U>>::value);
  STATIC_ASSERT_FALSE(
      std::is_default_constructible<array_distribution<non_default_constructible_distribution, 2U>>::value
  );
}

/// @test The adaptor exposes an @c N element array of the wrapped distribution's result type as @c result_type
CONSTEXPR_TEST(ArrayDistributionTest, ExposesResultType) {
  using distribution_type = array_distribution<bitwise_uniform_distribution<std::uint32_t>, 3U>;

  CONSTEXPR_ASSERT((std::is_same<typename distribution_type::result_type, arene::base::array<std::uint32_t, 3U>>::value)
  );
}

/// @test Every element of the produced array is a separate draw, taken in ascending index order
TEST(ArrayDistributionTest, DrawsEachElementInIndexOrder) {
  auto const dist = array_distribution<bitwise_uniform_distribution<std::uint32_t>, 4U>{};
  sequenced_mock_prng<std::uint32_t> prng{10U, 20U, 30U, 40U};

  auto const values = dist(prng);

  EXPECT_EQ(values[0], 10U);
  EXPECT_EQ(values[1], 20U);
  EXPECT_EQ(values[2], 30U);
  EXPECT_EQ(values[3], 40U);
}

/// @test A single-element array is supported
TEST(ArrayDistributionTest, SupportsSingleElement) {
  auto const dist = array_distribution<bitwise_uniform_distribution<std::uint32_t>, 1U>{};
  sequenced_mock_prng<std::uint32_t> prng{7U};

  auto const values = dist(prng);

  EXPECT_EQ(values[0], 7U);
}

/// @test A zero-element array is supported and draws nothing from the PRNG
TEST(ArrayDistributionTest, SupportsZeroElements) {
  auto const dist = array_distribution<bitwise_uniform_distribution<std::uint32_t>, 0U>{};
  // Two distinct values, so that a spurious draw would advance the PRNG past the first one
  sequenced_mock_prng<std::uint32_t> prng{7U, 8U};

  auto const values = dist(prng);

  EXPECT_EQ(values.size(), 0U);
  EXPECT_EQ(prng(), 7U);
}

/// @test The PRNG is used by reference, so a second call yields the next values rather than repeating the first array
TEST(ArrayDistributionTest, SuccessiveCallsAdvanceThePrng) {
  auto const dist = array_distribution<bitwise_uniform_distribution<std::uint32_t>, 2U>{};
  sequenced_mock_prng<std::uint32_t> prng{1U, 2U, 3U, 4U};

  auto const first = dist(prng);
  auto const second = dist(prng);

  EXPECT_EQ(first[0], 1U);
  EXPECT_EQ(first[1], 2U);
  EXPECT_EQ(second[0], 3U);
  EXPECT_EQ(second[1], 4U);
}

/// @test The wrapped distribution instance is used
TEST(ArrayDistributionTest, UsesTheSuppliedDistributionInstance) {
  auto const dist = array_distribution<offset_distribution, 2U>{offset_distribution{100U}};
  sequenced_mock_prng<std::uint32_t> prng{1U, 2U};

  auto const values = dist(prng);

  EXPECT_EQ(values[0], 101U);
  EXPECT_EQ(values[1], 102U);
}

}  // namespace
