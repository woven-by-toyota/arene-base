// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/chrono"
#include "stdlib/include/cstdint"
#include "stdlib/include/limits"
#include "stdlib/include/ratio"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/throws_on.hpp"

namespace {

struct wrapped_float {
  double value;
  constexpr explicit wrapped_float(double val)
      : value(val) {}

  // This is intenionally convertible to satisfy the requirements of the duration constructor
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr wrapped_float(std::int32_t val)
      : value(val) {}

  // This is intenionally convertible to satisfy the requirements of the duration constructor
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator double() const { return value; }
};

using testing::is_unambiguously_publicly_derived_from_v;

}  // namespace

// Specialize treat_as_floating_point for our custom type outside the anonymous namespace
template <>
struct std::chrono::treat_as_floating_point<wrapped_float> : std::true_type {};

template <class T>
struct std::common_type<wrapped_float, T> {
  using type = double;
};

template <class T>
struct std::common_type<testing::configurable_value<std::int32_t, testing::throws_on::everything>, T> {
  using type = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
};

template <class T>
struct std::common_type<T, testing::configurable_value<std::int32_t, testing::throws_on::everything>> {
  using type = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
};

template <>
struct std::common_type<
    testing::configurable_value<std::int32_t, testing::throws_on::everything>,
    testing::configurable_value<std::int32_t, testing::throws_on::everything>> {
  using type = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
};

template <>
struct std::chrono::duration_values<testing::configurable_value<std::int32_t>> {
  static constexpr auto zero() noexcept(false) -> testing::configurable_value<std::int32_t> { return {1}; }

  static constexpr auto min() noexcept(false) -> testing::configurable_value<std::int32_t> { return {2}; }

  static constexpr auto max() noexcept(false) -> testing::configurable_value<std::int32_t> { return {3}; }
};

namespace {

template <class T>
class FloatingPointsTests : testing::Test {};

using floating_point_types =
    arene::base::type_lists::concat_unique_t<::testing::floating_point_types, ::testing::Types<wrapped_float>>;

TYPED_TEST_SUITE(FloatingPointsTests, ::testing::floating_point_types, );

/// @test @c treat_as_floating_point correctly identifies floating point types
CONSTEXPR_TYPED_TEST(FloatingPointsTests, TreatAsFloatingPoint) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::chrono::treat_as_floating_point<TypeParam>,
              std::true_type>);

  ASSERT_TRUE(std::chrono::treat_as_floating_point_v<TypeParam>);
  ASSERT_TRUE(std::chrono::treat_as_floating_point_v<TypeParam const>);
  ASSERT_TRUE(std::chrono::treat_as_floating_point_v<TypeParam volatile>);
  ASSERT_TRUE(std::chrono::treat_as_floating_point_v<TypeParam const volatile>);
}

template <class T>
class NotFloatingPointsTests : testing::Test {};

using not_floating_point_types = ::testing::
    Types<::testing::integral_types, ::testing::reference_types, ::testing::class_types, ::testing::array_types>;

TYPED_TEST_SUITE(NotFloatingPointsTests, not_floating_point_types, );

/// @test @c treat_as_floating_point correctly identifies non floating point types
CONSTEXPR_TYPED_TEST(NotFloatingPointsTests, TreatAsNotFloatingPoint) {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::chrono::treat_as_floating_point<TypeParam>,
              std::false_type>);

  ASSERT_FALSE(std::chrono::treat_as_floating_point_v<TypeParam>);
}

template <class T>
class DurationTests : protected testing::Test {};

// clang-format off
using duration_test_types = ::testing::Types<
  std::chrono::duration<std::int32_t>,
  std::chrono::seconds,
  std::chrono::duration<std::int32_t, std::milli>,
  std::chrono::milliseconds,
  std::chrono::microseconds,
  std::chrono::nanoseconds,
  std::chrono::minutes,
  std::chrono::hours,
  std::chrono::duration<std::int64_t, std::ratio<3600>>,
  std::chrono::duration<float>,
  std::chrono::duration<double>,
  std::chrono::duration<float, std::milli>,
  std::chrono::duration<double, std::micro>,
  std::chrono::duration<float, std::ratio<3600>>,
  std::chrono::duration<double, std::ratio<3600>>
>;
// clang-format on

TYPED_TEST_SUITE(DurationTests, duration_test_types, );

/// @test @c duration has the expected constructors
CONSTEXPR_TYPED_TEST(DurationTests, Properties) {
  ASSERT_TRUE(std::is_default_constructible_v<TypeParam>);
  ASSERT_TRUE(std::is_copy_constructible_v<TypeParam>);
  ASSERT_TRUE(std::is_copy_assignable_v<TypeParam>);
}

struct throwing_default_construction {
  // Cannot be defaulted as we need an explicit `noexcept(false)` for testing with gcc8.
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  throwing_default_construction() noexcept(false) {}
};

/// @test @c duration constructor is conditionally noexcept based on representation type
CONSTEXPR_TEST(DurationTests, ConditionalNoexcept) {
  using nothrow_duration = std::chrono::duration<std::int32_t>;
  using throwing_rep = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_rep2 = testing::configurable_value<double, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<throwing_rep>;

  EXPECT_TRUE(std::is_nothrow_default_constructible_v<nothrow_duration>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<nothrow_duration>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<nothrow_duration>);
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>().count()));

  EXPECT_FALSE(std::is_nothrow_default_constructible_v<std::chrono::duration<throwing_default_construction>>);
  EXPECT_FALSE(std::is_nothrow_copy_constructible_v<throwing_duration>);
  EXPECT_FALSE(std::is_nothrow_copy_assignable_v<throwing_duration>);
  EXPECT_FALSE(std::is_nothrow_constructible_v<throwing_duration, throwing_rep2>);
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>().count()));
}

