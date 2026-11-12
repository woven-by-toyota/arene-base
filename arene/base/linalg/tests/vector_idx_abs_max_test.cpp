// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/vector_idx_abs_max.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/complex_norm.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::array;
using arene::base::dextents;
using arene::base::extents;
using arene::base::identity;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::mdspan;
using arene::base::linalg::vector_idx_abs_max;
using arene::base::linalg_testing::test_array;

template <class T>
struct LinalgVectorAbsIdxMaxTest : ::testing::Test {};

using unqualified_vector_types = ::testing::Types<
    // Mdspans that are input vectors only but not writeable
    arene::base::mdspan<double const, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<std::uint32_t const, arene::base::extents<std::size_t, 4>, arene::base::layout_left>,
    arene::base::mdspan<
        std::int32_t,
        arene::base::extents<std::size_t, 1>,
        arene::base::layout_right,
        arene::base::linalg_testing::doubled_accessor<std::int32_t>>,
    // Mdspans that are input/output vectors
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3>>,
    arene::base::mdspan<double, arene::base::extents<std::size_t, 1>>,
    arene::base::mdspan<std::uint32_t, arene::base::extents<std::size_t, 4>, arene::base::layout_left>>;

TYPED_TEST_SUITE(LinalgVectorAbsIdxMaxTest, unqualified_vector_types, );

/// @test @c vector_idx_abs_max is @c noexcept(false) if @c Norm::operator() is @c noexcept(false)
TEST(LinalgVectorAbsIdxMaxTest, CanThrowIfNormCanThrow) {
  using vector = mdspan<int, extents<std::size_t, 1>>;

  ASSERT_TRUE((is_invocable_v<decltype(vector_idx_abs_max), vector, identity>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(vector_idx_abs_max), vector, identity>));

  auto const throwing_norm = [](int) noexcept(false) -> int { return {}; };

  ASSERT_TRUE((is_invocable_v<decltype(vector_idx_abs_max), vector, decltype(throwing_norm)>));
  ASSERT_FALSE((is_nothrow_invocable_v<decltype(vector_idx_abs_max), vector, decltype(throwing_norm)>));
}

/// @test @c vector_idx_abs_max is @c noexcept(false) if <c>operator<</c> of the
/// norm result type is @c noexcept(false)
TEST(LinalgVectorAbsIdxMaxTest, CanThrowIfOperatorLessCanThrow) {
  using int_vector = mdspan<int, extents<std::size_t, 1>>;

  ASSERT_TRUE((is_invocable_v<decltype(vector_idx_abs_max), int_vector, identity>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(vector_idx_abs_max), int_vector, identity>));

  using testing::configurable_value;
  using testing::throws_on;
  using configurable_nothrow_vector = mdspan<configurable_value<int, throws_on::nothing>, extents<std::size_t, 1>>;

  ASSERT_TRUE((is_invocable_v<decltype(vector_idx_abs_max), configurable_nothrow_vector, identity>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(vector_idx_abs_max), configurable_nothrow_vector, identity>));

  using configurable_throwing_vector = mdspan<configurable_value<int, throws_on::less>, extents<std::size_t, 1>>;

  ASSERT_TRUE((is_invocable_v<decltype(vector_idx_abs_max), configurable_throwing_vector, identity>));
  ASSERT_FALSE((is_nothrow_invocable_v<decltype(vector_idx_abs_max), configurable_throwing_vector, identity>));
}

/// @test @c vector_idx_abs_max returns a sentinel value if input vector is empty
TEST(LinalgVectorAbsIdxMaxTest, EmptyVectorReturnsSentinelIndex) {
  auto data1 = int{};
  auto const non_empty_vec1 = mdspan<int, dextents<std::size_t, 1>>{&data1, array<int, 1>{1}};
  ASSERT_NE(static_cast<std::size_t>(-1), vector_idx_abs_max(non_empty_vec1));

  auto const empty_vec1 = mdspan<int, dextents<std::size_t, 1>>{nullptr, array<int, 1>{}};
  ASSERT_EQ(static_cast<std::size_t>(-1), vector_idx_abs_max(empty_vec1));

  auto data2 = int{};
  auto const non_empty_vec2 = mdspan<int, dextents<std::uint8_t, 1>>{&data2, array<int, 1>{1}};
  ASSERT_NE(std::uint8_t{255U}, vector_idx_abs_max(non_empty_vec2));

  auto const empty_vec2 = mdspan<int, dextents<std::uint8_t, 1>>{nullptr, array<int, 1>{}};
  ASSERT_EQ(std::uint8_t{255U}, vector_idx_abs_max(empty_vec2));
}

/// @test @c vector_idx_abs_max always returns index 0 for a vector with one element
ARENE_TYPED_PROPERTY(LinalgVectorAbsIdxMaxTest, AlwayFirstIfVectorHasOneElement, (test_array<TypeParam> test_vec)) {
  if (1U != test_vec.mdspan().extents().extent(0)) {
    GTEST_SKIP();
  }

  ARENE_PROP_ASSERT(0U == vector_idx_abs_max(test_vec.mdspan()));
}

