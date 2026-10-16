// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/linalg/dot.hpp"
#include "arene/base/linalg/tests/lapack_overload.hpp"
#include "arene/base/linalg/tests/property_compare.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/ulp_tolerance.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/nan_rejecting_distribution.hpp"
#include "arene/base/type_list/concat.hpp"

// Declare BLAS functions corresponding to our dot product
// The normal headers declaring these require hosted C library headers which are not available on all target platforms.
extern "C" {
// NOLINTBEGIN(readability-identifier-naming,readability-identifier-length)

/// @brief Dot product with an offset, returning a float
/// @param length Shared length of the two vectors
/// @param offset Offset to which the dot product is to be added before being returned
/// @param left_vec Left of the two vectors to be multiplied
/// @param left_increment Stride from one entry of @c left_vec to the next entry
/// @param right_vec Right of the two vectors to be multiplied
/// @param right_increment Stride from one entry of @c right_vec to the next entry
/// @return @c offset plus the dot product of @c left_vec and @c right_vec
auto sdsdot_(
    std::int32_t const* length,
    float const* offset,
    float const* left_vec,
    std::int32_t const* left_increment,
    float const* right_vec,
    std::int32_t const* right_increment
) -> float;

/// @brief Dot product with an offset, returning a float
/// @param length Shared length of the two vectors
/// @param left_vec Left of the two vectors to be multiplied
/// @param left_increment Stride from one entry of @c left_vec to the next entry
/// @param right_vec Right of the two vectors to be multiplied
/// @param right_increment Stride from one entry of @c right_vec to the next entry
/// @return The dot product of @c left_vec and @c right_vec
auto sdot_(
    std::int32_t const* length,
    float const* left_vec,
    std::int32_t const* left_increment,
    float const* right_vec,
    std::int32_t const* right_increment
) -> float;

/// @brief Dot product with an offset, returning a float
/// @param length Shared length of the two vectors
/// @param left_vec Left of the two vectors to be multiplied
/// @param left_increment Stride from one entry of @c left_vec to the next entry
/// @param right_vec Right of the two vectors to be multiplied
/// @param right_increment Stride from one entry of @c right_vec to the next entry
/// @return The dot product of @c left_vec and @c right_vec
auto ddot_(
    std::int32_t const* length,
    double const* left_vec,
    std::int32_t const* left_increment,
    double const* right_vec,
    std::int32_t const* right_increment
) -> double;

// NOLINTEND(readability-identifier-naming,readability-identifier-length)
}  // extern "C"

