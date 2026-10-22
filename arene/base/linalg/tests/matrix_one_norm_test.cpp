// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/matrix_one_norm.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/detail/index_permutation_range.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/linalg/row_column_view.hpp"
#include "arene/base/linalg/tests/property_compare.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/linalg/vector_abs_sum.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/ulp_tolerance.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/submdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::dextents;
using arene::base::extents;
using arene::base::identity;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::mdspan;
using arene::base::linalg::matrix_one_norm;
using arene::base::linalg_testing::test_array;

template <class T>
struct LinalgMatrixL1NormTest : ::testing::Test {};

using unqualified_matrix_types = ::testing::Types<
    // Mdspans that are input matrices only, not writeable
    mdspan<float const, extents<std::size_t, 2, 3>>,
    mdspan<float const, extents<std::size_t, 4, 2>, arene::base::layout_left>,
    mdspan<double const, extents<std::size_t, 2, 3>>,
    mdspan<double const, extents<std::size_t, 4, 2>, arene::base::layout_left>,
    mdspan<
        double,
        extents<std::size_t, 4, 2>,
        arene::base::layout_right,
        arene::base::linalg_testing::doubled_accessor<double>>,
    // Mdspans that are input/output matrices
    mdspan<float, extents<std::size_t, 2, 3>>,
    mdspan<float, extents<std::size_t, 4, 2>, arene::base::layout_left>,
    mdspan<double, extents<std::size_t, 2, 3>>,
    mdspan<double, extents<std::size_t, 4, 2>, arene::base::layout_left>>;

TYPED_TEST_SUITE(LinalgMatrixL1NormTest, unqualified_matrix_types, );

template <class T>
struct LinalgMatrixL1NormValueTypeTest : ::testing::Test {};

using value_types = ::testing::Types<int, float, double>;

TYPED_TEST_SUITE(LinalgMatrixL1NormValueTypeTest, value_types, );

template <class T>
using matrix = mdspan<T, dextents<std::size_t, 2>>;

/// @test @c matrix_one_norm is @c noexcept(false) if @c Norm::operator() is @c noexcept(false)
TEST(LinalgMatrixL1NormTest, CanThrowIfNormCanThrow) {
  EXPECT_TRUE((is_invocable_v<decltype(matrix_one_norm), matrix<int>>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(matrix_one_norm), matrix<int>>));

  auto const throwing_norm = [](int) noexcept(false) -> int { return {}; };

  EXPECT_TRUE((is_invocable_v<decltype(matrix_one_norm), matrix<int>, int, decltype(throwing_norm)>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(matrix_one_norm), matrix<int>, int, decltype(throwing_norm)>));
}

/// @test @c matrix_one_norm is @c noexcept(false) if the accumulation of the
/// norm result type is @c noexcept(false)
TEST(LinalgMatrixL1NormTest, CanThrowIfPlusCanThrow) {
  using testing::configurable_value;
  using testing::throws_on;
  using nothrow_scalar = configurable_value<int, throws_on::nothing>;

  EXPECT_TRUE((is_invocable_v<decltype(matrix_one_norm), matrix<nothrow_scalar>, nothrow_scalar, identity>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(matrix_one_norm), matrix<nothrow_scalar>, nothrow_scalar, identity>));

  using throws_on_plus = configurable_value<int, throws_on::plus>;

  EXPECT_TRUE((is_invocable_v<decltype(matrix_one_norm), matrix<throws_on_plus>, throws_on_plus, identity>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(matrix_one_norm), matrix<throws_on_plus>, throws_on_plus, identity>));

  using throws_on_assign = configurable_value<int, throws_on::move_assign>;

  EXPECT_TRUE((is_invocable_v<decltype(matrix_one_norm), matrix<throws_on_assign>, throws_on_assign, identity>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(matrix_one_norm), matrix<throws_on_assign>, throws_on_assign, identity>)
  );
}

/// @test @c matrix_one_norm is @c noexcept(false) if <c>operator<</c> of the
/// Scalar type is @c noexcept(false)
TEST(LinalgMatrixL1NormTest, CanThrowIfOperatorLessCanThrow) {
  using testing::configurable_value;
  using testing::throws_on;
  using nothrow_scalar = configurable_value<int, throws_on::nothing>;

  EXPECT_TRUE((is_invocable_v<decltype(matrix_one_norm), matrix<nothrow_scalar>, nothrow_scalar, identity>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(matrix_one_norm), matrix<nothrow_scalar>, nothrow_scalar, identity>));

  using throws_on_less = configurable_value<int, throws_on::less>;

  EXPECT_TRUE((is_invocable_v<decltype(matrix_one_norm), matrix<throws_on_less>, throws_on_less, identity>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(matrix_one_norm), matrix<throws_on_less>, throws_on_less, identity>));
}

/// @test @c matrix_one_norm of a matrix with zero columns returns the initial value
TYPED_TEST(LinalgMatrixL1NormTest, EmptyMatrixReturnsInit) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL(
      "-Wfloat-equal",
      "Checking exact equality is feasible with these values as these test cases perform no arithmetic."
  );

  using value_type = typename TypeParam::value_type;
  using arene::base::full_extent;
  using arene::base::submdspan;

  auto const mat = TypeParam{nullptr};

  auto const zero_rows = submdspan(mat, std::make_pair(0, 0), full_extent);

  ASSERT_EQ(value_type{}, matrix_one_norm(zero_rows, value_type{}));
  ASSERT_EQ(value_type{42}, matrix_one_norm(zero_rows, value_type{42}));

  auto const zero_cols = submdspan(mat, full_extent, std::make_pair(0, 0));

  ASSERT_EQ(value_type{}, matrix_one_norm(zero_cols, value_type{}));
  ASSERT_EQ(value_type{42}, matrix_one_norm(zero_cols, value_type{42}));

  auto const zero_rows_and_cols = submdspan(mat, std::make_pair(0, 0), std::make_pair(0, 0));

  ASSERT_EQ(value_type{}, matrix_one_norm(zero_rows_and_cols, value_type{}));
  ASSERT_EQ(value_type{42}, matrix_one_norm(zero_rows_and_cols, value_type{42}));

  ARENE_IGNORE_END();
}

