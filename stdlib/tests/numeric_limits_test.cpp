// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/cfloat"
#include "stdlib/include/climits"
#include "stdlib/include/limits"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

enum float_classification { float_nan = 1, float_infinite, float_normal, float_subnormal, float_zero };

constexpr auto classify_float(float val) -> int {
  return __builtin_fpclassify(float_nan, float_infinite, float_normal, float_subnormal, float_zero, val);
}

constexpr auto is_infinity(float val) -> bool { return classify_float(val) == float_infinite; }
constexpr auto is_nan(float val) -> bool { return classify_float(val) == float_nan; }
constexpr auto is_normal(float val) -> bool { return classify_float(val) == float_normal; }
constexpr auto is_subnormal(float val) -> bool { return classify_float(val) == float_subnormal; }
constexpr auto is_zero(float val) -> bool { return classify_float(val) == float_zero; }

constexpr auto classify_float(double val) -> int {
  return __builtin_fpclassify(float_nan, float_infinite, float_normal, float_subnormal, float_zero, val);
}

constexpr auto is_infinity(double val) -> bool { return classify_float(val) == float_infinite; }
constexpr auto is_nan(double val) -> bool { return classify_float(val) == float_nan; }
constexpr auto is_normal(double val) -> bool { return classify_float(val) == float_normal; }
constexpr auto is_subnormal(double val) -> bool { return classify_float(val) == float_subnormal; }
constexpr auto is_zero(double val) -> bool { return classify_float(val) == float_zero; }

class construction_tracker {
 public:
  enum class state : std::uint8_t {
    default_constructed,
    copy_constructed,
    move_constructed,
    direct_constructed,
    direct_rvalue_constructed,
    copy_assigned,
    move_assigned,
    swapped_lhs,
    swapped_rhs
  };

  struct some_arg_type {};
  struct explicit_arg_type {};

  constexpr construction_tracker() noexcept
      : state_(state::default_constructed) {}
  constexpr construction_tracker(construction_tracker const&) noexcept
      : state_(state::copy_constructed) {}
  constexpr construction_tracker(construction_tracker&&) noexcept
      : state_(state::move_constructed) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr construction_tracker(some_arg_type const&) noexcept
      : state_(state::direct_constructed) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr construction_tracker(some_arg_type&&) noexcept
      : state_(state::direct_rvalue_constructed) {}

  constexpr auto operator=(construction_tracker&&) noexcept -> construction_tracker& {
    state_ = state::move_assigned;
    return *this;
  }
  constexpr auto operator=(construction_tracker const&) noexcept -> construction_tracker& {
    state_ = state::copy_assigned;
    return *this;
  }

  ~construction_tracker() = default;

  constexpr auto get_tracked_state() const noexcept -> state { return state_; }

  constexpr void swap(construction_tracker& other) noexcept {
    state_ = state::swapped_lhs;
    other.state_ = state::swapped_rhs;
  }

 private:
  state state_;
};

template <typename>
struct expected_values;

template <>
struct expected_values<bool> {
  static constexpr bool max = true;
  static constexpr bool min = false;
  static constexpr int digits10 = 0;
};

template <>
struct expected_values<int> {
  static constexpr int max = INT_MAX;
  static constexpr int min = INT_MIN;
  static constexpr int digits10 = 9;
};

template <>
struct expected_values<unsigned> {
  static constexpr unsigned max = UINT_MAX;
  static constexpr unsigned min = 0;
  static constexpr int digits10 = 9;
};

template <>
struct expected_values<char> {
  static constexpr char max = CHAR_MAX;
  static constexpr char min = CHAR_MIN;
  static constexpr int digits10 = 2;
};

