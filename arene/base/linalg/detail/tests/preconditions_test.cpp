// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/detail/preconditions.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/cartesian_product.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::dextents;
using arene::base::dynamic_extent;
using arene::base::extents;
using arene::base::mdspan;
using arene::base::linalg::actually_addable;
using arene::base::linalg::actually_comparable;
using arene::base::linalg::actually_multipliable;

/// @brief Default value to use for a dynamic extent
constexpr std::size_t default_dynamic_extent{10U};
/// @brief Value to use for a dynamic extent which should be different from @c default_dynamic_extent
constexpr std::size_t other_dynamic_extent{11U};

/// @brief Make an @c extents object with all of its dynamic ranks set to @c extent_when_dynamic
template <typename Extents, std::size_t... DynamicRanks>
constexpr auto
make_extent_with_dynamic_as(std::size_t extent_when_dynamic, std::index_sequence<DynamicRanks...>) noexcept -> Extents {
  return Extents{(static_cast<void>(DynamicRanks), extent_when_dynamic)...};
}

/// @brief Make an @c mdspan object with all of its dynamic extents set to @c extent_when_dynamic
template <typename Mdspan>
constexpr auto construct_with_dynamic_as(std::size_t extent_when_dynamic) noexcept -> Mdspan {
  auto exts = make_extent_with_dynamic_as<typename Mdspan::extents_type>(
      extent_when_dynamic,
      std::make_index_sequence<Mdspan::extents_type::rank_dynamic()>{}
  );
  typename Mdspan::mapping_type const mapping{exts};
  return Mdspan(nullptr, mapping);
}

/// @brief Get an @c mdspan that's the same as the passed one, but with one extent different (unless it's dynamic)
/// @{
template <typename LinalgObject, std::size_t RankIdx>
struct with_different_extent_impl;

template <typename ElementType, std::size_t FirstExt, typename Layout, typename Accessor>
struct with_different_extent_impl<mdspan<ElementType, extents<std::size_t, FirstExt>, Layout, Accessor>, 0U> {
  static constexpr std::size_t different_extent{(FirstExt == dynamic_extent) ? dynamic_extent : (FirstExt + 1U)};

  using type = mdspan<ElementType, extents<std::size_t, different_extent>, Layout, Accessor>;
};

template <
    typename ElementType,
    std::size_t FirstExt,
    std::size_t SecondExt,
    typename Layout,
    typename Accessor,
    std::size_t RankIdx>
struct with_different_extent_impl<
    mdspan<ElementType, extents<std::size_t, FirstExt, SecondExt>, Layout, Accessor>,
    RankIdx> {
  static constexpr std::size_t output_first{
      ((RankIdx == 1U) || (FirstExt == dynamic_extent)) ? FirstExt : (FirstExt + 1U)
  };
  static constexpr std::size_t output_second{
      ((RankIdx == 0U) || (SecondExt == dynamic_extent)) ? SecondExt : (SecondExt + 1U)
  };

  using type = mdspan<ElementType, extents<std::size_t, output_first, output_second>, Layout, Accessor>;
};

template <typename LinalgObject, std::size_t RankIdx>
using with_different_extent = typename with_different_extent_impl<LinalgObject, RankIdx>::type;
/// @}

/// @brief Construct an @c mdspan based on @c Mdspan but with the extent @c RankIdx different from it.
//
/// If the original extent at that rank is static, returns an @c mdspan with a distinct static extent at that rank.
/// If the original extent is dynamic, returns the same type @c Mdspan with a non-default dynamic extent.
template <typename Mdspan, std::size_t RankIdx>
constexpr auto construct_with_different_extent() noexcept -> with_different_extent<Mdspan, RankIdx> {
  using return_type = with_different_extent<Mdspan, RankIdx>;
  using different_extents = typename return_type::extents_type;

  auto exts = make_extent_with_dynamic_as<different_extents>(
      other_dynamic_extent,
      std::make_index_sequence<different_extents::rank_dynamic()>{}
  );
  typename return_type::mapping_type const mapping{exts};
  return return_type(nullptr, mapping);
}

