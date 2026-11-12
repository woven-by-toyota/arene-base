// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

/// @file add_test.cpp
/// Provides tests for linalg::add. Additionally, used as a template with
/// expand_template to provide tests for linalg::subtract.
///

#include "arene/base/linalg/add.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/linalg/elementwise_binary_transform.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/tuple/detail/tuple_transform.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @test Ensure cannot invoke @c add if the span arguments are not mdspans
CONSTEXPR_TEST(LinalgAdd, CannotInvokeWithNonMdspans) {
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::add), int, int, int>));
}

/// @test Ensure can invoke @c add for matching rank-0 mdspans
CONSTEXPR_TEST(LinalgAdd, CanInvokeWithRank0Mdspans) {
  using span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t>>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::add), span_t, span_t, span_t>));
}

/// @test Ensure can invoke @c add for matching rank-1 mdspans
CONSTEXPR_TEST(LinalgAdd, CanInvokeWithRank1Mdspans) {
  using span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 4>>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::add), span_t, span_t, span_t>));
}

/// @test Ensure can invoke @c add for matching rank-2 mdspans
CONSTEXPR_TEST(LinalgAdd, CanInvokeWithRank2Mdspans) {
  using span_t = arene::base::mdspan<float, arene::base::extents<std::size_t, 3, 4>>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::add), span_t, span_t, span_t>));
}

/// @test Ensure can invoke @c add for matching rank-3 mdspans
CONSTEXPR_TEST(LinalgAdd, CanInvokeWithRank3Mdspans) {
  using span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::add), span_t, span_t, span_t>));
}

/// @test Ensure cannot invoke @c add if the span extents are not matching
CONSTEXPR_TEST(LinalgAdd, CannotInvokeWithMismatchedExtents) {
  using span1_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;
  using span2_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 7, 2, 4>>;

  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::add), span2_t, span1_t, span1_t>));
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::add), span1_t, span2_t, span1_t>));
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::add), span1_t, span1_t, span2_t>));
}

/// @test Ensure cannot invoke @c add when the result of @c std::plus<> cannot be assigned to the output reference
CONSTEXPR_TEST(LinalgAdd, CannotInvokeWithUnassignableOutput) {
  struct local {};
  using in_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 4>>;
  using out_span_t = arene::base::mdspan<local, arene::base::extents<std::size_t, 4>>;

  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::add), in_span_t, in_span_t, out_span_t>));
}

/// @test @c add applies @c std::plus<> elementwise on a rank-3 mdspan
CONSTEXPR_TEST(LinalgAdd, AppliesStdPlusElementwiseRank3) {
  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 1, 2, 3>>;
  using out_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;

  arene::base::array<std::int32_t, 6U> const lhs{1, 3, 7, 42, -100, -200};
  arene::base::array<std::int32_t, 6U> const rhs{100, 200, 300, 400, -500, 600};
  arene::base::array<std::int32_t, 6U> out{};

  linalg::add(in_span_t{lhs.data()}, in_span_t{rhs.data()}, out_span_t{out.data()});

  auto const expected = arene::base::tuple_detail::tuple_transform_to_array<>(lhs, rhs, std::plus<>{});
  CONSTEXPR_ASSERT_EQ(out, expected);
}

/// @test @c add over rank-1 mdspans yields the elementwise sum
CONSTEXPR_TEST(LinalgAdd, AppliesStdPlusElementwiseRank1) {
  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 4>>;
  using out_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 4>>;

  arene::base::array<std::int32_t, 4U> const lhs{1, 2, 3, 4};
  arene::base::array<std::int32_t, 4U> const rhs{10, 20, 30, 40};
  arene::base::array<std::int32_t, 4U> out{};

  linalg::add(in_span_t{lhs.data()}, in_span_t{rhs.data()}, out_span_t{out.data()});

  auto const expected = arene::base::tuple_detail::tuple_transform_to_array<>(lhs, rhs, std::plus<>{});
  CONSTEXPR_ASSERT_EQ(out, expected);
}

/// @test @c add over rank-2 mdspans yields the elementwise sum
CONSTEXPR_TEST(LinalgAdd, AppliesStdPlusElementwiseRank2) {
  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 2, 3>>;
  using out_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 2, 3>>;

  arene::base::array<std::int32_t, 6U> const lhs{1, 2, 3, 4, 5, 6};
  arene::base::array<std::int32_t, 6U> const rhs{10, 20, 30, 40, 50, 60};
  arene::base::array<std::int32_t, 6U> out{};

  linalg::add(in_span_t{lhs.data()}, in_span_t{rhs.data()}, out_span_t{out.data()});

  auto const expected = arene::base::tuple_detail::tuple_transform_to_array<>(lhs, rhs, std::plus<>{});
  CONSTEXPR_ASSERT_EQ(out, expected);
}

/// @test In-place add: @c add(x, y, y) writes the sum into @c y
CONSTEXPR_TEST(LinalgAdd, InPlaceAddWritesIntoY) {
  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 4>>;
  using out_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 4>>;

  arene::base::array<std::int32_t, 4U> const lhs{1, 2, 3, 4};
  arene::base::array<std::int32_t, 4U> rhs{10, 20, 30, 40};
  auto const expected = arene::base::tuple_detail::tuple_transform_to_array<>(lhs, rhs, std::plus<>{});

  linalg::add(in_span_t{lhs.data()}, in_span_t{rhs.data()}, out_span_t{rhs.data()});

  CONSTEXPR_ASSERT_EQ(rhs, expected);
}

