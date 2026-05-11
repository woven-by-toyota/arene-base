#include "arene/base/testing/property_assertions.hpp"

#include <gtest/gtest.h>  // IWYU pragma: keep

// IWYU pragma: no_include "arene/base/optional/optional.hpp"
// IWYU pragma: no_include "arene/base/testing/property_error.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/detail/property_context.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::testing::detail::property_context;
using error_code = arene::base::testing::property_error::error_code;
using ::arene::base::literals::operator""_asv;

struct ArenePropBase : ::testing::Test {
  auto SetUp() -> void override { property_context().reset(); }
};

struct ArenePropAssert : ArenePropBase {};
struct ArenePropAssertFalse : ArenePropBase {};
struct ArenePropPre : ArenePropBase {};

/// @test @c ARENE_PROP_ASSERT does not set the property context if the
/// expression is @c true.
TEST_F(ArenePropAssert, Success) {
  [] { ARENE_PROP_ASSERT(1 + 1 == 2); }();

  ASSERT_FALSE(property_context().has_value());
}

/// @test @c ARENE_PROP_ASSERT can be invoked in a constant expression
CONSTEXPR_TEST(AreneProp, AssertSuccessCaseCanBeConstexpr) {
  struct succeeds {
    constexpr auto operator()() const { ARENE_PROP_ASSERT(1 + 1 == 2); }
  };

  succeeds{}();
}

/// @test @c ARENE_PROP_ASSERT sets the property context to @c
/// error_code::failure if the expression is @c false, with the correct file,
/// line number, and stringified expression
TEST_F(ArenePropAssert, Failure) {
  [] { ARENE_PROP_ASSERT(1 + 1 == 1); }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 46U);
  ASSERT_EQ(outcome.strings.arg1, "1 + 1 == 1"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_ASSERT converts the expression to a boolean
TEST_F(ArenePropAssert, ConvertsExpression) {
  [] { ARENE_PROP_ASSERT(nullptr); }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 60U);
  ASSERT_EQ(outcome.strings.arg1, "nullptr"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_ASSERT can be used in a helper function
TEST_F(ArenePropAssert, UseInHelper) {
  auto const helper = [] { ARENE_PROP_ASSERT(nullptr); };
  [&helper] {
    helper();
    ARENE_PROP_ASSERT(false);
  }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 74U);
  ASSERT_EQ(outcome.strings.arg1, "nullptr"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_ASSERT_FALSE does not set the property context if the
/// expression is @c false.
TEST_F(ArenePropAssertFalse, Success) {
  [] { ARENE_PROP_ASSERT_FALSE(1 + 1 == 3); }();

  ASSERT_FALSE(property_context().has_value());
}

/// @test @c ARENE_PROP_ASSERT_FALSE can be invoked in a constant expression
CONSTEXPR_TEST(AreneProp, AssertFalseSuccessCaseCanBeConstexpr) {
  struct succeeds {
    constexpr auto operator()() const { ARENE_PROP_ASSERT_FALSE(1 + 1 == 3); }
  };

  succeeds{}();
}

/// @test @c ARENE_PROP_ASSERT_FALSE returns @c error_code::failure if the
// expression is @c true, with the correct file, line number, and stringified expression
TEST_F(ArenePropAssertFalse, Failure) {
  [] { ARENE_PROP_ASSERT_FALSE(1 + 2 == 3); }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 110U);
  ASSERT_EQ(outcome.strings.arg1, "!(1 + 2 == 3)"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_ASSERT_FALSE converts the expression to a boolean
TEST_F(ArenePropAssertFalse, ConvertsExpression) {
  [] { ARENE_PROP_ASSERT_FALSE(2); }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 124U);
  ASSERT_EQ(outcome.strings.arg1, "!(2)"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_ASSERT_FALSE can be used in a helper function
TEST_F(ArenePropAssertFalse, UseInHelper) {
  auto const helper = [] { ARENE_PROP_ASSERT_FALSE(2); };
  [&helper] {
    helper();
    ARENE_PROP_ASSERT_FALSE(true);
  }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 138U);
  ASSERT_EQ(outcome.strings.arg1, "!(2)"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_PRE returns @c error_code::discard if the
// expression is @c false, with the correct file, line number, and stringified expression
TEST_F(ArenePropPre, Discard) {
  [] { ARENE_PROP_PRE(false); }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::discard);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 157U);
  ASSERT_EQ(outcome.strings.arg1, "false"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_PRE is skipped if the expression is @c true,
TEST_F(ArenePropPre, Skip) {
  [] { ARENE_PROP_PRE(true); }();

  ASSERT_FALSE(property_context().has_value());
}

/// @test @c ARENE_PROP_PRE converts the expression to a boolean
TEST_F(ArenePropPre, ConvertsExpression) {
  [] { ARENE_PROP_PRE(1 + 1); }();

  ASSERT_FALSE(property_context().has_value());
}

/// @test @c ARENE_PROP_PRE can be used in a helper function
TEST_F(ArenePropPre, UseInHelper) {
  auto const helper = [] { ARENE_PROP_PRE(false); };
  [&helper] {
    helper();
    ARENE_PROP_PRE(0);
  }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::discard);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 185U);
  ASSERT_EQ(outcome.strings.arg1, "false"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

struct ArenePropAssertTrue : ArenePropBase {};

/// @test @c ARENE_PROP_ASSERT_TRUE does not set the property context if the
/// expression is @c true.
TEST_F(ArenePropAssertTrue, Success) {
  [] { ARENE_PROP_ASSERT_TRUE(1 + 1 == 2); }();

  ASSERT_FALSE(property_context().has_value());
}

/// @test @c ARENE_PROP_ASSERT_TRUE can be invoked in a constant expression
CONSTEXPR_TEST(AreneProp, AssertTrueSuccessCaseCanBeConstexpr) {
  struct succeeds {
    constexpr auto operator()() const { ARENE_PROP_ASSERT_TRUE(1 + 1 == 2); }
  };

  succeeds{}();
}

/// @test @c ARENE_PROP_ASSERT_TRUE sets the property context to @c
/// error_code::failure if the expression is @c false, with the correct file,
/// line number, and stringified expression
TEST_F(ArenePropAssertTrue, Failure) {
  [] { ARENE_PROP_ASSERT_TRUE(1 + 1 == 1); }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 224U);
  ASSERT_EQ(outcome.strings.arg1, "1 + 1 == 1"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_ASSERT_TRUE converts the expression to a boolean
TEST_F(ArenePropAssertTrue, ConvertsExpression) {
  [] { ARENE_PROP_ASSERT_TRUE(nullptr); }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 238U);
  ASSERT_EQ(outcome.strings.arg1, "nullptr"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}

/// @test @c ARENE_PROP_ASSERT_TRUE can be used in a helper function
TEST_F(ArenePropAssertTrue, UseInHelper) {
  auto const helper = [] { ARENE_PROP_ASSERT_TRUE(nullptr); };
  [&helper] {
    helper();
    ARENE_PROP_ASSERT_TRUE(false);
  }();

  auto const& outcome = *property_context();

  ASSERT_EQ(outcome.error, error_code::failure);
  ASSERT_EQ(outcome.location.file, "arene/base/testing/tests/property_assertions_test.cpp"_asv);
  ASSERT_EQ(outcome.location.line, 252U);
  ASSERT_EQ(outcome.strings.arg1, "nullptr"_asv);
  ASSERT_EQ(outcome.strings.description, ""_asv);
  ASSERT_EQ(outcome.strings.arg2, ""_asv);
}
}  // namespace