template <typename T>
class LinalgPreconditionsTest : public ::testing::Test {
 public:
  /// @brief Construct an instance of @c T using @c default_dynamic_extent for any dynamic extents it may have
  constexpr auto construct() noexcept -> T {
    auto exts = make_extent_with_dynamic_as<typename T::extents_type>(
        default_dynamic_extent,
        std::make_index_sequence<T::extents_type::rank_dynamic()>{}
    );
    typename T::mapping_type const mapping{exts};
    return T(nullptr, mapping);
  }
};

using vector_types = ::testing::Types<
    // Mdspans that are input vectors only but not writeable
    mdspan<double const, extents<std::size_t, 1>>,
    mdspan<double const, dextents<std::size_t, 1>>,
    mdspan<std::uint32_t const, extents<std::size_t, 4>, arene::base::layout_left>,
    // Mdspans that are input/output vectors
    mdspan<std::int32_t, extents<std::size_t, 3>>,
    mdspan<double, extents<std::size_t, 1>>,
    mdspan<double, dextents<std::size_t, 1>>,
    mdspan<std::uint32_t, extents<std::size_t, 4>, arene::base::layout_left>>;

using matrix_types = ::testing::Types<
    // Mdspans that are input matrices only but not writeable
    mdspan<std::int32_t const, extents<std::size_t, 3, 4>>,
    mdspan<double const, extents<std::size_t, 1, dynamic_extent>>,
    // Mdspans that are input/output matrices
    mdspan<std::int32_t, extents<std::size_t, 3, 4>>,
    mdspan<double, extents<std::size_t, 1, dynamic_extent>>,
    mdspan<double, dextents<std::size_t, 2>>,
    mdspan<std::uint32_t, extents<std::size_t, arene::base::dynamic_extent, 4>, arene::base::layout_left>>;

using linalg_object_types = arene::base::type_lists::concat_t<vector_types, matrix_types>;

TYPED_TEST_SUITE(LinalgPreconditionsTest, linalg_object_types, );

/// @test All linalg objects are addable to themselves
TYPED_TEST(LinalgPreconditionsTest, AlwaysAddableToSelf) {
  EXPECT_TRUE(actually_addable(this->construct(), this->construct(), this->construct()));

  STATIC_ASSERT_TRUE(noexcept(actually_addable(this->construct(), this->construct(), this->construct())));
}

/// @test All linalg objects are comparable with themselves
TYPED_TEST(LinalgPreconditionsTest, AlwaysComparableWithSelf) {
  EXPECT_TRUE(actually_comparable(this->construct(), this->construct()));

  STATIC_ASSERT_TRUE(noexcept(actually_comparable(this->construct(), this->construct())));
}

template <typename T>
class LinalgVectorPreconditionsTest : public LinalgPreconditionsTest<T> {};

TYPED_TEST_SUITE(LinalgVectorPreconditionsTest, vector_types, );

/// @test Vectors with different extents (static or dynamic) are not @c actually_addable
TYPED_TEST(LinalgVectorPreconditionsTest, NotAddableWithDifferentExtents) {
  auto const original = this->construct();
  auto const different_extent = construct_with_different_extent<TypeParam, 0U>();

  EXPECT_FALSE(actually_addable(original, original, different_extent));
  EXPECT_FALSE(actually_addable(original, different_extent, original));
  EXPECT_FALSE(actually_addable(different_extent, original, original));

  STATIC_ASSERT_TRUE(noexcept(actually_addable(original, original, different_extent)));
  STATIC_ASSERT_TRUE(noexcept(actually_addable(original, different_extent, original)));
  STATIC_ASSERT_TRUE(noexcept(actually_addable(different_extent, original, original)));
}