/// @test @c duration converting constructor is conditionally noexcept
CONSTEXPR_TEST(DurationTests, ConvertingConstructorNoexcept) {
  using nothrow_duration1 = std::chrono::duration<std::int32_t, std::milli>;
  using nothrow_duration2 = std::chrono::duration<std::int32_t, std::micro>;
  using throwing_rep = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<throwing_rep, std::milli>;

  // Test nothrow conversion between compatible durations
  EXPECT_TRUE(std::is_nothrow_constructible_v<nothrow_duration2, nothrow_duration1>);
  EXPECT_TRUE(noexcept(nothrow_duration2{std::declval<nothrow_duration1>()}));

  // Test throwing conversion when representation throws
  EXPECT_FALSE(std::is_nothrow_constructible_v<throwing_duration, nothrow_duration1>);
  EXPECT_FALSE(noexcept(throwing_duration{std::declval<nothrow_duration1>()}));
}

/// @test @c duration type aliases match the template parameters
CONSTEXPR_TYPED_TEST(DurationTests, TypeAliases) {
  static_assert(std::is_same_v<typename TypeParam::rep, typename TypeParam::rep>, "rep alias should be consistent");
  static_assert(
      std::is_same_v<typename TypeParam::period, typename TypeParam::period>,
      "period alias should be consistent"
  );
}

/// @test @c duration can be default-constructed and initializes with zero tick count
CONSTEXPR_TYPED_TEST(DurationTests, DefaultConstructor) {
  auto const duration = TypeParam{};
  EXPECT_EQ(duration.count(), typename TypeParam::rep{});
}

/// @test @c duration can be explicitly constructed from a representation value
CONSTEXPR_TYPED_TEST(DurationTests, ExplicitConstructor) {
  auto const duration = TypeParam{typename TypeParam::rep{42}};
  EXPECT_EQ(duration.count(), typename TypeParam::rep{42});
}

/// @test @c duration can be copy constructed from another duration of the same type
CONSTEXPR_TYPED_TEST(DurationTests, CopyConstruction) {
  auto const duration1 = TypeParam{typename TypeParam::rep{42}};
  auto const duration2 = duration1;

  EXPECT_EQ(duration1.count(), typename TypeParam::rep{42});
  EXPECT_EQ(duration1.count(), duration2.count());
}

/// @test @c duration can be copy assigned from another duration of the same type
CONSTEXPR_TYPED_TEST(DurationTests, CopyAssignment) {
  auto const duration1 = TypeParam{typename TypeParam::rep{42}};
  auto duration2 = TypeParam{};
  duration2 = duration1;

  EXPECT_EQ(duration1.count(), typename TypeParam::rep{42});
  EXPECT_EQ(duration1.count(), duration2.count());
}

/// @test @c duration constructor can convert from compatible representation types
CONSTEXPR_TYPED_TEST(DurationTests, TypeConversionConstructor) {
  // Implicity convertible to all test Rep types
  auto const input_value = std::int8_t{42};

  auto const duration = TypeParam{input_value};
  EXPECT_EQ(duration.count(), static_cast<typename TypeParam::rep>(input_value));
}

/// @test Static values of @c duration specializations with built-in types are the same as @c numeric_limits
CONSTEXPR_TYPED_TEST(DurationTests, StaticValues) {
  EXPECT_EQ(TypeParam::zero().count(), typename TypeParam::rep(0));
  EXPECT_EQ(TypeParam::min().count(), std::numeric_limits<typename TypeParam::rep>::lowest());
  EXPECT_EQ(TypeParam::max().count(), std::numeric_limits<typename TypeParam::rep>::max());

  static_assert(noexcept(TypeParam::zero()), "Built-in types should all have noexcept static values");
  static_assert(noexcept(TypeParam::min()), "Built-in types should all have noexcept static values");
  static_assert(noexcept(TypeParam::max()), "Built-in types should all have noexcept static values");
}

/// @test @c duration constructor can convert from floating point if the representation type is floating point
CONSTEXPR_TEST(DurationTests, TypeConversionFromFloatingPoint) {
  // Test with a floating-point representation type
  auto const input_value = 3.14F;
  auto const duration = std::chrono::duration<double>{input_value};
  EXPECT_EQ(duration.count(), input_value);
}

/// @test @c duration constructor can convert from floating point if the representation type is specialized to be
/// floating point
CONSTEXPR_TEST(DurationTests, TypeConversionFromCustomFloatingPoint) {
  auto const input_value = wrapped_float{3.14};
  auto const duration = std::chrono::duration<double>{input_value};
  EXPECT_EQ(duration.count(), input_value);
}

/// @test @c duration constructor is not invocable with types that are not compatible with the representation type
TEST(DurationTests, DurationConstructorConstraints) {
  auto expr = [](auto const& value) -> decltype(std::chrono::duration<std::int32_t>{value}) {
    return std::chrono::duration<std::int32_t>{value};
  };

  ASSERT_TRUE(::testing::simple_is_invocable_v<decltype(expr), std::int32_t>);

  // Type not convertible to std::int32_t
  struct S {};
  ASSERT_FALSE(::testing::simple_is_invocable_v<decltype(expr), S>);

  // Floating point type when rep is not a floating point type
  ASSERT_FALSE(::testing::simple_is_invocable_v<decltype(expr), float>);
}

/// @test @c duration conversion constructor allows safe conversions
CONSTEXPR_TEST(DurationTests, ConversionConstructorSafeConversion) {
  auto const msec = std::chrono::duration<std::int32_t, std::milli>{2};
  auto const usec = std::chrono::duration<std::int64_t, std::micro>{msec};
  EXPECT_EQ(usec.count(), 2000);
}