// NOLINTBEGIN(readability-identifier-length,hicpp-*)

/// @brief invoke @c matrix_one_norm on a rank-2 literal array
/// @cond
constexpr struct {
  template <class T, std::size_t R, std::size_t C>
  constexpr auto operator()(T const (&values)[R][C]) const {
    // pointer provenance requires us to copy all values to a single buffer
    // during constexpr invocation
    auto buf = arene::base::array<T, R * C>{};
    auto mat = mdspan<T, extents<std::size_t, R, C>>{buf.data()};

    for (std::size_t r{}; r != R; ++r) {
      for (std::size_t c{}; c != C; ++c) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        mat(r, c) = values[r][c];
      }
    }

    return matrix_one_norm(mat);
  }
} matrix_one_norm_result{};
/// @endcond

/// @test @c matrix_one_norm returns the maximum absolute column sum with a few
/// hard-coded examples
CONSTEXPR_TYPED_TEST(LinalgMatrixL1NormValueTypeTest, MaximumAbsoluteColumnSumWithExamples) {
  using T = TypeParam;

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL(
      "-Wfloat-equal",
      "Checking exact equality is feasible with small matrices as we do not exceed the floating point precision budget."
  );

  // positive values
  CONSTEXPR_ASSERT_EQ(
      T{6},
      matrix_one_norm_result(  //
          {
              {T{1}, T{2}},  //
              {T{3}, T{4}}   //
          }
      )
  );

  // negative values
  CONSTEXPR_ASSERT_EQ(
      T{6},
      matrix_one_norm_result(  //
          {
              {-T{1}, -T{2}},  //
              {-T{3}, -T{4}}   //
          }
      )
  );

  // single column
  CONSTEXPR_ASSERT_EQ(
      T{7},
      matrix_one_norm_result(  //
          {
              {T{3}},  //
              {T{4}}   //
          }
      )
  );

  // single row
  CONSTEXPR_ASSERT_EQ(
      T{5},
      matrix_one_norm_result(  //
          {
              {T{1}, -T{5}, T{3}}  //
          }
      )
  );

  ARENE_IGNORE_END();
}

/// @test @c matrix_one_norm returns the maximum absolute column sum using generated data
ARENE_TYPED_PROPERTY(LinalgMatrixL1NormTest, MaximumAbsoluteColumnSumWithProperty, (test_array<TypeParam> test_mat)) {
  auto mat = test_mat.mdspan();

  auto const result = matrix_one_norm(mat);
  ARENE_PROP_PRE(!arene::base::isnan(result));

  auto match_count = std::size_t{};

  for (auto col : arene::base::linalg::column_view(mat)) {
    auto const abs_col_sum = arene::base::linalg::vector_abs_sum(col);
    ARENE_PROP_ASSERT(abs_col_sum <= result);

    ARENE_IGNORE_START();
    ARENE_IGNORE_ALL(
        "-Wfloat-equal",
        "Checking exact equality is feasible as we do not expect to exceed the floating point precision budget. The "
        "implementation of 'matrix_one_norm' is not *required* to use 'vector_abs_sum' but if it does not, then it is "
        "probably a bad implementation."
    );

    if (abs_col_sum == result) {
      ++match_count;
    }

    ARENE_IGNORE_END();
  }

  ARENE_PROP_ASSERT(match_count > 0U);
}

/// @test @c matrix_one_norm adds the initial value to the result
ARENE_TYPED_PROPERTY(
    LinalgMatrixL1NormTest,
    AddsInitialValue,
    (typename TypeParam::value_type init, test_array<TypeParam> test_mat)
) {
  auto mat = test_mat.mdspan();

  auto const norm_without_init = matrix_one_norm(mat);
  auto const norm_with_init = matrix_one_norm(mat, init);

  auto const norm_plus_init = norm_without_init + init;

  if (arene::base::isnan(norm_plus_init)) {
    ARENE_PROP_ASSERT(arene::base::isnan(norm_with_init));
  } else {
    using arene::base::ulp_tolerance;

    constexpr auto tol = sizeof(norm_plus_init) == sizeof(float) ? 2U : 1U;

    ARENE_PROP_LINALG_ASSERT_COMPARE(ulp_tolerance(tol), norm_plus_init, norm_with_init);
  }
}

/// @test @c matrix_one_norm returns NaN if any element is NaN
ARENE_TYPED_PROPERTY(LinalgMatrixL1NormTest, ReturnsNanIfAnyElementIsNan, (test_array<TypeParam> test_mat)) {
  constexpr auto nan = std::numeric_limits<typename TypeParam::value_type>::quiet_NaN();

  using ::arene::base::detail::make_index_permutation_range;
  auto const extents_array = arene::base::mdspan_detail::to_array(test_mat.mdspan().extents());

  for (auto const& indices : make_index_permutation_range(extents_array)) {
    auto buffer = test_mat.buffer();
    auto mat = TypeParam{buffer.data(), test_mat.mdspan().mapping(), test_mat.mdspan().accessor()};

    auto index = arene::base::apply(test_mat.mdspan().mapping(), indices);
    buffer[index] = nan;

    ARENE_PROP_ASSERT(arene::base::isnan(matrix_one_norm(mat)));
  }
}

// NOLINTEND(readability-identifier-length,hicpp-*)

}  // namespace