/// @test @c actually_addable is not invocable with a vector and anything that isn't a vector
TYPED_TEST(LinalgVectorPreconditionsTest, ActuallyAddableNotInvocableWithNonVectors) {
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using scalar = typename TypeParam::value_type;
  using matrix = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U), TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;
  using rank_3 = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U), TypeParam::static_extent(0U), TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;

  using arene::base::is_invocable_v;

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), scalar, TypeParam, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, scalar, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, TypeParam, scalar>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), matrix, TypeParam, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, matrix, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, TypeParam, matrix>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), rank_3, TypeParam, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, rank_3, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, TypeParam, rank_3>);
}

/// @test Vectors with different extents (static or dynamic) are not @c actually_comparable
TYPED_TEST(LinalgVectorPreconditionsTest, NotComparableWithDifferentExtents) {
  auto const original = this->construct();
  auto const different_extent = construct_with_different_extent<TypeParam, 0U>();

  EXPECT_FALSE(actually_comparable(original, different_extent));
  EXPECT_FALSE(actually_comparable(different_extent, original));

  STATIC_ASSERT_TRUE(noexcept(actually_comparable(original, different_extent)));
  STATIC_ASSERT_TRUE(noexcept(actually_comparable(different_extent, original)));
}

/// @test @c actually_comparable is not invocable with a vector and anything that isn't a vector
TYPED_TEST(LinalgVectorPreconditionsTest, ActuallyComparableNotInvocableWithNonVectors) {
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using scalar = typename TypeParam::value_type;
  using matrix = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U), TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;
  using rank_3 = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U), TypeParam::static_extent(0U), TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;

  using arene::base::is_invocable_v;

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), scalar, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), TypeParam, scalar>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), matrix, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), TypeParam, matrix>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), rank_3, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), TypeParam, rank_3>);
}

template <typename T>
class LinalgMatrixPreconditionsTest : public LinalgPreconditionsTest<T> {};

TYPED_TEST_SUITE(LinalgMatrixPreconditionsTest, matrix_types, );

/// @test Matrices with different heights (static or dynamic) are not @c actually_addable
TYPED_TEST(LinalgMatrixPreconditionsTest, NotAddableWithDifferentHeights) {
  auto const original = this->construct();
  auto const different_height = construct_with_different_extent<TypeParam, 0U>();

  EXPECT_FALSE(actually_addable(original, original, different_height));
  EXPECT_FALSE(actually_addable(original, different_height, original));
  EXPECT_FALSE(actually_addable(different_height, original, original));

  STATIC_ASSERT_TRUE(noexcept(actually_addable(original, original, different_height)));
  STATIC_ASSERT_TRUE(noexcept(actually_addable(original, different_height, original)));
  STATIC_ASSERT_TRUE(noexcept(actually_addable(different_height, original, original)));
}

/// @test Matrices with different widths (static or dynamic) are not @c actually_addable
TYPED_TEST(LinalgMatrixPreconditionsTest, NotAddableWithDifferentWidths) {
  auto const original = this->construct();
  auto const different_width = construct_with_different_extent<TypeParam, 1U>();

  EXPECT_FALSE(actually_addable(original, original, different_width));
  EXPECT_FALSE(actually_addable(original, different_width, original));
  EXPECT_FALSE(actually_addable(different_width, original, original));

  STATIC_ASSERT_TRUE(noexcept(actually_addable(original, original, different_width)));
  STATIC_ASSERT_TRUE(noexcept(actually_addable(original, different_width, original)));
  STATIC_ASSERT_TRUE(noexcept(actually_addable(different_width, original, original)));
}

/// @test @c actually_addable is not invocable with a matrix and anything that isn't a matrix
TYPED_TEST(LinalgMatrixPreconditionsTest, ActuallyAddableNotInvocableWithNonMatrices) {
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using scalar = typename TypeParam::value_type;
  using vector = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;
  using rank_3 = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U), TypeParam::static_extent(0U), TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;

  using arene::base::is_invocable_v;

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), scalar, TypeParam, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, scalar, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, TypeParam, scalar>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), vector, TypeParam, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, vector, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, TypeParam, vector>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), rank_3, TypeParam, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, rank_3, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_addable), TypeParam, TypeParam, rank_3>);
}

