// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/matrix_vector_product.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/algorithm/fill.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/matrix_observers.hpp"
#include "arene/base/linalg/tests/configurable_scalar_value.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/mdspan/default_accessor.hpp"  // IWYU pragma: keep
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::default_accessor;
using arene::base::extents;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::mdspan;
using arene::base::linalg_testing::test_array;
using arene::base::linalg_testing::unqualified_linalg_matrix_types;
using arene::base::numbers::one_v;
using arene::base::numbers::zero_v;
using arene::base::type_lists::filter_t;
using testing::configurable_value;
using testing::throws_on;

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
class MatrixVectorProductTest : public ::testing::Test {};

TYPED_TEST_SUITE(MatrixVectorProductTest, unqualified_linalg_matrix_types, );

/// @test @c matrix_vector_product is invocable if and only if the arguments are linalg objects of the correct kinds
TYPED_TEST(MatrixVectorProductTest, InvocableIfArgumentsHaveCorrectLinalgCategories) {
  using prod = decltype(linalg::matrix_vector_product);
  using element = typename TypeParam::element_type;
  using in_mat = TypeParam;
  using in_vec = in_vector_type<TypeParam>;
  using out_vec = out_vector_type<TypeParam>;  // also works for the optional offset vector

  // We have *constraints* on each argument being an in-matrix, in-vector, or out-vector, so we can check that each
  // argument is the correct value category using the type trait. On the other hand, compatibility of the extents is a
  // *mandate* so failing it causes a hard compilation error and therefore can not be checked with a type trait.
  EXPECT_TRUE((is_invocable_v<prod, in_mat, in_vec, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_vec, in_mat, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_vec, out_vec, in_mat>));

  EXPECT_FALSE((is_invocable_v<prod, element, in_vec, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_mat, element, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_mat, in_vec, element>));

  EXPECT_TRUE((is_invocable_v<prod, in_mat, in_vec, out_vec, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_vec, in_mat, out_vec, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_vec, out_vec, in_mat, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_vec, out_vec, out_vec, in_mat>));

  EXPECT_FALSE((is_invocable_v<prod, element, in_vec, out_vec, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_mat, element, out_vec, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_mat, in_vec, element, out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, in_mat, in_vec, out_vec, element>));
}

/// @test @c matrix_vector_product is not invocable if the scalar type is incompatible with @c dot
TYPED_TEST(MatrixVectorProductTest, InvocabilityRequiresDotInvocability) {
  using prod = decltype(linalg::matrix_vector_product);
  using ::arene::base::linalg_testing::rebind_to_configurable_scalar_t;
  using ::testing::disable;

  using in_mat = rebind_to_configurable_scalar_t<TypeParam>;
  using in_vec = in_vector_type<in_mat>;
  using out_vec = out_vector_type<in_mat>;
  EXPECT_TRUE((is_invocable_v<prod, in_mat, in_vec, out_vec>));
  EXPECT_TRUE((is_invocable_v<prod, in_mat, in_vec, out_vec, out_vec>));

  using no_add_in_mat = rebind_to_configurable_scalar_t<TypeParam, disable::plus>;
  using no_add_in_vec = in_vector_type<no_add_in_mat>;
  using no_add_out_vec = out_vector_type<no_add_in_mat>;
  EXPECT_FALSE((is_invocable_v<prod, no_add_in_mat, no_add_in_vec, no_add_out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, no_add_in_mat, no_add_in_vec, no_add_out_vec, no_add_out_vec>));

  using no_mul_in_mat = rebind_to_configurable_scalar_t<TypeParam, disable::multiplies>;
  using no_mul_in_vec = in_vector_type<no_mul_in_mat>;
  using no_mul_out_vec = out_vector_type<no_mul_in_mat>;
  EXPECT_FALSE((is_invocable_v<prod, no_mul_in_mat, no_mul_in_vec, no_mul_out_vec>));
  EXPECT_FALSE((is_invocable_v<prod, no_mul_in_mat, no_mul_in_vec, no_mul_out_vec, no_mul_out_vec>));
}

/// @test @c matrix_vector_product is @c noexcept for "normal" types with accesses, assignments, and dot @c noexcept
TYPED_TEST(MatrixVectorProductTest, NoexceptForNormalTypes) {
  using prod = decltype(linalg::matrix_vector_product);
  using in_mat = TypeParam;
  using in_vec = in_vector_type<TypeParam>;
  using out_vec = out_vector_type<TypeParam>;  // also works for the optional offset vector

  EXPECT_TRUE((is_nothrow_invocable_v<prod, in_mat, in_vec, out_vec>));
  EXPECT_TRUE((is_nothrow_invocable_v<prod, in_mat, in_vec, out_vec, out_vec>));
}

