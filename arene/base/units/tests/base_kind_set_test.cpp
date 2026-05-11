#include "arene/base/units/base_kind_set.hpp"

#include <gtest/gtest.h>

#include "arene/base/units.hpp"

namespace {

namespace test {
/// @brief create a copy of a value
template <class T>
auto copy(T const& value) -> T {
  return value;
}
}  // namespace test

using arene::base::units_detail::base_kind_exponents;

/// @test @c positive returns the stored positive exponent value
TEST(BaseKindExponent, PositiveMember) {
  ASSERT_EQ(42, (base_kind_exponents{42, 0}.positive()));
  ASSERT_EQ(0, base_kind_exponents{}.positive());
}

/// @test @c negative returns the stored negative exponent value
TEST(BaseKindExponent, NegativeMember) {
  ASSERT_EQ(42, (base_kind_exponents{0, 42}.negative()));
  ASSERT_EQ(0, base_kind_exponents{}.negative());
}

/// @test @c * with a positive scale value multiplies the exponent values
TEST(BaseKindExponent, MultiplyWithPositive) {
  constexpr auto scale = 3;
  constexpr auto exp = base_kind_exponents{1, 2};
  ASSERT_EQ(scale * exp.positive(), (scale * exp).positive());
  ASSERT_EQ(scale * exp.negative(), (scale * exp).negative());
}

/// @test @c * with a negative scale value multiplies the exponent values
TEST(BaseKindExponent, MultiplyWithNegative) {
  constexpr auto scale = 3;
  constexpr auto exp = base_kind_exponents{1, 2};
  ASSERT_EQ(scale * exp.negative(), (-scale * exp).positive());
  ASSERT_EQ(scale * exp.positive(), (-scale * exp).negative());
}

/// @test @c += adds other exponent values to the existing ones
TEST(BaseKindExponent, PlusEquals) {
  constexpr auto exp1 = base_kind_exponents{1, 2};
  constexpr auto exp2 = base_kind_exponents{3, 4};
  // NOLINTBEGIN(bugprone-assignment-in-if-condition)
  ASSERT_EQ(exp1.positive() + exp2.positive(), (test::copy(exp1) += exp2).positive());
  ASSERT_EQ(exp1.negative() + exp2.negative(), (test::copy(exp1) += exp2).negative());
  // NOLINTEND(bugprone-assignment-in-if-condition)
}

struct Kind1 : arene::base::quantity_kind<Kind1> {};
struct Kind2 : arene::base::quantity_kind<Kind2> {};
struct Kind3 : arene::base::quantity_kind<Kind3> {};

using arene::base::units_detail::base_kind_set;

/// @test @c base_kind_set::exponent_for can access and update exponent values
TEST(BaseKindSet, ExponentFor) {
  auto bases = base_kind_set<Kind1, Kind2, Kind3>{};
  ASSERT_EQ(0, bases.exponent_for<Kind1>().positive());
  ASSERT_EQ(0, bases.exponent_for<Kind1>().negative());
  ASSERT_EQ(0, bases.exponent_for<Kind2>().positive());
  ASSERT_EQ(0, bases.exponent_for<Kind2>().negative());
  ASSERT_EQ(0, bases.exponent_for<Kind3>().positive());
  ASSERT_EQ(0, bases.exponent_for<Kind3>().negative());

  bases.exponent_for<Kind2>() = base_kind_exponents{1, 2};
  ASSERT_EQ(1, bases.exponent_for<Kind2>().positive());
  ASSERT_EQ(2, bases.exponent_for<Kind2>().negative());
}

/// @test @c base_kind_set::operator* scales all contained exponent values
TEST(BaseKindSet, Multiply) {
  auto const bases = base_kind_set<Kind1, Kind2>{{base_kind_exponents{1, 2}, base_kind_exponents{}}};
  ASSERT_EQ(1, bases.exponent_for<Kind1>().positive());
  ASSERT_EQ(2, bases.exponent_for<Kind1>().negative());
  ASSERT_EQ(0, bases.exponent_for<Kind2>().positive());
  ASSERT_EQ(0, bases.exponent_for<Kind2>().negative());

  auto const scaled_bases = 3 * bases;
  ASSERT_EQ(3, scaled_bases.exponent_for<Kind1>().positive());
  ASSERT_EQ(6, scaled_bases.exponent_for<Kind1>().negative());
  ASSERT_EQ(0, scaled_bases.exponent_for<Kind2>().positive());
  ASSERT_EQ(0, scaled_bases.exponent_for<Kind2>().negative());
}

/// @test @c base_kind_set::add_exponents_from combines exponent values from a subset of explicit bases
TEST(BaseKindSet, AddExponentsFrom) {
  auto const bases1 = base_kind_set<Kind1, Kind2>{{base_kind_exponents{1, 2}, base_kind_exponents{}}};
  auto const sum1 = test::copy(bases1).add_exponents_from(bases1);

  ASSERT_EQ(2, sum1.exponent_for<Kind1>().positive());
  ASSERT_EQ(4, sum1.exponent_for<Kind1>().negative());
  ASSERT_EQ(0, sum1.exponent_for<Kind2>().positive());
  ASSERT_EQ(0, sum1.exponent_for<Kind2>().negative());

  auto const bases2 = base_kind_set<Kind2>{{base_kind_exponents{3, 4}}};
  auto const sum2 = test::copy(bases1).add_exponents_from(bases2);

  ASSERT_EQ(1, sum2.exponent_for<Kind1>().positive());
  ASSERT_EQ(2, sum2.exponent_for<Kind1>().negative());
  ASSERT_EQ(3, sum2.exponent_for<Kind2>().positive());
  ASSERT_EQ(4, sum2.exponent_for<Kind2>().negative());
}

/// @test @c combine_base_kind_set can join explicit bases where one is not the subset of the other
TEST(BaseKindSet, CombineBaseKindSet) {
  auto const bases1 = base_kind_set<Kind1, Kind2>{{base_kind_exponents{1, 2}, base_kind_exponents{}}};
  auto const bases2 = base_kind_set<Kind3>{{base_kind_exponents{3, 4}}};
  auto const sum = set_union(bases1, bases2);

  ASSERT_EQ(1, sum.exponent_for<Kind1>().positive());
  ASSERT_EQ(2, sum.exponent_for<Kind1>().negative());
  ASSERT_EQ(0, sum.exponent_for<Kind2>().positive());
  ASSERT_EQ(0, sum.exponent_for<Kind2>().negative());
  ASSERT_EQ(3, sum.exponent_for<Kind3>().positive());
  ASSERT_EQ(4, sum.exponent_for<Kind3>().negative());
}

}  // namespace