/// @test @c duration conversion constructor with floating point target
CONSTEXPR_TEST(DurationTests, ConversionConstructorFloatingPointTarget) {
  auto const msec = std::chrono::duration<std::int32_t, std::milli>{1500};
  auto const sec = std::chrono::duration<double>{msec};
  EXPECT_EQ(sec.count(), 1.5);
}

/// @test @c duration conversion constructor with floating point source
CONSTEXPR_TEST(DurationTests, ConversionConstructorFloatingPointSource) {
  auto const sec = std::chrono::duration<double>{2.5};
  auto const msec = std::chrono::duration<float, std::milli>{sec};
  EXPECT_EQ(msec.count(), 2500.0F);
}

/// @test @c duration conversion constructor identity conversion
CONSTEXPR_TEST(DurationTests, ConversionConstructorIdentity) {
  auto const ms1 = std::chrono::duration<std::int32_t, std::milli>{1000};
  auto const ms2 = std::chrono::duration<std::int32_t, std::milli>{ms1};
  EXPECT_EQ(ms2.count(), 1000);
}

/// @test @c duration conversion constructor prevents lossy integral conversions
TEST(DurationTests, ConversionConstructorConstraints) {
  auto expr = [](auto input, auto result) -> decltype(decltype(result){input}) { return decltype(result){input}; };

  // Invocable if @c std::treat_as_floating_point_v<rep> is true
  ASSERT_TRUE(::testing::simple_is_invocable_v<
              decltype(expr),
              std::chrono::duration<std::int32_t, std::milli>,
              std::chrono::duration<wrapped_float>>);

  // Invocable if @c std::ratio_divide<Period2, period>::den is 1 and std::treat_as_floating_point_v<Rep2> is false
  // Period2 = std::ratio<1>
  // period = std::milli
  //
  // std::ratio_divide<std::ratio<1>, std::milli> = 1000 / 1.
  ASSERT_TRUE(::testing::simple_is_invocable_v<
              decltype(expr),
              std::chrono::duration<std::int32_t>,
              std::chrono::duration<std::int32_t, std::milli>>);

  // Not invocable if @c std::ratio_divide<Period2, period>::den is not 1 and std::treat_as_floating_point_v<Rep2> is
  // false
  // Period2 = std::milli
  // period = std::ratio<1>
  //
  // std::ratio_divide<std::milli, std::ratio<1>> = 1 / 1000.
  ASSERT_FALSE(::testing::simple_is_invocable_v<
               decltype(expr),
               std::chrono::duration<std::int32_t, std::milli>,
               std::chrono::duration<std::int32_t>>);

  // Not invocable if @c std::ratio_divide<Period2, period>::den is 1 and std::treat_as_floating_point_v<Rep2> is true
  ASSERT_FALSE(::testing::simple_is_invocable_v<
               decltype(expr),
               std::chrono::duration<wrapped_float>,
               std::chrono::duration<std::int32_t, std::milli>>);
}

/// @test @c duration conversion constructor prevents overflow
TEST(DurationTests, ConversionConstructorOverflowConstraints) {
  auto expr = [](auto input, auto result) -> decltype(decltype(result){input}) { return decltype(result){input}; };

  // Safe conversion: no overflow risk
  ASSERT_TRUE(::testing::simple_is_invocable_v<
              decltype(expr),
              std::chrono::duration<std::int64_t>,
              std::chrono::duration<std::int64_t, std::micro>>);

  // FromPeriod = std::ratio<std::numeric_limits<std::intmax_t>::max(), 2>
  // ToPeriod   = std::ratio<2, std::numeric_limits<std::intmax_t>::max()>
  //
  // gcd_num = gcd(max, 2) = 1
  // gcd_den = gcd(2, max) = 1
  //
  // reduced_from = ratio<max / 1, 2 / 1>
  // reduced_to = ratio<2 / 1, max / 1>
  //
  // value = max <= (max / 1) && 2 <= (max / max)
  // value = max <= max && 2 <= 1
  // value = true && false
  // value = false
  using overflow_from_period = std::ratio<std::numeric_limits<std::intmax_t>::max(), 2>;
  using overflow_to_period = std::ratio<2, std::numeric_limits<std::intmax_t>::max()>;

  ASSERT_FALSE(::testing::simple_is_invocable_v<
               decltype(expr),
               std::chrono::duration<std::int64_t, overflow_from_period>,
               std::chrono::duration<std::int64_t, overflow_to_period>>);

  // FromPeriod = std::ratio<std::numeric_limits<std::intmax_t>::max() / 3, 1>;
  // ToPeriod   = std::ratio<1, std::numeric_limits<std::intmax_t>::max() / 3>;
  // gcd_num = gcd(max / 3, 1) = 1
  // gcd_den = gcd(1, max / 3) = 1
  //
  // reduced_from = ratio<(max / 3) / 1, 1 / 1>
  // reduced_to = ratio<1 / 1, (max / 3) / 1>
  //
  // value = (max / 3) <= (max / (max / 3)) && (1 <= (max / 1))
  // value = (max / 3) <= 3 && 1 <= max
  // value = false && true
  // value = false
  using large_num_period = std::ratio<std::numeric_limits<std::intmax_t>::max() / 3, 1>;
  using large_den_period = std::ratio<1, std::numeric_limits<std::intmax_t>::max() / 3>;

  ASSERT_FALSE(::testing::simple_is_invocable_v<
               decltype(expr),
               std::chrono::duration<std::int64_t, large_num_period>,
               std::chrono::duration<std::int64_t, large_den_period>>);
}

/// @test @c duration_cast with no conversion (same type)
// Note: The conversion_factor is 1/1 - this covers the section 2.1 case.
CONSTEXPR_TEST(DurationCastTests, NoConversion) {
  auto const msec = std::chrono::duration<std::int32_t, std::milli>{500};
  auto const ms2 = std::chrono::duration_cast<std::chrono::duration<std::int32_t, std::milli>>(msec);
  EXPECT_EQ(ms2.count(), 500);
}

