// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

/// @file add_comparison_test.cpp
/// Provides tests for linalg::add. Additionally, used as a template with
/// expand_template to provide tests for linalg::subtract.
///

#include <gtest/gtest.h>

#include "arene/base/linalg/add.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/tests/lapack_overload.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/ulp_tolerance.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/concat.hpp"

extern "C" {
// NOLINTBEGIN(readability-identifier-naming,readability-identifier-length)
void saxpy_(
    std::int32_t const* n,
    float const* alpha,
    float const* x,
    std::int32_t const* incx,
    float* y,
    std::int32_t const* incy
);
void daxpy_(
    std::int32_t const* n,
    double const* alpha,
    double const* x,
    std::int32_t const* incx,
    double* y,
    std::int32_t const* incy
);
// NOLINTEND(readability-identifier-naming,readability-identifier-length)
}

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief BLAS xAXPY overload object; deduces the element type (@c float or @c double ) from the vector data
constexpr auto blas_axpy = arene::base::linalg_testing::make_lapack_overload(&saxpy_, &daxpy_);

template <typename T>
class LinalgAddLapackTest : public ::testing::Test {};

// Rank-1 float and double mdspan types covering both static and dynamic extents.
using float_vector_types = ::testing::Types<
    arene::base::mdspan<float, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 1>>>;

using double_vector_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>>;

// Rank-2 float and double mdspan types covering both static and dynamic extents. The default layout_right means the
// underlying storage is contiguous, so xAXPY can be invoked over the flattened element range.
using float_matrix_types = ::testing::Types<
    arene::base::mdspan<float, arene::base::extents<std::size_t, 4, 4>>,
    arene::base::mdspan<float, arene::base::dextents<std::size_t, 2>>>;

using double_matrix_types = ::testing::Types<
    arene::base::mdspan<double, arene::base::extents<std::size_t, 4, 4>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 2>>>;

using axpy_compatible_types =
    arene::base::type_lists::concat_t<float_vector_types, double_vector_types, float_matrix_types, double_matrix_types>;

TYPED_TEST_SUITE(LinalgAddLapackTest, axpy_compatible_types, );

/// @test @c arene::base::linalg::add agrees with BLAS @c xAXPY (alpha = 1) on randomly generated mdspans of any rank.
/// The default @c layout_right makes the underlying storage contiguous, so @c xAXPY can be invoked over the flattened
/// element range using @c mdspan::size() as the length.
ARENE_TYPED_PROPERTY(
    LinalgAddLapackTest,
    MatchesXAxpyAlphaOne,
    (test_array<TypeParam> x_arr, test_array<TypeParam> y_arr)
) {
  using value_type = typename TypeParam::value_type;

  test_array<TypeParam> z_arr{};

  // Arguments are ordered so that flipping alpha between 1 and -1 switches
  // between z = y + x (add) and z = y - x (subtract).
  linalg::add(  //
      y_arr.mdspan(),
      x_arr.mdspan(),
      z_arr.mdspan()
  );

  test_array<TypeParam> y_lapack_arr{y_arr};
  value_type const alpha = 1;
  auto const length = static_cast<std::int32_t>(x_arr.size());
  auto const incx = std::int32_t{1};
  auto const incy = std::int32_t{1};

  blas_axpy(&length, &alpha, x_arr.data(), &incx, y_lapack_arr.data(), &incy);

  static constexpr std::uint32_t tolerance_ulps{1U};
  static constexpr auto tolerance = arene::base::ulp_tolerance(tolerance_ulps);

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(  //
      z_arr.mdspan(),
      y_lapack_arr.mdspan(),
      [](auto lhs, auto rhs) {          //
        return arene::base::isnan(lhs)  //
                   ? arene::base::isnan(rhs)
                   : tolerance(lhs, rhs);
      }
  ));
}

}  // namespace
