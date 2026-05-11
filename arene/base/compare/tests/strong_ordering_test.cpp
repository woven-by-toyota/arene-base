// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compare/strong_ordering.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/string.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using ::arene::base::from_strong_ordering;
using ::arene::base::is_transparent_three_way_comparator_for;
using ::arene::base::is_transparent_three_way_comparator_for_v;
using ::arene::base::make_strong_ordering;
using ::arene::base::opposite_ordering;
using ::arene::base::strong_ordering;

constexpr auto container_max_size = std::size_t{256};

/// @test `strong_ordering` values can be compared.
TEST(StrongOrdering, ValuesCanBeCompared) {
  STATIC_ASSERT_LT(strong_ordering::less, strong_ordering::equal);
  STATIC_ASSERT_LT(strong_ordering::less, strong_ordering::greater);
  STATIC_ASSERT_LT(strong_ordering::equal, strong_ordering::greater);
}

/// @test `strong_ordering` values can be compared to `0`.
TEST(StrongOrdering, ValuesCanBeComparedToZero) {
  // NOLINTBEGIN(hicpp-use-nullptr,modernize-use-nullptr) false positive, 0 literal isn't logically a pointer.
  // These cases cannot use STATIC_ASSERT_XXX macros because strong_ordering isn't actually comparable against an
  // integer, only the literal value 0 which decays to some private pointer type. The underlying gtest ASSERT_EQ and
  // friends SFINAE the comparison operator against int, and fail to find it.
  static_assert(strong_ordering::less < 0, "less < 0");
  static_assert(strong_ordering::less <= 0, "less <= 0");
  static_assert(strong_ordering::less != 0, "less != 0");
  static_assert(0 > strong_ordering::less, "less < 0");
  static_assert(0 >= strong_ordering::less, "less <= 0");
  static_assert(0 != strong_ordering::less, "less != 0");
  static_assert(strong_ordering::equal == 0, "equal == 0");
  static_assert(0 == strong_ordering::equal, "equal == 0");
  static_assert(strong_ordering::greater > 0, "greater > 0");
  static_assert(strong_ordering::greater >= 0, "greater >= 0");
  static_assert(0 < strong_ordering::greater, "greater > 0");
  static_assert(0 <= strong_ordering::greater, "greater >= 0");

  constexpr strong_ordering less = strong_ordering::less;
  constexpr strong_ordering equal = strong_ordering::equal;
  constexpr strong_ordering greater = strong_ordering::greater;

  STATIC_ASSERT_TRUE(less < 0);
  STATIC_ASSERT_TRUE(less <= 0);
  STATIC_ASSERT_FALSE(less == 0);
  STATIC_ASSERT_TRUE(less != 0);
  STATIC_ASSERT_FALSE(less > 0);
  STATIC_ASSERT_FALSE(less >= 0);
  STATIC_ASSERT_FALSE(0 < less);
  STATIC_ASSERT_FALSE(0 <= less);
  STATIC_ASSERT_FALSE(0 == less);
  STATIC_ASSERT_TRUE(0 != less);
  STATIC_ASSERT_TRUE(0 > less);
  STATIC_ASSERT_TRUE(0 >= less);

  STATIC_ASSERT_FALSE(equal < 0);
  STATIC_ASSERT_TRUE(equal <= 0);
  STATIC_ASSERT_TRUE(equal == 0);
  STATIC_ASSERT_FALSE(equal != 0);
  STATIC_ASSERT_FALSE(equal > 0);
  STATIC_ASSERT_TRUE(equal >= 0);
  STATIC_ASSERT_FALSE(0 < equal);
  STATIC_ASSERT_TRUE(0 <= equal);
  STATIC_ASSERT_TRUE(0 == equal);
  STATIC_ASSERT_FALSE(0 != equal);
  STATIC_ASSERT_FALSE(0 > equal);
  STATIC_ASSERT_TRUE(0 >= equal);

  STATIC_ASSERT_FALSE(greater < 0);
  STATIC_ASSERT_FALSE(greater <= 0);
  STATIC_ASSERT_FALSE(greater == 0);
  STATIC_ASSERT_TRUE(greater != 0);
  STATIC_ASSERT_TRUE(greater > 0);
  STATIC_ASSERT_TRUE(greater >= 0);
  STATIC_ASSERT_TRUE(0 < greater);
  STATIC_ASSERT_TRUE(0 <= greater);
  STATIC_ASSERT_FALSE(0 == greater);
  STATIC_ASSERT_TRUE(0 != greater);
  STATIC_ASSERT_FALSE(0 > greater);
  STATIC_ASSERT_FALSE(0 >= greater);
  // NOLINTEND(hicpp-use-nullptr,modernize-use-nullptr)
}

/// @test When given `greater`, `opposite_ordering` returns `less`.
TEST(OppositeOrdering, ReturnsLessForGreater) {
  STATIC_ASSERT_EQ(strong_ordering::less, opposite_ordering(strong_ordering::greater));
}

/// @test When given `less`, `opposite_ordering` returns `greater`.
TEST(OppositeOrdering, ReturnsGreaterForLess) {
  STATIC_ASSERT_EQ(strong_ordering::greater, opposite_ordering(strong_ordering::less));
}

/// @test When given `equal`, `opposite_ordering` returns `equal`.
TEST(OppositeOrdering, ReturnsEqualForEqual) {
  STATIC_ASSERT_EQ(strong_ordering::equal, opposite_ordering(strong_ordering::equal));
}