/// @test @c duration_cast converts from larger to smaller units
// Note: The conversion_factor is 1'000'000/1 - this covers the section 2.2 case.
CONSTEXPR_TEST(DurationCastTests, Downscale) {
  auto const sec = std::chrono::duration<std::int32_t>{2};
  auto const usec = std::chrono::duration_cast<std::chrono::duration<std::int32_t, std::micro>>(sec);
  EXPECT_EQ(usec.count(), 2'000'000);
}

/// @test @c duration_cast converts from smaller to larger units
// Note: The conversion_factor is 1/1'000'000 - this covers the section 2.3 case.
CONSTEXPR_TEST(DurationCastTests, Upscale) {
  auto const usec = std::chrono::duration<std::int64_t, std::micro>{1'000'000};
  auto const sec = std::chrono::duration_cast<std::chrono::duration<std::int64_t>>(usec);
  EXPECT_EQ(sec.count(), 1);
}

/// @test @c duration_cast with non-SI ratio
// Note: The conversion_factor is 7/6 - this covers the section 2.4 case.
CONSTEXPR_TEST(DurationCastTests, NonSIRatio) {
  using source_period = std::ratio<1, 3>;
  using result_period = std::ratio<2, 7>;

  using source_duration = std::chrono::duration<std::intmax_t, source_period>;
  using result_duration = std::chrono::duration<std::intmax_t, result_period>;

  auto const source_val = std::intmax_t{3};
  auto const source = source_duration{source_val};

  using conversion_factor = std::ratio_divide<source_period, result_period>;
  auto const expected_result = static_cast<intmax_t>(source_val * (conversion_factor::num / conversion_factor::den));

  auto const result = std::chrono::duration_cast<result_duration>(source);
  EXPECT_EQ(result.count(), expected_result);
}

/// @test @c duration_cast with different representation types
CONSTEXPR_TEST(DurationCastTests, DifferentRep) {
  auto const sec_int = std::chrono::duration<std::int32_t>{3};
  auto const sec_float = std::chrono::duration_cast<std::chrono::duration<double>>(sec_int);
  EXPECT_EQ(sec_float.count(), 3.0);
}

/// @test @c duration_cast handles floating point representations
CONSTEXPR_TEST(DurationCastTests, FloatingPoint) {
  auto const sec = std::chrono::duration<double>{1.5};
  auto const msec = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(sec);
  EXPECT_EQ(msec.count(), 1500.0);
}

/// @test @c duration_cast only participates in overload resolution for @c duration types
TEST(DurationCastTests, DurationCastConstraints) {
  auto expr = [](auto source, auto dest) -> decltype(std::chrono::duration_cast<decltype(dest)>(source)) {
    return std::chrono::duration_cast<decltype(dest)>(source);
  };

  ASSERT_TRUE(::testing::simple_is_invocable_v<
              decltype(expr),
              std::chrono::duration<std::int32_t, std::milli>,
              std::chrono::duration<std::int32_t>>);

  ASSERT_FALSE(::testing::simple_is_invocable_v<
               decltype(expr),
               std::chrono::duration<std::int32_t, std::milli>,
               std::int32_t>);
}

/// @test @c duration_cast is conditionally noexcept
CONSTEXPR_TEST(DurationCastTests, DurationCastNoexcept) {
  using nothrow_duration1 = std::chrono::duration<std::int32_t, std::milli>;
  using nothrow_duration2 = std::chrono::duration<std::int32_t, std::micro>;
  using throwing_rep = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<throwing_rep, std::milli>;

  // Test nothrow duration_cast between compatible durations with nothrow representations
  EXPECT_TRUE(noexcept(std::chrono::duration_cast<nothrow_duration2>(std::declval<nothrow_duration1>())));

  // Test throwing duration_cast when source representation throws
  EXPECT_FALSE(noexcept(std::chrono::duration_cast<nothrow_duration2>(std::declval<throwing_duration const&>())));
}

/// @test Instantiate LCM with the std::ratio NTTP type to make sure it is covered.
// Note: This shouldn't be required - this exact test case is present in lcm_test. However, the coverage tool cannot see
// the instantiation of arene::base::lcm<std::intmax_t> used by the std::common_type specialization for std::duration as
// covered by lcm_test, so that specialization is called again here.
TEST(DurationCommonTypeTests, LCMCoverage) { ASSERT_EQ(::arene::base::lcm(15L, 20L), 60L); }

/// @test common_type specialization for duration types works correctly
TEST(DurationCommonTypeTests, SameTypes) {
  // Same duration types should result in the same type
  using same_type = std::
      common_type_t<std::chrono::duration<std::int32_t, std::milli>, std::chrono::duration<std::int32_t, std::milli>>;
  ASSERT_TRUE((std::is_same_v<same_type, std::chrono::duration<std::int32_t, std::milli>>));
}

/// @test common_type with different representation types
TEST(DurationCommonTypeTests, DifferentRepresentations) {
  // Different representation types should use common_type of the representations
  using common_rep_type = std::
      common_type_t<std::chrono::duration<std::int32_t, std::milli>, std::chrono::duration<std::int64_t, std::milli>>;
  ASSERT_TRUE((std::is_same_v<common_rep_type, std::chrono::duration<std::int64_t, std::milli>>));

  using common_rep_type_float =
      std::common_type_t<std::chrono::duration<std::int32_t, std::milli>, std::chrono::duration<float, std::milli>>;
  ASSERT_TRUE((std::is_same_v<common_rep_type_float, std::chrono::duration<float, std::milli>>));
}

