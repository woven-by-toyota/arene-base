// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/linalg/tests/lapack_overload.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/vector_idx_abs_max.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/concat.hpp"

// Declare the BLAS functions corresponding to our vector_idx_abs_max algorithm.
// The normal headers declaring these require hosted C library headers which are not available on all target platforms.
extern "C" {
// NOLINTBEGIN(readability-identifier-naming)

/// @brief finds the index of the first element having maximum absolute value
/// @param length number of elements of the input vector
/// @param x_vec input vector
/// @param incx stride from one entry of @c x_vec to the next entry
/// @see https://www.netlib.org/lapack/explore-html/dd/d52/group__iamax.html
auto isamax_(  //
    std::int32_t const* length,
    float const* x_vec,
    std::int32_t const* incx
) -> std::int32_t;

/// @brief finds the index of the first element having maximum absolute value
/// @param length number of elements of the input vector
/// @param x_vec input vector
/// @param incx stride from one entry of @c x_vec to the next entry
/// @see https://www.netlib.org/lapack/explore-html/dd/d52/group__iamax.html
auto idamax_(  //
    std::int32_t const* length,
    double const* x_vec,
    std::int32_t const* incx
) -> std::int32_t;

// NOLINTEND(readability-identifier-naming)
}  // extern "C"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief BLAS IxAMAX overload object; deduces the element type (@c float or @c double) from the vector data
constexpr auto blas_ixamax = arene::base::linalg_testing::make_lapack_overload(&isamax_, &idamax_);

template <typename T>
class LinalgVectorIdxAbsMaxLapackTest : public ::testing::Test {};

// Rank-1 float and double mdspan types covering both static and dynamic
// extents. Layout is arbitrary.
using float_vector_types = ::testing::Types<
    arene::base::mdspan<float, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 1>>>;

using double_vector_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>>;

using vector_idx_abs_max_compatible_types = arene::base::type_lists::concat_t<float_vector_types, double_vector_types>;

TYPED_TEST_SUITE(LinalgVectorIdxAbsMaxLapackTest, vector_idx_abs_max_compatible_types, );

/// @test @c arene::base::linalg::vector_idx_abs_max agrees with BLAS @c IxAMAX on
/// randomly generated mdspans of rank 1.
ARENE_TYPED_PROPERTY(LinalgVectorIdxAbsMaxLapackTest, MatchesIxAMAX, (test_array<TypeParam> source_arr)) {
  auto const linalg_result = linalg::vector_idx_abs_max(source_arr.mdspan());

  auto const length = static_cast<std::int32_t>(source_arr.size());
  auto const incx = std::int32_t{1};
  auto const blas_result = blas_ixamax(&length, source_arr.data(), &incx);

  // Fortran uses 1-based indexing by default
  ARENE_PROP_ASSERT(linalg_result + 1U == static_cast<std::size_t>(blas_result));
}

}  // namespace
