// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/dot.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/functional/bind_overloads.hpp"
#include "arene/base/linalg/tests/configurable_scalar_value.hpp"
#include "arene/base/linalg/tests/property_compare.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/ulp_tolerance.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/stdlib_choice/is_volatile.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::linalg::dot;
using arene::base::linalg_testing::test_array;

template <typename T>
class LinalgDotTest : public ::testing::Test {};

using unqualified_vector_types = ::testing::Types<
    // Mdspans that are input vectors only but not writeable
    arene::base::mdspan<double const, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<std::uint32_t const, arene::base::extents<std::size_t, 4>, arene::base::layout_left>,
    arene::base::mdspan<
        std::int32_t,
        arene::base::extents<std::size_t, 1>,
        arene::base::layout_right,
        arene::base::linalg_testing::doubled_accessor<std::int32_t>>,
    // Mdspans that are input/output vectors
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3>>,
    arene::base::mdspan<double, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<std::uint32_t, arene::base::extents<std::size_t, 4>, arene::base::layout_left>>;

TYPED_TEST_SUITE(LinalgDotTest, unqualified_vector_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests are precise enough to use exact equality in some cases");

/// @test The dot product is zero when both of the operands are zero
TYPED_TEST(LinalgDotTest, DotOfZeroVectorsIsZero) {
  test_array<TypeParam> zero_test_vec{};
  TypeParam const zero_vec = zero_test_vec;

  ARENE_PROP_ASSERT(dot(zero_vec, zero_vec) == typename TypeParam::value_type{});
}

/// @test The dot product, like all inner products, is zero when one of the operands is zero
ARENE_TYPED_PROPERTY(LinalgDotTest, DotWithZeroIsZero, (test_array<TypeParam> test_vec)) {
  test_array<TypeParam> zero_test_vec{};
  TypeParam const zero_vec = zero_test_vec;

  TypeParam const vec = test_vec;

  ARENE_PROP_ASSERT(dot(zero_vec, vec) == typename TypeParam::value_type{});
  ARENE_PROP_ASSERT(dot(vec, zero_vec) == typename TypeParam::value_type{});
}

/// @test The dot product, like all inner products, is strictly non-negative
ARENE_TYPED_PROPERTY(LinalgDotTest, SelfDotIsNonNegative, (test_array<TypeParam> test_vec)) {
  TypeParam const vec = test_vec;
  ARENE_PROP_ASSERT(dot(vec, vec) >= typename TypeParam::value_type{});
}

/// @test The dot product, like all inner products, is symmetric in the order of its arguments
ARENE_TYPED_PROPERTY(LinalgDotTest, DotIsSymmetric, (test_array<TypeParam> left, test_array<TypeParam> right)) {
  TypeParam const left_vec = left;
  TypeParam const right_vec = right;

  // NOLINTNEXTLINE(readability-suspicious-call-argument) The two arguments are deliberately swapped
  ARENE_PROP_ASSERT(dot(left_vec, right_vec) == dot(right_vec, left_vec));
}

/// @test The dot product, like all inner products, is linear in scalar multiplication of either argument
ARENE_TYPED_PROPERTY(LinalgDotTest, DotIsBilinear, (test_array<TypeParam> original)) {
  TypeParam const original_vec = original;

  test_array<TypeParam> negated{original};
  negated.negate();
  TypeParam const negated_vec = negated;

  test_array<TypeParam> doubled{original};
  doubled.scalar_mul(typename TypeParam::value_type{2});
  TypeParam const doubled_vec = doubled;

  typename TypeParam::value_type const original_product{dot(original_vec, original_vec)};

  ARENE_PROP_LINALG_ASSERT_COMPARE(dot(original_vec, negated_vec), -original_product);
  ARENE_PROP_LINALG_ASSERT_COMPARE(dot(negated_vec, original_vec), -original_product);

  auto const cmp = arene::base::bind_overloads(               //
      [](double) { return arene::base::ulp_tolerance(1U); },  //
      [](auto) { return std::equal_to<>{}; }                  //
  )(typename TypeParam::value_type{});

  ARENE_PROP_LINALG_ASSERT_COMPARE(
      cmp,
      dot(original_vec, doubled_vec),
      (typename TypeParam::value_type{2} * original_product)
  );
  ARENE_PROP_LINALG_ASSERT_COMPARE(
      cmp,
      dot(doubled_vec, original_vec),
      (typename TypeParam::value_type{2} * original_product)
  );
}

template <typename T>
class LinalgDotConstraintTest : public LinalgDotTest<T> {};

TYPED_TEST_SUITE(LinalgDotConstraintTest, arene::base::linalg_testing::mdspan_types, );

/// @test Dot is invocable for exactly those pairs of types where both are linalg vectors with matching dimension
TYPED_TEST(LinalgDotConstraintTest, DotOnlyInvocableForMatchingVectors) {
  using arene::base::linalg_testing::category;

  using type = typename TypeParam::type;
  constexpr bool should_work{
      !std::is_volatile<type>::value &&
      ((TypeParam::expectation == category::in_vector) || (TypeParam::expectation == category::inout_vector))
  };

  EXPECT_EQ((arene::base::is_invocable_v<decltype(dot), type, type>), should_work);
  EXPECT_EQ((arene::base::is_nothrow_invocable_v<decltype(dot), type, type>), should_work);
}

template <typename ValueType>
using vec3_span = arene::base::mdspan<ValueType, arene::base::extents<std::size_t, 3U>>;

/// @test Dot can be invoked with different heterogeneous vector value types
TEST(LinalgDotConstraintTest, DotWorksWithDifferentValueTypesIfDimensionsMatch) {
  using arene::base::is_nothrow_invocable_r_v;
  using int32_span = vec3_span<std::int32_t>;
  using int64_span = vec3_span<std::int64_t>;
  using int64_dspan = arene::base::mdspan<std::int64_t, arene::base::dextents<std::size_t, 1U>>;

  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int64_t, decltype(dot), int32_span, int64_span>));
  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int64_t, decltype(dot), int32_span, int64_span, std::int64_t>));
  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int32_t, decltype(dot), int32_span, int64_span, std::int32_t>));

  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int64_t, decltype(dot), int32_span, int64_dspan>));
  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int64_t, decltype(dot), int32_span, int64_dspan, std::int64_t>));
  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int32_t, decltype(dot), int32_span, int64_dspan, std::int32_t>));
}

