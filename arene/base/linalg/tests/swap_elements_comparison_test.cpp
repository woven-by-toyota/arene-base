// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/swap_elements.hpp"
#include "arene/base/linalg/tests/lapack_overload.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/concat.hpp"

// Declare the BLAS functions corresponding to our swap_elements algorithm.
// The normal headers declaring these require hosted C library headers which are not available on all target platforms.
extern "C" {
// NOLINTBEGIN(readability-identifier-naming,readability-identifier-length)

/// @brief Swap the contents of two vectors, returning nothing; overload for float vectors
/// @param length Shared length of the two vectors
/// @param x_vec First vector to swap elements from
/// @param incx Stride from one entry of @c x_vec to the next entry
/// @param y_vec Second vector to swap elements from
/// @param incy Stride from one entry of @c y_vec to the next entry
void sswap_(std::int32_t const* length, float* x_vec, std::int32_t const* incx, float* y_vec, std::int32_t const* incy);

/// @brief Swap the contents of two vectors, returning nothing; overload for double vectors
/// @param length Shared length of the two vectors
/// @param x_vec First vector to swap elements from
/// @param incx Stride from one entry of @c x_vec to the next entry
/// @param y_vec Second vector to swap elements from
/// @param incy Stride from one entry of @c y_vec to the next entry
void dswap_(
    std::int32_t const* length,
    double* x_vec,
    std::int32_t const* incx,
    double* y_vec,
    std::int32_t const* incy
);

// NOLINTEND(readability-identifier-naming,readability-identifier-length)
}  // extern "C"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief BLAS xSWAP overload object; deduces the element type (@c float or @c double ) from the vector data
constexpr auto blas_swap = arene::base::linalg_testing::make_lapack_overload(&sswap_, &dswap_);

template <typename T>
class LinalgSwapElementsLapackTest : public ::testing::Test {};

// Rank-1 and rank-2 float and double mdspan types covering both static and dynamic extents. The default layout_right
// means the underlying storage is contiguous, so xSWAP can be invoked over the flattened element range.
using float_vector_types = ::testing::Types<
    arene::base::mdspan<float, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 1>>>;

using double_vector_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>>;

using float_matrix_types = ::testing::Types<
    arene::base::mdspan<float, arene::base::extents<std::size_t, 4, 4>>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 2>>>;

using double_matrix_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 4, 4>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 2>>>;

using swap_compatible_types =
    arene::base::type_lists::concat_t<float_vector_types, double_vector_types, float_matrix_types, double_matrix_types>;

TYPED_TEST_SUITE(LinalgSwapElementsLapackTest, swap_compatible_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "swap_elements is an exact swap, so exact equality against BLAS xSWAP holds");

/// @test @c arene::base::linalg::swap_elements agrees with BLAS @c xSWAP on randomly generated mdspans of any rank. The
/// default @c layout_right makes the underlying storage contiguous, so @c xSWAP can be invoked over the flattened
/// element range using @c mdspan::size() as the length.
ARENE_TYPED_PROPERTY(
    LinalgSwapElementsLapackTest,
    MatchesXSwap,
    (test_array<TypeParam> const original_left_arr, test_array<TypeParam> const original_right_arr)
) {
  test_array<TypeParam> left_arr{original_left_arr};
  test_array<TypeParam> right_arr{original_right_arr};

  linalg::swap_elements(left_arr.mdspan(), right_arr.mdspan());

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(left_arr.mdspan(), original_right_arr.mdspan()));
  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(right_arr.mdspan(), original_left_arr.mdspan()));

  auto const length = static_cast<std::int32_t>(original_left_arr.size());
  auto const incx = std::int32_t{1};
  auto const incy = std::int32_t{1};
  blas_swap(&length, left_arr.data(), &incx, right_arr.data(), &incy);

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(left_arr.mdspan(), original_left_arr.mdspan()));
  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(right_arr.mdspan(), original_right_arr.mdspan()));
}

ARENE_IGNORE_END();

}  // namespace