template <>
struct expected_values<wchar_t> {
  using underlying_type = std::
      conditional_t<sizeof(wchar_t) == sizeof(int), std::conditional_t<std::is_signed_v<wchar_t>, int, unsigned>, void>;
  static constexpr wchar_t max = static_cast<wchar_t>(expected_values<underlying_type>::max);
  static constexpr wchar_t min = static_cast<wchar_t>(expected_values<underlying_type>::min);
  static constexpr int digits10 = expected_values<underlying_type>::digits10;
};

template <>
struct expected_values<signed char> {
  static constexpr signed char max = SCHAR_MAX;
  static constexpr signed char min = SCHAR_MIN;
  static constexpr int digits10 = 2;
};

template <>
struct expected_values<unsigned char> {
  static constexpr unsigned char max = UCHAR_MAX;
  static constexpr unsigned char min = 0;
  static constexpr int digits10 = 2;
};

template <>
// NOLINTNEXTLINE(google-runtime-int)
struct expected_values<short> {
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr short max = SHRT_MAX;
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr short min = SHRT_MIN;
  static constexpr int digits10 = 4;
};

template <>
// NOLINTNEXTLINE(google-runtime-int)
struct expected_values<unsigned short> {
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr unsigned short max = USHRT_MAX;
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr unsigned short min = 0;
  static constexpr int digits10 = 4;
};

template <>
struct expected_values<char16_t> {
  static constexpr char16_t max = 65535;
  static constexpr char16_t min = 0;
  static constexpr int digits10 = 4;
};

template <>
struct expected_values<char32_t> {
  static constexpr char32_t max = 4'294'967'295U;
  static constexpr char32_t min = 0;
  static constexpr int digits10 = 9;
};

template <>
// NOLINTNEXTLINE(google-runtime-int)
struct expected_values<long long> {
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr long long max = LLONG_MAX;
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr long long min = LLONG_MIN;
  static constexpr int digits10 = 18;
};

template <>
// NOLINTNEXTLINE(google-runtime-int)
struct expected_values<unsigned long long> {
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr unsigned long long max = ULLONG_MAX;
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr unsigned long long min = 0;
  static constexpr int digits10 = 19;
};

template <>
// NOLINTNEXTLINE(google-runtime-int)
struct expected_values<long> {
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr long max = LONG_MAX;
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr long min = LONG_MIN;
  static constexpr int digits10 =
      // NOLINTNEXTLINE(google-runtime-int)
      (sizeof(long) == sizeof(int)) ? expected_values<int>::digits10 : expected_values<long long>::digits10;
};

template <>
// NOLINTNEXTLINE(google-runtime-int)
struct expected_values<unsigned long> {
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr unsigned long max = ULONG_MAX;
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr unsigned long min = 0;
  // NOLINTNEXTLINE(google-runtime-int)
  static constexpr int digits10 = (sizeof(unsigned long) == sizeof(unsigned))
                                      ? expected_values<unsigned>::digits10
                                      // NOLINTNEXTLINE(google-runtime-int)
                                      : expected_values<unsigned long long>::digits10;
};

/// @test Check the enumerators of the rounding style enumeration have the right values
TEST(NumericLimits, FloatRoundStyle) {
  ASSERT_EQ(std::round_toward_zero, 0);
  ASSERT_EQ(std::round_indeterminate, -1);
  ASSERT_EQ(std::round_to_nearest, 1);
  ASSERT_EQ(std::round_toward_infinity, 2);
  ASSERT_EQ(std::round_toward_neg_infinity, 3);
}

/// @test Check the enumerators of the denorm style enumeration have the right values
TEST(NumericLimits, FloatDenormStyle) {
  ASSERT_EQ(std::denorm_indeterminate, -1);
  ASSERT_EQ(std::denorm_absent, 0);
  ASSERT_EQ(std::denorm_present, 1);
}

using non_arithmetic_types = testing::extend_with_cv_qualifiers_t<arene::base::type_lists::concat_unique_t<
    testing::void_types,
    testing::null_pointer_types,
    testing::member_pointer_types,
    testing::enum_types,
    testing::class_types,
    testing::union_types,
    testing::Types<construction_tracker>>>;

