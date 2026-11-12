// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/matrix_observers.hpp"
#include "arene/base/linalg/matrix_vector_product.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/default_accessor.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/testing/gtest.hpp"

// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::default_accessor;
using arene::base::dynamic_extent;
using arene::base::extents;
using arene::base::mdspan;
using arene::base::linalg_testing::dynamic_test_array;
using arene::base::linalg_testing::test_array;
using arene::base::linalg_testing::unqualified_linalg_matrix_types;

template <typename Matrix>
using in_vector_type = mdspan<
    typename Matrix::element_type,
    extents<typename Matrix::index_type, linalg::static_column_count_v<Matrix>>,
    typename Matrix::layout_type,
    typename Matrix::accessor_type>;

template <typename Matrix>
using out_vector_type = mdspan<
    std::remove_const_t<typename Matrix::element_type>,
    extents<typename Matrix::index_type, linalg::static_row_count_v<Matrix>>,
    typename Matrix::layout_type,
    default_accessor<std::remove_const_t<typename Matrix::element_type>>>;

template <typename T>
class MatrixVectorProductDeathTest : public ::testing::Test {};

TYPED_TEST_SUITE(MatrixVectorProductDeathTest, unqualified_linalg_matrix_types, );

/// @test @c matrix_vector_product with an extent mismatch between the inputs is a precondition violation
CONDITIONAL_TYPED_TEST(
    MatrixVectorProductDeathTest,
    MismatchedInputVectorIsPreconditionViolation,
    (linalg::static_column_count_v<TypeParam> == dynamic_extent &&
     std::is_constructible<typename TypeParam::mapping_type, typename TypeParam::extents_type>::value)
) {
  test_array<TypeParam> in_matrix{};
  test_array<in_vector_type<TypeParam>> in_vector{};
  in_vector_type<TypeParam> const in_vector_span{in_vector.data(), in_matrix.mdspan().extent(1U) - 1U};
  test_array<out_vector_type<TypeParam>> out_vector{};

  ASSERT_DEATH(linalg::matrix_vector_product(in_matrix.mdspan(), in_vector_span, out_vector.mdspan()), "Precondition");
}

/// @test @c matrix_vector_product with an output vector that doesn't match the matrix is a precondition violation
CONDITIONAL_TYPED_TEST(
    MatrixVectorProductDeathTest,
    MismatchedOutputVectorIsPreconditionViolation,
    (linalg::static_row_count_v<TypeParam> == dynamic_extent &&
     std::is_constructible<typename TypeParam::mapping_type, typename TypeParam::extents_type>::value)
) {
  test_array<TypeParam> in_matrix{};
  test_array<in_vector_type<TypeParam>> in_vector{};
  test_array<out_vector_type<TypeParam>> out_vector{};
  out_vector_type<TypeParam> const out_vector_span{out_vector.data(), in_matrix.mdspan().extent(0U) - 1U};

  ASSERT_DEATH(linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), out_vector_span), "Precondition");
}

/// @test @c matrix_vector_product with offset with an extent mismatch between the inputs is a precondition violation
CONDITIONAL_TYPED_TEST(
    MatrixVectorProductDeathTest,
    MismatchedInputVectorWithOffsetIsPreconditionViolation,
    (linalg::static_column_count_v<TypeParam> == dynamic_extent &&
     std::is_constructible<typename TypeParam::mapping_type, typename TypeParam::extents_type>::value)
) {
  test_array<TypeParam> in_matrix{};
  test_array<in_vector_type<TypeParam>> in_vector{};
  in_vector_type<TypeParam> const in_vector_span{in_vector.data(), in_matrix.mdspan().extent(1U) - 1U};
  test_array<out_vector_type<TypeParam>> offset_vector{};
  test_array<out_vector_type<TypeParam>> out_vector{};

  ASSERT_DEATH(
      linalg::matrix_vector_product(in_matrix.mdspan(), in_vector_span, offset_vector.mdspan(), out_vector.mdspan()),
      "Precondition"
  );
}

/// @test @c matrix_vector_product with offset with an output vector that doesn't match is a precondition violation
CONDITIONAL_TYPED_TEST(
    MatrixVectorProductDeathTest,
    MismatchedOutputVectorWithOffsetIsPreconditionViolation,
    (linalg::static_row_count_v<TypeParam> == dynamic_extent &&
     std::is_constructible<typename TypeParam::mapping_type, typename TypeParam::extents_type>::value)
) {
  test_array<TypeParam> in_matrix{};
  test_array<in_vector_type<TypeParam>> in_vector{};
  test_array<out_vector_type<TypeParam>> offset_vector{};
  test_array<out_vector_type<TypeParam>> out_vector{};
  out_vector_type<TypeParam> const out_vector_span{out_vector.data(), in_matrix.mdspan().extent(0U) - 1U};

  ASSERT_DEATH(
      linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), offset_vector.mdspan(), out_vector_span),
      "Precondition"
  );
}

/// @test @c matrix_vector_product with an offset vector that doesn't match the matrix is a precondition violation
CONDITIONAL_TYPED_TEST(
    MatrixVectorProductDeathTest,
    MismatchedOffsetVectorIsPreconditionViolation,
    (linalg::static_row_count_v<TypeParam> == dynamic_extent &&
     std::is_constructible<typename TypeParam::mapping_type, typename TypeParam::extents_type>::value)
) {
  test_array<TypeParam> in_matrix{};
  test_array<in_vector_type<TypeParam>> in_vector{};
  test_array<out_vector_type<TypeParam>> offset_vector{};
  out_vector_type<TypeParam> const offset_vector_span{offset_vector.data(), in_matrix.mdspan().extent(0U) - 1U};
  test_array<out_vector_type<TypeParam>> out_vector{};

  ASSERT_DEATH(
      linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), offset_vector_span, out_vector.mdspan()),
      "Precondition"
  );
}

}  // namespace
