// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <complex>

#include <gtest/gtest.h>

#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/matrix_observers.hpp"
#include "arene/base/linalg/matrix_vector_product.hpp"
#include "arene/base/linalg/scaled_accessor.hpp"
#include "arene/base/linalg/tests/lapack_overload.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/math/ulp_tolerance.hpp"
#include "arene/base/mdspan/default_accessor.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_traits/give_cv_to.hpp"

// Declare the BLAS functions corresponding to our matrix_vector_product algorithm.
// The normal headers declaring these require hosted C library headers which are not available on all target platforms.
extern "C" {
// NOLINTBEGIN(readability-identifier-*)

/// @brief Matrix-vector product with results written into a second vector
/// @param trans Transposition mode: 'n' for none, 'T' (transpose) or 'C' (conjugate) to multiply by A^T instead of A
/// @param row_count Row count of <c>matrix</c>; also the length of @c out_vector
/// @param column_count Column count of <c>matrix</c>; also the length of @c in_vector
/// @param product_scale_factor Scalar factor by which to multiply the product before adding it to @c out_vector
/// @param matrix Matrix @c A to use in the multiplication @c A*x
/// @param lda Physical leading dimension of @c matrix such that @c lda entries could be accessed without UB
/// (this must be at least @c row_count and for our purposes can simply be set to @c row_count unconditionally)
/// @param in_vector Vector @c x to use in the multiplication <c>y = A*x</c>
/// @param in_vector_stride The stride (a.k.a. increment) in memory between entries of @c in_vector
/// @param prior_value_scale_factor Factor to multiply the prior value of @c out_vector before adding the product to it
/// @param out_vector Vector @c y to use in the multiplication <c>y = A*x</c>
/// @param out_vector_stride The stride (a.k.a. increment) in memory between entries of @c out_vector
/// @see https://www.netlib.org/lapack/explore-html/d7/dda/group__gemv.html
auto cgemv_(  //
    char* trans,
    std::int32_t const* row_count,
    std::int32_t const* column_count,
    std::complex<float> const* product_scale_factor,  // called "alpha" in BLAS
    std::complex<float> const* matrix,
    std::int32_t const* lda,
    std::complex<float> const* in_vector,
    std::int32_t const* in_vector_stride,
    std::complex<float> const* prior_value_scale_factor,  // called "beta" in BLAS
    std::complex<float>* out_vector,
    std::int32_t const* out_vector_stride
) -> void;

/// @brief Matrix-vector product with results written into a second vector
/// @param trans Transposition mode: 'n' for none, 'T' (transpose) or 'C' (conjugate) to multiply by A^T instead of A
/// @param row_count Row count of <c>matrix</c>; also the length of @c out_vector
/// @param column_count Column count of <c>matrix</c>; also the length of @c in_vector
/// @param product_scale_factor Scalar factor by which to multiply the product before adding it to @c out_vector
/// @param matrix Matrix @c A to use in the multiplication @c A*x
/// @param lda Physical leading dimension of @c matrix such that @c lda entries could be accessed without UB
/// (this must be at least @c row_count and for our purposes can simply be set to @c row_count unconditionally)
/// @param in_vector Vector @c x to use in the multiplication <c>y = A*x</c>
/// @param in_vector_stride The stride (a.k.a. increment) in memory between entries of @c in_vector
/// @param prior_value_scale_factor Factor to multiply the prior value of @c out_vector before adding the product to it
/// @param out_vector Vector @c y to use in the multiplication <c>y = A*x</c>
/// @param out_vector_stride The stride (a.k.a. increment) in memory between entries of @c out_vector
/// @see https://www.netlib.org/lapack/explore-html/d7/dda/group__gemv.html
auto zgemv_(  //
    char* trans,
    std::int32_t const* row_count,
    std::int32_t const* column_count,
    std::complex<double> const* product_scale_factor,  // called "alpha" in BLAS
    std::complex<double> const* matrix,
    std::int32_t const* lda,
    std::complex<double> const* in_vector,
    std::int32_t const* in_vector_stride,
    std::complex<double> const* prior_value_scale_factor,  // called "beta" in BLAS
    std::complex<double>* out_vector,
    std::int32_t const* out_vector_stride
) -> void;

// NOLINTEND(readability-identifier-*)
}  // extern "C"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::default_accessor;
using arene::base::extents;
using arene::base::give_cv_to_t;
using arene::base::layout_left;
using arene::base::mdspan;
using arene::base::linalg_testing::test_array;
using arene::base::linalg_testing::unqualified_linalg_matrix_types;
using arene::base::numbers::one_v;
using arene::base::numbers::zero_v;
using arene::base::type_lists::apply_each_t;
using arene::base::type_lists::filter_t;

