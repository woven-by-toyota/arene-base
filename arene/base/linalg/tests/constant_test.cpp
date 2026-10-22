// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/constant.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/index_permutation_range.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::extents;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::linalg::constant;

using extent_types = ::testing::Types<
    arene::base::extents<std::size_t>,            // rank 0
    arene::base::extents<std::size_t, 4>,         // rank 1 static
    arene::base::dextents<std::size_t, 1>,        // rank 1 dynamic
    arene::base::extents<std::size_t, 3, 4>,      // rank 2 static
    arene::base::dextents<std::size_t, 2>,        // rank 2 dynamic
    arene::base::extents<std::size_t, 1, 2, 3>>;  // rank 3 static

template <class T>
struct LinalgConstant : ::testing::Test {};

TYPED_TEST_SUITE(LinalgConstant, extent_types, );

/// @test constant returns an mdspan with specified value and extents
CONSTEXPR_TYPED_TEST(LinalgConstant, ReturnsConstantWithValueAndExtents) {
  auto const constant_span = constant(42, TypeParam{});

  static_assert(arene::base::is_mdspan_v<decltype(constant_span)>, "must be 'mdspan' type");

  // math_reference_t is limited to rank-1 and rank-2
  ::testing::StaticAssertTypeEq<int, typename decltype(constant_span)::reference>();

  ::testing::StaticAssertTypeEq<TypeParam, typename decltype(constant_span)::extents_type>();
  CONSTEXPR_ASSERT_EQ(constant_span.extents(), TypeParam{});
}

/// @test all elements of constant are equal to the stored value for all valid indices
CONSTEXPR_TYPED_TEST(LinalgConstant, AllElementsOfConstantAreEqual) {
  constexpr auto stored_value = int{42};
  auto const constant_span = constant(stored_value, TypeParam{});

  using ::arene::base::detail::make_index_permutation_range;
  auto const extents_array = arene::base::mdspan_detail::to_array(constant_span.extents());

  for (auto const& indices : make_index_permutation_range(extents_array)) {
    CONSTEXPR_ASSERT_EQ(stored_value, constant_span(indices));
  }
}

/// @test constant can be used with an integral constant
CONSTEXPR_TYPED_TEST(LinalgConstant, CanStoreIntegralConstants) {
  constexpr auto stored_value = int{42};
  auto const constant_span = constant(std::integral_constant<int, stored_value>{}, TypeParam{});

  using ::arene::base::detail::make_index_permutation_range;
  auto const extents_array = arene::base::mdspan_detail::to_array(constant_span.extents());

  for (auto const& indices : make_index_permutation_range(extents_array)) {
    CONSTEXPR_ASSERT_EQ(stored_value, constant_span(indices));
  }
}

/// @test constant is not invocable if the value parameter doesn't model scalar or the extents parameter is not a
/// specialization of extents
TEST(LinalgConstant, InvocabilityTests) {
  ASSERT_TRUE((is_invocable_v<decltype(constant), int, extents<int>>));

  ASSERT_FALSE((is_invocable_v<
                decltype(constant),
                testing::configurable_value<int, testing::throws_on::nothing, testing::disable::default_construct>,
                extents<int>>));
  ASSERT_FALSE((is_invocable_v<decltype(constant), int, int>));
}

/// @test constant is noexcept(false) if copy construction of the value type is noexcept(false)
TEST(LinalgConstant, NotInvocableForNonExtents) {
  using throws_on_copy = testing::configurable_value<int, testing::throws_on::copy_construct>;

  ASSERT_TRUE((is_invocable_v<decltype(constant), int, extents<int>>));
  ASSERT_TRUE((is_invocable_v<decltype(constant), throws_on_copy, extents<int>>));

  ASSERT_TRUE((is_nothrow_invocable_v<decltype(constant), int, extents<int>>));
  ASSERT_FALSE((is_nothrow_invocable_v<decltype(constant), throws_on_copy, extents<int>>));
}

}  // namespace