/// @test When given `less`, `from_strong_ordering` returns `-1`.
TEST(FromStrongOrdering, ReturnsNegativeOneForLess) {
  STATIC_ASSERT_EQ(-1, from_strong_ordering(strong_ordering::less));
}

/// @test When given `equal`, `from_strong_ordering` returns `0`.
TEST(FromStrongOrdering, ReturnsZeroForEqual) { STATIC_ASSERT_EQ(0, from_strong_ordering(strong_ordering::equal)); }

/// @test When given `greater`, `from_strong_ordering` returns `1`.
TEST(FromStrongOrdering, ReturnsOneForGreater) { STATIC_ASSERT_EQ(1, from_strong_ordering(strong_ordering::greater)); }

/// @test When given a value less than `0`, `make_strong_ordering` returns `less`.
TEST(MakeStrongOrdering, ReturnsLessForLessThanZero) {
  STATIC_ASSERT_EQ(strong_ordering::less, make_strong_ordering(-1));
  for (std::int8_t value = std::numeric_limits<std::int8_t>::lowest(); value < 0; ++value) {
    EXPECT_EQ(strong_ordering::less, make_strong_ordering(value));
  }
}

/// @test When given `0`, `make_strong_ordering` returns `equal`.
TEST(MakeStrongOrdering, ReturnsEqualForZero) { STATIC_ASSERT_EQ(strong_ordering::equal, make_strong_ordering(0)); }

/// @test When given a value greater than `0`, `make_strong_ordering` returns `greater`.
TEST(MakeStrongOrdering, ReturnsGreaterForGreaterThanZero) {
  STATIC_ASSERT_EQ(strong_ordering::greater, make_strong_ordering(1));
  for (std::int8_t value = std::numeric_limits<std::int8_t>::max(); value > 0; --value) {
    EXPECT_EQ(strong_ordering::greater, make_strong_ordering(value));
  }
}

template <typename IsTransparentType = void>
class user_transparent_cmp {
 public:
  using is_transparent = IsTransparentType;

  template <
      typename T,
      typename U,
      ::arene::base::constraints<
          std::enable_if_t<arene::base::is_invocable_r_v<bool, std::less<>, T const&, U const&>>> = nullptr>
  constexpr auto operator()(T const&, U const&) -> strong_ordering;
};

using transparent_comparators = ::testing::
    Types<user_transparent_cmp<void>, user_transparent_cmp<int>, user_transparent_cmp<arene::base::string_view>>;

class no_is_transparent_member {
 public:
  template <typename T, typename U>
  constexpr auto operator()(T const&, U const&) -> strong_ordering;
};

class is_transparent_member_but_not_type {
 public:
  ARENE_MAYBE_UNUSED static constexpr bool is_transparent = true;
  template <typename T, typename U>
  constexpr auto operator()(T const&, U const&) -> strong_ordering;
};

using not_transparent_comparators =
    ::testing::Types<arene::base::string_view, no_is_transparent_member, is_transparent_member_but_not_type>;

template <typename T>
class IsTransparentThreeWayComparatorForGivenTypeWithMemberTypedefIsTransparentTest : public ::testing::Test {};

TYPED_TEST_SUITE(
    IsTransparentThreeWayComparatorForGivenTypeWithMemberTypedefIsTransparentTest,
    transparent_comparators,
);

/// @test Transparent three-way comparator trait for types with `is_transparent` member typedef returns `true` if
/// `bool operator()` for the three-way comparison input types exists.
TYPED_TEST(
    IsTransparentThreeWayComparatorForGivenTypeWithMemberTypedefIsTransparentTest,
    IsTrueIfBoolReturningCallOperatorForInputTypesExists
) {
  {
    using cmp_type = int;
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type = arene::base::string_view;
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type1 = arene::base::string_view;
    using cmp_type2 = char const*;
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
    STATIC_ASSERT_TRUE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
  }
}

/// @test Transparent three-way comparator trait for types with `is_transparent` member typedef returns `false` if
/// `bool operator()` for the three-way comparison input types does not exist.
TYPED_TEST(
    IsTransparentThreeWayComparatorForGivenTypeWithMemberTypedefIsTransparentTest,
    IsFalseIfBoolReturningCallOperatorForInputTypesDoesNotExist
) {
  using cmp_type1 = arene::base::string_view;
  using cmp_type2 = arene::base::array<char, container_max_size>;
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
  STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
}

template <typename T>
class IsTransparentThreeWayComparatorForGivenTypeWithoutMemberTypedefIsTransparentTest : public ::testing::Test {};

TYPED_TEST_SUITE(
    IsTransparentThreeWayComparatorForGivenTypeWithoutMemberTypedefIsTransparentTest,
    not_transparent_comparators,
);

/// @test Transparent three-way comparator trait for types with non-typedef `is_transparent` member returns `false`
/// regardless of the three-way comparison input types.
TYPED_TEST(IsTransparentThreeWayComparatorForGivenTypeWithoutMemberTypedefIsTransparentTest, IsFalse) {
  {
    using cmp_type = int;
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type = arene::base::string_view;
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type1 = arene::base::string_view;
    using cmp_type2 = char const*;
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
  }
  {
    using cmp_type1 = arene::base::string_view;
    using cmp_type2 = arene::base::array<char, container_max_size>;
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_three_way_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
  }
}

}  // namespace