template <typename... Ts>
constexpr bool is_invocable_and_noexcept_false_v{is_invocable_v<Ts...> && !is_nothrow_invocable_v<Ts...>};

template <typename Mdspan>
struct with_throwing_accessor_impl;

template <typename ElementType, typename Extents, typename Layout, typename Accessor>
struct with_throwing_accessor_impl<mdspan<ElementType, Extents, Layout, Accessor>> {
  using type = mdspan<ElementType, Extents, Layout, arene::base::linalg_testing::throwing_accessor<ElementType>>;
};

template <typename Mdspan>
using with_throwing_accessor = typename with_throwing_accessor_impl<Mdspan>::type;

/// @test @c matrix_vector_product is @c noexcept(false) when the accessor of any of its arguments is
TYPED_TEST(MatrixVectorProductTest, NoexceptFalseWhenAccessorCanThrow) {
  using prod = decltype(linalg::matrix_vector_product);
  using in_mat = TypeParam;
  using in_vec = in_vector_type<TypeParam>;
  using out_vec = out_vector_type<TypeParam>;  // also works for the optional offset vector

  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, with_throwing_accessor<in_mat>, in_vec, out_vec>));
  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, with_throwing_accessor<in_vec>, out_vec>));
  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, in_vec, with_throwing_accessor<out_vec>>));

  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, with_throwing_accessor<in_mat>, in_vec, out_vec, out_vec>));
  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, with_throwing_accessor<in_vec>, out_vec, out_vec>));
  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, in_vec, with_throwing_accessor<out_vec>, out_vec>));
  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, in_vec, out_vec, with_throwing_accessor<out_vec>>));
}

template <typename Mdspan>
struct with_throwing_element_type_impl;

template <typename ElementType, typename Extents, typename Layout, template <typename> class Accessor>
struct with_throwing_element_type_impl<mdspan<ElementType, Extents, Layout, Accessor<ElementType>>> {
  using new_element_type = std::conditional_t<
      std::is_const<ElementType>::value,
      configurable_value<std::remove_const_t<ElementType>, throws_on::move_assign> const,
      configurable_value<ElementType, throws_on::move_assign>>;

  using type = mdspan<new_element_type, Extents, Layout, Accessor<new_element_type>>;
};

template <typename Mdspan>
using with_throwing_element_type = typename with_throwing_element_type_impl<Mdspan>::type;

/// @test @c matrix_vector_product is @c noexcept(false) when the move assignment of the element type is
TYPED_TEST(MatrixVectorProductTest, NoexceptFalseWhenAssignmentCanThrow) {
  using prod = decltype(linalg::matrix_vector_product);
  using in_mat = with_throwing_element_type<TypeParam>;
  using in_vec = with_throwing_element_type<in_vector_type<TypeParam>>;
  using out_vec = with_throwing_element_type<out_vector_type<TypeParam>>;  // also works for the optional offset vector

  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, in_vec, out_vec>));
  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, in_vec, out_vec, out_vec>));
}

template <typename Mdspan>
struct with_throwing_arithmetic_impl;

template <typename ElementType, typename Extents, typename Layout, template <typename> class Accessor>
struct with_throwing_arithmetic_impl<mdspan<ElementType, Extents, Layout, Accessor<ElementType>>> {
  using new_element_type = std::conditional_t<
      std::is_const<ElementType>::value,
      configurable_value<std::remove_const_t<ElementType>, throws_on::plus> const,
      configurable_value<ElementType, throws_on::plus>>;

  using type = mdspan<new_element_type, Extents, Layout, Accessor<new_element_type>>;
};

template <typename Mdspan>
using with_throwing_arithmetic = typename with_throwing_arithmetic_impl<Mdspan>::type;

/// @test @c matrix_vector_product is @c noexcept(false) when the arithmetic in the dot product is
TYPED_TEST(MatrixVectorProductTest, NoexceptFalseWhenDotProductCanThrow) {
  using prod = decltype(linalg::matrix_vector_product);
  using in_mat = with_throwing_arithmetic<TypeParam>;
  using in_vec = with_throwing_arithmetic<in_vector_type<TypeParam>>;
  using out_vec = with_throwing_arithmetic<out_vector_type<TypeParam>>;  // also works for the optional offset vector

  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, in_vec, out_vec>));
  EXPECT_TRUE((is_invocable_and_noexcept_false_v<prod, in_mat, in_vec, out_vec, out_vec>));
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "This test does not perform arithmetic so exact equality is correct");