/// @test Matrices with different dimensions (static or dynamic) are not @c actually_comparable
TYPED_TEST(LinalgMatrixPreconditionsTest, NotComparableWithDifferentDimensions) {
  auto const original = this->construct();
  auto const different_height = construct_with_different_extent<TypeParam, 0U>();
  auto const different_width = construct_with_different_extent<TypeParam, 1U>();

  EXPECT_FALSE(actually_comparable(original, different_width));
  EXPECT_FALSE(actually_comparable(different_width, original));
  EXPECT_FALSE(actually_comparable(original, different_height));
  EXPECT_FALSE(actually_comparable(different_height, original));

  STATIC_ASSERT_TRUE(noexcept(actually_comparable(original, different_width)));
  STATIC_ASSERT_TRUE(noexcept(actually_comparable(different_width, original)));
  STATIC_ASSERT_TRUE(noexcept(actually_comparable(original, different_height)));
  STATIC_ASSERT_TRUE(noexcept(actually_comparable(different_height, original)));
}

/// @test @c actually_comparable is not invocable with a matrix and anything that isn't a matrix
TYPED_TEST(LinalgMatrixPreconditionsTest, ActuallyComparableNotInvocableWithNonMatrices) {
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using scalar = typename TypeParam::value_type;
  using vector = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;
  using rank_3 = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U), TypeParam::static_extent(0U), TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;

  using arene::base::is_invocable_v;

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), scalar, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), TypeParam, scalar>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), vector, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), TypeParam, vector>);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), rank_3, TypeParam>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(actually_comparable), TypeParam, rank_3>);
}

/// @test Matrices can be left-multiplied by a covector with their height and the result is a vector with their width
TYPED_TEST(LinalgMatrixPreconditionsTest, LeftMultiplicationAllowed) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using left_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(0U)>, layout_type, accessor_type>;
  using out_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(1U)>, layout_type, accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);
  auto const left_vector = construct_with_dynamic_as<left_vector_type>(default_dynamic_extent);
  auto const out_vector = construct_with_dynamic_as<out_vector_type>(default_dynamic_extent);

  EXPECT_TRUE(actually_multipliable(left_vector, matrix, out_vector));

  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(left_vector, matrix, out_vector)));
}

/// @test Matrices can be right-multiplied by a vector with their width and the result is a vector with their height
TYPED_TEST(LinalgMatrixPreconditionsTest, RightMultiplicationAllowed) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using right_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(1U)>, layout_type, accessor_type>;
  using out_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(0U)>, layout_type, accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);
  auto const right_vector = construct_with_dynamic_as<right_vector_type>(default_dynamic_extent);
  auto const out_vector = construct_with_dynamic_as<out_vector_type>(default_dynamic_extent);

  EXPECT_TRUE(actually_multipliable(matrix, right_vector, out_vector));

  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(matrix, right_vector, out_vector)));
}