/// @brief BLAS xGEMV overload object; deduces the element type (@c float or @c double) from the element types
constexpr auto blas_gemv = arene::base::linalg_testing::make_lapack_overload(
    arene::base::linalg_testing::unspecified_function,  // float is not tested in this file
    arene::base::linalg_testing::unspecified_function,  // double is not tested in this file
    &cgemv_,
    &zgemv_
);

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
class LinalgMatrixVectorProductLapackTest : public ::testing::Test {};

template <typename Mdspan>
using element_type_is_floating_point =
    std::integral_constant<bool, std::is_floating_point<typename Mdspan::element_type>::value>;

// xGEMV assumes that the layout is column-major, so in the row-major case, we need to ask it to transpose the indices.
// (test_array uses layout_right when constructing a layout_stride so it's also row major within our unit tests)
template <typename Mdspan>
constexpr bool layout_is_row_major{
    // NOLINTNEXTLINE(misc-redundant-expression) False positive: layout_right and layout_stride are not the same
    std::is_same<typename Mdspan::layout_type, arene::base::layout_right>::value ||
    std::is_same<typename Mdspan::layout_type, arene::base::layout_stride>::value
};

using floating_point_matrix_types = filter_t<unqualified_linalg_matrix_types, element_type_is_floating_point>;

template <class Mdspan>
class make_element_type_complex {};

template <class ElementType, class Extents, class Layout, template <class> class Accessor>
class make_element_type_complex<mdspan<ElementType, Extents, Layout, Accessor<ElementType>>> {
  using new_el = give_cv_to_t<ElementType, std::complex<std::remove_cv_t<ElementType>>>;

 public:
  using type = mdspan<new_el, Extents, Layout, Accessor<new_el>>;
};

template <class Mdspan>
using make_element_type_complex_t = typename make_element_type_complex<Mdspan>::type;

using complex_matrix_types = apply_each_t<floating_point_matrix_types, make_element_type_complex_t>;

/// @brief Complex version of @c ulp_tolerance which checks the tolerance for both real and imaginary components
/// @{
template <typename UlpTolerance>
class complex_ulp_comparator {
  arene::base::ulp_tolerance_detail::ulp_tolerance_comparator<UlpTolerance> real_comp_;

 public:
  explicit constexpr complex_ulp_comparator(UlpTolerance tol) noexcept
      : real_comp_{arene::base::ulp_tolerance(tol)} {}

  template <typename T, typename U>
  constexpr auto operator()(T left, U right) const noexcept -> bool {
    using arene::base::math::imag;
    using arene::base::math::real;

    bool const real_ok{real_comp_(real(left), real(right))};
    bool const imag_ok{real_comp_(imag(left), imag(right))};

    return real_ok && imag_ok;
  }
};

class complex_ulp_tolerance_impl {
 public:
  template <typename UlpTolerance>
  constexpr auto operator()(UlpTolerance tol) const noexcept -> complex_ulp_comparator<UlpTolerance> {
    return complex_ulp_comparator<UlpTolerance>{tol};
  }
};

constexpr complex_ulp_tolerance_impl complex_ulp_tolerance{};
/// @}

TYPED_TEST_SUITE(LinalgMatrixVectorProductLapackTest, complex_matrix_types, );