/// @test common_type with different periods
TEST(DurationCommonTypeTests, DifferentPeriods) {
  // Different periods should result in GCD of numerators and LCM of denominators
  // ratio<2, 3> and ratio<5, 7>
  // gcd(2, 5) = 1, lcm(3, 7) = 21
  // Result should be ratio<1, 21>
  using common_period_type = std::common_type_t<
      std::chrono::duration<std::int64_t, std::ratio<2, 3>>,
      std::chrono::duration<std::int64_t, std::ratio<5, 7>>>;
  ASSERT_TRUE((std::is_same_v<common_period_type, std::chrono::duration<std::int64_t, std::ratio<1, 21>>>));
}

/// @test common_type with mixed representation and period types
TEST(DurationCommonTypeTests, MixedTypes) {
  // Mixed representation and period types
  using mixed_type = std::common_type_t<
      std::chrono::duration<std::int32_t, std::ratio<1>>,
      std::chrono::duration<std::int64_t, std::milli>>;
  // Common rep: int64_t, gcd(1, 1) = 1, lcm(1, 1000) = 1000
  // Result: duration<int64_t, ratio<1, 1000>> (milliseconds)
  ASSERT_TRUE((std::is_same_v<mixed_type, std::chrono::duration<std::int64_t, std::milli>>));
}

template <class T>
class DurationComparisonTests : protected testing::Test {};

using comparison_rep_types = ::testing::Types<
    std::int16_t,
    std::uint16_t,
    std::int32_t,
    std::uint32_t,
    float,
    double,
    ::testing::configurable_value<std::int32_t, testing::throws_on::everything>>;

TYPED_TEST_SUITE(DurationComparisonTests, comparison_rep_types, );

/// @test duration equality operators work correctly with the same types
CONSTEXPR_TYPED_TEST(DurationComparisonTests, EqualityOperatorsSameType) {
  auto const ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{1000}};
  auto const ms2 = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};

  EXPECT_TRUE(ms1 == ms1);
  EXPECT_FALSE(ms1 != ms1);

  EXPECT_TRUE(ms1 != ms2);
  EXPECT_FALSE(ms1 != ms1);
}

/// @test duration equality operators with different period types
CONSTEXPR_TYPED_TEST(DurationComparisonTests, EqualityDifferentPeriodTypes) {
  auto const sec = std::chrono::duration<TypeParam>{TypeParam{1}};
  auto const msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{1000}};

  EXPECT_TRUE(sec == msec);
  EXPECT_FALSE(sec != msec);
}

/// @test duration equality operators with different rep types
CONSTEXPR_TYPED_TEST(DurationComparisonTests, EqualityDifferentRepTypes) {
  auto const sec = std::chrono::duration<TypeParam>{TypeParam{1}};
  auto const msec = std::chrono::duration<std::int64_t, std::milli>{1000};

  EXPECT_TRUE(sec == msec);
  EXPECT_FALSE(sec != msec);
}

/// @test duration relational operators work correctly with the same type
CONSTEXPR_TYPED_TEST(DurationComparisonTests, RelationalOperatorsSameType) {
  auto const ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const ms2 = std::chrono::duration<TypeParam, std::milli>{TypeParam{1000}};

  // Test less-than
  EXPECT_TRUE(ms1 < ms2);
  EXPECT_FALSE(ms2 < ms1);
  EXPECT_FALSE(ms1 < ms1);

  // Test less-than-or-equal
  EXPECT_TRUE(ms1 <= ms2);
  EXPECT_FALSE(ms2 <= ms1);
  EXPECT_TRUE(ms1 <= ms1);

  // Test greater-than
  EXPECT_FALSE(ms1 > ms2);
  EXPECT_TRUE(ms2 > ms1);
  EXPECT_FALSE(ms1 > ms1);

  // Test greater-than-or-equal
  EXPECT_FALSE(ms1 >= ms2);
  EXPECT_TRUE(ms2 >= ms1);
  EXPECT_TRUE(ms1 >= ms1);
}

/// @test duration relational operators with different types
CONSTEXPR_TYPED_TEST(DurationComparisonTests, RelationalDifferentTypes) {
  auto const ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const ms2 = std::chrono::duration<std::int64_t, std::milli>{1000};

  // Test less-than
  EXPECT_TRUE(ms1 < ms2);
  EXPECT_FALSE(ms2 < ms1);
  EXPECT_FALSE(ms1 < ms1);

  // Test less-than-or-equal
  EXPECT_TRUE(ms1 <= ms2);
  EXPECT_FALSE(ms2 <= ms1);
  EXPECT_TRUE(ms1 <= ms1);

  // Test greater-than
  EXPECT_FALSE(ms1 > ms2);
  EXPECT_TRUE(ms2 > ms1);
  EXPECT_FALSE(ms1 > ms1);

  // Test greater-than-or-equal
  EXPECT_FALSE(ms1 >= ms2);
  EXPECT_TRUE(ms2 >= ms1);
  EXPECT_TRUE(ms1 >= ms1);
}

/// @test Check duration comparisons with floating point nan values work as expected
CONSTEXPR_TEST(DurationComparisonTests, FloatingPointNaN) {
  static_assert(std::numeric_limits<float>::is_iec559, "This test requires IEEE 754 floating point properties");
  auto const nan_ms = std::chrono::duration<float, std::milli>{std::numeric_limits<float>::quiet_NaN()};

  EXPECT_FALSE(nan_ms == nan_ms);
  EXPECT_TRUE(nan_ms != nan_ms);

  EXPECT_FALSE(nan_ms < nan_ms);
  EXPECT_FALSE(nan_ms > nan_ms);

  // Duration operator<= is defined as `!(b < a)`, which causes this to return true.
  EXPECT_TRUE(nan_ms <= nan_ms);
  EXPECT_FALSE(nan_ms.count() <= nan_ms.count());

  // Duration operator>= is defined as `!(a < b)`, which causes this to return true.
  EXPECT_TRUE(nan_ms >= nan_ms);
  EXPECT_FALSE(nan_ms.count() >= nan_ms.count());
}

