// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/disjunction.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::disjunction;
using ::arene::base::disjunction_v;

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

/// @test The value of `disjunction<>::value` is false
TEST(Disjunction, NoParametersIsFalse) { STATIC_ASSERT_FALSE(disjunction<>::value); }

/// @test Given a boolean trait `BoolTrait`, then the value of `disjunction<BoolTrait>::value` is that parameter's value
TEST(Disjunction, SingleParameterIsValue) {
  STATIC_ASSERT_TRUE(disjunction<std::true_type>::value);
  STATIC_ASSERT_FALSE(disjunction<std::false_type>::value);
}

/// @test Given a set of boolean traits `BoolTraits...`, then the value of `disjunction<BoolTraits...>::value` is the
/// logical OR of those parameters' values
TEST(Disjunction, MultipleParametersIsLogicalOr) {
  STATIC_ASSERT_TRUE((disjunction<std::true_type, std::true_type, std::true_type>::value));
  STATIC_ASSERT_TRUE((disjunction<std::true_type, std::true_type, std::false_type>::value));
  STATIC_ASSERT_TRUE((disjunction<std::true_type, std::false_type, std::true_type>::value));
  STATIC_ASSERT_TRUE((disjunction<std::false_type, std::true_type, std::true_type>::value));
  STATIC_ASSERT_TRUE((disjunction<std::false_type, std::false_type, std::true_type>::value));
  STATIC_ASSERT_TRUE((disjunction<std::false_type, std::true_type, std::false_type>::value));
  STATIC_ASSERT_TRUE((disjunction<std::true_type, std::false_type, std::false_type>::value));
  STATIC_ASSERT_FALSE((disjunction<std::false_type, std::false_type, std::false_type>::value));
}

/// @test Given a set of boolean traits `BoolTraits...`, then the variable template `disjunction_v<BoolTraits...>` is
/// equivalent to `disjunction<BoolTraits>::value`
TEST(DisjunctionV, IsEquivalentToValueOfDisjunction) {
  STATIC_ASSERT_EQ(disjunction<>::value, disjunction_v<>);
  STATIC_ASSERT_EQ(disjunction<std::true_type>::value, disjunction_v<std::true_type>);
  STATIC_ASSERT_EQ(disjunction<std::false_type>::value, disjunction_v<std::false_type>);
  STATIC_ASSERT_EQ(
      (disjunction<std::true_type, std::true_type, std::true_type>::value),
      (disjunction_v<std::true_type, std::true_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (disjunction<std::true_type, std::true_type, std::false_type>::value),
      (disjunction_v<std::true_type, std::true_type, std::false_type>)
  );
  STATIC_ASSERT_EQ(
      (disjunction<std::true_type, std::false_type, std::true_type>::value),
      (disjunction_v<std::true_type, std::false_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (disjunction<std::false_type, std::true_type, std::true_type>::value),
      (disjunction_v<std::false_type, std::true_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (disjunction<std::false_type, std::false_type, std::true_type>::value),
      (disjunction_v<std::false_type, std::false_type, std::true_type>)
  );
  STATIC_ASSERT_EQ(
      (disjunction<std::false_type, std::true_type, std::false_type>::value),
      (disjunction_v<std::false_type, std::true_type, std::false_type>)
  );
  STATIC_ASSERT_EQ(
      (disjunction<std::true_type, std::false_type, std::false_type>::value),
      (disjunction_v<std::true_type, std::false_type, std::false_type>)
  );
  STATIC_ASSERT_EQ(
      (disjunction<std::false_type, std::false_type, std::false_type>::value),
      (disjunction_v<std::false_type, std::false_type, std::false_type>)
  );
}

template <typename T>
struct invalid_if_int_otherwise_true : std::true_type {
  static_assert(!std::is_same<T, int>::value, "T must not be int");
};

/// @test Given a series of boolean traits `BoolTraits...`, when `disjunction<BoolTraits...>` is invoked, and when
/// `BoolTrait[N]::value` is `true`, then the remaining `BoolTraits::value...` are not evaluated.
TEST(Disjunction, ShortCircuitsEvaluation) {
  STATIC_ASSERT_TRUE((disjunction<
                      invalid_if_int_otherwise_true<bool>,
                      invalid_if_int_otherwise_true<int>,  // This would cause a static assertion failure if evaluated
                      invalid_if_int_otherwise_true<float>>::value));
  STATIC_ASSERT_TRUE((disjunction_v<
                      invalid_if_int_otherwise_true<bool>,
                      invalid_if_int_otherwise_true<int>,  // This would cause a static assertion failure if evaluated
                      invalid_if_int_otherwise_true<float>>));
}

/// @test Given a series of boolean traits `BoolTraits...`, when `disjunction<BoolTraits...>` is invoked with all falsy
/// values, then the resulting type is derived from the last trait in the list.
TEST(Disjunction, ResultTypeIsLastTraitIfAllFalsy) {
  STATIC_ASSERT_TRUE((std::is_convertible<
                      disjunction<std::false_type, bool_like_false_trait, trait_with_udt_convertible_to_bool<false>>*,
                      trait_with_udt_convertible_to_bool<false>*>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<
                      disjunction<bool_like_false_trait, trait_with_udt_convertible_to_bool<false>, std::false_type>*,
                      std::false_type*>::value));
  STATIC_ASSERT_TRUE((std::is_convertible<
                      disjunction<trait_with_udt_convertible_to_bool<false>, std::false_type, bool_like_false_trait>*,
                      bool_like_false_trait*>::value));
}

/// @test Given a series of boolean traits `BoolTraits...`, when `disjunction<BoolTraits...>` is invoked with any truthy
/// values, then the resulting type is derived from the first truthy trait in the list.
TEST(Disjunction, ResultTypeIsFirstTruthyTraitIfAnyTruthy) {
  STATIC_ASSERT_TRUE(
      (std::is_base_of<
          trait_with_udt_convertible_to_bool<true>,
          disjunction<std::false_type, bool_like_false_trait, trait_with_udt_convertible_to_bool<true>>>::value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_base_of<
          std::true_type,
          disjunction<bool_like_false_trait, std::true_type, trait_with_udt_convertible_to_bool<false>>>::value)
  );
  STATIC_ASSERT_TRUE(
      (std::is_base_of<
          bool_like_true_trait,
          disjunction<bool_like_true_trait, trait_with_udt_convertible_to_bool<false>, std::false_type>>::value)
  );
}

}  // namespace
