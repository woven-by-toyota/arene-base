// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compare/compare_three_way.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"

namespace {

using ::arene::base::compare_three_way;
using ::arene::base::is_transparent_three_way_comparator_for_v;
using ::arene::base::strong_ordering;

/// @test `compare_three_way` can be used with integers and returns the expected `strong_ordering` when called on
/// integers.
TEST(CompareThreeWay, ComparisonOnIntegersIsCorrect) {
  STATIC_ASSERT_EQ(compare_three_way{}(1, 1), strong_ordering::equal);
  STATIC_ASSERT_EQ(compare_three_way{}(1, 2), strong_ordering::less);
  STATIC_ASSERT_EQ(compare_three_way{}(2, 1), strong_ordering::greater);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<int>);
}

/// @test `compare_three_way` can be used with strings and returns the expected `strong_ordering` when called on
/// strings.
TEST(CompareThreeWay, CanCompareStrings) {
  arene::base::string_view const string1 = "hello";
  arene::base::string_view const string2 = "world";
  ASSERT_EQ(compare_three_way{}(string1, string1), strong_ordering::equal);
  ASSERT_EQ(compare_three_way{}(string1, string2), strong_ordering::less);
  ASSERT_EQ(compare_three_way{}(string2, string1), strong_ordering::greater);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<arene::base::string_view>);
}

/// @test `compare_three_way` can be used with pointers and returns the expected `strong_ordering` when called on
/// pointers. Pointers to `arene::base::string_view` arranged in an `arene::base::array` are used for that purpose.
TEST(CompareThreeWay, CanComparePointers) {
  arene::base::array<arene::base::string_view, 2> const data{{"hello", "world"}};
  ASSERT_EQ(compare_three_way{}(data.data(), data.data()), strong_ordering::equal);
  ASSERT_EQ(compare_three_way{}(data.data(), &data[1]), strong_ordering::less);
  ASSERT_EQ(compare_three_way{}(&data[1], data.data()), strong_ordering::greater);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<arene::base::string_view*>);
}

/// @test `compare_three_way` returns the expected `strong_ordering` when called on pointers without ordering
/// relationship. Pointers to `arene::base::string_view` on the stack are used for that purpose and the actual ordering
/// is verified with `std::less`.
TEST(CompareThreeWay, CanCompareUnrelatedPointers) {
  arene::base::string_view const string1 = "hello";
  arene::base::string_view const string2 = "world";
  bool const one_before_two = std::less<>{}(&string1, &string2);
  ASSERT_EQ(compare_three_way{}(&string1, &string1), strong_ordering::equal);
  ASSERT_EQ(compare_three_way{}(&string1, &string2), one_before_two ? strong_ordering::less : strong_ordering::greater);
  ASSERT_EQ(compare_three_way{}(&string2, &string1), one_before_two ? strong_ordering::greater : strong_ordering::less);
}

struct comp_tracker {
  unsigned val;

  // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly counting interaction across instances
  static unsigned lt_count;
  static unsigned eq_count;
  // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

  static void reset() {
    lt_count = 0;
    eq_count = 0;
  }

  friend auto operator<(comp_tracker const& lhs, comp_tracker const& rhs) noexcept -> bool {
    ++lt_count;
    return lhs.val < rhs.val;
  }
  friend auto operator==(comp_tracker const& lhs, comp_tracker const& rhs) noexcept -> bool {
    ++eq_count;
    return lhs.val == rhs.val;
  }
};
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
unsigned comp_tracker::lt_count = 0;
unsigned comp_tracker::eq_count = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/// @test Make sure `operator==` and `operator<` are used when the compared types support them. A `comp_tracker` type
/// supporting the operations and counting the calls ensures they are called an expected number of times for each
/// `strong_ordering`.
TEST(CompareThreeWay, UseEqualityIfAvailable) {
  comp_tracker::reset();
  ASSERT_EQ(compare_three_way{}(comp_tracker{1}, comp_tracker{1}), strong_ordering::equal);
  ASSERT_EQ(comp_tracker::eq_count, 1);
  ASSERT_EQ(comp_tracker::lt_count, 0);

  comp_tracker::reset();
  ASSERT_EQ(compare_three_way{}(comp_tracker{1}, comp_tracker{2}), strong_ordering::less);
  ASSERT_EQ(comp_tracker::eq_count, 1);
  ASSERT_EQ(comp_tracker::lt_count, 1);

  comp_tracker::reset();
  ASSERT_EQ(compare_three_way{}(comp_tracker{2}, comp_tracker{1}), strong_ordering::greater);
  ASSERT_EQ(comp_tracker::eq_count, 1);
  ASSERT_EQ(comp_tracker::lt_count, 1);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<comp_tracker>);
}