/// @test Matrices can be multiplied by other matrices when all of the extents match up correctly
TYPED_TEST(LinalgMatrixPreconditionsTest, MatrixMultiplicationAllowed) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  // Different argument types are needed for each of the three positions that TypeParam could go into.

  constexpr std::size_t unmatched_extent{default_dynamic_extent};
  using right_matrix_type = mdspan<
      element_type,
      extents<std::size_t, TypeParam::static_extent(1U), unmatched_extent>,
      layout_type,
      accessor_type>;
  using out_matrix_type_with_tp_on_left = mdspan<
      element_type,
      extents<std::size_t, TypeParam::static_extent(0U), unmatched_extent>,
      layout_type,
      accessor_type>;

  using left_matrix_type = mdspan<
      element_type,
      extents<std::size_t, unmatched_extent, TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;
  using out_matrix_type_with_tp_on_right = mdspan<
      element_type,
      extents<std::size_t, unmatched_extent, TypeParam::static_extent(1U)>,
      layout_type,
      accessor_type>;

  using left_matrix_type_with_tp_out = mdspan<
      element_type,
      extents<std::size_t, TypeParam::static_extent(0U), unmatched_extent>,
      layout_type,
      accessor_type>;
  using right_matrix_type_with_tp_out = mdspan<
      element_type,
      extents<std::size_t, unmatched_extent, TypeParam::static_extent(1U)>,
      layout_type,
      accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);

  auto const right_matrix = construct_with_dynamic_as<right_matrix_type>(default_dynamic_extent);
  auto const out_matrix_with_tp_on_left =
      construct_with_dynamic_as<out_matrix_type_with_tp_on_left>(default_dynamic_extent);

  auto const left_matrix = construct_with_dynamic_as<left_matrix_type>(default_dynamic_extent);
  auto const out_matrix_with_tp_on_right =
      construct_with_dynamic_as<out_matrix_type_with_tp_on_right>(default_dynamic_extent);

  auto const left_matrix_with_tp_out = construct_with_dynamic_as<left_matrix_type_with_tp_out>(default_dynamic_extent);
  auto const right_matrix_with_tp_out =
      construct_with_dynamic_as<right_matrix_type_with_tp_out>(default_dynamic_extent);

  EXPECT_TRUE(actually_multipliable(matrix, right_matrix, out_matrix_with_tp_on_left));
  EXPECT_TRUE(actually_multipliable(left_matrix, matrix, out_matrix_with_tp_on_right));
  EXPECT_TRUE(actually_multipliable(left_matrix_with_tp_out, right_matrix_with_tp_out, matrix));

  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(matrix, right_matrix, out_matrix_with_tp_on_left)));
  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(left_matrix, matrix, out_matrix_with_tp_on_right)));
  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(left_matrix_with_tp_out, right_matrix_with_tp_out, matrix)));
}

/// @test Matrix left-multiplications are not acceptable when the input or output vector has the wrong extent
TYPED_TEST(LinalgMatrixPreconditionsTest, LeftMultiplicationNotAllowedWhenExtentWrong) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using left_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(0U)>, layout_type, accessor_type>;
  using out_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(1U)>, layout_type, accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);
  auto const left_vector = construct_with_dynamic_as<left_vector_type>(default_dynamic_extent);
  auto const wrong_left_vector = construct_with_different_extent<left_vector_type, 0U>();
  auto const out_vector = construct_with_dynamic_as<out_vector_type>(default_dynamic_extent);
  auto const wrong_out_vector = construct_with_different_extent<out_vector_type, 0U>();

  EXPECT_FALSE(actually_multipliable(wrong_left_vector, matrix, out_vector));
  EXPECT_FALSE(actually_multipliable(left_vector, matrix, wrong_out_vector));

  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(wrong_left_vector, matrix, out_vector)));
  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(left_vector, matrix, wrong_out_vector)));
}

/// @test Matrix right-multiplications are not acceptable when the input or output vector has the wrong extent
TYPED_TEST(LinalgMatrixPreconditionsTest, RightMultiplicationNotAllowedWhenExtentWrong) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using right_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(1U)>, layout_type, accessor_type>;
  using out_vector_type =
      mdspan<element_type, extents<std::size_t, TypeParam::static_extent(0U)>, layout_type, accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);
  auto const right_vector = construct_with_dynamic_as<right_vector_type>(default_dynamic_extent);
  auto const wrong_right_vector = construct_with_different_extent<right_vector_type, 0U>();
  auto const out_vector = construct_with_dynamic_as<out_vector_type>(default_dynamic_extent);
  auto const wrong_out_vector = construct_with_different_extent<out_vector_type, 0U>();

  EXPECT_FALSE(actually_multipliable(matrix, wrong_right_vector, out_vector));
  EXPECT_FALSE(actually_multipliable(matrix, right_vector, wrong_out_vector));

  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(matrix, wrong_right_vector, out_vector)));
  STATIC_ASSERT_TRUE(noexcept(actually_multipliable(matrix, right_vector, wrong_out_vector)));
}

