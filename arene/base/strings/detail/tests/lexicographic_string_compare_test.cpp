// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/detail/lexicographic_string_compare.hpp"

#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "arene/base/compare.hpp"
#include "arene/base/span.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::opposite_ordering;
using ::arene::base::span;
using ::arene::base::strong_ordering;
using ::arene::base::detail::lexicographic_string_compare;

/// @test Given two input strings `span<char const> lhs` and `span<char const> rhs` where `lhs` is a default constructed
/// span and `rhs` is not, `lexicographic_string_compare(lhs,rhs)` returns `strong_ordering::less`.
TEST(SpanLexicographicStringCompare, ReturnsLessIfLhsIsDefaultConstructed) {
  constexpr auto* str = "foo";
  STATIC_ASSERT_EQ(lexicographic_string_compare(span<char const>{}, span<char const>{str, 3}), strong_ordering::less);
}

/// @test Given two input strings `span<char const> lhs` and `span<char const> rhs` where `rhs` is a default constructed
/// span and `lhs` is not, `lexicographic_string_compare(lhs,rhs)` returns `strong_ordering::greater`.
TEST(SpanLexicographicStringCompare, ReturnsGreaterIfRhsIsDefaultConstructed) {
  constexpr auto* str = "foo";
  STATIC_ASSERT_EQ(
      lexicographic_string_compare(span<char const>{str, 3}, span<char const>{}),
      strong_ordering::greater
  );
}

/// @test Given two input strings `span<char const> lhs` and `span<char const> rhs` where both `lhs` and `rhs` are
/// default constructed spans, `lexicographic_string_compare(lhs,rhs)` returns `strong_ordering::equal`.
TEST(SpanLexicographicStringCompare, ReturnsEqualIfLhsAndRhsAreDefaultConstructed) {
  STATIC_ASSERT_EQ(lexicographic_string_compare(span<char const>{}, span<char const>{}), strong_ordering::equal);
}

struct LexicographicStringCompareTest : ::testing::TestWithParam<std::tuple<std::string, std::string>> {
  std::string const lhs;
  std::string const rhs;
  strong_ordering const expected_from_string_compare;
  LexicographicStringCompareTest()
      : lhs(std::get<0>(this->GetParam())),
        rhs(std::get<1>(this->GetParam())),
        expected_from_string_compare(::arene::base::make_strong_ordering(lhs.compare(rhs))) {}
};

/// @test Given two input strings `const char* lhs` and `const char* rhs` which are not nullptrs,
/// `lexicographic_string_compare(lhs,rhs)` returns a `strong_ordering` equivalent to the result of
/// `std::string::compare` on two strings constructed from the inputs strings.
TEST_P(LexicographicStringCompareTest, BothOperandsAsCStringsProducesAStrongOrderingEquivalentToStdString) {
  EXPECT_EQ(lexicographic_string_compare(lhs.data(), rhs.data()), expected_from_string_compare);
  EXPECT_EQ(
      lexicographic_string_compare(lhs.data(), rhs.data()),
      opposite_ordering(lexicographic_string_compare(rhs.data(), lhs.data()))
  );
}

/// @test Given two input strings `span<char const> lhs` and `const char* rhs`, where rhs it not nullptr,
/// `lexicographic_string_compare(lhs,rhs)` returns a `strong_ordering` equivalent to the result of
/// `std::string::compare` on two strings constructed from the inputs strings.
TEST_P(LexicographicStringCompareTest, LhsAsSpanRhsAsCStringsProducesAStrongOrderingEquivalentToStdString) {
  EXPECT_EQ(
      lexicographic_string_compare(span<char const>{lhs.data(), lhs.size()}, rhs.data()),
      expected_from_string_compare
  );
  EXPECT_EQ(
      lexicographic_string_compare(span<char const>{lhs.data(), lhs.size()}, rhs.data()),
      opposite_ordering(lexicographic_string_compare(span<char const>{rhs.data(), rhs.size()}, lhs.data()))
  );
}

/// @test Given two input strings `span<char const> lhs` and `span<char const> rhs`,
/// `lexicographic_string_compare(lhs,rhs)` returns a `strong_ordering` equivalent to the result of
/// `std::string::compare` on two strings constructed from the inputs strings.
TEST_P(LexicographicStringCompareTest, LhsAsSpanRhsAsSpanProducesAStrongOrderingEquivalentToStdString) {
  EXPECT_EQ(
      lexicographic_string_compare(span<char const>{lhs.data(), lhs.size()}, span<char const>{rhs.data(), rhs.size()}),
      expected_from_string_compare
  );
  EXPECT_EQ(
      lexicographic_string_compare(span<char const>{lhs.data(), lhs.size()}, span<char const>{rhs.data(), rhs.size()}),
      opposite_ordering(lexicographic_string_compare(
          span<char const>{rhs.data(), rhs.size()},
          span<char const>{lhs.data(), lhs.size()}
      ))
  );
}

INSTANTIATE_TEST_SUITE_P(
    NonNullPtrStrings,
    LexicographicStringCompareTest,
    ::testing::Combine(
        ::testing::Values(
            // basic ASCII cases
            std::string{""},
            std::string{"a"},
            std::string{"ab"},
            std::string{"ba"},
            std::string{"A"},
            std::string{"B"},
            std::string{"AB"},
            std::string{"BA"},
            std::string{"1"},
            std::string{"123"},
            // UTF-8 cases
            std::string{u8"名古屋市"},
            std::string{u8"東京"},
            std::string{u8"ア"}
        ),
        ::testing::Values(
            // basic ASCII cases
            std::string{""},
            std::string{"a"},
            std::string{"ab"},
            std::string{"ba"},
            std::string{"A"},
            std::string{"B"},
            std::string{"AB"},
            std::string{"BA"},
            std::string{"1"},
            std::string{"123"},
            // UTF-8 cases
            std::string{u8"名古屋市"},
            std::string{u8"東京"},
            std::string{u8"ア"}
        )
    )
);

/// @test Given two input strings `const char* lhs` and `const char* rhs`, if either is `nullptr` then invoking
/// `lexicographic_string_compare(lhs,rhs)` results in an `ARENE_PRECONDITION` violation.
TEST(LexicographicStringCompareDeathTest, NullptrAsOperandIsAPreconditionViolation) {
  char const* dummy_str = "does not matter";
  constexpr arene::base::span<char const> dummy_span;
  EXPECT_DEATH(lexicographic_string_compare(dummy_span, nullptr), "rhs != nullptr");
  EXPECT_DEATH(lexicographic_string_compare(dummy_str, nullptr), "rhs != nullptr");
  EXPECT_DEATH(lexicographic_string_compare(nullptr, dummy_str), "lhs != nullptr");
  // We have intentionally used a capture-all regex in the matcher as we don't want to get
  // tied to the order in which the input strings are checked for null. It makes us brittle
  // to any changes future authors might make to the order of null checks.
  EXPECT_DEATH(lexicographic_string_compare(nullptr, nullptr), "Precondition violation");
}

}  // namespace