struct less_than_tracker {
  unsigned val;

  // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly counting interaction across instances
  static unsigned lt_count;
  // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

  static void reset() { lt_count = 0; }

  friend auto operator<(less_than_tracker const& lhs, less_than_tracker const& rhs) noexcept -> bool {
    ++lt_count;
    return lhs.val < rhs.val;
  }
};
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
unsigned less_than_tracker::lt_count = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/// @test Make sure `operator<` is used when the compared types support it but no other comparison operations. A
/// `less_than_tracker` type supporting the operation and counting the calls ensures it is called an expected number of
/// times for each `strong_ordering`.
TEST(CompareThreeWay, UseLessThanIfNoEquality) {
  less_than_tracker::reset();
  ASSERT_EQ(compare_three_way{}(less_than_tracker{1}, less_than_tracker{1}), strong_ordering::equal);
  ASSERT_EQ(less_than_tracker::lt_count, 2);

  less_than_tracker::reset();
  ASSERT_EQ(compare_three_way{}(less_than_tracker{1}, less_than_tracker{2}), strong_ordering::less);
  ASSERT_EQ(less_than_tracker::lt_count, 1);

  less_than_tracker::reset();
  ASSERT_EQ(compare_three_way{}(less_than_tracker{2}, less_than_tracker{1}), strong_ordering::greater);
  ASSERT_EQ(less_than_tracker::lt_count, 2);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<less_than_tracker>);
}

/// @test Make sure `compare_three_way` can be used in a `constexpr` context provided the compared type supports
/// `operator<`. A `constexpr_less_than_comparable` type with a `constexpr` `operator<` is defined for that purpose.
TEST(CompareThreeWay, CanCompareInConstexprWithJustLessThan) {
  struct constexpr_less_than_comparable {
    unsigned val;

    constexpr auto operator<(constexpr_less_than_comparable const& other) const noexcept -> bool {
      return val < other.val;
    }
  };

  STATIC_ASSERT_EQ(
      compare_three_way{}(constexpr_less_than_comparable{5}, constexpr_less_than_comparable{1}),
      strong_ordering::greater
  );
}

struct three_way_comparable {
  unsigned val;

  static constexpr auto three_way_compare(three_way_comparable const& lhs, three_way_comparable const& rhs) noexcept
      -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.val, rhs.val);
  }
};

/// @test Make sure `compare_three_way` uses the `static` `three_way_compare` member function of the compared types when
/// it is available. Verify the expected `strong_ordering` is returned.
TEST(CompareThreeWay, UseThreeWayCompareStaticMemberIfPossible) {
  STATIC_ASSERT_EQ(compare_three_way{}(three_way_comparable{1}, three_way_comparable{1}), strong_ordering::equal);
  STATIC_ASSERT_EQ(compare_three_way{}(three_way_comparable{1}, three_way_comparable{2}), strong_ordering::less);
  STATIC_ASSERT_EQ(compare_three_way{}(three_way_comparable{2}, three_way_comparable{1}), strong_ordering::greater);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<three_way_comparable>);
}

struct three_way_comparable_with_less_than {
  unsigned val;

