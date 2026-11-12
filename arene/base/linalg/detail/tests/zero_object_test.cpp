// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/detail/zero_object.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/mdspan/constant_accessor.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::linalg::detail::zero_object;
using arene::base::linalg::detail::zero_object_t;

using static_matrix_t = arene::base::mdspan<int, arene::base::extents<std::size_t, 2, 3>>;
using dynamic_matrix_t = arene::base::mdspan<double, arene::base::dextents<std::size_t, 2>>;
using vector_t = arene::base::mdspan<int, arene::base::extents<std::size_t, 4>>;
using rank_0_t = arene::base::mdspan<int, arene::base::extents<std::size_t>>;
using rank_3_t = arene::base::mdspan<int, arene::base::extents<std::size_t, 2, 3, 4>>;

struct throwing_copy_scalar {
  int value{};
  constexpr throwing_copy_scalar() noexcept = default;
  constexpr throwing_copy_scalar(throwing_copy_scalar const&) noexcept(false) {}
  constexpr throwing_copy_scalar(throwing_copy_scalar&&) noexcept = default;
  constexpr auto operator=(throwing_copy_scalar const&) noexcept -> throwing_copy_scalar& = default;
  constexpr auto operator=(throwing_copy_scalar&&) noexcept -> throwing_copy_scalar& = default;
  ~throwing_copy_scalar() = default;
};
using throwing_matrix_t = arene::base::mdspan<throwing_copy_scalar, arene::base::extents<std::size_t, 2, 3>>;

/// @test @c zero_object has the extents of its argument
CONSTEXPR_TEST(ZeroObject, HasRequestedExtents) {
  auto const static_zero = zero_object<static_matrix_t>(static_matrix_t::extents_type{});
  CONSTEXPR_ASSERT_EQ(static_zero.extents(), static_matrix_t::extents_type{});

  auto const dynamic_zero = zero_object<dynamic_matrix_t>(dynamic_matrix_t::extents_type{2U, 5U});
  CONSTEXPR_ASSERT_EQ(dynamic_zero.extent(0), 2U);
  CONSTEXPR_ASSERT_EQ(dynamic_zero.extent(1), 5U);

  auto const rank_0_zero = zero_object<rank_0_t>(rank_0_t::extents_type{});
  CONSTEXPR_ASSERT_EQ(rank_0_zero.extents(), rank_0_t::extents_type{});

  auto const rank_3_zero = zero_object<rank_3_t>(rank_3_t::extents_type{});
  CONSTEXPR_ASSERT_EQ(rank_3_zero.extents(), rank_3_t::extents_type{});
}

/// @test every entry of @c zero_object is zero of the value type of the source object
CONSTEXPR_TEST(ZeroObject, EveryEntryIsZero) {
  auto const int_zero = zero_object<static_matrix_t>(static_matrix_t::extents_type{});
  for (std::size_t row{}; row < 2U; ++row) {
    for (std::size_t col{}; col < 3U; ++col) {
      CONSTEXPR_ASSERT_EQ(int_zero(row, col), 0);
    }
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Checking exact floating-point zero representation");
  auto const double_zero = zero_object<dynamic_matrix_t>(dynamic_matrix_t::extents_type{3U, 2U});
  for (std::size_t row{}; row < 3U; ++row) {
    for (std::size_t col{}; col < 2U; ++col) {
      CONSTEXPR_ASSERT_EQ(double_zero(row, col), 0.0);
    }
  }
  ARENE_IGNORE_END();

  auto const vector_zero = zero_object<vector_t>(vector_t::extents_type{});
  for (std::size_t idx{}; idx < 4U; ++idx) {
    CONSTEXPR_ASSERT_EQ(vector_zero(idx), 0);
  }

  auto const rank_0_zero = zero_object<rank_0_t>(rank_0_t::extents_type{});
  CONSTEXPR_ASSERT_EQ(rank_0_zero(), 0);

  auto const rank_3_zero = zero_object<rank_3_t>(rank_3_t::extents_type{});
  for (std::size_t i{}; i < 2U; ++i) {
    for (std::size_t j{}; j < 3U; ++j) {
      for (std::size_t k{}; k < 4U; ++k) {
        CONSTEXPR_ASSERT_EQ(rank_3_zero(i, j, k), 0);
      }
    }
  }
}

/// @test @c zero_object_t is the type returned by @c zero_object
TEST(ZeroObject, ReturnTypeIsZeroObjectT) {
  ::testing::StaticAssertTypeEq<
      zero_object_t<static_matrix_t>,
      decltype(zero_object<static_matrix_t>(std::declval<static_matrix_t::extents_type const&>()))>();
}

/// @test @c zero_object is @c noexcept when construction is noexcept
TEST(ZeroObject, IsNoexcept) {
  EXPECT_TRUE((noexcept(zero_object<static_matrix_t>(std::declval<static_matrix_t::extents_type const&>()))));
  EXPECT_FALSE((noexcept(zero_object<throwing_matrix_t>(std::declval<throwing_matrix_t::extents_type const&>()))));
}

/// @test entries of @c zero_object are not assignable
TEST(ZeroObject, EntriesAreNotAssignable) {
  using element_ref = typename zero_object_t<static_matrix_t>::reference;
  EXPECT_FALSE((std::is_assignable<element_ref, int>::value));
}

}  // namespace
