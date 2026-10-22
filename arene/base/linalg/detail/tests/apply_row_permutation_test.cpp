// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/detail/apply_row_permutation.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/iota.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/linalg/detail/swap_rows_or_elements.hpp"
#include "arene/base/linalg/detail/tests/proxy_accessor.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/math/abs_cpo.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
// IWYU pragma: no_include "arene/base/testing/detail/any_prng_ref.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/utility/swap.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

namespace testing {

/// @brief ADL customization of @c abs for @c configurable_value
template <class T, throws_on ThrowsOn, disable Disable, is_constexpr Constexpr>
constexpr auto abs(configurable_value<T, ThrowsOn, Disable, Constexpr> const& value
) noexcept(noexcept(arene::base::math::abs(value.value))) -> configurable_value<T, ThrowsOn, Disable, Constexpr> {
  return configurable_value<T, ThrowsOn, Disable, Constexpr>{arene::base::math::abs(value.value)};
}

}  // namespace testing

namespace {

namespace linalg = arene::base::linalg;
using arene::base::array;
using arene::base::extents;
using arene::base::mdspan;
using arene::base::linalg_testing::integer_reference_proxy;
using arene::base::linalg_testing::proxy_accessor;
using arene::base::linalg_testing::test_array;
using arene::base::testing::mdspan_transform_element_type_t;
using linalg::detail::apply_row_permutation;
using testing::disable;
using testing::throws_on;

using permute_fn = decltype(apply_row_permutation);

/// @brief Matrix value without a usable norm or ordering
struct non_orderable_value {};

/// @brief Storage for a square permutation matrix whose row @c i has its single one in column <c>row_order[i]</c>
/// @tparam Rows Size of the permutation
/// @tparam Value Element type of the permutation matrix
/// @tparam IndexType Index type of the permutation matrix, which must match that of the object it is applied to
/// @param row_order Original row index placed at each output row
template <std::size_t Rows, typename Value = int, typename IndexType = std::size_t>
constexpr auto make_permutation(array<std::size_t, Rows> const& row_order)
    -> test_array<mdspan<Value, extents<IndexType, Rows, Rows>>> {
  test_array<mdspan<Value, extents<IndexType, Rows, Rows>>> storage{};
  auto const permutation = storage.mdspan();
  for (std::size_t row{}; row < Rows; ++row) {
    permutation(static_cast<IndexType>(row), static_cast<IndexType>(row_order[row])) =
        arene::base::numbers::one_v<Value>;
  }
  return storage;
}

/// @brief Make a random four-row permutation matrix by shuffling the original row order
/// @tparam Prng Type of the property test's random number generator
/// @param prng Random number generator
/// @return A valid four-by-four permutation matrix
template <typename Prng>
auto make_random_permutation(Prng const& prng) -> test_array<mdspan<int, extents<std::size_t, 4, 4>>> {
  array<std::size_t, 4> row_order{{0, 1, 2, 3}};
  for (auto remaining = row_order.size(); remaining > 1U; --remaining) {
    auto const selected = static_cast<std::size_t>(prng() % remaining);
    arene::base::swap(row_order[remaining - 1U], row_order[selected]);
  }
  return make_permutation(row_order);
}

/// @brief Vector type used by the concrete tests below
using vector_type = mdspan<int, extents<std::size_t, 4>>;

template <throws_on ThrowsOn = throws_on::nothing, disable Disable = disable::nothing>
struct to_configurable_value {
  template <typename T>
  using type = ::testing::configurable_value<T, ThrowsOn, Disable>;
};

/// @brief Permutation type matching @c vector_type
using permutation_type = mdspan<int const, extents<std::size_t, 4, 4>>;

/// @test Given permutation inspection and element swaps that may throw, when checking the operation, then its noexcept
/// follows both operations.
TEST(ApplyRowPermutationTest, NoexceptFollowsPermutationInspectionAndElementSwap) {
  using nothrow_object = mdspan_transform_element_type_t<vector_type, to_configurable_value<>::type>;
  using throwing_object = mdspan_transform_element_type_t<vector_type, to_configurable_value<throws_on::swap>::type>;
  using nothrow_permutation = mdspan<testing::configurable_value<int> const, extents<std::size_t, 4, 4>>;
  using throwing_permutation =
      mdspan<testing::configurable_value<int, throws_on::less> const, extents<std::size_t, 4, 4>>;

  EXPECT_TRUE((arene::base::is_nothrow_invocable_v<permute_fn, permutation_type, nothrow_object>));
  EXPECT_TRUE((arene::base::is_invocable_v<permute_fn, permutation_type, throwing_object>));
  EXPECT_FALSE((arene::base::is_nothrow_invocable_v<permute_fn, permutation_type, throwing_object>));
  EXPECT_TRUE((arene::base::is_nothrow_invocable_v<permute_fn, nothrow_permutation, vector_type>));
  EXPECT_TRUE((arene::base::is_invocable_v<permute_fn, throwing_permutation, vector_type>));
  EXPECT_FALSE((arene::base::is_nothrow_invocable_v<permute_fn, throwing_permutation, vector_type>));
}

/// @test Given arguments that cannot form <c>permutation * object</c> or cannot be permuted in place, when checking
/// invocability, then the operation is disabled.
TEST(ApplyRowPermutationTest, NotInvocableForIncompatibleArguments) {
  using arene::base::is_invocable_v;

  // A permutation whose size cannot match the object's row count
  EXPECT_FALSE((is_invocable_v<permute_fn, mdspan<int const, extents<std::size_t, 3, 3>>, vector_type>));
  // A permutation that is not square, so the product would not have the object's shape
  EXPECT_FALSE((is_invocable_v<permute_fn, mdspan<int const, extents<std::size_t, 3, 4>>, vector_type>));
  // A vector in place of the permutation matrix
  EXPECT_FALSE((is_invocable_v<permute_fn, mdspan<int const, extents<std::size_t, 4>>, vector_type>));
  // A read-only object cannot be permuted in place
  EXPECT_FALSE((is_invocable_v<permute_fn, permutation_type, mdspan<int const, extents<std::size_t, 4>>>));
  // Elements that cannot be swapped
  using unswappable_object = mdspan_transform_element_type_t<
      vector_type,
      to_configurable_value<throws_on::nothing, disable::move_construct | disable::move_assign | disable::swap>::type>;
  EXPECT_FALSE((is_invocable_v<permute_fn, permutation_type, unswappable_object>));
}

/// @test Given vector or matrix accessors returning swappable proxies by value, when checking invocability, then the
/// operation is disabled because row swapping requires lvalue references.
TEST(ApplyRowPermutationTest, RejectsProxyReferences) {
  using proxy_vector_type = mdspan<int, extents<std::size_t, 4>, arene::base::layout_right, proxy_accessor>;
  using proxy_matrix_type = mdspan<int, extents<std::size_t, 4, 2>, arene::base::layout_right, proxy_accessor>;
  auto values = array<int, 2>{{1, 2}};
  swap(integer_reference_proxy{values[0]}, integer_reference_proxy{values[1]});

  EXPECT_EQ(values, (array<int, 2>{{2, 1}}));
  EXPECT_TRUE(arene::base::linalg::is_inout_vector_v<proxy_vector_type>);
  EXPECT_TRUE(arene::base::linalg::is_inout_matrix_v<proxy_matrix_type>);
  EXPECT_TRUE(arene::base::is_swappable_v<proxy_vector_type::reference>);
  EXPECT_FALSE((arene::base::is_invocable_v<permute_fn, permutation_type, proxy_vector_type>));
  EXPECT_FALSE((arene::base::is_invocable_v<permute_fn, permutation_type, proxy_matrix_type>));
}

/// @test Given a permutation value without a usable norm, when checking invocability, then the operation is disabled.
TEST(ApplyRowPermutationTest, RejectsPermutationValuesWithoutANorm) {
  using unsupported_permutation = mdspan<non_orderable_value const, extents<std::size_t, 4, 4>>;

  EXPECT_FALSE((arene::base::is_invocable_v<permute_fn, unsupported_permutation, vector_type>));
}

/// @brief Four-element vector holding <c>{1, 2, 3, 4}</c>, so that each element names the row it started in
constexpr test_array<vector_type> counting_vector{array<int, 4>{{1, 2, 3, 4}}};

/// @test Given the identity permutation, when permuting a vector, then the vector is unchanged.
CONSTEXPR_TEST(ApplyRowPermutationTest, IdentityLeavesVectorUnchanged) {
  auto storage = counting_vector;
  auto const identity = make_permutation(array<std::size_t, 4>{{0, 1, 2, 3}});

  apply_row_permutation(identity.mdspan(), storage.mdspan());

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 4>{{1, 2, 3, 4}}));
}

