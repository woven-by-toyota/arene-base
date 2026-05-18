// Copyright 2025, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/float_properties.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/cartesian_product.hpp"

namespace {

/// @brief Struct to hold the test expectations for each of the kinds of floating point number
struct test_expectation {
  bool zero;
  bool finite;
  bool subnormal;
  bool infinite;
  bool nan;
};

/// @brief Fixture specifically for isfinite which will be inherited by the generic test fixture
class isfinite_fixture {
 public:
  static constexpr test_expectation expectation{
      true,   // zero
      true,   // finite
      true,   // subnormal
      false,  // infinite
      false   // nan
  };

  template <typename T>
  static constexpr auto check(T num) noexcept(noexcept(arene::base::isfinite(num))) -> bool {
    return arene::base::isfinite(num);
  }
};

/// @brief Fixture specifically for isnormal which will be inherited by the generic test fixture
class isnormal_fixture {
 public:
  static constexpr test_expectation expectation{
      false,  // zero
      true,   // finite
      false,  // subnormal
      false,  // infinite
      false   // nan
  };

  template <typename T>
  static constexpr auto check(T num) noexcept(noexcept(arene::base::isnormal(num))) -> bool {
    return arene::base::isnormal(num);
  }
};

/// @brief Fixture specifically for isinf which will be inherited by the generic test fixture
class isinf_fixture {
 public:
  static constexpr test_expectation expectation{
      false,  // zero
      false,  // finite
      false,  // subnormal
      true,   // infinite
      false   // nan
  };

  template <typename T>
  static constexpr auto check(T num) noexcept(noexcept(arene::base::isinf(num))) -> bool {
    return arene::base::isinf(num);
  }
};

/// @brief Fixture specifically for isnan which will be inherited by the generic test fixture
class isnan_fixture {
 public:
  static constexpr test_expectation expectation{
      false,  // zero
      false,  // finite
      false,  // subnormal
      false,  // infinite
      true    // nan
  };

  template <typename T>
  static constexpr auto check(T num) noexcept(noexcept(arene::base::isnan(num))) -> bool {
    return arene::base::isnan(num);
  }
};

/// @brief Holder for a cross of a base fixture with a concrete floating point type to conduct the test with
template <typename BaseFixture, typename TestType>
struct test_param {
  using Fixture = BaseFixture;
  using Type = TestType;
};

/// @brief Test fixture in the GoogleTest sense; inherits from our manually-written fixture as well as the GTest one
template <typename Params>
class FloatPropertyTest
    : public testing::Test
    , public Params::Fixture {};

using fixtures = ::testing::Types<isfinite_fixture, isnormal_fixture, isinf_fixture, isnan_fixture>;
using float_types = ::testing::Types<float, double>;
using test_types = ::arene::base::type_lists::cartesian_product_as_t<test_param, fixtures, float_types>;

TYPED_TEST_SUITE(FloatPropertyTest, test_types, );

/// @test The floating point property function is always marked as non-throwing
TYPED_TEST(FloatPropertyTest, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(TestFixture::check(typename TypeParam::Type{})));
}

/// @test Check the floating point property function for zero values.
TYPED_TEST(FloatPropertyTest, Zero) {
  using Type = typename TypeParam::Type;
  constexpr bool this_expectation{TestFixture::expectation.zero};

  STATIC_ASSERT_EQ(TestFixture::check(Type{0.0}), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(Type{-0.0}), this_expectation);
}

/// @test Check the floating point property function for normal finite values.
TYPED_TEST(FloatPropertyTest, Normal) {
  using Type = typename TypeParam::Type;
  constexpr bool this_expectation{TestFixture::expectation.finite};

  STATIC_ASSERT_EQ(TestFixture::check(Type{1.0}), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(Type{-1.0}), this_expectation);

  // This is the only literal that loses precision when converted to TypeParam so it needs a static_cast.
  STATIC_ASSERT_EQ(TestFixture::check(static_cast<Type>(1.1)), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(static_cast<Type>(-1.1)), this_expectation);

  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::max()), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::lowest()), this_expectation);

  // min() for a floating point type is the smallest positive normal value.
  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::min()), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(-std::numeric_limits<Type>::min()), this_expectation);

  // epsilon() is actually the difference between 1.0 and the next representable value, so it's normal.
  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::epsilon()), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(-std::numeric_limits<Type>::epsilon()), this_expectation);
}

/// @test Check the floating point property function for subnormal values.
TYPED_TEST(FloatPropertyTest, Subnormal) {
  using Type = typename TypeParam::Type;
  constexpr bool this_expectation{TestFixture::expectation.subnormal};

  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::min() / Type{2.0}), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(-std::numeric_limits<Type>::min() / Type{2.0}), this_expectation);

  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::denorm_min()), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(-std::numeric_limits<Type>::denorm_min()), this_expectation);
}

/// @test Check the floating point property function for infinite values.
TYPED_TEST(FloatPropertyTest, Infinite) {
  using Type = typename TypeParam::Type;
  constexpr bool this_expectation{TestFixture::expectation.infinite};

  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::infinity()), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(-std::numeric_limits<Type>::infinity()), this_expectation);
}

/// @test Check the floating point property function for NaN values.
TYPED_TEST(FloatPropertyTest, NaN) {
  using Type = typename TypeParam::Type;
  constexpr bool this_expectation{TestFixture::expectation.nan};

  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::quiet_NaN()), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(-std::numeric_limits<Type>::quiet_NaN()), this_expectation);

  STATIC_ASSERT_EQ(TestFixture::check(std::numeric_limits<Type>::signaling_NaN()), this_expectation);
  STATIC_ASSERT_EQ(TestFixture::check(-std::numeric_limits<Type>::signaling_NaN()), this_expectation);
}

}  // namespace
