// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_transparent_comparator_for.hpp"

#include <functional>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/compare.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::is_transparent_comparator;
using ::arene::base::is_transparent_comparator_for;
using ::arene::base::is_transparent_comparator_for_v;
using ::arene::base::is_transparent_comparator_v;

template <typename IsTransparentType = void>
class user_transparent_cmp {
 public:
  using is_transparent = IsTransparentType;

  template <
      typename T,
      typename U,
      arene::base::constraints<std::enable_if_t<arene::base::is_equality_comparable_v<T, U>>> = nullptr>
  constexpr auto operator()(T const&, U const&) -> bool;
};

using transparent_comparators = ::testing::Types<
    std::less<>,
    std::greater<>,
    std::equal_to<>,
    user_transparent_cmp<void>,
    user_transparent_cmp<int>,
    user_transparent_cmp<std::string>>;

class no_is_transparent_member {
 public:
  template <typename T, typename U>
  constexpr auto operator()(T const&, U const&) -> bool;
};

class is_transparent_member_but_not_type {
 public:
  ARENE_MAYBE_UNUSED static constexpr bool is_transparent = true;
  template <typename T, typename U>
  constexpr auto operator()(T const&, U const&) -> bool;
};

using not_transparent_comparators =
    ::testing::Types<std::string, no_is_transparent_member, is_transparent_member_but_not_type>;

template <typename T>
class IsTransparentComparatorGivenTypeWithMemberTypedefIsTransparentTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsTransparentComparatorGivenTypeWithMemberTypedefIsTransparentTest, transparent_comparators, );

/// @test `is_transparent_comparator` evaluates to `true` when given a comparator that has an `is_transparent` member
/// type.
/// @tparam TypeParam The type of the comparator
TYPED_TEST(IsTransparentComparatorGivenTypeWithMemberTypedefIsTransparentTest, IsTrue) {
  STATIC_ASSERT_TRUE(is_transparent_comparator<TypeParam>::value);
  STATIC_ASSERT_TRUE(is_transparent_comparator_v<TypeParam>);
}

template <typename T>
class IsTransparentComparatorGivenTypeWithoutMemberTypedefIsTransparentTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsTransparentComparatorGivenTypeWithoutMemberTypedefIsTransparentTest, not_transparent_comparators, );

/// @test `is_transparent_comparator` evaluates to `false` when given a comparator that does not have an
/// `is_transparent` member type.
/// @tparam TypeParam The type of the comparator
TYPED_TEST(IsTransparentComparatorGivenTypeWithoutMemberTypedefIsTransparentTest, IsFalse) {
  STATIC_ASSERT_FALSE(is_transparent_comparator<TypeParam>::value);
  STATIC_ASSERT_FALSE(is_transparent_comparator_v<TypeParam>);
}

template <typename T>
class IsTransparentComparatorForGivenTypeWithMemberTypedefIsTransparentTest : public ::testing::Test {};

TYPED_TEST_SUITE(IsTransparentComparatorForGivenTypeWithMemberTypedefIsTransparentTest, transparent_comparators, );

/// @test `is_transparent_comparator_for` evaluates to `true` when given a comparator that has an `is_transparent`
/// member type, and the function call operator returns `bool` when invoked with the specified argument types.
/// @tparam TypeParam The type of the comparator
TYPED_TEST(
    IsTransparentComparatorForGivenTypeWithMemberTypedefIsTransparentTest,
    IsTrueIfBoolReturningCallOperatorForInputTypesExists
) {
  {
    using cmp_type = int;
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type = std::string;
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type1 = std::string;
    using cmp_type2 = char const*;
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
    STATIC_ASSERT_TRUE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
  }
}

/// @test `is_transparent_comparator_for` evaluates to `false` when given a comparator that has an `is_transparent`
/// member type, and there is not an overload of the function call operator that returns `bool` when invoked with the
/// specified argument types.
/// @tparam TypeParam The type of the comparator
TYPED_TEST(
    IsTransparentComparatorForGivenTypeWithMemberTypedefIsTransparentTest,
    IsFalseIfBoolReturningCallOperatorForInputTypesDoesNotExist
) {
  using cmp_type1 = std::string;
  using cmp_type2 = std::vector<char>;
  STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
  STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
  STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
  STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
  STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
  STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
  STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
  STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
}

template <typename T>
class IsTransparentComparatorForGivenTypeWithoutMemberTypedefIsTransparentTest : public ::testing::Test {};

TYPED_TEST_SUITE(
    IsTransparentComparatorForGivenTypeWithoutMemberTypedefIsTransparentTest,
    not_transparent_comparators,
);

/// @test `is_transparent_comparator_for` evaluates to `false` when given a comparator that does not have an
/// `is_transparent` member type.
/// @tparam TypeParam The type of the comparator
TYPED_TEST(IsTransparentComparatorForGivenTypeWithoutMemberTypedefIsTransparentTest, IsFalse) {
  {
    using cmp_type = int;
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type = std::string;
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type, cmp_type>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type, cmp_type>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type&, cmp_type const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type const&, cmp_type&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type const&, cmp_type&>);
  }

  {
    using cmp_type1 = std::string;
    using cmp_type2 = char const*;
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
  }
  {
    using cmp_type1 = std::string;
    using cmp_type2 = std::vector<char>;
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1, cmp_type2>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1, cmp_type2>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2 const&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1&, cmp_type2 const&>);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for<TypeParam, cmp_type1 const&, cmp_type2&>::value);
    STATIC_ASSERT_FALSE(is_transparent_comparator_for_v<TypeParam, cmp_type1 const&, cmp_type2&>);
  }
}

}  // namespace
