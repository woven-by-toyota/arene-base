// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/identity_matrix.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/arithmetic_identities.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// IWYU pragma: no_include "arene/base/mdspan/mdspan.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::dynamic_extent;
using arene::base::extents;
using arene::base::linalg_testing::unqualified_linalg_matrix_types;
using arene::base::linalg_testing::unqualified_linalg_object_types;
using arene::base::type_lists::filter_t;

/// @brief A set of square extents of the given type; if either extent is static, both are set to it, otherwise to 10
/// @{
template <typename Extents>
constexpr Extents square_extents{};

template <typename IndexType, std::size_t Height>
constexpr extents<IndexType, Height, dynamic_extent> square_extents<extents<IndexType, Height, dynamic_extent>>{Height};

template <typename IndexType, std::size_t Width>
constexpr extents<IndexType, dynamic_extent, Width> square_extents<extents<IndexType, dynamic_extent, Width>>{Width};

template <typename IndexType>
constexpr extents<IndexType, dynamic_extent, dynamic_extent>
    square_extents<extents<IndexType, dynamic_extent, dynamic_extent>>{10U, 10U};
/// @}

template <typename T>
class IdentityMatrixInvocabilityTest : public ::testing::Test {};

TYPED_TEST_SUITE(IdentityMatrixInvocabilityTest, unqualified_linalg_object_types, );

/// @test @c identity_matrix is invocable if and only if the extents type is rank 2 and the extents may be equal
TYPED_TEST(IdentityMatrixInvocabilityTest, InvocableForPossiblyInBoundsIndexTypes) {
  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  using element_type = typename TypeParam::element_type;
  using identity_matrix_fn = decltype(linalg::identity_matrix<element_type>);

  constexpr bool possibly_square{linalg::possibly_square_v<TypeParam>};

  EXPECT_EQ((is_invocable_v<identity_matrix_fn, typename TypeParam::extents_type>), possibly_square);
  EXPECT_EQ((is_nothrow_invocable_v<identity_matrix_fn, typename TypeParam::extents_type>), possibly_square);
}

/// @test The mandate that the argument type is an @c extents can be executed successfully for all extents types
TYPED_TEST(IdentityMatrixInvocabilityTest, ManuallyExecuteExtentsMandate) {
  // This needs to be explicitly executed at run time to get test coverage for it, since it would normally only be
  // executed at compile time during the constraint check.
  EXPECT_TRUE(linalg::identity_matrix_detail::mandate_is_extents<typename TypeParam::extents_type>());
}

template <typename T>
class IdentityMatrixTest : public ::testing::Test {};

using square_matrix_types = filter_t<unqualified_linalg_matrix_types, linalg::possibly_square>;

TYPED_TEST_SUITE(IdentityMatrixTest, square_matrix_types, );

/// @test The matrix returned by @c identity_matrix has the same dimensions as the passed extents
TYPED_TEST(IdentityMatrixTest, ReturnedMatrixHasSameDimensions) {
  using element_type = typename TypeParam::element_type;
  using extents_type = typename TypeParam::extents_type;

  auto id_mat = linalg::identity_matrix<element_type>(square_extents<extents_type>);

  EXPECT_EQ(id_mat.extent(0), square_extents<extents_type>.extent(0));
  EXPECT_EQ(id_mat.extent(1), square_extents<extents_type>.extent(1));
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "This test does not perform arithmetic so exact equality is correct");

/// @test The matrix returned by @c identity_matrix has ones on the diagonal and zeros off of it
CONDITIONAL_TYPED_TEST(
    IdentityMatrixTest,
    ReturnedMatrixIsIdentity,
    TypeParam::extents_type::static_extent(0) >= 2 && TypeParam::extents_type::static_extent(1) >= 2
) {
  using arene::base::numbers::one_v;
  using arene::base::numbers::zero_v;

  using element_type = typename TypeParam::element_type;
  using extents_type = typename TypeParam::extents_type;

  auto id_mat = linalg::identity_matrix<element_type>(square_extents<extents_type>);

  EXPECT_EQ(id_mat(0, 0), one_v<element_type>);
  EXPECT_EQ(id_mat(0, 1), zero_v<element_type>);
  EXPECT_EQ(id_mat(1, 0), zero_v<element_type>);
  EXPECT_EQ(id_mat(1, 1), one_v<element_type>);

  // id_mat is square so the overall condition on this test guarantees the extent is at least 2, so last_idx >= 1.
  typename extents_type::index_type const last_idx{id_mat.extent(0) - 1};

  EXPECT_EQ(id_mat(last_idx, 0), zero_v<element_type>);
  EXPECT_EQ(id_mat(last_idx, last_idx - 1), zero_v<element_type>);
  EXPECT_EQ(id_mat(0, last_idx), zero_v<element_type>);
  EXPECT_EQ(id_mat(last_idx - 1, last_idx), zero_v<element_type>);
  EXPECT_EQ(id_mat(last_idx, last_idx), one_v<element_type>);
}

ARENE_IGNORE_END();

}  // namespace
