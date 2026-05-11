// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/de_ice.hpp"

#include <gtest/gtest.h>

#include "arene/base/mdspan/tests/test_index_types.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

template <class T>
class MdspanDetailDeIceTest : public testing::Test {};

TYPED_TEST_SUITE(MdspanDetailDeIceTest, test::index_types, );

/// @test @c de_ice returns an integral unchanged
CONSTEXPR_TYPED_TEST(MdspanDetailDeIceTest, IntegralUnchanged) {
  auto value = TypeParam{42};

  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          TypeParam const&,
          decltype(arene::base::mdspan_detail::de_ice(value))  //
          >::value
  );

  CONSTEXPR_ASSERT(arene::base::mdspan_detail::de_ice(value) == value);
}

/// @test @c de_ice returns a user defined type unchanged
TYPED_TEST(MdspanDetailDeIceTest, UserDefinedUnchanged) {
  struct user_defined_type {
    int value;
  };

  auto value = user_defined_type{42};

  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          user_defined_type const&,
          decltype(arene::base::mdspan_detail::de_ice(value))  //
          >::value
  );

  CONSTEXPR_ASSERT(arene::base::mdspan_detail::de_ice(value).value == value.value);
}

/// @test @c de_ice returns the value of an @c integral_constant
TYPED_TEST(MdspanDetailDeIceTest, IntegralConstantValue) {
  auto value = std::integral_constant<TypeParam, 42>{};

  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          TypeParam const&,
          decltype(arene::base::mdspan_detail::de_ice(value))  //
          >::value
  );

  CONSTEXPR_ASSERT(arene::base::mdspan_detail::de_ice(value) == value.value);
}

struct zero_constant {
  static constexpr int value{};
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator int() const noexcept { return value; }
};
int const zero_constant::value;

/// @test @c de_ice returns the value of an integral-constant-like
TYPED_TEST(MdspanDetailDeIceTest, IntegralConstantLikeValue) {
  auto value = zero_constant{};

  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          int const&,
          decltype(arene::base::mdspan_detail::de_ice(value))  //
          >::value
  );

  CONSTEXPR_ASSERT(arene::base::mdspan_detail::de_ice(value) == value.value);
}
}  // namespace
