// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/conjunction.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::conjunction;
using ::arene::base::conjunction_v;

template <bool B>
class trait_with_udt_convertible_to_bool {
  struct convertible_to_bool {
    // NOLINTNEXTLINE(hicpp-explicit-conversions) need implicit conversion to test interface.
    constexpr operator bool() const { return B; }
  };

 public:
  static constexpr bool value = convertible_to_bool{};
};

template <bool B>
constexpr bool trait_with_udt_convertible_to_bool<B>::value;

using bool_like_true_trait = std::integral_constant<int, 4>;
using bool_like_false_trait = std::integral_constant<int, 0>;

/// @test The value of `conjunction<>::value` is true
TEST(Conjunction, NoParametersIsTrue) { STATIC_ASSERT_TRUE(conjunction<>::value); }

/// @test Given a boolean trait `BoolTrait`, then the value of `conjunction<BoolTrait>::value` is that parameter's value
TEST(Conjunction, SingleParameterIsValue) {
  STATIC_ASSERT_TRUE(conjunction<std::true_type>::value);
  STATIC_ASSERT_TRUE(conjunction<bool_like_true_trait>::value);
  STATIC_ASSERT_TRUE(conjunction<trait_with_udt_convertible_to_bool<true>>::value);
  STATIC_ASSERT_FALSE(conjunction<std::false_type>::value);
  STATIC_ASSERT_FALSE(conjunction<bool_like_false_trait>::value);
  STATIC_ASSERT_FALSE(conjunction<trait_with_udt_convertible_to_bool<false>>::value);
}

/// @test Given a set of boolean traits `BoolTraits...`, then the value of `conjunction<BoolTraits...>::value` is the
/// logical AND of those parameters' values
TEST(Conjunction, MultipleParametersIsLogicalAnd) {
  STATIC_ASSERT_TRUE((conjunction<std::true_type, bool_like_true_trait, trait_with_udt_convertible_to_bool<true>>::value
  ));
  STATIC_ASSERT_FALSE((conjunction<std::true_type, std::true_type, std::false_type>::value));
  STATIC_ASSERT_FALSE((conjunction<std::true_type, bool_like_false_trait, std::true_type>::value));
  STATIC_ASSERT_FALSE((conjunction<trait_with_udt_convertible_to_bool<false>, std::true_type, std::true_type>::value));
  STATIC_ASSERT_FALSE((conjunction<std::false_type, std::false_type, std::true_type>::value));
  STATIC_ASSERT_FALSE((conjunction<std::false_type, std::true_type, std::false_type>::value));
  STATIC_ASSERT_FALSE((conjunction<std::true_type, std::false_type, std::false_type>::value));
  STATIC_ASSERT_FALSE((conjunction<std::false_type, std::false_type, std::false_type>::value));
}

/// @test Given a set of boolean traits `BoolTraits...`, then the variable template `conjunction_v<BoolTraits...>` is
/// equivalent to `conjunction<BoolTraits>::value`
TEST(ConjunctionV, IsEquivalentToValueOfConjunction) {
  STATIC_ASSERT_EQ(conjunction<>::value, conjunction_v<>);
  STATIC_ASSERT_EQ(conjunction<std::true_type>::value, conjunction_v<std::true_type>);
  STATIC_ASSERT_EQ(conjunction<std::false_type>::value, conjunction_v<std::false_type>);
  STATIC_ASSERT_EQ(
      (conjunction<std::true_type, std::true_type, std::true_type>::value),
      (conjunction_v<std::true_type, std::true_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<std::true_type, std::true_type, std::false_type>::value),
      (conjunction_v<std::true_type, std::true_type, std::false_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<std::true_type, std::false_type, std::true_type>::value),
      (conjunction_v<std::true_type, std::false_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<std::false_type, std::true_type, std::true_type>::value),
      (conjunction_v<std::false_type, std::true_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<std::false_type, std::false_type, std::true_type>::value),
      (conjunction_v<std::false_type, std::false_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<std::false_type, std::true_type, std::false_type>::value),
      (conjunction_v<std::false_type, std::true_type, std::false_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<std::true_type, std::false_type, std::false_type>::value),
      (conjunction_v<std::true_type, std::false_type, std::false_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<std::false_type, std::false_type, std::false_type>::value),
      (conjunction_v<std::false_type, std::false_type, std::false_type>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<bool_like_true_trait, trait_with_udt_convertible_to_bool<true>>::value),
      (conjunction_v<bool_like_true_trait, trait_with_udt_convertible_to_bool<true>>)
  );
  STATIC_ASSERT_EQ(
      (conjunction<bool_like_true_trait, trait_with_udt_convertible_to_bool<false>>::value),
      (conjunction_v<bool_like_true_trait, trait_with_udt_convertible_to_bool<false>>)
  );
}

template <typename T>
struct invalid_if_int_otherwise_false : std::false_type {
  static_assert(!std::is_same<T, int>::value, "T must not be int");
};

/// @test Given a series of boolean traits `BoolTraits...`, when `conjunction<BoolTraits...>` is invoked, and when
/// `BoolTrait[N]::value` is `false`, then the remaining `BoolTraits::value...` are not evaluated.
TEST(Conjunction, ShortCircuitsEvaluation) {
  STATIC_ASSERT_FALSE((conjunction<
                       invalid_if_int_otherwise_false<bool>,
                       invalid_if_int_otherwise_false<int>,  // This would cause a static assertion failure if evaluated
                       invalid_if_int_otherwise_false<float>>::value));
  STATIC_ASSERT_FALSE((conjunction_v<
                       invalid_if_int_otherwise_false<bool>,
                       invalid_if_int_otherwise_false<int>,  // This would cause a static assertion failure if evaluated
                       invalid_if_int_otherwise_false<float>>));
}

/// @test Given a series of boolean traits `BoolTraits...`, when `conjunction<BoolTraits...>` is invoked with all truthy
/// values, then the resulting type is derived from the last trait in the list.
TEST(Conjunction, ResultTypeIsLastTraitIfAllTruthy) {
  STATIC_ASSERT_TRUE((std::is_convertible<
                      conjunction<std::true_type, bool_like_true_trait, trait_with_udt_convertible_to_bool<true>>*,
                      trait_with_udt_convertible_to_bool<true>*>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<
                      conjunction<bool_like_true_trait, trait_with_udt_convertible_to_bool<true>, std::true_type>*,
                      std::true_type*>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<
                      conjunction<trait_with_udt_convertible_to_bool<true>, std::true_type, bool_like_true_trait>*,
                      bool_like_true_trait*>::value));
}

/// @test Given a series of boolean traits `BoolTraits...`, when `conjunction<BoolTraits...>` is invoked with any falsy
/// values, then the resulting type is derived from the first falsy trait in the list.
TEST(Conjunction, ResultTypeIsFirstFalsyTraitIfAnyFalsy) {
  STATIC_ASSERT_TRUE(
      (std::is_base_of<
          trait_with_udt_convertible_to_bool<false>,
          conjunction<std::true_type, bool_like_true_trait, trait_with_udt_convertible_to_bool<false>>>::value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_base_of<
          bool_like_false_trait,
          conjunction<std::true_type, bool_like_false_trait, trait_with_udt_convertible_to_bool<false>>>::value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_base_of<std::false_type, conjunction<std::false_type, std::true_type, bool_like_true_trait>>::value)
  );
}

}  // namespace
