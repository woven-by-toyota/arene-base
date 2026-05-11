// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/variant/traits.hpp"

#include <gtest/gtest.h>

#include "arene/base/monostate/monostate.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_list/size.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {

template <typename TL>
using as_variant_t = ::arene::base::type_lists::apply_all_t<TL, arene::base::variant>;

using alternative_sets = ::arene::base::type_list<
    ::arene::base::type_list<std::int32_t>,
    ::arene::base::type_list<std::int32_t, float>,
    ::arene::base::type_list<std::int32_t, float, double>,
    ::arene::base::type_list<arene::base::monostate, std::int32_t>,
    ::arene::base::type_list<std::int32_t, std::int32_t const>,
    ::arene::base::type_list<std::int32_t const, float const>,
    ::arene::base::type_list<arene::base::variant<arene::base::monostate>>>;

/// @test `arene::base::variant_npos` is equal to `std::numeric_limits<std::size_t>::max()`.
TEST(VariantNpos, IsMaxOfSizeT) {
  STATIC_ASSERT_EQ(arene::base::variant_npos, std::numeric_limits<std::size_t>::max());
}

template <typename TL>
class VariantSizeTest : public ::testing::Test {};

TYPED_TEST_SUITE(VariantSizeTest, alternative_sets, );

/// @test Given `T` which is not an instantiation of `arene::base::variant`, then
/// `arene::base::variant_size_v<T> == arene::base::variant_npos`.
TYPED_TEST(VariantSizeTest, VariantSizeVOfNonVariantIsVariantNpos) {
  STATIC_ASSERT_EQ(arene::base::variant_size_v<TypeParam>, arene::base::variant_npos);
  STATIC_ASSERT_EQ(arene::base::variant_size_v<TypeParam const>, arene::base::variant_npos);
}

/// @test Given `arene::base::variant<Ts...>`, then `arene::base::variant_size_v<variant<Ts...>> == sizeof...(Ts)`.
TYPED_TEST(VariantSizeTest, VariantSizeVIsNumberOfAlternativesInTypeList) {
  using variant_t = as_variant_t<TypeParam>;
  STATIC_ASSERT_EQ(arene::base::variant_size_v<variant_t>, arene::base::type_lists::size_v<TypeParam>);
  STATIC_ASSERT_EQ(arene::base::variant_size_v<variant_t const>, arene::base::type_lists::size_v<TypeParam>);
}

/// @test Given `arene::base::variant<Ts...>`, then
/// `arene::base::variant_size<variant<Ts...>>::value == arene::base::variant_size_v<variant<Ts...>>`.
TYPED_TEST(VariantSizeTest, VariantSizeValueIsVariantSizeV) {
  STATIC_ASSERT_EQ(arene::base::variant_size<TypeParam>::value, arene::base::variant_size_v<TypeParam>);
  STATIC_ASSERT_EQ(arene::base::variant_size<TypeParam const>::value, arene::base::variant_size_v<TypeParam const>);

  using variant_t = as_variant_t<TypeParam>;
  STATIC_ASSERT_EQ(arene::base::variant_size<variant_t>::value, arene::base::variant_size_v<variant_t>);
  STATIC_ASSERT_EQ(arene::base::variant_size<variant_t const>::value, arene::base::variant_size_v<variant_t const>);
}

template <typename TL>
class VariantAlternativeTest : public ::testing::Test {};

TYPED_TEST_SUITE(VariantAlternativeTest, alternative_sets, );

/// @test Given `arene::base::variant<Ts...>`, then `arene::base::variant_alternative_t<I, variant<Ts...>>` is the I-th
/// type in `Ts...` with matching const qualification to the variant.
TYPED_TEST(
    VariantAlternativeTest,
    VariantAlternativeTIsTheIthTypeInTheVariantWithConstQualificationMatchingTheVariant
) {
  using variant_t = as_variant_t<TypeParam>;
  ::testing::StaticAssertTypeEq<
      arene::base::variant_alternative_t<0, variant_t>,
      typename ::arene::base::type_lists::at_t<TypeParam, 0>>();
  ::testing::StaticAssertTypeEq<
      arene::base::variant_alternative_t<0, variant_t const>,
      typename ::arene::base::type_lists::at_t<TypeParam, 0> const>();

  constexpr auto variant_size = arene::base::variant_size_v<variant_t>;

  ::testing::StaticAssertTypeEq<
      arene::base::variant_alternative_t<variant_size - 1, variant_t>,
      typename ::arene::base::type_lists::at_t<TypeParam, variant_size - 1>>();
  ::testing::StaticAssertTypeEq<
      arene::base::variant_alternative_t<variant_size - 1, variant_t const>,
      typename ::arene::base::type_lists::at_t<TypeParam, variant_size - 1> const>();
}

/// @test Given `arene::base::variant<Ts...>`, then `arene::base::variant_alternative<I, variant<Ts...>>::type` is the
/// same as `arene::base::variant_alternative_t<I, variant<Ts...>>`.
TYPED_TEST(VariantAlternativeTest, VariantAlternativeTypeIsTheSameAsVariantAlternativeT) {
  using variant_t = as_variant_t<TypeParam>;
  ::testing::StaticAssertTypeEq<
      typename arene::base::variant_alternative<0, variant_t>::type,
      arene::base::variant_alternative_t<0, variant_t>>();
  ::testing::StaticAssertTypeEq<
      typename arene::base::variant_alternative<0, variant_t const>::type,
      arene::base::variant_alternative_t<0, variant_t const>>();

  constexpr auto variant_size = arene::base::variant_size_v<variant_t>;

  ::testing::StaticAssertTypeEq<
      typename arene::base::variant_alternative<variant_size - 1, variant_t>::type,
      arene::base::variant_alternative_t<variant_size - 1, variant_t>>();
  ::testing::StaticAssertTypeEq<
      typename arene::base::variant_alternative<variant_size - 1, variant_t const>::type,
      arene::base::variant_alternative_t<variant_size - 1, variant_t const>>();
}

}  // namespace
