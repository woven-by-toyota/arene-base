// Copyright 2025, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/float_properties.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/cartesian_product.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/float32.hpp"

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

namespace user_types {

/// @brief A user type whose float-property ADL overloads are not @c noexcept, to verify the CPOs propagate
/// (the absence of) @c noexcept.
struct throwy {
  friend auto isnan(throwy) -> bool { return false; }
  friend auto isinf(throwy) -> bool { return false; }
  friend auto isfinite(throwy) -> bool { return true; }
  friend auto isnormal(throwy) -> bool { return true; }
};

/// @brief A user type with no ADL overloads for any of the float-property predicates; used to verify that
/// the CPOs SFINAE-out cleanly for unsupported types rather than producing a hard error.
struct no_overloads {};

/// @brief A user type that overloads only @c isnan via ADL; used to verify per-predicate SFINAE independence.
struct only_isnan {
  friend constexpr auto isnan(only_isnan) noexcept -> bool { return false; }
};

}  // namespace user_types

namespace {

using ::testing::float32;

/// @test The CPOs dispatch to user-provided ADL overloads for non-arithmetic types.
TEST(FloatPropertyAdl, DispatchesToAdlForUserType) {
  constexpr float32 nan_value{std::numeric_limits<float>::quiet_NaN()};
  constexpr float32 inf_value{std::numeric_limits<float>::infinity()};
  constexpr float32 subnormal_value{std::numeric_limits<float>::denorm_min()};
  constexpr float32 normal_value{1.0F};

  STATIC_ASSERT_TRUE(arene::base::isnan(nan_value));
  STATIC_ASSERT_FALSE(arene::base::isnan(normal_value));

  STATIC_ASSERT_TRUE(arene::base::isinf(inf_value));
  STATIC_ASSERT_FALSE(arene::base::isinf(normal_value));

  STATIC_ASSERT_TRUE(arene::base::isfinite(normal_value));
  STATIC_ASSERT_FALSE(arene::base::isfinite(nan_value));
  STATIC_ASSERT_FALSE(arene::base::isfinite(inf_value));

  STATIC_ASSERT_TRUE(arene::base::isnormal(normal_value));
  STATIC_ASSERT_FALSE(arene::base::isnormal(subnormal_value));
  STATIC_ASSERT_FALSE(arene::base::isnormal(nan_value));
}

/// @test The CPOs propagate the @c noexcept specification of the user's ADL overload.
TEST(FloatPropertyAdl, NoexceptPropagates) {
  STATIC_ASSERT_TRUE(noexcept(arene::base::isnan(float32{0.0F})));
  STATIC_ASSERT_TRUE(noexcept(arene::base::isinf(float32{0.0F})));
  STATIC_ASSERT_TRUE(noexcept(arene::base::isfinite(float32{0.0F})));
  STATIC_ASSERT_TRUE(noexcept(arene::base::isnormal(float32{0.0F})));

  STATIC_ASSERT_FALSE(noexcept(arene::base::isnan(std::declval<user_types::throwy>())));
  STATIC_ASSERT_FALSE(noexcept(arene::base::isinf(std::declval<user_types::throwy>())));
  STATIC_ASSERT_FALSE(noexcept(arene::base::isfinite(std::declval<user_types::throwy>())));
  STATIC_ASSERT_FALSE(noexcept(arene::base::isnormal(std::declval<user_types::throwy>())));
}

/// @test The CPOs are invocable for floating point types and for user types providing an ADL overload.
TEST(FloatPropertyAdl, InvocableForSupportedTypes) {
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isnan), double>));
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isinf), double>));
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isfinite), double>));
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isnormal), double>));

  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isnan), float32>));
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isinf), float32>));
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isfinite), float32>));
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isnormal), float32>));
}

/// @test The CPOs are not invocable for integer types
TEST(FloatPropertyAdl, NotInvocableForIntegerTypes) {
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnan), int>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isinf), int>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isfinite), int>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnormal), int>));

  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnan), unsigned>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnan), bool>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnan), char>));
}

/// @test The CPOs SFINAE-out (rather than hard-error) for user types with no matching ADL overload.
TEST(FloatPropertyAdl, NotInvocableForUnsupportedType) {
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnan), user_types::no_overloads>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isinf), user_types::no_overloads>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isfinite), user_types::no_overloads>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnormal), user_types::no_overloads>));
}

/// @test SFINAE on each CPO is independent: providing @c isnan must not make the others invocable.
TEST(FloatPropertyAdl, SfinaeIsPerPredicate) {
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(arene::base::isnan), user_types::only_isnan>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isinf), user_types::only_isnan>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isfinite), user_types::only_isnan>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(arene::base::isnormal), user_types::only_isnan>));
}

}  // namespace