/// @test Matrices can not be left-multiplied by other matrices when any of the extents fail to match up correctly
TYPED_TEST(LinalgMatrixPreconditionsTest, MatrixMultiplicationOnLeftNotAllowedWhenExtentWrong) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  constexpr std::size_t unmatched_extent{default_dynamic_extent};
  using right_matrix_type = mdspan<
      element_type,
      extents<std::size_t, TypeParam::static_extent(1U), unmatched_extent>,
      layout_type,
      accessor_type>;
  using out_matrix_type = mdspan<
      element_type,
      extents<std::size_t, TypeParam::static_extent(0U), unmatched_extent>,
      layout_type,
      accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);
  auto const right_matrix = construct_with_dynamic_as<right_matrix_type>(default_dynamic_extent);
  auto const right_matrix_wrong_height = construct_with_different_extent<right_matrix_type, 0U>();
  auto const right_matrix_wrong_width = construct_with_different_extent<right_matrix_type, 1U>();
  auto const out_matrix = construct_with_dynamic_as<out_matrix_type>(default_dynamic_extent);
  auto const out_matrix_wrong_height = construct_with_different_extent<out_matrix_type, 0U>();
  auto const out_matrix_wrong_width = construct_with_different_extent<out_matrix_type, 1U>();

  EXPECT_TRUE(actually_multipliable(matrix, right_matrix, out_matrix));

  EXPECT_FALSE(actually_multipliable(matrix, right_matrix, out_matrix_wrong_height));
  EXPECT_FALSE(actually_multipliable(matrix, right_matrix, out_matrix_wrong_width));
  EXPECT_FALSE(actually_multipliable(matrix, right_matrix_wrong_height, out_matrix));
  EXPECT_FALSE(actually_multipliable(matrix, right_matrix_wrong_height, out_matrix_wrong_height));
  EXPECT_FALSE(actually_multipliable(matrix, right_matrix_wrong_height, out_matrix_wrong_width));
  EXPECT_FALSE(actually_multipliable(matrix, right_matrix_wrong_width, out_matrix));
  EXPECT_FALSE(actually_multipliable(matrix, right_matrix_wrong_width, out_matrix_wrong_height));
}

/// @test Matrices can not be right-multiplied by other matrices when any of the extents fail to match up correctly
TYPED_TEST(LinalgMatrixPreconditionsTest, MatrixMultiplicationOnRightNotAllowedWhenExtentWrong) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  constexpr std::size_t unmatched_extent{default_dynamic_extent};
  using left_matrix_type = mdspan<
      element_type,
      extents<std::size_t, unmatched_extent, TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;
  using out_matrix_type = mdspan<
      element_type,
      extents<std::size_t, unmatched_extent, TypeParam::static_extent(1U)>,
      layout_type,
      accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);
  auto const left_matrix = construct_with_dynamic_as<left_matrix_type>(default_dynamic_extent);
  auto const left_matrix_wrong_height = construct_with_different_extent<left_matrix_type, 0U>();
  auto const left_matrix_wrong_width = construct_with_different_extent<left_matrix_type, 1U>();
  auto const out_matrix = construct_with_dynamic_as<out_matrix_type>(default_dynamic_extent);
  auto const out_matrix_wrong_height = construct_with_different_extent<out_matrix_type, 0U>();
  auto const out_matrix_wrong_width = construct_with_different_extent<out_matrix_type, 1U>();

  EXPECT_TRUE(actually_multipliable(left_matrix, matrix, out_matrix));

  EXPECT_FALSE(actually_multipliable(left_matrix, matrix, out_matrix_wrong_height));
  EXPECT_FALSE(actually_multipliable(left_matrix, matrix, out_matrix_wrong_width));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_height, matrix, out_matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_height, matrix, out_matrix_wrong_width));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_width, matrix, out_matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_width, matrix, out_matrix_wrong_height));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_width, matrix, out_matrix_wrong_width));
}

