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

}  // namespace
