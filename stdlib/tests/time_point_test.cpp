// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "All arithmetic is to/from zero, so equality is OK even for floating point");

#include "arene/base/type_traits/is_invocable.hpp"
#include "stdlib/include/chrono"
#include "stdlib/include/cstdint"
#include "stdlib/include/functional"
#include "stdlib/include/ratio"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {

template <typename Rep, typename Tag = void>
class dummy_clock {
 public:
  using rep = Rep;
  using period = std::ratio<1>;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<dummy_clock>;

  static constexpr bool is_steady{true};

  static constexpr auto now() noexcept -> time_point { return {}; }
};

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::time_point;
using std::chrono::time_point_cast;

template <typename Rep>
using test_time_point = time_point<dummy_clock<Rep>>;

template <class T>
class TimePoint : testing::Test {
 protected:
  /// @brief A duration type convertible to that of @c T
  using convertible_dur = duration<typename T::rep, std::ratio_multiply<typename T::duration::period, std::kilo>>;
  /// @brief A time point on the same clock as @c T but using @c convertible_dur
  using convertible_point = time_point<typename T::clock, convertible_dur>;
  // Need to use min and max from TypeParam's duration for this so there's no overflow when converting
  static constexpr convertible_dur max_convertible_duration{duration_cast<convertible_dur>(T::duration::max())};
  static constexpr convertible_dur min_convertible_duration{duration_cast<convertible_dur>(T::duration::min())};
  static constexpr convertible_point max_convertible_point{max_convertible_duration};
  static constexpr convertible_point min_convertible_point{min_convertible_duration};

  /// @brief A duration type *not* implicitly convertible to that of @c T
  using unconvertible_dur = duration<typename T::rep, std::ratio_multiply<typename T::duration::period, std::milli>>;
  /// @brief A time point on the same clock as @c T but using @c unconvertible_dur
  using unconvertible_point = time_point<typename T::clock, unconvertible_dur>;

  /// @brief A time_point that differs from @c T only in that they refer to different clocks
  using other_clock_point = time_point<dummy_clock<typename T::rep, struct distinct_tag>, typename T::duration>;
};

template <class T>
constexpr typename TimePoint<T>::convertible_dur TimePoint<T>::max_convertible_duration;
template <class T>
constexpr typename TimePoint<T>::convertible_dur TimePoint<T>::min_convertible_duration;
template <class T>
constexpr typename TimePoint<T>::convertible_point TimePoint<T>::max_convertible_point;
template <class T>
constexpr typename TimePoint<T>::convertible_point TimePoint<T>::min_convertible_point;

using time_point_types = ::testing::
    Types<test_time_point<float>, test_time_point<double>, test_time_point<int64_t>, test_time_point<uint64_t>>;

TYPED_TEST_SUITE(TimePoint, time_point_types, );