/// @test Dot is not noexcept when the scalar type's addition, multiplication, or assignment are not noexcept
TEST(LinalgDotConstraintTest, DotNoexceptIsCorrect) {
  using non_throwing = ::testing::configurable_value<int>;
  using throwing_add = ::testing::configurable_value<int, ::testing::throws_on::plus>;
  using throwing_mul = ::testing::configurable_value<int, ::testing::throws_on::multiply>;
  using throwing_move = ::testing::configurable_value<int, ::testing::throws_on::move_assign>;
  using throwing_add_assign = ::testing::configurable_value<int, ::testing::throws_on::plus_equals_difference>;

  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  EXPECT_TRUE((is_invocable_v<decltype(dot), vec3_span<non_throwing>, vec3_span<non_throwing>>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(dot), vec3_span<non_throwing>, vec3_span<non_throwing>>));

  EXPECT_TRUE((is_invocable_v<decltype(dot), vec3_span<throwing_add>, vec3_span<throwing_add>>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(dot), vec3_span<throwing_add>, vec3_span<throwing_add>>));

  EXPECT_TRUE((is_invocable_v<decltype(dot), vec3_span<throwing_mul>, vec3_span<throwing_mul>>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(dot), vec3_span<throwing_mul>, vec3_span<throwing_mul>>));

  EXPECT_TRUE((is_invocable_v<decltype(dot), vec3_span<throwing_move>, vec3_span<throwing_move>>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(dot), vec3_span<throwing_move>, vec3_span<throwing_move>>));

  // The implementation doesn't actually use add-assign so it has no effect on its noexcept status
  EXPECT_TRUE((is_invocable_v<decltype(dot), vec3_span<throwing_add_assign>, vec3_span<throwing_add_assign>>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(dot), vec3_span<throwing_add_assign>, vec3_span<throwing_add_assign>>));
}

/// @test Dot is not invocable when the scalar type's addition, multiplication, or assignment are disabled
TEST(LinalgDotConstraintTest, DotInvocabilityIsCorrect) {
  using ::arene::base::linalg_testing::configurable_scalar_value;
  using ::testing::disable;

  using all_enabled = configurable_scalar_value<int>;
  using no_add = configurable_scalar_value<int, disable::plus>;
  using no_mul = configurable_scalar_value<int, disable::multiplies>;
  using no_move = configurable_scalar_value<int, disable::move_assign>;
  using no_add_assign = configurable_scalar_value<int, disable::plus_assign>;

  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  EXPECT_TRUE((is_invocable_v<decltype(dot), vec3_span<all_enabled>, vec3_span<all_enabled>>));

  EXPECT_FALSE((is_invocable_v<decltype(dot), vec3_span<no_add>, vec3_span<no_add>>));
  EXPECT_FALSE((is_invocable_v<decltype(dot), vec3_span<no_mul>, vec3_span<no_mul>>));
  EXPECT_FALSE((is_invocable_v<decltype(dot), vec3_span<no_move>, vec3_span<no_move>>));

  // The implementation doesn't actually use add-assign so it has no effect on its invocability
  EXPECT_TRUE((is_invocable_v<decltype(dot), vec3_span<no_add_assign>, vec3_span<no_add_assign>>));
}

ARENE_IGNORE_END();

}  // namespace
