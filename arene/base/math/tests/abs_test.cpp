// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/abs.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <cmath>
#endif

namespace {
template <typename T>
class AbsIntegerTypedTest : public testing::Test {
 public:
  using type = std::remove_reference_t<T>;
};

template <typename T>
class AbsUnsignedIntegerTypedTest : public testing::Test {
 public:
  using type = std::remove_reference_t<T>;
};

template <typename T>
class AbsFloatingTypedTest : public testing::Test {
 public:
  using type = std::remove_reference_t<T>;
};

using integer_types = arene::base::type_lists::remove_duplicates_t<
    ::testing::Types<std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::intmax_t>>;
using unsigned_integer_types = arene::base::type_lists::remove_duplicates_t<
    ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, std::uintmax_t>>;
using floating_types = ::testing::Types<float, double>;

TYPED_TEST_SUITE(AbsIntegerTypedTest, integer_types, );
TYPED_TEST_SUITE(AbsUnsignedIntegerTypedTest, unsigned_integer_types, );
TYPED_TEST_SUITE(AbsFloatingTypedTest, floating_types, );

/// @test `abs` returns the expected values with integer types.
TYPED_TEST(AbsIntegerTypedTest, IntegerNumber) {
  using value_type = typename TestFixture::type;
  STATIC_ASSERT_EQ(arene::base::abs(value_type{0}), value_type{0});
  STATIC_ASSERT_EQ(arene::base::abs(-1 * value_type{0}), value_type{0});
  STATIC_ASSERT_EQ(arene::base::abs(value_type{42}), value_type{42});
  STATIC_ASSERT_EQ(arene::base::abs(value_type{-42}), value_type{42});
  STATIC_ASSERT_EQ(arene::base::abs(std::numeric_limits<value_type>::max()), std::numeric_limits<value_type>::max());
  STATIC_ASSERT_EQ(
      arene::base::abs(std::numeric_limits<value_type>::min() + value_type{1}),
      std::numeric_limits<value_type>::max()
  );
  // Close to boundary cases
  STATIC_ASSERT_EQ(
      arene::base::abs(std::numeric_limits<value_type>::max() - value_type{1}),
      std::numeric_limits<value_type>::max() - value_type{1}
  );
  STATIC_ASSERT_EQ(
      arene::base::abs(std::numeric_limits<value_type>::min() + value_type{2}),
      std::numeric_limits<value_type>::max() - value_type{1}
  );
}

/// @test `abs` returns the expected values with unsigned integer types.
TYPED_TEST(AbsUnsignedIntegerTypedTest, UnsignedIntegerNumber) {
  using value_type = typename TestFixture::type;
  STATIC_ASSERT_EQ(arene::base::abs(value_type{0}), value_type{0});
  STATIC_ASSERT_EQ(arene::base::abs(value_type{42}), value_type{42});
  STATIC_ASSERT_EQ(arene::base::abs(std::numeric_limits<value_type>::max()), std::numeric_limits<value_type>::max());
  STATIC_ASSERT_EQ(arene::base::abs(std::numeric_limits<value_type>::min()), std::numeric_limits<value_type>::min());
}

/// @test `abs` returns the expected values with floating types, within an allowed floating point calculation error,
/// and including NaN and Infinity.
TYPED_TEST(AbsFloatingTypedTest, FloatingNumber) {
  using value_type = typename TestFixture::type;
  constexpr auto eps = std::numeric_limits<value_type>::epsilon();
  STATIC_ASSERT_NEAR(arene::base::abs(value_type{0}), value_type{0}, eps);
  STATIC_ASSERT_NEAR(arene::base::abs(value_type{-1} * value_type{0}), value_type{0}, eps);

  STATIC_ASSERT_NEAR(arene::base::abs(std::numeric_limits<value_type>::denorm_min()), value_type{0}, eps);
  STATIC_ASSERT_NEAR(
      arene::base::abs(value_type{-1} * std::numeric_limits<value_type>::denorm_min()),
      value_type{0},
      eps
  );

  // The following flag is disabled because we want to make sure the result of abs of denorm_min is not
  // flush to zero

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");
  STATIC_ASSERT_NE(arene::base::abs(value_type{-1} * std::numeric_limits<value_type>::denorm_min()), 0.0);
  STATIC_ASSERT_NE(arene::base::abs(std::numeric_limits<value_type>::denorm_min()), 0.0);

  STATIC_ASSERT_NE(arene::base::abs(value_type{2.802597e-45F}), 0);
  STATIC_ASSERT_NE(arene::base::abs(-value_type{2.802597e-45F}), 0);
  STATIC_ASSERT_NE(arene::base::abs(value_type{4.203895e-45F}), 0);
  STATIC_ASSERT_NE(arene::base::abs(-value_type{4.203895e-45F}), 0);
  STATIC_ASSERT_NE(arene::base::abs(value_type{5.605194e-45F}), 0);
  STATIC_ASSERT_NE(arene::base::abs(-value_type{5.605194e-45F}), 0);
  STATIC_ASSERT_NE(arene::base::abs(value_type{7.006492e-45F}), 0);
  STATIC_ASSERT_NE(-arene::base::abs(value_type{7.006492e-45F}), 0);
  STATIC_ASSERT_NE(arene::base::abs(value_type{8.407791e-45F}), 0);
  STATIC_ASSERT_NE(-arene::base::abs(value_type{8.407791e-45F}), 0);

  // Test that the absolute value of -0.0 is 0.0
  STATIC_ASSERT_EQ(arene::base::abs(value_type{-0.0F}), 0.0);
  STATIC_ASSERT_EQ(arene::base::abs(value_type{0.0F}), 0.0);

  ARENE_IGNORE_END();

  STATIC_ASSERT_NEAR(arene::base::abs(value_type{42}), value_type{42}, eps);
  STATIC_ASSERT_NEAR(arene::base::abs(value_type{-42}), value_type{42}, eps);
  // Workaround since std::numeric_limits<value_type>::max() evaluates to inf for long double
  STATIC_ASSERT_NEAR(
      arene::base::abs(std::numeric_limits<value_type>::max()) - std::numeric_limits<value_type>::max(),
      value_type{0},
      eps
  );
  STATIC_ASSERT_NEAR(
      arene::base::abs(value_type{-1.0} * std::numeric_limits<value_type>::max()) -
          std::numeric_limits<value_type>::max(),
      value_type{0},
      eps
  );
  // Close to boundary cases
  STATIC_ASSERT_NEAR(
      arene::base::abs(std::numeric_limits<value_type>::epsilon()),
      std::numeric_limits<value_type>::epsilon(),
      eps
  );
  STATIC_ASSERT_NEAR(
      arene::base::abs(value_type{-1.0} * std::numeric_limits<value_type>::epsilon()),
      std::numeric_limits<value_type>::epsilon(),
      eps
  );

  STATIC_ASSERT_TRUE(arene::base::isnan(arene::base::abs(std::numeric_limits<value_type>::quiet_NaN())));
  STATIC_ASSERT_TRUE(arene::base::isnan(arene::base::abs(-std::numeric_limits<value_type>::quiet_NaN())));

  STATIC_ASSERT_TRUE(arene::base::isinf(arene::base::abs(std::numeric_limits<value_type>::infinity())));
  STATIC_ASSERT_TRUE(arene::base::isinf(arene::base::abs(-std::numeric_limits<value_type>::infinity())));

  // This section will need to be revisited if std::isinf and std::signbit are provided by Arene Base
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  // signbit are not constexpr in C++14, thus cannot test with STATIC_ASSERT_X
  ASSERT_FALSE(std::signbit(arene::base::abs(std::numeric_limits<value_type>::quiet_NaN())));
  ASSERT_FALSE(std::signbit(arene::base::abs(-std::numeric_limits<value_type>::quiet_NaN())));
#endif
}
}  // namespace