// NOLINTBEGIN(readability-identifier-length)

/// @test for a vector with no NaN elements, let i be the index returned by @c
/// vector_idx_abs_max and j be any other valid index. Then norm(vec(i)) >
/// norm(vec(j)) for i != j or norm(vec(i)) == norm(vec(j)) and i < j.
ARENE_TYPED_PROPERTY(LinalgVectorAbsIdxMaxTest, IndexIsAlwaysLargestForNonNaN, (test_array<TypeParam> test_vec)) {
  auto vec = test_vec.mdspan();

  auto const i = vector_idx_abs_max(vec);
  auto const norm = arene::base::complex_l1_norm;

  for (std::size_t j{}; j != vec.extents().extent(0); ++j) {
    if (i != j) {
      ARENE_IGNORE_START();
      ARENE_IGNORE_ALL(
          "-Wfloat-equal",
          "Checking exact equality is required for verifying properties of a strict weak ordering"
      );

      ARENE_PROP_ASSERT((norm(vec(i)) > norm(vec(j))) || ((norm(vec(i)) == norm(vec(j))) && (i < j)));

      ARENE_IGNORE_END();
    }
  }
}

/// @test for a vector with no NaN elements, let i be the index returned by @c
/// vector_idx_abs_max and j be any other valid index, and let norm be the
/// identity operation. Then norm(vec(i)) > norm(vec(j)) for i != j or
/// norm(vec(i)) == norm(vec(j)) and i < j.
ARENE_TYPED_PROPERTY(
    LinalgVectorAbsIdxMaxTest,
    IndexIsAlwaysLargestForNonNaNWithIdentityNorm,
    (test_array<TypeParam> test_vec)
) {
  auto vec = test_vec.mdspan();

  auto const norm = identity{};
  auto const i = vector_idx_abs_max(vec, norm);

  for (std::size_t j{}; j != vec.extents().extent(0); ++j) {
    if (i != j) {
      ARENE_IGNORE_START();
      ARENE_IGNORE_ALL(
          "-Wfloat-equal",
          "Checking exact equality is required for verifying properties of a strict weak ordering"
      );

      ARENE_PROP_ASSERT((norm(vec(i)) > norm(vec(j))) || ((norm(vec(i)) == norm(vec(j))) && (i < j)));

      ARENE_IGNORE_END();
    }
  }
}

// NOLINTBEGIN(hicpp-*)
/// @cond
constexpr struct {
  template <std::size_t N>
  auto operator()(double const (&values)[N]) const {
    auto vec = mdspan<double const, extents<std::size_t, N>>{values};
    return vector_idx_abs_max(vec);
  }
} vector_idx_abs_max_result{};
/// @endcond
// NOLINTEND(hicpp-*)

/// @test for a vector with NaN elements, @c vector_idx_abs_max returns the
/// index of the first NaN element
TEST(LinalgVectorAbsIdxMaxTest, ReturnsIndexOfFirstNaN) {
  constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
  constexpr auto inf = std::numeric_limits<double>::infinity();

  ASSERT_EQ(0U, vector_idx_abs_max_result({nan, 0., inf, -inf}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({nan, nan, inf, -inf}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({nan, 0., inf, -nan}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({-nan, nan, inf, -nan}));

  ASSERT_EQ(1U, vector_idx_abs_max_result({inf, nan, 0., inf, -inf}));
  ASSERT_EQ(1U, vector_idx_abs_max_result({inf, nan, nan, inf, -inf}));
  ASSERT_EQ(1U, vector_idx_abs_max_result({inf, nan, 0., inf, -nan}));
  ASSERT_EQ(1U, vector_idx_abs_max_result({inf, -nan, nan, inf, -nan}));

  ASSERT_EQ(2U, vector_idx_abs_max_result({1., inf, nan, 0., inf, -inf}));
  ASSERT_EQ(2U, vector_idx_abs_max_result({1., inf, nan, nan, inf, -inf}));
  ASSERT_EQ(2U, vector_idx_abs_max_result({1., inf, nan, 0., inf, -nan}));
  ASSERT_EQ(2U, vector_idx_abs_max_result({1., inf, -nan, nan, inf, -nan}));
}

/// @test for a vector with all equal elements, @c vector_idx_abs_max returns
/// the index of the first element
TEST(LinalgVectorAbsIdxMaxTest, ReturnsIndexOfFirstElementForAllEqualElements) {
  ASSERT_EQ(0U, vector_idx_abs_max_result({0.}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({0., 0.}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({0., 0., 0.}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({0., 0., 0., 0.}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({0., 0., 0., 0.}));
  ASSERT_EQ(0U, vector_idx_abs_max_result({0., 0., 0., 0., 0.}));
}

// NOLINTEND(readability-identifier-length)

}  // namespace