/// @test Given a permutation exchanging two rows, when permuting a vector, then only those two elements move.
CONSTEXPR_TEST(ApplyRowPermutationTest, TranspositionExchangesTwoRows) {
  auto storage = counting_vector;
  auto const transposition = make_permutation(array<std::size_t, 4>{{0, 2, 1, 3}});

  apply_row_permutation(transposition.mdspan(), storage.mdspan());

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 4>{{1, 3, 2, 4}}));
}

/// @test Given a permutation reversing the rows, when permuting a vector, then both disjoint pairs are exchanged.
CONSTEXPR_TEST(ApplyRowPermutationTest, ReversalExchangesDisjointPairs) {
  auto storage = counting_vector;
  auto const reversal = make_permutation(array<std::size_t, 4>{{3, 2, 1, 0}});

  apply_row_permutation(reversal.mdspan(), storage.mdspan());

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 4>{{4, 3, 2, 1}}));
}

/// @test Given a cyclic permutation, when permuting a vector, then every element moves up one row and the first wraps
/// round to the end.
///
/// Each swap displaces the row that the last destination needs, so the implementation has to follow the permutation
/// through several already-finished rows to find where that row currently lives.
CONSTEXPR_TEST(ApplyRowPermutationTest, CycleRotatesEveryRow) {
  auto storage = counting_vector;
  auto const cycle = make_permutation(array<std::size_t, 4>{{1, 2, 3, 0}});

  apply_row_permutation(cycle.mdspan(), storage.mdspan());

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 4>{{2, 3, 4, 1}}));
}