/// @test @c arene::base::linalg::matrix_vector_product agrees with BLAS @c xGEMV on randomly generated arguments.
ARENE_TYPED_PROPERTY(
    LinalgMatrixVectorProductLapackTest,
    MatchesxGEMV,
    (test_array<TypeParam> in_matrix,
     test_array<in_vector_type<TypeParam>> in_vector,
     test_array<out_vector_type<TypeParam>> linalg_out_vector,
     test_array<out_vector_type<TypeParam>> blas_out_vector)
) {
  using element_type = typename TypeParam::element_type;

  linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), linalg_out_vector.mdspan());

  constexpr bool row_major{layout_is_row_major<TypeParam>};

  char trans{row_major ? 'T' : 'N'};  // i.e. "transpose if the data layout is row major"
  auto const first_extent = static_cast<std::int32_t>(
      row_major ? linalg::column_count(in_matrix.mdspan()) : linalg::row_count(in_matrix.mdspan())
  );
  auto const second_extent = static_cast<std::int32_t>(
      row_major ? linalg::row_count(in_matrix.mdspan()) : linalg::column_count(in_matrix.mdspan())
  );
  element_type const product_scale_factor{one_v<element_type>};  // i.e. "just give me the product", no scale
  element_type const* in_matrix_data = in_matrix.data();
  std::int32_t const lda{first_extent};
  element_type const* in_vector_data = in_vector.data();
  auto const in_vector_stride = static_cast<std::int32_t>(in_vector.mdspan().stride(0U));
  element_type const prior_value_scale_factor{zero_v<element_type>};  // i.e. "overwrite the output vector"
  std::remove_const_t<element_type>* out_vector_data = blas_out_vector.data();
  auto const out_vector_stride = static_cast<std::int32_t>(blas_out_vector.mdspan().stride(0U));

  blas_gemv(
      &trans,
      &first_extent,
      &second_extent,
      &product_scale_factor,
      in_matrix_data,
      &lda,
      in_vector_data,
      &in_vector_stride,
      &prior_value_scale_factor,
      out_vector_data,
      &out_vector_stride
  );

  ARENE_PROP_ASSERT(
      linalg::equal_elements(linalg_out_vector.mdspan(), blas_out_vector.mdspan(), complex_ulp_tolerance(128U))
  );
}

/// @test @c arene::base::linalg::matrix_vector_product agrees with BLAS @c xGEMV when given an offset.
ARENE_TYPED_PROPERTY(
    LinalgMatrixVectorProductLapackTest,
    MatchesxGEMVWithOffset,
    (test_array<TypeParam> in_matrix,
     test_array<in_vector_type<TypeParam>> multiplicand_vector,
     test_array<out_vector_type<TypeParam>> offset_vector,
     test_array<out_vector_type<TypeParam>> linalg_out_vector)
) {
  using element_type = typename TypeParam::element_type;

  linalg::matrix_vector_product(
      in_matrix.mdspan(),
      multiplicand_vector.mdspan(),
      offset_vector.mdspan(),
      linalg_out_vector.mdspan()
  );

  // The BLAS computation here is the same as the above test case except that beta is set to 1.0 so that the result is
  // added to the pre-existing contents of the output vector (which is offset_vector) rather than overwriting them.

  constexpr bool row_major{layout_is_row_major<TypeParam>};

  char trans{row_major ? 'T' : 'N'};  // i.e. "transpose if the data layout is row major"
  auto const first_extent = static_cast<std::int32_t>(
      row_major ? linalg::column_count(in_matrix.mdspan()) : linalg::row_count(in_matrix.mdspan())
  );
  auto const second_extent = static_cast<std::int32_t>(
      row_major ? linalg::row_count(in_matrix.mdspan()) : linalg::column_count(in_matrix.mdspan())
  );
  element_type const product_scale_factor{one_v<element_type>};  // i.e. "just give me the product", no scale
  element_type const* in_matrix_data = in_matrix.data();
  std::int32_t const lda{first_extent};
  element_type const* multiplicand_vector_data = multiplicand_vector.data();
  auto const multiplicand_vector_stride = static_cast<std::int32_t>(multiplicand_vector.mdspan().stride(0U));
  element_type const prior_value_scale_factor{one_v<element_type>};  // i.e. "add to the output vector"
  std::remove_const_t<element_type>* out_vector_data = offset_vector.data();
  auto const out_vector_stride = static_cast<std::int32_t>(offset_vector.mdspan().stride(0U));

  blas_gemv(
      &trans,
      &first_extent,
      &second_extent,
      &product_scale_factor,
      in_matrix_data,
      &lda,
      multiplicand_vector_data,
      &multiplicand_vector_stride,
      &prior_value_scale_factor,
      out_vector_data,
      &out_vector_stride
  );

  // offset_vector now contains the result of the BLAS computation.

  ARENE_PROP_ASSERT(
      linalg::equal_elements(linalg_out_vector.mdspan(), offset_vector.mdspan(), complex_ulp_tolerance(64U))
  );
}