namespace {

/// @brief Perform a dot product using the official BLAS functions; no offset
/// @details The element type (@c float or @c double ) is deduced from the vector data.
template <typename LeftVec, typename RightVec>
auto blas_dot(LeftVec const& left_vec, RightVec const& right_vec) noexcept {
  ARENE_PRECONDITION(left_vec.extent(0) == right_vec.extent(0));

  static constexpr auto xdot = arene::base::linalg_testing::make_lapack_overload(&sdot_, &ddot_);

  auto const length = static_cast<std::int32_t>(left_vec.extent(0));
  auto const left_stride = static_cast<std::int32_t>(left_vec.stride(0));
  auto const right_stride = static_cast<std::int32_t>(right_vec.stride(0));

  return xdot(&length, left_vec.data_handle(), &left_stride, right_vec.data_handle(), &right_stride);
}

/// @brief Perform a dot product using the official BLAS functions; overload for floats with offset
template <
    typename LeftVec,
    typename RightVec,
    arene::base::constraints<
        std::enable_if_t<std::is_same<std::remove_const_t<typename LeftVec::value_type>, float>::value>,
        std::enable_if_t<std::is_same<std::remove_const_t<typename RightVec::value_type>, float>::value>> = nullptr>
auto blas_dot(LeftVec const& left_vec, RightVec const& right_vec, float offset) noexcept -> float {
  ARENE_PRECONDITION(left_vec.extent(0) == right_vec.extent(0));

  auto const length = static_cast<std::int32_t>(left_vec.extent(0));
  auto const left_stride = static_cast<std::int32_t>(left_vec.stride(0));
  auto const right_stride = static_cast<std::int32_t>(right_vec.stride(0));

  return sdsdot_(&length, &offset, left_vec.data_handle(), &left_stride, right_vec.data_handle(), &right_stride);
}

using arene::base::linalg::dot;
using arene::base::linalg_testing::layout_static_stride;
using arene::base::linalg_testing::test_array;

template <typename T>
class LinalgDotLapackTest : public ::testing::Test {};

using float_vector_types = ::testing::Types<
    // Mdspans that are input vectors only but not writeable
    arene::base::mdspan<float const, arene::base::extents<std::size_t, 4>, arene::base::layout_left>,
    arene::base::mdspan<float const, arene::base::dextents<std::size_t, 1>, layout_static_stride<3>>,
    // Mdspans that are input/output vectors
    arene::base::mdspan<float, arene::base::extents<std::size_t, 3>>,
    arene::base::mdspan<float const, arene::base::extents<std::size_t, 37>, layout_static_stride<2>>>;

using double_vector_types = ::testing::Types<
    // Mdspans that are input vectors only but not writeable
    arene::base::mdspan<double const, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<double const, arene::base::dextents<std::size_t, 1>, arene::base::layout_right>,
    arene::base::mdspan<double const, arene::base::extents<std::size_t, 15>, layout_static_stride<4>>,
    // Mdspans that are input/output vectors
    arene::base::mdspan<double, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<double, arene::base::extents<std::size_t, 4>, arene::base::layout_left>>;

using compatible_vector_types = arene::base::type_lists::concat_t<float_vector_types, double_vector_types>;

TYPED_TEST_SUITE(LinalgDotLapackTest, compatible_vector_types, );

/// @test The dot product is zero when both of the operands are zero
TYPED_TEST(LinalgDotLapackTest, ZeroWithZeroMatches) {
  test_array<TypeParam> zero_test_vec{};
  TypeParam const zero_vec = zero_test_vec;

  // The result of this is 0 so exact equality is fine.
  ARENE_PROP_LINALG_ASSERT_COMPARE(dot(zero_vec, zero_vec), blas_dot(zero_vec, zero_vec));
}

/// @test The dot product, like all inner products, is zero when one of the operands is zero
ARENE_TYPED_PROPERTY(LinalgDotLapackTest, NonzeroWithZeroMatches, (test_array<TypeParam> test_vec)) {
  test_array<TypeParam> zero_test_vec{};
  TypeParam const zero_vec = zero_test_vec;

  TypeParam const vec = test_vec;

  // The result of this is 0 so exact equality is fine.
  ARENE_PROP_LINALG_ASSERT_COMPARE(dot(zero_vec, vec), blas_dot(zero_vec, vec));
  ARENE_PROP_LINALG_ASSERT_COMPARE(dot(vec, zero_vec), blas_dot(vec, zero_vec));
}

/// @test Our dot product result matches that of LAPACK when a vector is multiplied by itself
ARENE_TYPED_PROPERTY(LinalgDotLapackTest, NonzeroWithSelfMatches, (test_array<TypeParam> test_vec)) {
  TypeParam vec = test_vec;

  // The dot product seems to be simple enough that exact equality actually holds.
  ARENE_PROP_LINALG_ASSERT_COMPARE(dot(vec, vec), blas_dot(vec, vec));
}

/// @test Our dot product result matches that of LAPACK when two non-zero vectors are multiplied
ARENE_TYPED_PROPERTY(
    LinalgDotLapackTest,
    NonzeroWithNonzeroMatches,
    (test_array<TypeParam> test_vec_left, test_array<TypeParam> test_vec_right)
) {
  TypeParam vec_left = test_vec_left;
  TypeParam vec_right = test_vec_right;

  // The dot product seems to be simple enough that exact equality actually holds.
  ARENE_PROP_LINALG_ASSERT_COMPARE(dot(vec_left, vec_right), blas_dot(vec_left, vec_right));
}

template <typename T>
class LinalgDotLapackFloatTest : public LinalgDotLapackTest<T> {};

TYPED_TEST_SUITE(LinalgDotLapackFloatTest, float_vector_types, );

/// @test Our dot product result matches that of LAPACK when two non-zero vectors are multiplied with an offset
ARENE_TYPED_PROPERTY(LinalgDotLapackFloatTest, NonzeroWithNonzeroAndOffsetMatches, (test_array<TypeParam> test_vec)) {
  TypeParam vec = test_vec;

  using arene::base::testing::bitwise_uniform_distribution;
  using arene::base::testing::nan_rejecting_distribution;
  nan_rejecting_distribution<bitwise_uniform_distribution<typename TypeParam::value_type>> const dist{{}};
  typename TypeParam::value_type offset{dist(prng)};
  while (offset < -100.0F || offset > 100.0F) {
    offset = dist(prng);
  }

  auto res_arene = dot(vec, vec, offset);
  auto res_blas = blas_dot(vec, vec, offset);
  ARENE_PROP_ASSERT(arene::base::isnan(res_arene) == arene::base::isnan(res_blas));
  ARENE_PROP_ASSERT(arene::base::isinf(res_arene) == arene::base::isinf(res_blas));
  if (!arene::base::isnan(res_arene) && !arene::base::isinf(res_arene)) {
    constexpr std::uint32_t tolerance_ulps{
        std::is_same<float const, typename TypeParam::value_type const>::value ? 0x12U : 0U
    };
    ARENE_PROP_LINALG_ASSERT_COMPARE(arene::base::ulp_tolerance(tolerance_ulps), res_arene, res_blas);
  }
}

}  // namespace