/// @test Given a matrix, when permuting its rows, then each row moves as a unit.
CONSTEXPR_TEST(ApplyRowPermutationTest, MatrixRowsMoveAsUnits) {
  // Rows {1, 2}, {3, 4} and {5, 6}
  test_array<mdspan<int, extents<std::size_t, 3, 2>>> storage{array<int, 6>{{1, 2, 3, 4, 5, 6}}};
  auto const cycle = make_permutation(array<std::size_t, 3>{{1, 2, 0}});

  apply_row_permutation(cycle.mdspan(), storage.mdspan());

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 6>{{3, 4, 5, 6, 1, 2}}));
}

/// @test Given a permutation matrix with a different element type from the object, when permuting, then the rows still
/// move.
CONSTEXPR_TEST(ApplyRowPermutationTest, PermutationElementTypeMayDifferFromObject) {
  auto storage = counting_vector;
  auto const transposition = make_permutation<4, bool>(array<std::size_t, 4>{{0, 2, 1, 3}});

  apply_row_permutation(transposition.mdspan(), storage.mdspan());

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 4>{{1, 3, 2, 4}}));
}

/// @test Given an object with no rows or with no columns, when permuting, then nothing happens and no precondition
/// fails.
CONSTEXPR_TEST(ApplyRowPermutationTest, EmptyObjectsArePermutedTrivially) {
  test_array<mdspan<int, extents<std::size_t, 0, 2>>> no_rows{};
  auto const empty = make_permutation(array<std::size_t, 0>{});
  apply_row_permutation(empty.mdspan(), no_rows.mdspan());

  test_array<mdspan<int, extents<std::size_t, 3, 0>>> no_columns{};
  auto const cycle = make_permutation(array<std::size_t, 3>{{1, 2, 0}});
  apply_row_permutation(cycle.mdspan(), no_columns.mdspan());

  // Evaluating this test at compile time is itself the check: a precondition violation would fail to compile.
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Permuting rows performs no arithmetic, so elements must compare exactly equal");

/// @test Given a vector containing NaN, when permuting, then the NaN moves with its row and no other element changes.
TEST(ApplyRowPermutationTest, NaNMovesWithItsRowWithoutPropagating) {
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  test_array<mdspan<double, extents<std::size_t, 3>>> storage{array<double, 3>{{nan, 1.0, 2.0}}};
  auto const cycle = make_permutation(array<std::size_t, 3>{{1, 2, 0}});

  apply_row_permutation(cycle.mdspan(), storage.mdspan());

  EXPECT_EQ(storage.buffer()[0], 1.0);
  EXPECT_EQ(storage.buffer()[1], 2.0);
  EXPECT_TRUE(arene::base::isnan(storage.buffer()[2]));
}

/// @test Given compatible dynamic dimensions, when applying a cyclic permutation, then vector and matrix rows rotate.
CONSTEXPR_TEST(ApplyRowPermutationTest, DynamicDimensionsAllowCompatiblePermutation) {
  test_array<mdspan<int, extents<std::size_t, 3, 3>>, true> cycle{array<int, 9>{{0, 1, 0, 0, 0, 1, 1, 0, 0}}};
  test_array<mdspan<double, extents<std::size_t, 3>>, true> vector{array<double, 3>{{1.0, 2.0, 3.0}}};
  test_array<mdspan<double, extents<std::size_t, 3, 2>>, true> matrix{array<double, 6>{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}}};

  apply_row_permutation(cycle.mdspan(), vector.mdspan());
  apply_row_permutation(cycle.mdspan(), matrix.mdspan());

  CONSTEXPR_ASSERT_EQ(vector.buffer(), (array<double, 3>{{2.0, 3.0, 1.0}}));
  CONSTEXPR_ASSERT_EQ(matrix.buffer(), (array<double, 6>{{3.0, 4.0, 5.0, 6.0, 1.0, 2.0}}));
}