/// @test @c arene::base::linalg::matrix_vector_product agrees with BLAS @c xGEMV with random scale parameters.
ARENE_TYPED_PROPERTY(
    LinalgMatrixVectorProductLapackTest,
    MatchesxGEMVWithRandomScales,
    (test_array<TypeParam> in_matrix,
     test_array<in_vector_type<TypeParam>> multiplicand_vector,
     test_array<out_vector_type<TypeParam>> offset_vector,
     test_array<out_vector_type<TypeParam>> linalg_out_vector,
     test_array<mdspan<typename TypeParam::element_type, extents<int, 2>>> scales)
) {
  using element_type = typename TypeParam::element_type;

  // These are generated as a test_array so that their range is limited in the same way as mdspan elements are.
  // There is currently no way to do this directly from the property testing library.
  element_type const input_scale = scales.mdspan()(0);
  element_type const offset_scale = scales.mdspan()(1);

  linalg::matrix_vector_product(
      in_matrix.mdspan(),
      linalg::scaled(input_scale, multiplicand_vector.mdspan()),
      linalg::scaled(offset_scale, offset_vector.mdspan()),
      linalg_out_vector.mdspan()
  );

  // The BLAS computation here is the same as the above test case except that alpha is set to input_scale and beta is
  // set to offset_scale. The computation is effected on the linalg side by scaling the arguments, but on the BLAS side
  // the scales are additional function arguments.

  constexpr bool row_major{layout_is_row_major<TypeParam>};

  char trans{row_major ? 'T' : 'N'};  // i.e. "transpose if the data layout is row major"
  auto const first_extent = static_cast<std::int32_t>(
      row_major ? linalg::column_count(in_matrix.mdspan()) : linalg::row_count(in_matrix.mdspan())
  );
  auto const second_extent = static_cast<std::int32_t>(
      row_major ? linalg::row_count(in_matrix.mdspan()) : linalg::column_count(in_matrix.mdspan())
  );
  element_type const* in_matrix_data = in_matrix.data();
  std::int32_t const lda{first_extent};
  element_type const* multiplicand_vector_data = multiplicand_vector.data();
  auto const multiplicand_vector_stride = static_cast<std::int32_t>(multiplicand_vector.mdspan().stride(0U));
  std::remove_const_t<element_type>* out_vector_data = offset_vector.data();
  auto const out_vector_stride = static_cast<std::int32_t>(offset_vector.mdspan().stride(0U));

  blas_gemv(
      &trans,
      &first_extent,
      &second_extent,
      &input_scale,
      in_matrix_data,
      &lda,
      multiplicand_vector_data,
      &multiplicand_vector_stride,
      &offset_scale,
      out_vector_data,
      &out_vector_stride
  );

  // offset_vector now contains the result of the BLAS computation.

  ARENE_PROP_ASSERT(
      linalg::equal_elements(linalg_out_vector.mdspan(), offset_vector.mdspan(), complex_ulp_tolerance(32U))
  );
}

}  // namespace