template <typename>
class NumericLimitsNonArithmeticTests : public testing::Test {};

TYPED_TEST_SUITE(NumericLimitsNonArithmeticTests, non_arithmetic_types, );

/// @test The primary template for non-arithmetic types gives default values
CONSTEXPR_TYPED_TEST(NumericLimitsNonArithmeticTests, DefaultValuesForUnknownTypes) {
  static_assert(!std::numeric_limits<TypeParam>::is_specialized, "Not specialized");
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::min()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::max()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::lowest()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::infinity()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::epsilon()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::round_error()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::quiet_NaN()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::signaling_NaN()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::denorm_min()), std::remove_cv_t<TypeParam>>();

  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::min()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::max()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::lowest()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::infinity()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::epsilon()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::round_error()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::quiet_NaN()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::signaling_NaN()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::denorm_min()));

  ASSERT_EQ(std::numeric_limits<TypeParam>::radix, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::digits, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::digits10, 0);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::is_signed);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::is_integer);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::is_exact);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::is_bounded);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::is_modulo);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::traps);
  testing::
      StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<TypeParam>::round_style)>, std::float_round_style>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::round_style, std::round_toward_zero);
  testing::
      StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<TypeParam>::has_denorm)>, std::float_denorm_style>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::has_denorm, std::denorm_absent);
  ASSERT_EQ(std::numeric_limits<TypeParam>::max_digits10, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::min_exponent, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::min_exponent10, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::max_exponent, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::max_exponent10, 0);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_infinity);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_quiet_NaN);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_signaling_NaN);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_denorm_loss);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::is_iec559);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::tinyness_before);
}

/// @test The primary template for unknown types gives default values
CONSTEXPR_TEST(NumericLimits, DefaultValuesForUnknownTypes) {
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::min().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::max().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::lowest().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::epsilon().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::round_error().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::infinity().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::quiet_NaN().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::signaling_NaN().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
  ASSERT_EQ(
      std::numeric_limits<construction_tracker>::denorm_min().get_tracked_state(),
      construction_tracker::state::default_constructed
  );
}

template <typename>
class NumericLimitsIntegralTests : public testing::Test {};

using int_types = testing::extend_with_cv_qualifiers_t<testing::integral_types>;

TYPED_TEST_SUITE(NumericLimitsIntegralTests, int_types, );

/// @test The min and max values for integral types are correct
CONSTEXPR_TYPED_TEST(NumericLimitsIntegralTests, MinMaxForIntegralTypes) {
  ASSERT_EQ(std::numeric_limits<TypeParam>::min(), expected_values<std::remove_cv_t<TypeParam>>::min);
  ASSERT_EQ(std::numeric_limits<TypeParam>::max(), expected_values<std::remove_cv_t<TypeParam>>::max);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::min()), std::remove_cv_t<TypeParam>>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::max()), std::remove_cv_t<TypeParam>>();
}

/// @test The lowest value for integral types is correct
CONSTEXPR_TYPED_TEST(NumericLimitsIntegralTests, LowestForIntegralTypes) {
  ASSERT_EQ(std::numeric_limits<TypeParam>::lowest(), std::numeric_limits<TypeParam>::min());
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::lowest()), std::remove_cv_t<TypeParam>>();
}

