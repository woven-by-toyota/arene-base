// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file linalg_test.cpp
/// @brief Provides unit tests to validate export of content in linalg.hpp
///

#include "arene/base/linalg.hpp"

#include <gtest/gtest.h>

namespace {

// Object traits

/// @test `is_scalar_v` is declared
TEST(Mdspan, IsScalarVIsDeclared) { using arene::base::linalg::is_scalar_v; }

/// @test `is_in_vector_v` is declared
TEST(Mdspan, IsInVectorVIsDeclared) { using arene::base::linalg::is_in_vector_v; }

/// @test `is_out_vector_v` is declared
TEST(Mdspan, IsOutVectorVIsDeclared) { using arene::base::linalg::is_out_vector_v; }

/// @test `is_inout_vector_v` is declared
TEST(Mdspan, IsInoutVectorVIsDeclared) { using arene::base::linalg::is_inout_vector_v; }

/// @test `is_in_matrix_v` is declared
TEST(Mdspan, IsInMatrixVIsDeclared) { using arene::base::linalg::is_in_matrix_v; }

/// @test `is_out_matrix_v` is declared
TEST(Mdspan, IsOutMatrixVIsDeclared) { using arene::base::linalg::is_out_matrix_v; }

/// @test `is_inout_matrix_v` is declared
TEST(Mdspan, IsInoutMatrixVIsDeclared) { using arene::base::linalg::is_inout_matrix_v; }

/// @test `is_in_object_v` is declared
TEST(Mdspan, IsInLinalgObjectVIsDeclared) { using arene::base::linalg::is_in_object_v; }

/// @test `is_out_object_v` is declared
TEST(Mdspan, IsOutLinalgObjectVIsDeclared) { using arene::base::linalg::is_out_object_v; }

/// @test `is_inout_object_v` is declared
TEST(Mdspan, IsInoutLinalgObjectVIsDeclared) { using arene::base::linalg::is_inout_object_v; }

/// @test `is_diagonal_storage_tag_v` is declared
TEST(Mdspan, IsDiagonalStorageTagVIsDeclared) { using arene::base::linalg::is_diagonal_storage_tag_v; }

/// @test `is_triangular_tag_v` is declared
TEST(Mdspan, IsTriangularTagVIsDeclared) { using arene::base::linalg::is_triangular_tag_v; }

// Math traits

/// @test `math_traits` is declared
TEST(Mdspan, MathTraitsIsDeclared) { using arene::base::linalg::math_traits; }

/// @test `math_element_t` is declared
TEST(Mdspan, MathElementTectorVIsDeclared) { using arene::base::linalg::math_element_t; }

/// @test `math_value_t` is declared
TEST(Mdspan, MathValueTectorVIsDeclared) { using arene::base::linalg::math_value_t; }

/// @test `math_reference_t` is declared
TEST(Mdspan, MathReferenceTectorVIsDeclared) { using arene::base::linalg::math_reference_t; }

/// @test `math_index_t` is declared
TEST(Mdspan, MathIndexTIsDeclared) { using arene::base::linalg::math_index_t; }

/// @test `math_size_t` is declared
TEST(Mdspan, MathSizeTIsDeclared) { using arene::base::linalg::math_size_t; }

/// @test `math_layout_t` is declared
TEST(Mdspan, MathLayoutTIsDeclared) { using arene::base::linalg::math_layout_t; }

/// @test `math_rank_t` is declared
TEST(Mdspan, MathRankTIsDeclared) { using arene::base::linalg::math_rank_t; }

/// @test `math_extents_t` is declared
TEST(Mdspan, MathExtentsTIsDeclared) { using arene::base::linalg::math_extents_t; }

/// @test `math_accessor_t` is declared
TEST(Mdspan, MathAccessorTIsDeclared) { using arene::base::linalg::math_accessor_t; }

// Mandates

/// @test `compatible_static_extents` is declared
TEST(Mdspan, CompatibleStaticExtentsIsDeclared) { using arene::base::linalg::compatible_static_extents; }

/// @test `possibly_addable_v` is declared
TEST(Mdspan, PossiblyAddableVIsDeclared) { using arene::base::linalg::possibly_addable_v; }

/// @test `possibly_comparable_v` is declared
TEST(Mdspan, PossiblyComparableVIsDeclared) { using arene::base::linalg::possibly_comparable_v; }

/// @test `possibly_multipliable_v` is declared
TEST(Mdspan, PossiblyMultipliableVIsDeclared) { using arene::base::linalg::possibly_multipliable_v; }

/// @test `possibly_square` is declared
TEST(Mdspan, PossiblySquareIsDeclared) { using arene::base::linalg::possibly_square; }

/// @test `possibly_square_v` is declared
TEST(Mdspan, PossiblySquareVIsDeclared) { using arene::base::linalg::possibly_square_v; }

// Classes

/// @test `factor_result` is declared
TEST(Mdspan, FactorResultIsDeclared) { using arene::base::linalg::factor_result; }

/// @test `identity_matrix` is declared
TEST(Mdspan, IdentityMatrixIsDeclared) { using arene::base::linalg::identity_matrix; }

/// @test `layout_constant_diagonal` is declared
TEST(Mdspan, LayoutConstantDiagonalIsDeclared) { using arene::base::linalg::layout_constant_diagonal; }

/// @test `layout_transpose` is declared
TEST(Mdspan, LayoutTransposedIsDeclared) { using arene::base::linalg::layout_transpose; }

/// @test `scaled_accessor` is declared
TEST(Mdspan, ScaledAccessorIsDeclared) { using arene::base::linalg::scaled_accessor; }

/// @test `explicit_diagonal_t` is declared
TEST(Mdspan, ExplicitDiagonalTIsDeclared) { using arene::base::linalg::explicit_diagonal_t; }

/// @test `implicit_unit_diagonal_t` is declared
TEST(Mdspan, ImplicitUnitDiagonalTIsDeclared) { using arene::base::linalg::implicit_unit_diagonal_t; }

/// @test `lower_triangle_t` is declared
TEST(Mdspan, LowerTriangleTIsDeclared) { using arene::base::linalg::lower_triangle_t; }

/// @test `upper_triangle_t` is declared
TEST(Mdspan, UpperTriangleTIsDeclared) { using arene::base::linalg::upper_triangle_t; }

/// @test `explicit_diagonal` is declared
TEST(Mdspan, ExplicitDiagonalIsDeclared) { using arene::base::linalg::explicit_diagonal; }

/// @test `implicit_unit_diagonal` is declared
TEST(Mdspan, ImplicitUnitDiagonalIsDeclared) { using arene::base::linalg::implicit_unit_diagonal; }

/// @test `lower_triangle` is declared
TEST(Mdspan, LowerTriangleIsDeclared) { using arene::base::linalg::lower_triangle; }

/// @test `upper_triangle` is declared
TEST(Mdspan, UpperTriangleIsDeclared) { using arene::base::linalg::upper_triangle; }

// Operations on objects

/// @test `add` is declared
TEST(Mdspan, AddIsDeclared) { using arene::base::linalg::add; }

/// @test `column_count` is declared
TEST(Mdspan, ColumnCountIsDeclared) { using arene::base::linalg::column_count; }

/// @test `column_view` is declared
TEST(Mdspan, ColumnViewIsDeclared) { using arene::base::linalg::column_view; }

/// @test `constant` is declared
TEST(Mdspan, ConstantIsDeclared) { using arene::base::linalg::constant; }

/// @test `copy_elements` is declared
TEST(Mdspan, CopyElementsIsDeclared) { using arene::base::linalg::copy_elements; }

/// @test `dot` is declared
TEST(Mdspan, DotIsDeclared) { using arene::base::linalg::dot; }

/// @test `elementwise_unary_transform` is declared
TEST(Mdspan, ElementwiseUnaryTransformIsDeclared) { using arene::base::linalg::elementwise_unary_transform; }

/// @test `elementwise_binary_transform` is declared
TEST(Mdspan, ElementwiseBinaryTransformIsDeclared) { using arene::base::linalg::elementwise_binary_transform; }

/// @test `equal_elements` is declared
TEST(Mdspan, EqualElementsIsDeclared) { using arene::base::linalg::equal_elements; }

/// @test `is_square` is declared
TEST(Mdspan, IsSquareIsDeclared) { using arene::base::linalg::is_square; }

/// @test `matrix_one_norm` is declared
TEST(Mdspan, MatrixOneNormIsDeclared) { using arene::base::linalg::matrix_one_norm; }

/// @test `matrix_vector_product` is declared
TEST(Mdspan, MatrixVectorProductIsDeclared) { using arene::base::linalg::matrix_vector_product; }

/// @test `row_count` is declared
TEST(Mdspan, RowCountIsDeclared) { using arene::base::linalg::row_count; }

/// @test `row_view` is declared
TEST(Mdspan, RowViewIsDeclared) { using arene::base::linalg::row_view; }

/// @test `static_column_count` is declared
TEST(Mdspan, StaticColumnCountIsDeclared) { using arene::base::linalg::static_column_count; }

/// @test `static_column_count_v` is declared
TEST(Mdspan, StaticColumnCountVIsDeclared) { using arene::base::linalg::static_column_count_v; }

/// @test `static_row_count` is declared
TEST(Mdspan, StaticRowCountIsDeclared) { using arene::base::linalg::static_row_count; }

/// @test `static_row_count_v` is declared
TEST(Mdspan, StaticRowCountVIsDeclared) { using arene::base::linalg::static_row_count_v; }

/// @test `subtract` is declared
TEST(Mdspan, SubtractIsDeclared) { using arene::base::linalg::subtract; }

/// @test `swap_elements` is declared
TEST(Mdspan, SwapElementsIsDeclared) { using arene::base::linalg::swap_elements; }

/// @test `swap_rows` is declared
TEST(Mdspan, SwapRowsIsDeclared) { using arene::base::linalg::swap_rows; }

/// @test `vector_abs_sum`
TEST(Mdspan, VectorAbsSum) { using arene::base::linalg::vector_abs_sum; }

/// @test `vector_idx_abs_max` is declared
TEST(Mdspan, VectorIdxAbsMaxIsDeclared) { using arene::base::linalg::vector_idx_abs_max; }

}  // namespace