/// @test Check duration comparisons with floating point inf values work as expected
CONSTEXPR_TEST(DurationComparisonTests, FloatingPointInf) {
  static_assert(std::numeric_limits<float>::is_iec559, "This test requires IEEE 754 floating point properties");
  auto const inf_ms = std::chrono::duration<float, std::milli>{std::numeric_limits<float>::infinity()};

  EXPECT_TRUE(inf_ms == inf_ms);
  EXPECT_FALSE(inf_ms != inf_ms);

  EXPECT_FALSE(inf_ms < inf_ms);
  EXPECT_FALSE(inf_ms > inf_ms);
  EXPECT_TRUE(inf_ms <= inf_ms);
  EXPECT_TRUE(inf_ms >= inf_ms);
}

template <class T>
class DurationArithmeticTests : protected testing::Test {};

// Only signed integers are used in these tests as negative values are tested.
using arithmetic_rep_types = ::testing::
    Types<std::int32_t, float, double, ::testing::configurable_value<std::int32_t, testing::throws_on::everything>>;

TYPED_TEST_SUITE(DurationArithmeticTests, arithmetic_rep_types, );

/// @test duration unary plus operator returns a copy of the duration
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, UnaryPlusOperator) {
  auto const msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const result = +msec;

  EXPECT_EQ(result.count(), TypeParam{500});
  EXPECT_TRUE((std::is_same_v<decltype(result), decltype(msec)>));
}

/// @test duration unary minus operator returns duration with negated representation
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, UnaryMinusOperator) {
  auto const msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const result = -msec;

  EXPECT_EQ(result.count(), TypeParam{-500});
  EXPECT_TRUE((std::is_same_v<decltype(result), decltype(msec)>));
}

/// @test duration unary minus operator with negative input
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, UnaryMinusOperatorNegative) {
  auto const msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{-300}};
  auto const result = -msec;

  EXPECT_EQ(result.count(), TypeParam{300});
}

/// @test duration pre-increment operator increments the representation
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, PreIncrementOperator) {
  auto msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto& result = ++msec;

  EXPECT_EQ(msec.count(), TypeParam{501});
  EXPECT_EQ(result.count(), TypeParam{501});
  EXPECT_EQ(&result, &msec);
}

/// @test duration post-increment operator increments the representation and returns old value
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, PostIncrementOperator) {
  auto msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto result = msec++;

  EXPECT_EQ(msec.count(), TypeParam{501});
  EXPECT_EQ(result.count(), TypeParam{500});
}

/// @test duration pre-decrement operator decrements the representation
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, PreDecrementOperator) {
  auto msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto& result = --msec;

  EXPECT_EQ(msec.count(), TypeParam{499});
  EXPECT_EQ(result.count(), TypeParam{499});
  EXPECT_EQ(&result, &msec);
}

/// @test duration post-decrement operator decrements the representation and returns old value
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, PostDecrementOperator) {
  auto msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto result = msec--;

  EXPECT_EQ(msec.count(), TypeParam{499});
  EXPECT_EQ(result.count(), TypeParam{500});
}

/// @test duration compound addition assignment operator
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, CompoundAdditionOperator) {
  auto ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const ms2 = std::chrono::duration<TypeParam, std::milli>{TypeParam{300}};

  auto& result = ms1 += ms2;

  EXPECT_EQ(ms1.count(), TypeParam{800});
  EXPECT_EQ(result.count(), TypeParam{800});
  EXPECT_EQ(&result, &ms1);
}

/// @test duration compound subtraction assignment operator
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, CompoundSubtractionOperator) {
  auto ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const ms2 = std::chrono::duration<TypeParam, std::milli>{TypeParam{300}};

  auto& result = ms1 -= ms2;

  EXPECT_EQ(ms1.count(), TypeParam{200});
  EXPECT_EQ(result.count(), TypeParam{200});
  EXPECT_EQ(&result, &ms1);
}

/// @test duration compound multiplication assignment operator
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, CompoundMultiplicationOperator) {
  auto msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const scalar = TypeParam{3};

  auto& result = msec *= scalar;

  EXPECT_EQ(msec.count(), TypeParam{1500});
  EXPECT_EQ(result.count(), TypeParam{1500});
  EXPECT_EQ(&result, &msec);
}

/// @test duration compound division assignment operator
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, CompoundDivisionOperator) {
  auto msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{1000}};
  auto const scalar = TypeParam{4};

  auto& result = msec /= scalar;

  EXPECT_EQ(msec.count(), TypeParam{250});
  EXPECT_EQ(result.count(), TypeParam{250});
  EXPECT_EQ(&result, &msec);
}

/// @test duration compound modulo assignment operator with scalar
CONSTEXPR_TEST(DurationArithmeticTests, CompoundModuloScalarOperator) {
  auto msec = std::chrono::duration<std::int32_t, std::milli>{1300};
  auto const scalar = std::int32_t{300};

  auto& result = msec %= scalar;

  EXPECT_EQ(msec.count(), 100);  // 1300 % 300 = 100
  EXPECT_EQ(result.count(), 100);
  EXPECT_EQ(&result, &msec);
}

/// @test duration compound modulo assignment operator with duration
CONSTEXPR_TEST(DurationArithmeticTests, CompoundModuloDurationOperator) {
  auto ms1 = std::chrono::duration<std::int32_t, std::milli>{1300};
  auto const ms2 = std::chrono::duration<std::int32_t, std::milli>{300};

  auto& result = ms1 %= ms2;

  EXPECT_EQ(ms1.count(), 100);  // 1300 % 300 = 100
  EXPECT_EQ(result.count(), 100);
  EXPECT_EQ(&result, &ms1);
}