/// @brief Typed fixture over four-row vectors and matrices with static and dynamic extents, a signed index type and
/// every layout
template <typename T>
class ApplyRowPermutationTypedTest : public ::testing::Test {};

using object_storage_types = ::testing::Types<
    test_array<mdspan<double, extents<std::size_t, 4>>>,
    test_array<mdspan<int, extents<int, 4>>, true>,
    test_array<mdspan<double, extents<std::size_t, 4, 3>>>,
    test_array<mdspan<int, extents<int, 4, 3>>, true>,
    test_array<mdspan<double, extents<std::size_t, 4, 3>, arene::base::layout_left>>,
    test_array<mdspan<double, extents<std::size_t, 4, 3>, arene::base::layout_stride>, true>>;

TYPED_TEST_SUITE(ApplyRowPermutationTypedTest, object_storage_types, );

/// @test Given an object of any shape, when applying a cyclic permutation, then every row moves up one place and the
/// first wraps round to the end.
CONSTEXPR_TYPED_TEST(ApplyRowPermutationTypedTest, CycleRotatesEveryRow) {
  using index_type = typename TypeParam::mdspan_type::index_type;
  // Every element holds a different value so that any misplaced element is detectable
  TypeParam storage{};
  arene::base::iota(storage.buffer().begin(), storage.buffer().end(), 1);
  auto expected = storage;
  auto const cycle = make_permutation<4, int, index_type>({{1, 2, 3, 0}});

  apply_row_permutation(cycle.mdspan(), storage.mdspan());

  // Rotating the rows up by one carries the first row to the end through three adjacent swaps
  linalg::detail::swap_rows_or_elements(expected.mdspan(), 0, 1);
  linalg::detail::swap_rows_or_elements(expected.mdspan(), 1, 2);
  linalg::detail::swap_rows_or_elements(expected.mdspan(), 2, 3);
  CONSTEXPR_ASSERT(linalg::equal_elements(storage.mdspan(), expected.mdspan()));
}

