// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/linalg/matrix_observers.hpp"
#include "arene/base/linalg/matrix_one_norm.hpp"
#include "arene/base/linalg/tests/lapack_overload.hpp"
#include "arene/base/linalg/tests/property_compare.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/concat.hpp"

// Declare the BLAS functions corresponding to our matrix_one_norm algorithm.
// The normal headers declaring these require hosted C library headers which are not available on all target platforms.
extern "C" {
// NOLINTBEGIN(readability-identifier-*)

/// @brief determines a norm of a general rectangular matrix
/// @param norm specifies the type of norm
/// @param rows number of rows of the matrix
/// @param cols number of columns of the matrix
/// @param a_mat input matrix
/// @param lda leading dimension of @c a_mat (i.e. stride)
/// @param work workspace, only used for normI (infinity norm)
/// @see https://www.netlib.org/lapack/explore-html/d8/d2e/group__lange.html
auto slange_(  //
    char const* norm,
    std::int32_t const* rows,
    std::int32_t const* cols,
    float const* a_mat,
    std::int32_t const* lda,
    float* work
) -> float;

/// @brief determines a norm of a general rectangular matrix
/// @param norm specifies the type of norm
/// @param rows number of rows of the matrix
/// @param cols number of columns of the matrix
/// @param a_mat input matrix
/// @param lda leading dimension of @c a_mat (i.e. stride)
/// @param work workspace, only used for normI (infinity norm)
/// @see https://www.netlib.org/lapack/explore-html/d8/d2e/group__lange.html
auto dlange_(  //
    char const* norm,
    std::int32_t const* rows,
    std::int32_t const* cols,
    double const* a_mat,
    std::int32_t const* lda,
    double* work
) -> double;

// NOLINTEND(readability-identifier-*)
}  // extern "C"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief LAPACK xLANGE overload object; deduces the element type (@c float or @c double) from the vector data
constexpr auto lapack_lange = arene::base::linalg_testing::make_lapack_overload(&slange_, &dlange_);

template <typename T>
class LinalgMatrixL1NormLapackTest : public ::testing::Test {};

// Rank-2 float and double mdspan types covering both static and dynamic
// extents. Layout left matches Fortran order.
using float_matrix_types = ::testing::Types<
    arene::base::mdspan<float, arene::base::extents<std::size_t, 4, 4>, arene::base::layout_left>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 2>, arene::base::layout_left>>;

using double_matrix_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 4, 4>, arene::base::layout_left>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 2>, arene::base::layout_left>>;

using matrix_one_norm_compatible_types = arene::base::type_lists::concat_t<float_matrix_types, double_matrix_types>;

TYPED_TEST_SUITE(LinalgMatrixL1NormLapackTest, matrix_one_norm_compatible_types, );

/// @test @c arene::base::linalg::matrix_one_norm agrees with LAPACK @c xLANGE on
/// randomly generated mdspans of rank 2.
ARENE_TYPED_PROPERTY(LinalgMatrixL1NormLapackTest, MatchesxLANGE, (test_array<TypeParam> source_arr)) {
  auto const linalg_result = linalg::matrix_one_norm(source_arr.mdspan());

  auto const norm = '1';  // requests the one norm (maximum column sum), which matches matrix_one_norm
  auto const rows = static_cast<std::int32_t>(arene::base::linalg::row_count(source_arr.mdspan()));
  auto const cols = static_cast<std::int32_t>(arene::base::linalg::column_count(source_arr.mdspan()));
  auto const lda = rows;

  auto const lapack_result = lapack_lange(&norm, &rows, &cols, source_arr.data(), &lda, nullptr);

  ARENE_PROP_LINALG_ASSERT_COMPARE(linalg_result, lapack_result);
}

}  // namespace
