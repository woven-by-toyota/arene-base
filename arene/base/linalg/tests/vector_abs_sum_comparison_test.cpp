// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/linalg/tests/lapack_overload.hpp"
#include "arene/base/linalg/tests/property_compare.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/vector_abs_sum.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/type_list/concat.hpp"

// Declare the BLAS functions corresponding to our vector_abs_sum algorithm.
// The normal headers declaring these require hosted C library headers which are not available on all target platforms.
extern "C" {
// NOLINTBEGIN(readability-identifier-*)

/// @brief Sum of absolute values of a vector
/// @param length Number of elements of the input vector
/// @param x_vec Input vector
/// @param incx Stride from one entry of @c x_vec to the next entry
/// @see https://www.netlib.org/lapack/explore-html/d5/d72/group__asum.html
auto sasum_(  //
    std::int32_t const* length,
    float const* x_vec,
    std::int32_t const* incx
) -> float;

/// @brief Sum of absolute values of a vector
/// @param length Number of elements of the input vector
/// @param x_vec Input vector
/// @param incx Stride from one entry of @c x_vec to the next entry
/// @see https://www.netlib.org/lapack/explore-html/d5/d72/group__asum.html
auto dasum_(  //
    std::int32_t const* length,
    double const* x_vec,
    std::int32_t const* incx
) -> double;

// NOLINTEND(readability-identifier-*)
}  // extern "C"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief BLAS xASUM overload object; deduces the element type (@c float or @c double) from the vector data
constexpr auto blas_asum = arene::base::linalg_testing::make_lapack_overload(&sasum_, &dasum_);

template <typename T>
class LinalgVectorAbsSumLapackTest : public ::testing::Test {};

// Rank-1 -2 float and double mdspan types covering both static and dynamic
// extents. Layout is arbitrary.
using float_vector_types = ::testing::Types<
    arene::base::mdspan<float, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 1>>>;

using double_vector_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>>;

using vector_abs_sum_compatible_types = arene::base::type_lists::concat_t<float_vector_types, double_vector_types>;

TYPED_TEST_SUITE(LinalgVectorAbsSumLapackTest, vector_abs_sum_compatible_types, );

/// @test @c arene::base::linalg::vector_abs_sum agrees with BLAS @c xASUM on
/// randomly generated mdspans of rank 1.
ARENE_TYPED_PROPERTY(LinalgVectorAbsSumLapackTest, MatchesxASUM, (test_array<TypeParam> source_arr)) {
  auto const linalg_result = linalg::vector_abs_sum(source_arr.mdspan());

  auto const length = static_cast<std::int32_t>(source_arr.size());
  auto const incx = std::int32_t{1};
  auto const blas_result = blas_asum(&length, source_arr.data(), &incx);

  ARENE_PROP_LINALG_ASSERT_COMPARE(linalg_result, blas_result);
}

}  // namespace