/// @test The "all types" constants for integral types are correct
CONSTEXPR_TYPED_TEST(NumericLimitsIntegralTests, ConstantsForIntegralTypes) {
  bool const is_bool = std::is_same_v<std::remove_cv_t<TypeParam>, bool>;
  static_assert(std::numeric_limits<TypeParam>::is_specialized, "Should be specialized");
  ASSERT_EQ(std::numeric_limits<TypeParam>::radix, 2);
  ASSERT_EQ(
      std::numeric_limits<TypeParam>::digits,
      (is_bool ? 1 : ((CHAR_BIT * sizeof(TypeParam)) - (std::is_signed_v<TypeParam> ? 1 : 0)))
  );
  ASSERT_EQ(std::numeric_limits<TypeParam>::digits10, expected_values<std::remove_cv_t<TypeParam>>::digits10);
  ASSERT_EQ(std::numeric_limits<TypeParam>::is_signed, std::is_signed_v<TypeParam>);
  ASSERT_TRUE(std::numeric_limits<TypeParam>::is_integer);
  ASSERT_TRUE(std::numeric_limits<TypeParam>::is_exact);
  ASSERT_TRUE(std::numeric_limits<TypeParam>::is_bounded);
  ASSERT_EQ(std::numeric_limits<TypeParam>::is_modulo, (!std::numeric_limits<TypeParam>::is_signed && !is_bool));
  ASSERT_EQ(std::numeric_limits<TypeParam>::traps, !is_bool);
}

/// @test The "floating point types" constants for integral types are correct
CONSTEXPR_TYPED_TEST(NumericLimitsIntegralTests, FloatingPointConstantsForIntegralTypes) {
  ASSERT_EQ(std::numeric_limits<TypeParam>::epsilon(), 0);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::epsilon()), std::remove_cv_t<TypeParam>>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::round_error(), 0);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::round_error()), std::remove_cv_t<TypeParam>>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::infinity(), 0);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::infinity()), std::remove_cv_t<TypeParam>>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::quiet_NaN(), 0);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::quiet_NaN()), std::remove_cv_t<TypeParam>>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::signaling_NaN(), 0);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::signaling_NaN()), std::remove_cv_t<TypeParam>>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::denorm_min(), 0);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<TypeParam>::denorm_min()), std::remove_cv_t<TypeParam>>();
  testing::
      StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<TypeParam>::round_style)>, std::float_round_style>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::round_style, std::round_toward_zero);
  testing::
      StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<TypeParam>::has_denorm)>, std::float_denorm_style>();
  ASSERT_EQ(std::numeric_limits<TypeParam>::has_denorm, std::denorm_absent);
  ASSERT_EQ(std::numeric_limits<TypeParam>::max_digits10, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::min_exponent, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::min_exponent10, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::max_exponent, 0);
  ASSERT_EQ(std::numeric_limits<TypeParam>::max_exponent10, 0);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_infinity);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_quiet_NaN);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_signaling_NaN);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::has_denorm_loss);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::is_iec559);
  ASSERT_FALSE(std::numeric_limits<TypeParam>::tinyness_before);
}

/// @test Ensure that the function calls are @c noexcept for @c numeric_limits<integral-type>
TYPED_TEST(NumericLimitsIntegralTests, NoexceptOperationsForIntegralTypes) {
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::min()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::max()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::lowest()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::infinity()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::epsilon()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::round_error()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::quiet_NaN()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::signaling_NaN()));
  ASSERT_TRUE(noexcept(std::numeric_limits<TypeParam>::denorm_min()));
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");

/// @test The min and max values for @c float are correct
CONSTEXPR_TEST(NumericLimits, MinMaxForFloat) {
  ASSERT_EQ(std::numeric_limits<float>::min(), FLT_MIN);
  ASSERT_EQ(std::numeric_limits<float>::max(), FLT_MAX);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::min()), float>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::max()), float>();
}

/// @test The lowest value for float is correct
CONSTEXPR_TEST(NumericLimits, LowestForFloat) {
  ASSERT_EQ(std::numeric_limits<float>::lowest(), -FLT_MAX);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::lowest()), float>();
}