/// @test Given random vector contents and a valid permutation, when permuting, then the result equals the permutation
/// matrix multiplied by the original vector.
ARENE_PROPERTY(ApplyRowPermutationTest, VectorMatchesMatrixProduct, (test_array<vector_type> storage)) {
  auto const original = storage;
  auto const original_vector = original.mdspan();

  auto const permutation = make_random_permutation(prng);
  auto const permutation_matrix = permutation.mdspan();

  auto const vector_to_permute = storage.mdspan();
  test_array<vector_type> expected_storage{};
  auto const expected_vector = expected_storage.mdspan();

  // Note: This could be replaced by matrix_vector_product once available.
  for (std::size_t row{}; row < permutation_matrix.extent(0); ++row) {
    int product{};
    for (std::size_t input_row{}; input_row < permutation_matrix.extent(1); ++input_row) {
      product += permutation_matrix(row, input_row) * original_vector(input_row);
    }
    expected_vector(row) = product;
  }

  apply_row_permutation(permutation_matrix, vector_to_permute);
  ARENE_PROP_ASSERT(linalg::equal_elements(vector_to_permute, expected_vector));
}

/// @test Given random matrix contents and a valid permutation, when permuting, then the result equals the permutation
/// matrix multiplied by the original matrix.
ARENE_PROPERTY(
    ApplyRowPermutationTest,
    MatrixMatchesMatrixProduct,
    (test_array<mdspan<int, extents<std::size_t, 4, 3>>> storage)
) {
  auto const original = storage;
  auto const original_matrix = original.mdspan();

  auto const permutation = make_random_permutation(prng);
  auto const permutation_matrix = permutation.mdspan();

  auto const matrix_to_permute = storage.mdspan();
  test_array<mdspan<int, extents<std::size_t, 4, 3>>> expected_storage{};
  auto const expected_matrix = expected_storage.mdspan();

  // Note: This could be replaced by matrix_matrix_product once available.
  for (std::size_t row{}; row < permutation_matrix.extent(0); ++row) {
    for (std::size_t column{}; column < matrix_to_permute.extent(1); ++column) {
      int product{};
      for (std::size_t input_row{}; input_row < permutation_matrix.extent(1); ++input_row) {
        product += permutation_matrix(row, input_row) * original_matrix(input_row, column);
      }
      expected_matrix(row, column) = product;
    }
  }

  apply_row_permutation(permutation_matrix, matrix_to_permute);
  ARENE_PROP_ASSERT(linalg::equal_elements(matrix_to_permute, expected_matrix));
}

ARENE_IGNORE_END();

template <typename T>
using to_counting_wrapper = ::testing::counting_wrapper<T>;

/// @brief A 4×2 matrix whose elements count their special member function and swap invocations
using counting_matrix_type =
    mdspan_transform_element_type_t<mdspan<int, extents<std::size_t, 4, 2>>, to_counting_wrapper>;
using counting_element = typename counting_matrix_type::value_type;

/// @test Given a permutation exchanging two rows, when permuting, then elements move only through their swap function,
/// once per element of the exchanged rows.
TEST(ApplyRowPermutationTest, RowsMoveOnlyThroughElementSwap) {
  test_array<counting_matrix_type> storage{};
  auto const transposition = make_permutation(array<std::size_t, 4>{{0, 2, 1, 3}});

  counting_element::count = {};
  apply_row_permutation(transposition.mdspan(), storage.mdspan());

  auto expected = decltype(counting_element::count){};
  expected.swap = 2U;  // one swap per column of the two exchanged rows
  EXPECT_EQ(counting_element::count, expected);
}

}  // namespace