  static constexpr auto three_way_compare(
      three_way_comparable_with_less_than const& lhs,
      three_way_comparable_with_less_than const& rhs
  ) noexcept -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.val, rhs.val);
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wunused-function", "testing that this isn't called");
  friend auto operator<(three_way_comparable_with_less_than const&, three_way_comparable_with_less_than const&)
      -> bool {
    ARENE_INVARIANT_UNREACHABLE("Bad compare");
  }
  ARENE_IGNORE_END();
};

/// @test Make sure `compare_three_way` uses the `static` `three_way_compare` member function of the compared types over
/// the `operator<` when both are available. A `three_way_comparable_with_less_than` type is defined with a working
/// `three_way_compare` implementation and a throwing `operator<` implementation for that purpose. Verify the expected
/// `strong_ordering` is returned.
TEST(CompareThreeWay, UseThreeWayCompareStaticMemberInPreferenceToLessThan) {
  ASSERT_EQ(
      compare_three_way{}(three_way_comparable_with_less_than{1}, three_way_comparable_with_less_than{1}),
      strong_ordering::equal
  );
  ASSERT_EQ(
      compare_three_way{}(three_way_comparable_with_less_than{1}, three_way_comparable_with_less_than{2}),
      strong_ordering::less
  );
  ASSERT_EQ(
      compare_three_way{}(three_way_comparable_with_less_than{2}, three_way_comparable_with_less_than{1}),
      strong_ordering::greater
  );
}

struct three_way_comparable_with_less_than_and_equals {
  unsigned val;

  static auto three_way_compare(
      three_way_comparable_with_less_than_and_equals const& lhs,
      three_way_comparable_with_less_than_and_equals const& rhs
  ) noexcept -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.val, rhs.val);
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wunused-function", "testing that this isn't called");
  friend auto
  operator<(three_way_comparable_with_less_than_and_equals const&, three_way_comparable_with_less_than_and_equals const&)
      -> bool {
    ARENE_INVARIANT_UNREACHABLE("Bad compare");
  }
  friend auto
  operator==(three_way_comparable_with_less_than_and_equals const&, three_way_comparable_with_less_than_and_equals const&)
      -> bool {
    ARENE_INVARIANT_UNREACHABLE("Bad compare");
  }
  ARENE_IGNORE_END();
};

/// @test Make sure `compare_three_way` uses the `static` `three_way_compare` member function of the compared types over
/// the `operator<` and the `operator==` when all are available. A `three_way_comparable_with_less_than_and_equals` type
/// is defined with a working `three_way_compare` implementation and throwing implementations of the other operators for
/// that purpose. Verify the expected `strong_ordering` is returned.
TEST(CompareThreeWay, UseThreeWayCompareStaticMemberInPreferenceToLessThanAndEquals) {
  ASSERT_EQ(
      compare_three_way{
      }(three_way_comparable_with_less_than_and_equals{1}, three_way_comparable_with_less_than_and_equals{1}),
      strong_ordering::equal
  );
  ASSERT_EQ(
      compare_three_way{
      }(three_way_comparable_with_less_than_and_equals{1}, three_way_comparable_with_less_than_and_equals{2}),
      strong_ordering::less
  );
  ASSERT_EQ(
      compare_three_way{
      }(three_way_comparable_with_less_than_and_equals{2}, three_way_comparable_with_less_than_and_equals{1}),
      strong_ordering::greater
  );
}