/// @test The "all types" constants for float are correct
CONSTEXPR_TEST(NumericLimits, ConstantsForFloat) {
  static_assert(std::numeric_limits<float>::is_specialized, "Should be specialized");
  ASSERT_EQ(std::numeric_limits<float>::radix, 2);
  ASSERT_EQ(std::numeric_limits<float>::digits, FLT_MANT_DIG);
  ASSERT_EQ(std::numeric_limits<float>::digits10, FLT_DIG);
  ASSERT_EQ(std::numeric_limits<float>::is_signed, true);
  ASSERT_FALSE(std::numeric_limits<float>::is_integer);
  ASSERT_TRUE(std::numeric_limits<float>::is_exact);
  ASSERT_TRUE(std::numeric_limits<float>::is_bounded);
  ASSERT_FALSE(std::numeric_limits<float>::is_modulo);
  ASSERT_FALSE(std::numeric_limits<float>::traps);
}

/// @test The "floating point types" constants for float are correct
CONSTEXPR_TEST(NumericLimits, FloatingPointConstantsForFloat) {
  ASSERT_EQ(std::numeric_limits<float>::epsilon(), FLT_EPSILON);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::epsilon()), float>();
  ASSERT_EQ(std::numeric_limits<float>::round_error(), 0.5F);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::round_error()), float>();
  ASSERT_TRUE(is_infinity(std::numeric_limits<float>::infinity()));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::infinity()), float>();
  ASSERT_TRUE(is_nan(std::numeric_limits<float>::quiet_NaN()));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::quiet_NaN()), float>();
  ASSERT_TRUE(is_nan(std::numeric_limits<float>::signaling_NaN()));
  ASSERT_FALSE(is_infinity(std::numeric_limits<float>::signaling_NaN()));
  ASSERT_FALSE(is_normal(std::numeric_limits<float>::signaling_NaN()));
  ASSERT_FALSE(is_subnormal(std::numeric_limits<float>::signaling_NaN()));
  ASSERT_FALSE(is_zero(std::numeric_limits<float>::signaling_NaN()));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::signaling_NaN()), float>();
  ASSERT_EQ(std::numeric_limits<float>::denorm_min(), FLT_MIN / (1ULL << static_cast<unsigned>(FLT_MANT_DIG - 1)));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<float>::denorm_min()), float>();
  testing::StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<float>::round_style)>, std::float_round_style>(
  );
  ASSERT_EQ(std::numeric_limits<float>::round_style, std::round_to_nearest);
  testing::StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<float>::has_denorm)>, std::float_denorm_style>(
  );
  ASSERT_EQ(std::numeric_limits<float>::has_denorm, std::denorm_present);
  ASSERT_EQ(std::numeric_limits<float>::max_digits10, 9);
  ASSERT_EQ(std::numeric_limits<float>::min_exponent, FLT_MIN_EXP);
  ASSERT_EQ(std::numeric_limits<float>::min_exponent10, FLT_MIN_10_EXP);
  ASSERT_EQ(std::numeric_limits<float>::max_exponent, FLT_MAX_EXP);
  ASSERT_EQ(std::numeric_limits<float>::max_exponent10, FLT_MAX_10_EXP);
  ASSERT_TRUE(std::numeric_limits<float>::has_infinity);
  ASSERT_TRUE(std::numeric_limits<float>::has_quiet_NaN);
  ASSERT_TRUE(std::numeric_limits<float>::has_signaling_NaN);
  ASSERT_FALSE(std::numeric_limits<float>::has_denorm_loss);
  ASSERT_TRUE(std::numeric_limits<float>::is_iec559);
  ASSERT_FALSE(std::numeric_limits<float>::tinyness_before);
}

/// @test The min and max values for @c double are correct
CONSTEXPR_TEST(NumericLimits, MinMaxForDouble) {
  ASSERT_EQ(std::numeric_limits<double>::min(), DBL_MIN);
  ASSERT_EQ(std::numeric_limits<double>::max(), DBL_MAX);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::min()), double>();
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::max()), double>();
}

/// @test The lowest value for double is correct
CONSTEXPR_TEST(NumericLimits, LowestForDouble) {
  ASSERT_EQ(std::numeric_limits<double>::lowest(), -DBL_MAX);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::lowest()), double>();
}