/// @test Multiplying an arbitrary matrix with a zero vector yields a zero vector as output
ARENE_TYPED_PROPERTY(MatrixVectorProductTest, MultiplyingWithZeroVectorYieldsZero, (test_array<TypeParam> in_matrix)) {
  test_array<in_vector_type<TypeParam>> in_vector{};
  test_array<out_vector_type<TypeParam>> out_vector{};

  linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), out_vector.mdspan());

  for (typename TypeParam::index_type row_idx{}; row_idx < out_vector.mdspan().extent(0U); ++row_idx) {
    ARENE_PROP_ASSERT(out_vector.mdspan()(row_idx) == zero_v<typename TypeParam::element_type>);
  }
}

/// @test Multiplying an arbitrary vector with a zero matrix yields a zero vector as output
ARENE_TYPED_PROPERTY(
    MatrixVectorProductTest,
    MultiplyingWithZeroMatrixYieldsZero,
    (test_array<in_vector_type<TypeParam>> in_vector)
) {
  test_array<TypeParam> in_matrix{};
  test_array<out_vector_type<TypeParam>> out_vector{};

  linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), out_vector.mdspan());

  for (typename TypeParam::index_type row_idx{}; row_idx < out_vector.mdspan().extent(0U); ++row_idx) {
    ARENE_PROP_ASSERT(out_vector.mdspan()(row_idx) == zero_v<typename TypeParam::element_type>);
  }
}

/// @test Multiplying a matrix of all ones with a vector of all ones yields the width of the matrix in each entry
TYPED_TEST(MatrixVectorProductTest, MultiplyingOnesByOnesYieldsWidth) {
  using element_type = typename TypeParam::element_type;

  test_array<TypeParam> in_matrix{};
  test_array<in_vector_type<TypeParam>> in_vector{};
  test_array<out_vector_type<TypeParam>> out_vector{};

  arene::base::fill(in_matrix.data(), arene::base::next(in_matrix.data(), in_matrix.size()), one_v<element_type>);
  arene::base::fill(in_vector.data(), arene::base::next(in_vector.data(), in_vector.size()), one_v<element_type>);

  linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), out_vector.mdspan());

  // Using a non-default accessor can result in an extra constant factor even when the underlying data is all ones.
  element_type const accessor_factor{in_matrix.mdspan()(0, 0) * in_vector.mdspan()(0)};

  EXPECT_TRUE(arene::base::all_of(
      out_vector.data(),
      arene::base::next(out_vector.data(), out_vector.size()),
      [accessor_factor, width = static_cast<element_type>(linalg::column_count(in_matrix.mdspan()))](auto const& val) {
        return val == accessor_factor * width;
      }
  ));
}

/// @test Multiplying with a zero product and a non-zero offset yields the offset
ARENE_TYPED_PROPERTY(
    MatrixVectorProductTest,
    MultiplyingZeroWithOffsetPreservesOffset,
    (test_array<out_vector_type<TypeParam>> offset_vector)
) {
  test_array<TypeParam> in_matrix{};
  test_array<in_vector_type<TypeParam>> in_vector{};
  test_array<out_vector_type<TypeParam>> out_vector{};

  linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), offset_vector.mdspan(), out_vector.mdspan());

  ARENE_PROP_ASSERT(linalg::equal_elements(offset_vector.mdspan(), out_vector.mdspan()));
}

ARENE_IGNORE_END();

template <typename T>
using element_type_is_integral = std::integral_constant<bool, std::is_integral<typename T::element_type>::value>;

template <typename T>
class MatrixVectorProductIntegerTest : public ::testing::Test {};

using integral_matrix_types = filter_t<unqualified_linalg_matrix_types, element_type_is_integral>;

TYPED_TEST_SUITE(MatrixVectorProductIntegerTest, integral_matrix_types, );

/// @test Multipying with a non-zero product and an exactly negative offset cancels out to zero
///
/// This test only works for integral element types because they don't suffer from rounding: the order of operations is
/// different between the overload with an offset and the overload without an offset, so they can round differently.
ARENE_TYPED_PROPERTY(
    MatrixVectorProductIntegerTest,
    OffsetIsAddedToMultiplicationResult,
    (test_array<TypeParam> in_matrix,
     test_array<in_vector_type<TypeParam>> in_vector,
     test_array<out_vector_type<TypeParam>> out_vector)
) {
  test_array<out_vector_type<TypeParam>> offset_vector{};

  // First, save the result of the no-offset product as offset_vector and then negate it.
  linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), offset_vector.mdspan());
  offset_vector.negate();

  // Now multiplying using this offset should yield a zero vector (i.e. the offset cancels out the product).

  linalg::matrix_vector_product(in_matrix.mdspan(), in_vector.mdspan(), offset_vector.mdspan(), out_vector.mdspan());

  test_array<out_vector_type<TypeParam>> zero_vector{};
  ARENE_PROP_ASSERT(linalg::equal_elements(out_vector.mdspan(), zero_vector.mdspan()));
}

}  // namespace