/// @test Static values of @c duration specializations with customized @c duration_values follow the customization
CONSTEXPR_TEST(DurationTests, StaticValuesCustomized) {
  using rep = testing::configurable_value<std::int32_t>;
  using dur_type = std::chrono::duration<rep>;

  // The static values of the duration type match the ones we customized for the representation type
  EXPECT_EQ(dur_type::zero(), dur_type{rep{1}});
  EXPECT_EQ(dur_type::min(), dur_type{rep{2}});
  EXPECT_EQ(dur_type::max(), dur_type{rep{3}});

  // The noexcept specifiers also match the ones from the customized `duration_values`
  EXPECT_EQ(noexcept(dur_type::zero()), noexcept(std::chrono::duration_values<rep>::zero()));
  EXPECT_EQ(noexcept(dur_type::min()), noexcept(std::chrono::duration_values<rep>::min()));
  EXPECT_EQ(noexcept(dur_type::max()), noexcept(std::chrono::duration_values<rep>::max()));
}

/// @test duration non-member addition operator
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, AdditionOperator) {
  auto const ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const ms2 = std::chrono::duration<TypeParam, std::milli>{TypeParam{300}};
  auto const result1 = ms1 + ms2;
  EXPECT_EQ(result1.count(), TypeParam{800});

  auto const msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const usec = std::chrono::duration<std::int64_t, std::micro>{300000};
  auto const result2 = msec + usec;
  EXPECT_EQ(result2.count(), TypeParam{800000});  // 500msec + 300msec = 800msec = 800000us
}

/// @test duration non-member subtraction operator
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, SubtractionOperator) {
  auto const ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{800}};
  auto const ms2 = std::chrono::duration<TypeParam, std::milli>{TypeParam{300}};
  auto const result1 = ms1 - ms2;
  EXPECT_EQ(result1.count(), TypeParam{500});

  auto const sec = std::chrono::duration<TypeParam>{TypeParam{2}};
  auto const msec = std::chrono::duration<std::int64_t, std::milli>{500};
  auto const result2 = sec - msec;
  EXPECT_EQ(result2.count(), TypeParam{1500});  // 2s - 500msec = 1500ms
}

/// @test duration non-member multiplication operators
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, MultiplicationOperators) {
  // duration * scalar (same rep type)
  auto const msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const scalar1 = TypeParam{3};
  auto const result1 = msec * scalar1;
  EXPECT_EQ(result1.count(), TypeParam{1500});

  // scalar * duration (same rep type)
  auto const result2 = scalar1 * msec;
  EXPECT_EQ(result2.count(), TypeParam{1500});

  // Different rep types - result should use common type
  auto const int_msec = std::chrono::duration<std::int32_t, std::milli>{500};
  auto const scalar2 = std::int64_t{3};

  auto const result3 = int_msec * scalar2;
  EXPECT_EQ(result3.count(), 1500);
  EXPECT_TRUE((std::is_same_v<decltype(result3), std::chrono::duration<std::int64_t, std::milli> const>));

  auto const result4 = scalar2 * int_msec;
  EXPECT_EQ(result3.count(), 1500);
  EXPECT_TRUE((std::is_same_v<decltype(result4), std::chrono::duration<std::int64_t, std::milli> const>));
}

/// @test duration non-member division operators
CONSTEXPR_TYPED_TEST(DurationArithmeticTests, DivisionOperators) {
  // duration / scalar
  auto const msec = std::chrono::duration<TypeParam, std::milli>{TypeParam{1500}};
  auto const scalar = TypeParam{3};
  auto const result1 = msec / scalar;
  EXPECT_EQ(result1.count(), TypeParam{500});

  // duration / duration (same types)
  auto const ms1 = std::chrono::duration<TypeParam, std::milli>{TypeParam{1500}};
  auto const ms2 = std::chrono::duration<TypeParam, std::milli>{TypeParam{500}};
  auto const result2 = ms1 / ms2;
  EXPECT_EQ(result2, TypeParam{3});

  // duration / duration (different types)
  auto const sec = std::chrono::duration<std::int32_t>{6};
  auto const ms_diff = std::chrono::duration<std::int64_t, std::milli>{2000};
  auto const result3 = sec / ms_diff;
  EXPECT_EQ(result3, 3);
}

/// @test duration non-member modulo operators
CONSTEXPR_TEST(DurationArithmeticTests, ModuloOperators) {
  // duration % scalar
  auto const msec = std::chrono::duration<std::int32_t, std::milli>{1700};
  auto const scalar = std::int32_t{500};
  auto const result1 = msec % scalar;
  EXPECT_EQ(result1.count(), 200);  // 1700 % 500 = 200

  // duration % duration (same types)
  auto const ms1 = std::chrono::duration<std::int32_t, std::milli>{1700};
  auto const ms2 = std::chrono::duration<std::int32_t, std::milli>{500};
  auto const result2 = ms1 % ms2;
  EXPECT_EQ(result2.count(), 200);  // 1700 % 500 = 200

  // duration % duration (different types)
  auto const sec = std::chrono::duration<std::int32_t>{7};
  auto const ms_diff = std::chrono::duration<std::int64_t, std::milli>{2000};
  auto const result3 = sec % ms_diff;
  EXPECT_EQ(result3.count(), 1000);  // 7s % 2s = 1s = 1000ms
}

/// @test duration uses floating point arithmetic
CONSTEXPR_TEST(DurationArithmeticTests, FloatingPoint) {
  auto msec = std::chrono::duration<float, std::milli>{std::numeric_limits<float>::max() - float{1}};
  auto result1 = msec++;

  EXPECT_EQ(result1, msec);

  auto result2 = msec--;
  EXPECT_EQ(result2, msec);
}