/// @test Can instantiate a @c time_point with its default value
CONSTEXPR_TYPED_TEST(TimePoint, DefaultConstruction) {
  TypeParam point{};
  EXPECT_EQ(point.time_since_epoch(), TypeParam::duration::zero());

  static_assert(
      std::is_nothrow_default_constructible_v<TypeParam> ==
          std::is_nothrow_default_constructible_v<typename TypeParam::duration>,
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Can construct a @c time_point from an instance of its duration type
CONSTEXPR_TYPED_TEST(TimePoint, ConstructionFromDuration) {
  typename TypeParam::duration const dur{100};
  TypeParam point{dur};
  EXPECT_EQ(point.time_since_epoch(), dur);

  static_assert(
      std::is_nothrow_constructible_v<TypeParam, typename TypeParam::duration> ==
          std::is_nothrow_copy_constructible_v<typename TypeParam::duration>,
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Can convert a @c time_point from another one if and only if the clocks match and the durations are convertible
CONSTEXPR_TYPED_TEST(TimePoint, ConversionFromOtherTimePoint) {
  using int_dur = duration<int>;
  using int_point = time_point<typename TypeParam::clock, int_dur>;
  using float_dur = duration<float>;
  using float_point = time_point<typename TypeParam::clock, float_dur>;

  static_assert(
      std::is_convertible_v<TypeParam, int_point> == std::is_convertible_v<typename TypeParam::duration, int_dur>,
      "int convertibility did not match"
  );
  static_assert(
      std::is_convertible_v<TypeParam, float_point> == std::is_convertible_v<typename TypeParam::duration, float_dur>,
      "float convertibility did not match"
  );

  static_assert(
      std::is_nothrow_constructible_v<TypeParam, int_point> ==
          std::is_nothrow_constructible_v<typename TypeParam::duration, int_dur>,
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      std::is_nothrow_constructible_v<TypeParam, float_point> ==
          std::is_nothrow_constructible_v<typename TypeParam::duration, float_dur>,
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Conversion from another time point gives the right value
CONDITIONAL_TYPED_TEST(
    TimePoint,
    ConversionYieldsCorrectValue,
    (std::is_constructible_v<TypeParam, dummy_clock<int>::time_point>)
) {
  using int_point = dummy_clock<int>::time_point;

  constexpr int_point ipt{int_point::duration{100}};
  constexpr TypeParam point{ipt};
  static_assert(point == ipt, "Converting from another point type should give equal values");
}

/// @test Can copy construct a @c time_point
CONSTEXPR_TYPED_TEST(TimePoint, CopyConstruction) {
  TypeParam point{typename TypeParam::duration{100}};
  TypeParam copy{point};
  ASSERT_EQ(copy, point);

  static_assert(
      std::is_nothrow_copy_constructible_v<TypeParam> ==
          std::is_nothrow_copy_constructible_v<typename TypeParam::duration>,
      "noexcept should match that of the one data member"
  );
}

/// @test Can move construct a @c time_point
CONSTEXPR_TYPED_TEST(TimePoint, MoveConstruction) {
  TypeParam point{typename TypeParam::duration{100}};
  TypeParam copy{point};
  TypeParam moved{std::move(copy)};
  ASSERT_EQ(moved, point);

  static_assert(
      std::is_nothrow_move_constructible_v<TypeParam> ==
          std::is_nothrow_move_constructible_v<typename TypeParam::duration>,
      "noexcept should match that of the one data member"
  );
}

/// @test Can copy assign a @c time_point
CONSTEXPR_TYPED_TEST(TimePoint, CopyAssignment) {
  TypeParam point{typename TypeParam::duration{100}};
  TypeParam copy{};
  copy = point;
  ASSERT_EQ(copy, point);

  static_assert(
      std::is_nothrow_copy_assignable_v<TypeParam> == std::is_nothrow_copy_assignable_v<typename TypeParam::duration>,
      "noexcept should match that of the one data member"
  );
}

/// @test Can move assign a @c time_point
CONSTEXPR_TYPED_TEST(TimePoint, MoveAssignment) {
  TypeParam point{typename TypeParam::duration{100}};
  TypeParam copy{point};
  TypeParam moved{};
  moved = std::move(copy);
  ASSERT_EQ(moved, point);

  static_assert(
      std::is_nothrow_move_assignable_v<TypeParam> == std::is_nothrow_move_assignable_v<typename TypeParam::duration>,
      "noexcept should match that of the one data member"
  );
}

/// @test Can move a @c time_point forward and backward
TYPED_TEST(TimePoint, AddAndSubtract) {
  // This test is not CONSTEXPR_ because these operations are not constexpr until C++17.
  TypeParam point{};
  EXPECT_EQ(point.time_since_epoch(), TypeParam::duration::zero());

  point += TypeParam::duration::max();
  EXPECT_EQ(point, TypeParam::max());

  point -= TypeParam::duration::max();
  EXPECT_EQ(point.time_since_epoch(), TypeParam::duration::zero());

  point += TypeParam::duration::min();
  EXPECT_EQ(point, TypeParam::min());

  point -= TypeParam::duration::min();
  EXPECT_EQ(point.time_since_epoch(), TypeParam::duration::zero());

  static_assert(
      noexcept(point += TypeParam::duration::max()) ==
          noexcept(std::declval<typename TypeParam::duration&>() += TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(point -= TypeParam::duration::max()) ==
          noexcept(std::declval<typename TypeParam::duration&>() -= TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Can move a @c time_point forward and backward using a different (but convertible) duration type
TYPED_TEST(TimePoint, AddAndSubtractWithConvertibleDuration) {
  // This test is not CONSTEXPR_ because these operations are not constexpr until C++17.

  using other_dur = typename TestFixture::convertible_dur;
  constexpr other_dur max_dur{TestFixture::max_convertible_duration};
  constexpr other_dur min_dur{TestFixture::min_convertible_duration};
  constexpr auto max_point = TestFixture::max_convertible_point;
  constexpr auto min_point = TestFixture::min_convertible_point;

  TypeParam point{};
  ASSERT_EQ(point.time_since_epoch(), other_dur::zero());

  point += max_dur;
  ASSERT_EQ(point, max_point);

  point -= max_dur;
  ASSERT_EQ(point.time_since_epoch(), other_dur::zero());

  point += min_dur;
  ASSERT_EQ(point, min_point);

  point -= min_dur;
  ASSERT_EQ(point.time_since_epoch(), other_dur::zero());

  static_assert(
      noexcept(point += max_dur) == noexcept(std::declval<typename TypeParam::duration&>() += max_dur),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(point -= max_dur) == noexcept(std::declval<typename TypeParam::duration&>() -= max_dur),
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Can use arithmetic operators on a @c time_point
CONSTEXPR_TYPED_TEST(TimePoint, NonMemberArithmetic) {
  ASSERT_EQ(TypeParam{} + TypeParam::duration::max(), TypeParam::max());
  ASSERT_EQ(TypeParam::duration::max() + TypeParam{}, TypeParam::max());
  ASSERT_EQ(TypeParam::max() - TypeParam::duration::max(), TypeParam{});
  ASSERT_EQ(TypeParam::max() - TypeParam::max(), TypeParam::duration::zero());

  // NOLINTBEGIN(misc-redundant-expression) These tests are confirming that the expressions are in fact equivalent
  static_assert(
      noexcept(TypeParam{} + TypeParam::duration::max()) ==
          noexcept(TypeParam::duration::max() + TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam::duration::max() + TypeParam{}) ==
          noexcept(TypeParam::duration::max() + TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} - TypeParam::duration::max()) ==
          noexcept(TypeParam::duration::max() - TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} - TypeParam{}) == noexcept(TypeParam::duration::max() - TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  // NOLINTEND(misc-redundant-expression)
}

/// @test Can use arithmetic operators on a @c time_point with a different (but convertible) duration type
CONSTEXPR_TYPED_TEST(TimePoint, NonMemberArithmeticWithConvertibleDuration) {
  constexpr auto max_dur{TestFixture::max_convertible_duration};
  constexpr auto min_dur{TestFixture::min_convertible_duration};
  constexpr TypeParam max_point{max_dur};
  constexpr TypeParam min_point{min_dur};

  ASSERT_EQ(TypeParam{} + max_dur, max_point);
  ASSERT_EQ(max_dur + TypeParam{}, max_point);
  ASSERT_EQ(min_point - min_dur, TypeParam{});
  ASSERT_EQ(max_point - TestFixture::max_convertible_point, TypeParam::duration::zero());

  static_assert(
      noexcept(TypeParam{} + max_dur) == noexcept(TypeParam::duration::max() + max_dur),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(max_dur + TypeParam{}) == noexcept(max_dur + TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} - max_dur) == noexcept(TypeParam::duration::max() - max_dur),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} - TestFixture::max_convertible_point) == noexcept(TypeParam::duration::max() - max_dur),
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Can use arithmetic operators on a @c time_point with a different (and not convertible) duration type
CONSTEXPR_TYPED_TEST(TimePoint, NonMemberArithmeticWithUnconvertibleDuration) {
  using other_point = typename TestFixture::unconvertible_point;
  constexpr auto other_dur = typename TestFixture::unconvertible_dur{10'000};
  constexpr auto converted_dur = duration_cast<typename TypeParam::duration>(other_dur);

  ASSERT_EQ(TypeParam{} + other_dur, other_point{other_dur});
  ASSERT_EQ(other_dur + TypeParam{}, other_point{other_dur});
  ASSERT_EQ(TypeParam{converted_dur} - other_dur, TypeParam{});
  ASSERT_EQ(TypeParam{converted_dur} - other_point{other_dur}, TypeParam::duration::zero());

  static_assert(
      noexcept(TypeParam{} + other_dur) == noexcept(TypeParam::duration::max() + other_dur),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(other_dur + TypeParam{}) == noexcept(other_dur + TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} - other_dur) == noexcept(TypeParam::duration::max() - other_dur),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} - other_point{}) == noexcept(TypeParam::duration::max() - other_dur),
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Comparisons all work between @c min() and @c max()
CONSTEXPR_TYPED_TEST(TimePoint, Comparisons) {
  ASSERT_EQ(TypeParam::min(), TypeParam::min());
  ASSERT_EQ(TypeParam::max(), TypeParam::max());

  ASSERT_NE(TypeParam::min(), TypeParam::max());
  ASSERT_LT(TypeParam::min(), TypeParam::max());
  ASSERT_LE(TypeParam::min(), TypeParam::max());
  ASSERT_GT(TypeParam::max(), TypeParam::min());
  ASSERT_GE(TypeParam::max(), TypeParam::min());

  ASSERT_FALSE(TypeParam::min() == TypeParam::max());
  ASSERT_FALSE(TypeParam::min() != TypeParam::min());
  ASSERT_FALSE(TypeParam::max() < TypeParam::min());
  ASSERT_FALSE(TypeParam::max() <= TypeParam::min());
  ASSERT_FALSE(TypeParam::min() > TypeParam::max());
  ASSERT_FALSE(TypeParam::min() >= TypeParam::max());

  // NOLINTBEGIN(misc-redundant-expression) These tests are confirming that the expressions are in fact equivalent
  static_assert(
      noexcept(TypeParam{} == TypeParam{}) == noexcept(TypeParam::duration::max() == TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} != TypeParam{}) == noexcept(TypeParam::duration::max() != TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} < TypeParam{}) == noexcept(TypeParam::duration::max() < TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} <= TypeParam{}) == noexcept(TypeParam::duration::max() <= TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} > TypeParam{}) == noexcept(TypeParam::duration::max() > TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(TypeParam{} >= TypeParam{}) == noexcept(TypeParam::duration::max() >= TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  // NOLINTEND(misc-redundant-expression)
}

/// @test Comparisons all work between @c min() and @c max() even when one of the types is different
CONSTEXPR_TYPED_TEST(TimePoint, ComparisonsWithDifferentType) {
  constexpr TypeParam low{typename TypeParam::duration{10}};
  constexpr TypeParam high{typename TypeParam::duration{100}};
  constexpr auto other_low = typename TestFixture::unconvertible_point{low};
  constexpr auto other_high = typename TestFixture::unconvertible_point{high};

  ASSERT_EQ(low, other_low);
  ASSERT_EQ(other_high, high);

  ASSERT_NE(low, other_high);
  ASSERT_LT(low, other_high);
  ASSERT_LE(low, other_high);
  ASSERT_GT(high, other_low);
  ASSERT_GE(high, other_low);

  ASSERT_FALSE(other_low == high);
  ASSERT_FALSE(other_low != low);
  ASSERT_FALSE(other_high < low);
  ASSERT_FALSE(other_high <= low);
  ASSERT_FALSE(other_low > high);
  ASSERT_FALSE(other_low >= high);

  static_assert(
      noexcept(low == other_low) == noexcept(TypeParam::duration::max() == TestFixture::unconvertible_dur::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(low != other_low) == noexcept(TypeParam::duration::max() != TestFixture::unconvertible_dur::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(low < other_low) == noexcept(TypeParam::duration::max() < TestFixture::unconvertible_dur::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(low <= other_low) == noexcept(TypeParam::duration::max() <= TestFixture::unconvertible_dur::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(low > other_low) == noexcept(TypeParam::duration::max() > TestFixture::unconvertible_dur::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(low >= other_low) == noexcept(TypeParam::duration::max() >= TestFixture::unconvertible_dur::max()),
      "noexcept specification should match TypeParam::duration"
  );

  static_assert(
      noexcept(other_low == low) == noexcept(TestFixture::unconvertible_dur::max() == TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(other_low != low) == noexcept(TestFixture::unconvertible_dur::max() != TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(other_low < low) == noexcept(TestFixture::unconvertible_dur::max() < TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(other_low <= low) == noexcept(TestFixture::unconvertible_dur::max() <= TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(other_low > low) == noexcept(TestFixture::unconvertible_dur::max() > TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
  static_assert(
      noexcept(other_low >= low) == noexcept(TestFixture::unconvertible_dur::max() >= TypeParam::duration::max()),
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Comparisons with NaN behave in the weird way required by the standard
CONDITIONAL_TYPED_TEST(TimePoint, ComparisonWithNan, std::numeric_limits<typename TypeParam::rep>::has_quiet_NaN) {
  constexpr auto nan = std::numeric_limits<typename TypeParam::rep>::quiet_NaN();
  constexpr typename TypeParam::duration nan_dur{nan};
  constexpr TypeParam nan_point{nan_dur};

  // All direct comparisons with NaN are false except for !=
  ASSERT_FALSE(0 == nan);
  ASSERT_TRUE(0 != nan);
  ASSERT_FALSE(0 < nan);
  ASSERT_FALSE(0 <= nan);
  ASSERT_FALSE(0 > nan);
  ASSERT_FALSE(0 >= nan);

  // However, due to the fact these are defined in terms of <, <= and >= are both true until C++20
  ASSERT_FALSE(TypeParam{} == nan_point);
  ASSERT_TRUE(TypeParam{} != nan_point);
  ASSERT_FALSE(TypeParam{} < nan_point);
  ASSERT_TRUE(TypeParam{} <= nan_point);
  ASSERT_FALSE(TypeParam{} > nan_point);
  ASSERT_TRUE(TypeParam{} >= nan_point);
}

/// @test @c time_point_cast works
CONSTEXPR_TYPED_TEST(TimePoint, TimePointCast) {
  TypeParam point{typename TypeParam::duration{10'000}};

  using coarse_duration = duration<
      typename TypeParam::duration::rep,
      std::ratio_multiply<std::ratio<100>, typename TypeParam::duration::period>>;
  using coarse_time_point = time_point<typename TypeParam::clock, coarse_duration>;
  coarse_time_point const coarse_point{coarse_duration{100}};

  auto converted_point = time_point_cast<typename TypeParam::duration>(coarse_point);
  EXPECT_EQ(point, converted_point);

  static_assert(
      noexcept(time_point_cast<typename TypeParam::duration>(coarse_point)) ==
          noexcept(duration_cast<typename TypeParam::duration>(coarse_duration{})),
      "noexcept specification should match TypeParam::duration"
  );
}

/// @test Subtraction fails between time point types with different clocks
TYPED_TEST(TimePoint, SubtractionFailsForDifferentClocks) {
  using other_point = typename TestFixture::other_clock_point;
  ASSERT_FALSE(arene::base::is_invocable_v<std::minus<void>, TypeParam, other_point>);
}

/// @test Comparison operators fail between time point types with different clocks
TYPED_TEST(TimePoint, CompareFailsForDifferentClocks) {
  using other_point = typename TestFixture::other_clock_point;

  ASSERT_FALSE(arene::base::is_invocable_v<std::equal_to<void>, TypeParam, other_point>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::not_equal_to<void>, TypeParam, other_point>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::less<void>, TypeParam, other_point>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::less_equal<void>, TypeParam, other_point>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::greater<void>, TypeParam, other_point>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::greater_equal<void>, TypeParam, other_point>);

  ASSERT_FALSE(arene::base::is_invocable_v<std::equal_to<void>, other_point, TypeParam>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::not_equal_to<void>, other_point, TypeParam>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::less<void>, other_point, TypeParam>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::less_equal<void>, other_point, TypeParam>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::greater<void>, other_point, TypeParam>);
  ASSERT_FALSE(arene::base::is_invocable_v<std::greater_equal<void>, other_point, TypeParam>);
}

/// @test @c time_point_cast fails between time point types with different clocks
TYPED_TEST(TimePoint, TimePointCastFailsForDifferentClocks) {
  auto cast = FUNCTION_LIFT(time_point_cast<typename TestFixture::other_clock_point>);
  ASSERT_FALSE(arene::base::is_invocable_v<decltype(cast), TypeParam>);
}

ARENE_IGNORE_END();

}  // namespace