/// @test @c add(m, m, m) doubles each element of @c m
CONSTEXPR_TEST(LinalgAdd, FullyAliasedAddDoublesElements) {
  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 2, 2>>;
  using out_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 2, 2>>;

  arene::base::array<std::int32_t, 4U> data{1, 2, 3, 4};
  auto const expected = arene::base::tuple_detail::tuple_transform_to_array<>(data, data, std::plus<>{});

  linalg::add(in_span_t{data.data()}, in_span_t{data.data()}, out_span_t{data.data()});

  CONSTEXPR_ASSERT_EQ(data, expected);
}

/// @brief Property-based add test parameterized over a few representative inout-vector mdspan types
template <typename T>
class LinalgAddPropertyTest : public ::testing::Test {};

using add_property_vector_types = ::testing::Types<
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<float, arene::base::extents<std::size_t, 16>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>>;

TYPED_TEST_SUITE(LinalgAddPropertyTest, add_property_vector_types, );

/// @test @c add over randomly generated rank-1 mdspans yields the elementwise sum
ARENE_TYPED_PROPERTY(
    LinalgAddPropertyTest,
    RandomInputsMatchElementwiseSum,
    (test_array<TypeParam> x_arr, test_array<TypeParam> y_arr)
) {
  test_array<TypeParam> z_arr{};

  TypeParam const x_vec = x_arr;
  TypeParam const y_vec = y_arr;
  TypeParam const z_vec = z_arr;

  linalg::add(x_vec, y_vec, z_vec);

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL(
      "-Wfloat-equal",
      "The two implementations should perform the exact same operations, so the results should be the same."
  );
  for (std::size_t i{}; i < z_vec.extent(0); ++i) {
    ARENE_PROP_ASSERT(z_vec(i) == static_cast<typename TypeParam::value_type>(std::plus<>{}(x_vec(i), y_vec(i))));
  }
  ARENE_IGNORE_END();
}

/// @test @c noexcept-ness of @c add follows the noexcept-ness of the underlying assignment
CONSTEXPR_TEST(LinalgAdd, NoexceptFollowsUnderlyingOps) {
  using non_throwing = ::testing::configurable_value<std::int32_t>;
  using throwing_assign = ::testing::configurable_value<std::int32_t, ::testing::throws_on::move_assign>;

  using span_t = arene::base::mdspan<non_throwing, arene::base::extents<std::size_t, 1, 2, 3>>;
  CONSTEXPR_ASSERT((arene::base::is_nothrow_invocable_v<decltype(linalg::add), span_t, span_t, span_t>));

  using throwing_span_t = arene::base::mdspan<throwing_assign, arene::base::extents<std::size_t, 1, 2, 3>>;
  CONSTEXPR_ASSERT(
      (arene::base::is_invocable_v<decltype(linalg::add), throwing_span_t, throwing_span_t, throwing_span_t>)
  );
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_nothrow_invocable_v<decltype(linalg::add), throwing_span_t, throwing_span_t, throwing_span_t>)
  );
}

/// @brief Functor that adds two values and casts the result to @c Out
template <typename Out>
struct plus_cast {
  template <typename A, typename B>
  constexpr auto operator()(A const& first, B const& second) const noexcept -> Out {
    return static_cast<Out>(std::plus<>{}(first, second));
  }
};

/// @test @c add widens the @c std::plus<> result when the output element type is wider than the inputs
CONSTEXPR_TEST(LinalgAdd, WideningConversionToOutputElement) {
  using in_span_t = arene::base::mdspan<std::int16_t const, arene::base::extents<std::size_t, 4>>;
  using out_span_t = arene::base::mdspan<std::int64_t, arene::base::extents<std::size_t, 4>>;

  arene::base::array<std::int16_t, 4U> const lhs{1, 2, 3, 4};
  arene::base::array<std::int16_t, 4U> const rhs{10, 20, 30, 40};
  arene::base::array<std::int64_t, 4U> out{};

  linalg::add(in_span_t{lhs.data()}, in_span_t{rhs.data()}, out_span_t{out.data()});

  auto const expected =
      arene::base::tuple_detail::tuple_transform_to_array<std::int64_t>(lhs, rhs, plus_cast<std::int64_t>{});
  CONSTEXPR_ASSERT_EQ(out, expected);
}

/// @test @c noexcept-ness of @c add follows the noexcept-ness of @c std::plus on the element type
CONSTEXPR_TEST(LinalgAdd, NoexceptFollowsUnderlyingPlus) {
  using throwing_plus = ::testing::configurable_value<std::int32_t, ::testing::throws_on::plus>;

  using throwing_span_t = arene::base::mdspan<throwing_plus, arene::base::extents<std::size_t, 1, 2, 3>>;
  CONSTEXPR_ASSERT(
      (arene::base::is_invocable_v<decltype(linalg::add), throwing_span_t, throwing_span_t, throwing_span_t>)
  );
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_nothrow_invocable_v<decltype(linalg::add), throwing_span_t, throwing_span_t, throwing_span_t>)
  );
}

}  // namespace
