// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/diagonal_tags.hpp"

#include <gtest/gtest.h>

#include "arene/base/linalg/triangular_tags.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_only_explicitly_constructible.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace {

using unrelated_mdspan = arene::base::mdspan<int, arene::base::extents<int>>;

/// @test Given the diagonal storage tags, when their trait is queried, then it identifies both tags.
CONSTEXPR_TEST(DiagonalTagsTest, TraitIdentifiesDiagonalStorageTags) {
  CONSTEXPR_ASSERT(arene::base::linalg::is_diagonal_storage_tag_v<arene::base::linalg::implicit_unit_diagonal_t>);
  CONSTEXPR_ASSERT(arene::base::linalg::is_diagonal_storage_tag_v<arene::base::linalg::explicit_diagonal_t>);
}

/// @test Given cv-qualified diagonal tags, when the trait is queried, then it identifies every tag.
CONSTEXPR_TEST(DiagonalTagsTest, TraitIdentifiesCvQualifiedDiagonalStorageTags) {
  CONSTEXPR_ASSERT(arene::base::linalg::is_diagonal_storage_tag_v<arene::base::linalg::implicit_unit_diagonal_t const>);
  CONSTEXPR_ASSERT(arene::base::linalg::is_diagonal_storage_tag_v<arene::base::linalg::explicit_diagonal_t volatile>);
  CONSTEXPR_ASSERT(arene::base::linalg::is_diagonal_storage_tag_v<
                   arene::base::linalg::implicit_unit_diagonal_t const volatile>);
}

/// @test Given non-diagonal and reference-qualified types, when queried, then the trait rejects every type.
CONSTEXPR_TEST(DiagonalTagsTest, TraitRejectsOtherAndReferenceQualifiedTypes) {
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::is_diagonal_storage_tag_v<arene::base::linalg::upper_triangle_t>);
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::is_diagonal_storage_tag_v<arene::base::linalg::lower_triangle_t>);
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::is_diagonal_storage_tag_v<
                         arene::base::linalg::implicit_unit_diagonal_t&>);
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::is_diagonal_storage_tag_v<arene::base::linalg::explicit_diagonal_t&&>);
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::is_diagonal_storage_tag_v<int>);
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::is_diagonal_storage_tag_v<void>);
  CONSTEXPR_ASSERT_FALSE(arene::base::linalg::is_diagonal_storage_tag_v<unrelated_mdspan>);
}

/// @test The diagonal constants have the corresponding tag types.
CONSTEXPR_TEST(DiagonalTagsTest, ConstantsHaveExpectedTypes) {
  CONSTEXPR_ASSERT((std::is_same<
                    arene::base::remove_cvref_t<decltype(arene::base::linalg::implicit_unit_diagonal)>,
                    arene::base::linalg::implicit_unit_diagonal_t>::value));
  CONSTEXPR_ASSERT((std::is_same<
                    arene::base::remove_cvref_t<decltype(arene::base::linalg::explicit_diagonal)>,
                    arene::base::linalg::explicit_diagonal_t>::value));
}

/// @test Default construction of the diagonal tag types is explicit.
CONSTEXPR_TEST(DiagonalTagsTest, TagTypesAreOnlyExplicitlyDefaultConstructible) {
  CONSTEXPR_ASSERT(arene::base::is_only_explicitly_constructible_v<arene::base::linalg::implicit_unit_diagonal_t>);
  CONSTEXPR_ASSERT(arene::base::is_only_explicitly_constructible_v<arene::base::linalg::explicit_diagonal_t>);
}

}  // namespace