/// @test Make sure the `compare_three_way` `operator()` is `noexcept` when the operations it uses for comparison are
/// also `noexcept`. All combinations are tested for failing and successful cases.
TEST(CompareThreeWay, NoexceptIfUnderlyingComparisonsAre) {
  STATIC_ASSERT_TRUE(noexcept(compare_three_way{}(1, 2)));

  struct throwing_less_than {
    auto operator<(throwing_less_than) const -> bool;
  };

  STATIC_ASSERT_FALSE(noexcept(compare_three_way{}(throwing_less_than{}, throwing_less_than{})));

  struct noexcept_less_than {
    auto operator<(noexcept_less_than) const noexcept -> bool;
  };

  STATIC_ASSERT_TRUE(noexcept(compare_three_way{}(noexcept_less_than{}, noexcept_less_than{})));

  struct noexcept_less_than_and_equality {
    auto operator<(noexcept_less_than_and_equality) const noexcept -> bool;
    auto operator==(noexcept_less_than_and_equality) const noexcept -> bool;
  };

  STATIC_ASSERT_TRUE(noexcept(compare_three_way{}(noexcept_less_than_and_equality{}, noexcept_less_than_and_equality{}))
  );

  struct noexcept_less_than_throwing_equality {
    auto operator<(noexcept_less_than_throwing_equality) const noexcept -> bool;
    auto operator==(noexcept_less_than_throwing_equality) const -> bool;
  };

  STATIC_ASSERT_FALSE(noexcept(compare_three_way{
  }(noexcept_less_than_throwing_equality{}, noexcept_less_than_throwing_equality{})));

  struct throwing_less_than_noexcept_equality {
    auto operator<(throwing_less_than_noexcept_equality) const -> bool;
    auto operator==(throwing_less_than_noexcept_equality) const noexcept -> bool;
  };

  STATIC_ASSERT_FALSE(noexcept(compare_three_way{
  }(throwing_less_than_noexcept_equality{}, throwing_less_than_noexcept_equality{})));

  struct noexcept_three_way {
    static auto three_way_compare(noexcept_three_way, noexcept_three_way) noexcept -> strong_ordering;
  };

  STATIC_ASSERT_TRUE(noexcept(noexcept_three_way::three_way_compare(noexcept_three_way{}, noexcept_three_way{})));

  struct throwing_three_way {
    static auto three_way_compare(throwing_three_way, throwing_three_way) -> strong_ordering;
  };

  STATIC_ASSERT_FALSE(noexcept(compare_three_way{}(throwing_three_way{}, throwing_three_way{})));
}

struct mixed_comparison_a {};
struct mixed_comparison_b {
  friend constexpr auto operator<(mixed_comparison_a, mixed_comparison_b) -> bool { return true; }
  friend constexpr auto operator<(mixed_comparison_b, mixed_comparison_a) -> bool { return false; }
};

struct mixed_comparison_three_way {
  static constexpr auto three_way_compare(mixed_comparison_three_way, mixed_comparison_a) -> strong_ordering {
    return strong_ordering::less;
  }
};

/// @test `compare_three_way` can be used with mixed types, one with no comparison method, the other either defining
/// `operator<` or a `static` `three_way_compare` method, and returns the expected `strong_ordering`.
TEST(CompareThreeWay, MixedComparisons) {
  ASSERT_EQ(compare_three_way{}(arene::base::string_view("hello"), "hello"), strong_ordering::equal);

  STATIC_ASSERT_EQ(compare_three_way{}(mixed_comparison_a{}, mixed_comparison_b{}), strong_ordering::less);
  STATIC_ASSERT_EQ(compare_three_way{}(mixed_comparison_b{}, mixed_comparison_a{}), strong_ordering::greater);
  STATIC_ASSERT_EQ(compare_three_way{}(mixed_comparison_three_way{}, mixed_comparison_a{}), strong_ordering::less);
  STATIC_ASSERT_EQ(compare_three_way{}(mixed_comparison_a{}, mixed_comparison_three_way{}), strong_ordering::greater);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<mixed_comparison_a, mixed_comparison_b>);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<mixed_comparison_b, mixed_comparison_a>);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<mixed_comparison_three_way, mixed_comparison_a>);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<mixed_comparison_a, mixed_comparison_three_way>);
}

/// @test `compare_three_way` is a _transparent comparator_.
TEST(CompareThreeWay, IsATransparentComparator) {
  STATIC_ASSERT_TRUE(arene::base::is_transparent_comparator_v<compare_three_way>);
}

/// @test `compare_three_way` is a _transparent comparator_ for integer and mixed types.
TEST(CompareThreeWay, IsATransparentThreeWayComparatorFor) {
  STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<compare_three_way, int, int>);
  STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<
                     compare_three_way,
                     mixed_comparison_a,
                     mixed_comparison_b>);
}

}  // namespace