/// @test The "all types" constants for double are correct
CONSTEXPR_TEST(NumericLimits, ConstantsForDouble) {
  static_assert(std::numeric_limits<double>::is_specialized, "Should be specialized");
  ASSERT_EQ(std::numeric_limits<double>::radix, 2);
  ASSERT_EQ(std::numeric_limits<double>::digits, DBL_MANT_DIG);
  ASSERT_EQ(std::numeric_limits<double>::digits10, DBL_DIG);
  ASSERT_EQ(std::numeric_limits<double>::is_signed, true);
  ASSERT_FALSE(std::numeric_limits<double>::is_integer);
  ASSERT_TRUE(std::numeric_limits<double>::is_exact);
  ASSERT_TRUE(std::numeric_limits<double>::is_bounded);
  ASSERT_FALSE(std::numeric_limits<double>::is_modulo);
  ASSERT_FALSE(std::numeric_limits<double>::traps);
}

/// @test The "floating point types" constants for double are correct
CONSTEXPR_TEST(NumericLimits, FloatingPointConstantsForDouble) {
  ASSERT_EQ(std::numeric_limits<double>::epsilon(), DBL_EPSILON);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::epsilon()), double>();
  ASSERT_EQ(std::numeric_limits<double>::round_error(), 0.5);
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::round_error()), double>();
  ASSERT_TRUE(is_infinity(std::numeric_limits<double>::infinity()));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::infinity()), double>();
  ASSERT_TRUE(is_nan(std::numeric_limits<double>::quiet_NaN()));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::quiet_NaN()), double>();
  ASSERT_TRUE(is_nan(std::numeric_limits<double>::signaling_NaN()));
  ASSERT_FALSE(is_infinity(std::numeric_limits<double>::signaling_NaN()));
  ASSERT_FALSE(is_normal(std::numeric_limits<double>::signaling_NaN()));
  ASSERT_FALSE(is_subnormal(std::numeric_limits<double>::signaling_NaN()));
  ASSERT_FALSE(is_zero(std::numeric_limits<double>::signaling_NaN()));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::signaling_NaN()), double>();
  ASSERT_EQ(std::numeric_limits<double>::denorm_min(), DBL_MIN / (1ULL << static_cast<unsigned>(DBL_MANT_DIG - 1)));
  testing::StaticAssertTypeEq<decltype(std::numeric_limits<double>::denorm_min()), double>();
  testing::StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<double>::round_style)>, std::float_round_style>(
  );
  ASSERT_EQ(std::numeric_limits<double>::round_style, std::round_to_nearest);
  testing::StaticAssertTypeEq<std::decay_t<decltype(std::numeric_limits<double>::has_denorm)>, std::float_denorm_style>(
  );
  ASSERT_EQ(std::numeric_limits<double>::has_denorm, std::denorm_present);
  ASSERT_EQ(std::numeric_limits<double>::max_digits10, 17);
  ASSERT_EQ(std::numeric_limits<double>::min_exponent, DBL_MIN_EXP);
  ASSERT_EQ(std::numeric_limits<double>::min_exponent10, DBL_MIN_10_EXP);
  ASSERT_EQ(std::numeric_limits<double>::max_exponent, DBL_MAX_EXP);
  ASSERT_EQ(std::numeric_limits<double>::max_exponent10, DBL_MAX_10_EXP);
  ASSERT_TRUE(std::numeric_limits<double>::has_infinity);
  ASSERT_TRUE(std::numeric_limits<double>::has_quiet_NaN);
  ASSERT_TRUE(std::numeric_limits<double>::has_signaling_NaN);
  ASSERT_FALSE(std::numeric_limits<double>::has_denorm_loss);
  ASSERT_TRUE(std::numeric_limits<double>::is_iec559);
  ASSERT_FALSE(std::numeric_limits<double>::tinyness_before);
}

ARENE_IGNORE_END();

}  // namespace