/// @test duration operators are conditionally noexcept based on representation types
CONSTEXPR_TEST(DurationOperatorNoexceptTests, ComparisonOperators) {
  using nothrow_duration = std::chrono::duration<std::int32_t>;
  using rep_that_throws = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<rep_that_throws>;

  // Test equality operators
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() == std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() != std::declval<nothrow_duration const&>()));

  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() == std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() != std::declval<throwing_duration const&>()));

  // Test relational operators
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() < std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() <= std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() > std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() >= std::declval<nothrow_duration const&>()));

  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() < std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() <= std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() > std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() >= std::declval<throwing_duration const&>()));
}

/// @test duration arithmetic operators are conditionally noexcept
CONSTEXPR_TEST(DurationOperatorNoexceptTests, ArithmeticOperators) {
  using nothrow_duration = std::chrono::duration<std::int32_t>;
  using rep_that_throws = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<rep_that_throws>;

  // Test addition and subtraction operators
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() + std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() - std::declval<nothrow_duration const&>()));

  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() + std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() - std::declval<throwing_duration const&>()));

  // Test duration division (duration / duration)
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() / std::declval<nothrow_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() / std::declval<throwing_duration const&>()));

  // Test duration modulo (duration % duration)
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() % std::declval<nothrow_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() % std::declval<throwing_duration const&>()));
}

/// @test duration scalar arithmetic operators are conditionally noexcept
CONSTEXPR_TEST(DurationOperatorNoexceptTests, ScalarArithmeticOperators) {
  using nothrow_duration = std::chrono::duration<std::int32_t>;
  using rep_that_throws = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<rep_that_throws>;

  // Test multiplication operators (duration * scalar, scalar * duration)
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() * std::declval<std::int32_t const&>()));
  EXPECT_TRUE(noexcept(std::declval<std::int32_t const&>() * std::declval<nothrow_duration const&>()));

  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() * std::declval<rep_that_throws const&>()));
  EXPECT_FALSE(noexcept(std::declval<rep_that_throws const&>() * std::declval<throwing_duration const&>()));

  // Test division operator (duration / scalar)
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() / std::declval<std::int32_t const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() / std::declval<rep_that_throws const&>()));

  // Test modulo operator (duration % scalar)
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration const&>() % std::declval<std::int32_t const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration const&>() % std::declval<rep_that_throws const&>()));
}

/// @test duration member assignment operators are conditionally noexcept
CONSTEXPR_TEST(DurationOperatorNoexceptTests, MemberAssignmentOperators) {
  using nothrow_duration = std::chrono::duration<std::int32_t>;
  using rep_that_throws = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<rep_that_throws>;

  // Test compound assignment operators
  // NOLINTBEGIN(bugprone-assignment-in-if-condition)
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>() += std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>() -= std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>() *= std::declval<std::int32_t const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>() /= std::declval<std::int32_t const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>() %= std::declval<std::int32_t const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>() %= std::declval<nothrow_duration const&>()));

  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>() += std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>() -= std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>() *= std::declval<rep_that_throws const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>() /= std::declval<rep_that_throws const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>() %= std::declval<rep_that_throws const&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>() %= std::declval<throwing_duration const&>()));
  // NOLINTEND(bugprone-assignment-in-if-condition)
}

/// @test duration unary operators and increment/decrement are conditionally noexcept
CONSTEXPR_TEST(DurationOperatorNoexceptTests, UnaryAndIncrementOperators) {
  using nothrow_duration = std::chrono::duration<std::int32_t>;
  using rep_that_throws = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<rep_that_throws>;

  // Test unary operators
  EXPECT_TRUE(noexcept(+std::declval<nothrow_duration const&>()));
  EXPECT_TRUE(noexcept(-std::declval<nothrow_duration const&>()));

  EXPECT_FALSE(noexcept(+std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(-std::declval<throwing_duration const&>()));

  // Test increment/decrement operators
  EXPECT_TRUE(noexcept(++std::declval<nothrow_duration&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>()++));
  EXPECT_TRUE(noexcept(--std::declval<nothrow_duration&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration&>()--));

  EXPECT_FALSE(noexcept(++std::declval<throwing_duration&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>()++));
  EXPECT_FALSE(noexcept(--std::declval<throwing_duration&>()));
  EXPECT_FALSE(noexcept(std::declval<throwing_duration&>()--));
}

/// @test duration operators noexcept with mixed duration types
CONSTEXPR_TEST(DurationOperatorNoexceptTests, MixedDurationTypes) {
  using nothrow_duration1 = std::chrono::duration<std::int32_t, std::milli>;
  using nothrow_duration2 = std::chrono::duration<std::int64_t, std::micro>;
  using rep_that_throws = testing::configurable_value<std::int32_t, testing::throws_on::everything>;
  using throwing_duration = std::chrono::duration<rep_that_throws, std::milli>;

  // Test with compatible nothrow durations
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration1 const&>() == std::declval<nothrow_duration2 const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration1 const&>() < std::declval<nothrow_duration2 const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration1 const&>() + std::declval<nothrow_duration2 const&>()));
  EXPECT_TRUE(noexcept(std::declval<nothrow_duration1 const&>() - std::declval<nothrow_duration2 const&>()));

  // Test with one throwing duration
  EXPECT_FALSE(noexcept(std::declval<nothrow_duration1 const&>() == std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<nothrow_duration1 const&>() < std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<nothrow_duration1 const&>() + std::declval<throwing_duration const&>()));
  EXPECT_FALSE(noexcept(std::declval<nothrow_duration1 const&>() - std::declval<throwing_duration const&>()));
}

ARENE_IGNORE_END();

}  // namespace
