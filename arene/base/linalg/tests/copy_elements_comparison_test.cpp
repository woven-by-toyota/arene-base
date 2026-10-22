// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/linalg/copy_elements.hpp"
#include "arene/base/linalg/equal_elements.hpp"
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

// Declare the BLAS functions corresponding to our copy_elements algorithm.
// The normal headers declaring these require hosted C library headers which are not available on all target platforms.
extern "C" {
// NOLINTBEGIN(readability-identifier-naming,readability-identifier-length)

/// @brief Copy a vector into another vector, returning nothing
/// @param length Shared length of the two vectors
/// @param x_vec Source vector to copy from
/// @param incx Stride from one entry of @c x_vec to the next entry
/// @param y_vec Destination vector to copy into
/// @param incy Stride from one entry of @c y_vec to the next entry
void scopy_(
    std::int32_t const* length,
    float const* x_vec,
    std::int32_t const* incx,
    float* y_vec,
    std::int32_t const* incy
);

/// @brief Copy a vector into another vector, returning nothing
/// @param length Shared length of the two vectors
/// @param x_vec Source vector to copy from
/// @param incx Stride from one entry of @c x_vec to the next entry
/// @param y_vec Destination vector to copy into
/// @param incy Stride from one entry of @c y_vec to the next entry
void dcopy_(
    std::int32_t const* length,
    double const* x_vec,
    std::int32_t const* incx,
    double* y_vec,
    std::int32_t const* incy
);

// NOLINTEND(readability-identifier-naming,readability-identifier-length)
}  // extern "C"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief BLAS xCOPY overload object; deduces the element type (@c float or @c double ) from the vector data
constexpr auto blas_copy = arene::base::linalg_testing::make_lapack_overload(&scopy_, &dcopy_);

template <typename T>
class LinalgCopyElementsLapackTest : public ::testing::Test {};

// Rank-1 and rank-2 float and double mdspan types covering both static and dynamic extents. The default layout_right
// means the underlying storage is contiguous, so xCOPY can be invoked over the flattened element range.
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

using copy_compatible_types =
    arene::base::type_lists::concat_t<float_vector_types, double_vector_types, float_matrix_types, double_matrix_types>;

TYPED_TEST_SUITE(LinalgCopyElementsLapackTest, copy_compatible_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "copy_elements is an exact copy, so exact equality against BLAS xCOPY holds");

/// @test @c arene::base::linalg::copy_elements agrees with BLAS @c xCOPY on randomly generated mdspans of any rank. The
/// default @c layout_right makes the underlying storage contiguous, so @c xCOPY can be invoked over the flattened
/// element range using @c mdspan::size() as the length.
ARENE_TYPED_PROPERTY(LinalgCopyElementsLapackTest, MatchesXCopy, (test_array<TypeParam> source_arr)) {
  test_array<TypeParam> dest_arr{};
  test_array<TypeParam> blas_dest_arr{};

  linalg::copy_elements(source_arr.mdspan(), dest_arr.mdspan());

  auto const length = static_cast<std::int32_t>(source_arr.size());
  auto const incx = std::int32_t{1};
  auto const incy = std::int32_t{1};
  blas_copy(&length, source_arr.data(), &incx, blas_dest_arr.data(), &incy);

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(dest_arr.mdspan(), blas_dest_arr.mdspan()));
}

ARENE_IGNORE_END();

}  // namespace
