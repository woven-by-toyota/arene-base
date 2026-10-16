// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_GCC("-Wconversion", "Conversion from 'long int' to 'int' (in use of 'transform_reduce') may change value");
#include "arene/base/linalg/vector_abs_sum.hpp"
ARENE_IGNORE_END();

#include <gtest/gtest.h>

#include "arene/base/functional/identity.hpp"
#include "arene/base/linalg/tests/configurable_scalar_value.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::linalg::vector_abs_sum;
using arene::base::linalg_testing::test_array;

template <typename T>
class LinalgVectorAbsSumTest : public ::testing::Test {};

// all scalar types are signed
using unqualified_vector_types = ::testing::Types<
    // Mdspans that are input vectors only but not writeable
    arene::base::mdspan<double const, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<
        std::int32_t,
        arene::base::extents<std::size_t, 1>,
        arene::base::layout_right,
        arene::base::linalg_testing::doubled_accessor<std::int32_t>>,
    // Mdspans that are input/output vectors
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3>>,
    arene::base::mdspan<double, arene::base::extents<std::size_t, 1>>>;

TYPED_TEST_SUITE(LinalgVectorAbsSumTest, unqualified_vector_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests are precise enough to use exact equality in some cases");

/// @test The vector_abs_sum result is zero if the operand is zero
TYPED_TEST(LinalgVectorAbsSumTest, ASumOfZeroVectorIsZero) {
  test_array<TypeParam> zero_test_vec{};

  ARENE_PROP_ASSERT(vector_abs_sum(zero_test_vec.mdspan()) == typename TypeParam::value_type{});
}

/// @test The vector_abs_sum result is never less than initial value
ARENE_TYPED_PROPERTY(
    LinalgVectorAbsSumTest,
    VectorAbsSumAlreadyGreaterThanInitialValue,
    (test_array<TypeParam> test_vec)
) {
  using value_type = typename TypeParam::value_type;

  ARENE_PROP_ASSERT(vector_abs_sum(test_vec.mdspan()) >= value_type{});
  ARENE_PROP_ASSERT(vector_abs_sum(test_vec.mdspan(), value_type{1}) >= value_type{1});
  ARENE_PROP_ASSERT(vector_abs_sum(test_vec.mdspan(), -value_type{1}) >= -value_type{1});
}

template <typename T>
class LinalgVectorAbsSumConstraintTest : public LinalgVectorAbsSumTest<T> {};

TYPED_TEST_SUITE(LinalgVectorAbsSumConstraintTest, arene::base::linalg_testing::mdspan_types, );

template <typename ValueType>
using vec3_span = arene::base::mdspan<ValueType, arene::base::extents<std::size_t, 3U>>;

/// @test vector_abs_sum can be invoked with a different scalar type from the vector value type
TEST(LinalgVectorAbsSumConstraintTest, VectorAbsSumDifferentScalarAndVectorValueType) {
  using arene::base::is_nothrow_invocable_r_v;
  using int32_span = vec3_span<std::int32_t>;
  using int64_span = vec3_span<std::int64_t>;

  // This test requires '-Wconversion' suppression, which is done when the
  // `vector_abs_sum.hpp` header is included.

  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int32_t, decltype(vector_abs_sum), int32_span, std::int32_t>));
  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int64_t, decltype(vector_abs_sum), int32_span, std::int64_t>));

  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int32_t, decltype(vector_abs_sum), int64_span, std::int32_t>));
  EXPECT_TRUE((is_nothrow_invocable_r_v<std::int64_t, decltype(vector_abs_sum), int64_span, std::int64_t>));
}

/// @test vector_abs_sum is not noexcept when the scalar type's addition or assignment are not noexcept
TEST(LinalgVectorAbsSumConstraintTest, VectorAbsSumNoexceptDependsOnScalarOps) {
  using non_throwing = ::testing::configurable_value<int>;
  using throwing_add = ::testing::configurable_value<int, ::testing::throws_on::plus>;
  using throwing_move = ::testing::configurable_value<int, ::testing::throws_on::move_assign>;
  using throwing_add_assign = ::testing::configurable_value<int, ::testing::throws_on::plus_equals_difference>;

  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  using dummy_norm = arene::base::identity;

  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<non_throwing>, non_throwing, dummy_norm>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(vector_abs_sum), vec3_span<non_throwing>, non_throwing, dummy_norm>));

  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<throwing_add>, throwing_add, dummy_norm>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(vector_abs_sum), vec3_span<throwing_add>, throwing_add, dummy_norm>));

  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<throwing_move>, throwing_move, dummy_norm>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(vector_abs_sum), vec3_span<throwing_move>, throwing_move, dummy_norm>));

  // The implementation doesn't actually use add-assign so it has no effect on its noexcept status
  EXPECT_TRUE(
      (is_invocable_v<decltype(vector_abs_sum), vec3_span<throwing_add_assign>, throwing_add_assign, dummy_norm>)
  );
  EXPECT_TRUE((is_nothrow_invocable_v<
               decltype(vector_abs_sum),
               vec3_span<throwing_add_assign>,
               throwing_add_assign,
               dummy_norm>));
}

/// @test vector_abs_sum is not noexcept when the norm is not noexcept
TEST(LinalgVectorAbsSumConstraintTest, VectorAbsSumNoexceptDependsOnNorm) {
  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  struct throwing_norm {
    constexpr auto operator()(int) const noexcept(false) -> int { return {}; }
  };

  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<int>, int, arene::base::identity>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(vector_abs_sum), vec3_span<int>, int, arene::base::identity>));

  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<int>, int, throwing_norm>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(vector_abs_sum), vec3_span<int>, int, throwing_norm>));
}

/// @test vector_abs_sum is not invocable when the scalar type's addition or assignment are disabled
TEST(LinalgVectorAbsSumConstraintTest, VectorAbsSumInvocabilityDependsOnScalarOps) {
  using ::arene::base::linalg_testing::configurable_scalar_value;
  using ::testing::disable;

  using all_enabled = configurable_scalar_value<int>;
  using no_add = configurable_scalar_value<int, disable::plus>;
  using no_move = configurable_scalar_value<int, disable::move_assign>;
  using no_add_assign = configurable_scalar_value<int, disable::plus_assign>;

  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  using dummy_norm = arene::base::identity;

  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<all_enabled>, all_enabled, dummy_norm>));

  EXPECT_FALSE((is_invocable_v<decltype(vector_abs_sum), vec3_span<no_add>, no_add, dummy_norm>));
  EXPECT_FALSE((is_invocable_v<decltype(vector_abs_sum), vec3_span<no_move>, no_move, dummy_norm>));

  // The implementation doesn't actually use add-assign so it has no effect on its invocability
  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<no_add_assign>, no_add_assign, dummy_norm>));
}

/// @test vector_abs_sum is not invocable when the norm is not invocable with the range value type
TEST(LinalgVectorAbsSumConstraintTest, VectorAbsSumInvocabilityDependsOnNorm) {
  using ::arene::base::linalg_testing::configurable_scalar_value;

  using all_enabled = configurable_scalar_value<int>;

  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  using dummy_norm = arene::base::identity;

  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<int>, int>));
  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<int>, int, dummy_norm>));

  EXPECT_FALSE((is_invocable_v<decltype(vector_abs_sum), vec3_span<all_enabled>, all_enabled>));
  EXPECT_TRUE((is_invocable_v<decltype(vector_abs_sum), vec3_span<all_enabled>, all_enabled, dummy_norm>));
}

ARENE_IGNORE_END();

}  // namespace