/// @test Matrices can not be a multiplication output with matrices when any of the extents fail to match up correctly
TYPED_TEST(LinalgMatrixPreconditionsTest, MatrixMultiplicationAsOutputNotAllowedWhenExtentWrong) {
  using element_type = typename TypeParam::element_type;
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  constexpr std::size_t unmatched_extent{default_dynamic_extent};
  using left_matrix_type = mdspan<
      element_type,
      extents<std::size_t, TypeParam::static_extent(0U), unmatched_extent>,
      layout_type,
      accessor_type>;
  using right_matrix_type = mdspan<
      element_type,
      extents<std::size_t, unmatched_extent, TypeParam::static_extent(1U)>,
      layout_type,
      accessor_type>;

  auto const matrix = construct_with_dynamic_as<TypeParam>(default_dynamic_extent);
  auto const left_matrix = construct_with_dynamic_as<left_matrix_type>(default_dynamic_extent);
  auto const left_matrix_wrong_height = construct_with_different_extent<left_matrix_type, 0U>();
  auto const left_matrix_wrong_width = construct_with_different_extent<left_matrix_type, 1U>();
  auto const right_matrix = construct_with_dynamic_as<right_matrix_type>(default_dynamic_extent);
  auto const right_matrix_wrong_height = construct_with_different_extent<right_matrix_type, 0U>();
  auto const right_matrix_wrong_width = construct_with_different_extent<right_matrix_type, 1U>();

  EXPECT_TRUE(actually_multipliable(left_matrix, right_matrix, matrix));

  EXPECT_FALSE(actually_multipliable(left_matrix, right_matrix_wrong_height, matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix, right_matrix_wrong_width, matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_height, right_matrix, matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_height, right_matrix_wrong_height, matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_height, right_matrix_wrong_width, matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_width, right_matrix, matrix));
  EXPECT_FALSE(actually_multipliable(left_matrix_wrong_width, right_matrix_wrong_width, matrix));
}

/// @brief Check that the invocability of @c actually_multipliable is correct for the given combination of types
template <typename Left, typename Right, typename Out>
constexpr auto multipliable_invocability_is_correct(::testing::Types<Left, Right, Out>) noexcept -> bool {
  using arene::base::is_invocable_v;

  constexpr std::size_t left_rank{arene::base::linalg_testing::rank_v<Left>};
  constexpr std::size_t right_rank{arene::base::linalg_testing::rank_v<Right>};
  constexpr std::size_t out_rank{arene::base::linalg_testing::rank_v<Out>};

  constexpr bool should_work{
      ((left_rank == 1U) && (right_rank == 2U) && (out_rank == 1U)) ||
      ((left_rank == 2U) && (right_rank == 1U) && (out_rank == 1U)) ||
      ((left_rank == 2U) && (right_rank == 2U) && (out_rank == 2U))
  };

  constexpr bool actually_invocable{is_invocable_v<decltype(actually_multipliable), Left, Right, Out>};
  static_assert(actually_invocable == should_work, "invocability does not match expectation");
  return actually_invocable == should_work;
}

/// @brief Check that the invocability of @c actually_multipliable is correct for all types in the given list
template <typename... ParameterSets>
constexpr auto multipliabile_invocability_is_correct_for_all(::testing::Types<ParameterSets...>) noexcept -> bool {
  return arene::base::all_of(multipliable_invocability_is_correct(ParameterSets{})...);
}

/// @test @c actually_multipliable is only invocable with vec-mat-vec, mat-vec-vec, or mat-mat-mat combinations
TYPED_TEST(LinalgMatrixPreconditionsTest, ActuallyMultipliableNotInvocableWithWrongTypes) {
  using layout_type = typename TypeParam::layout_type;
  using accessor_type = typename TypeParam::accessor_type;

  using scalar = typename TypeParam::value_type;
  using vector = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;
  using matrix = TypeParam;
  using rank_3 = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0U), TypeParam::static_extent(0U), TypeParam::static_extent(0U)>,
      layout_type,
      accessor_type>;

  using arg_types = ::testing::Types<scalar, vector, matrix, rank_3>;
  using combinations = arene::base::type_lists::cartesian_product_t<arg_types, arg_types, arg_types>;
  STATIC_ASSERT_TRUE(multipliabile_invocability_is_correct_for_all(combinations{}));
}

}  // namespace
