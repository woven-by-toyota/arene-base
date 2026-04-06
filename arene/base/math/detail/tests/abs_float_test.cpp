// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This file conditionally includes either `builtin_abs_float.hpp` or
// `custom_abs_float.hpp` based on the macro ARENE_BASE_MATH_DETAIL_USE_BUILTIN_ABS_FLOAT.

// Note that this macro is only used for testing purpose

#include <gtest/gtest.h>

// This _must_ come before any other includes from compiler_support or else symbols will be redefined.

#if defined(ARENE_BASE_MATH_DETAIL_USE_BUILTIN_ABS_FLOAT)
#include "arene/base/math/detail/builtin_abs_float.hpp"  // IWYU pragma: keep
#else
#undef ARENE_HAS_BUILTIN_FABS_I_
#undef ARENE_HAS_BUILTIN_FABSF_I_
#define ARENE_HAS_BUILTIN_FABS_I_ ARENE_OFF_BY_DEFAULT
#define ARENE_HAS_BUILTIN_FABSF_I_ ARENE_OFF_BY_DEFAULT
#include "arene/base/math/detail/custom_abs_float.hpp"  // IWYU pragma: keep
#endif

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/float_sign.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/testing/gtest.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <cmath>
#endif

namespace {

template <typename T>
class AbsFloatingTypedTest : public testing::Test {
 public:
  using type = std::remove_reference_t<T>;
};

using floating_types = ::testing::Types<float, double>;

TYPED_TEST_SUITE(AbsFloatingTypedTest, floating_types, );

/// @test Test detailed behavior of `abs` with floating point numbers
TYPED_TEST(AbsFloatingTypedTest, FloatingNumber) {
  using value_type = typename TestFixture::type;
  constexpr auto eps = std::numeric_limits<value_type>::epsilon();
  STATIC_ASSERT_NEAR(arene::base::abs(static_cast<value_type>(0)), static_cast<value_type>(0), eps);
  STATIC_ASSERT_NEAR(
      arene::base::abs(static_cast<value_type>(-1.0) * static_cast<value_type>(0)),
      static_cast<value_type>(0),
      eps
  );

  STATIC_ASSERT_NEAR(
      arene::base::abs(static_cast<value_type>(std::numeric_limits<value_type>::denorm_min())),
      static_cast<value_type>(0.0),
      eps
  );
  STATIC_ASSERT_NEAR(
      arene::base::abs(
          static_cast<value_type>(static_cast<value_type>(-1.0) * std::numeric_limits<value_type>::denorm_min())
      ),
      static_cast<value_type>(0.0),
      eps
  );

  // The following flag is disabled because we want to make sure the result of abs of denorm_min is not
  // flushed to zero
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

  STATIC_ASSERT_NE(
      arene::base::abs(static_cast<value_type>(-1.0 * std::numeric_limits<value_type>::denorm_min())),
      static_cast<value_type>(0.0)
  );
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(std::numeric_limits<value_type>::denorm_min())), 0.0);

  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(2.802597e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(-1.0 * 2.802597e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(4.203895e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(-1.0 * 4.203895e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(5.605194e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(-1.0 * 5.605194e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(7.006492e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(-1.0 * 7.006492e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(8.407791e-45)), 0);
  STATIC_ASSERT_NE(arene::base::abs(static_cast<value_type>(-1.0 * 8.407791e-45)), 0);

  // Test that the absolute value of -0.0 is 0.0
  STATIC_ASSERT_EQ(arene::base::abs(static_cast<value_type>(-0.0)), 0.0);
  STATIC_ASSERT_EQ(arene::base::abs(static_cast<value_type>(0.0)), 0.0);

  ARENE_IGNORE_END();

  STATIC_ASSERT_NEAR(arene::base::abs(static_cast<value_type>(42)), static_cast<value_type>(42), eps);
  STATIC_ASSERT_NEAR(arene::base::abs(static_cast<value_type>(-42)), static_cast<value_type>(42), eps);
  // Workaround since std::numeric_limits<value_type>::max() evaluates to inf for long double
  STATIC_ASSERT_NEAR(
      arene::base::abs(std::numeric_limits<value_type>::max()) - std::numeric_limits<value_type>::max(),
      static_cast<value_type>(0),
      eps
  );
  STATIC_ASSERT_NEAR(
      arene::base::abs(static_cast<value_type>(-1.0) * std::numeric_limits<value_type>::max()) -
          std::numeric_limits<value_type>::max(),
      static_cast<value_type>(0),
      eps
  );
  // Close to boundary cases
  STATIC_ASSERT_NEAR(
      arene::base::abs(std::numeric_limits<value_type>::epsilon()),
      std::numeric_limits<value_type>::epsilon(),
      eps
  );
  STATIC_ASSERT_NEAR(
      arene::base::abs(static_cast<value_type>(-1.0) * std::numeric_limits<value_type>::epsilon()),
      std::numeric_limits<value_type>::epsilon(),
      eps
  );

  STATIC_ASSERT_TRUE(arene::base::isinf(arene::base::abs(std::numeric_limits<value_type>::infinity())));
  STATIC_ASSERT_TRUE(arene::base::isinf(arene::base::abs(-1.0 * std::numeric_limits<value_type>::infinity())));

  // GCC/QCC incorrectly thinks that using a NaN in a constant expression is not allowed, so abs(NaN) does not work on
  // GCC/QCC at compile time. We have to do a runtime assertion in that case.
#if ARENE_IS_ON(ARENE_COMPILER_GCC) || defined(__QNX__)
  EXPECT_FALSE(arene::base::signbit(arene::base::abs(std::numeric_limits<value_type>::quiet_NaN())));
  EXPECT_FALSE(arene::base::signbit(arene::base::abs(-std::numeric_limits<value_type>::quiet_NaN())));
#else
  STATIC_ASSERT_FALSE(arene::base::signbit(arene::base::abs(std::numeric_limits<value_type>::quiet_NaN())));
  STATIC_ASSERT_FALSE(arene::base::signbit(arene::base::abs(-std::numeric_limits<value_type>::quiet_NaN())));
#endif
}

/// @test abs behaves correctly when passed NaN values
TYPED_TEST(AbsFloatingTypedTest, NaN) {
  using arene::base::abs;
  constexpr auto positive_quiet = std::numeric_limits<typename TestFixture::type>::quiet_NaN();
  constexpr auto negative_quiet = -positive_quiet;
  constexpr auto positive_signaling = std::numeric_limits<typename TestFixture::type>::signaling_NaN();
  constexpr auto negative_signaling = -positive_signaling;

  // GCC/QCC incorrectly thinks that using a NaN in a constant expression is not allowed, so abs(NaN) does not work on
  // GCC/QCC at compile time. We have to do a runtime assertion in that case.
#if ARENE_IS_ON(ARENE_COMPILER_GCC) || defined(__QNX__)
  EXPECT_TRUE(arene::base::isnan(abs(positive_quiet)));
  EXPECT_TRUE(arene::base::isnan(abs(negative_quiet)));
  EXPECT_TRUE(arene::base::isnan(abs(positive_signaling)));
  EXPECT_TRUE(arene::base::isnan(abs(negative_signaling)));

  // ISO/IEC 60559 specifies that abs(NaN) returns the same kind of NaN that was passed in, with the signbit set to 0.
  EXPECT_EQ(arene::base::signbit(abs(positive_quiet)), arene::base::signbit(positive_quiet));
  EXPECT_EQ(arene::base::signbit(abs(negative_quiet)), arene::base::signbit(positive_quiet));
  EXPECT_EQ(arene::base::signbit(abs(positive_signaling)), arene::base::signbit(positive_signaling));
  EXPECT_EQ(arene::base::signbit(abs(negative_signaling)), arene::base::signbit(positive_signaling));
#else
  STATIC_ASSERT_TRUE(arene::base::isnan(abs(positive_quiet)));
  STATIC_ASSERT_TRUE(arene::base::isnan(abs(negative_quiet)));
  STATIC_ASSERT_TRUE(arene::base::isnan(abs(positive_signaling)));
  STATIC_ASSERT_TRUE(arene::base::isnan(abs(negative_signaling)));

  // ISO/IEC 60559 specifies that abs(NaN) returns the same kind of NaN that was passed in, with the signbit set to 0.
  STATIC_ASSERT_EQ(arene::base::signbit(abs(positive_quiet)), arene::base::signbit(positive_quiet));
  STATIC_ASSERT_EQ(arene::base::signbit(abs(negative_quiet)), arene::base::signbit(positive_quiet));
  STATIC_ASSERT_EQ(arene::base::signbit(abs(positive_signaling)), arene::base::signbit(positive_signaling));
  STATIC_ASSERT_EQ(arene::base::signbit(abs(negative_signaling)), arene::base::signbit(positive_signaling));
#endif
}

}  // namespace
