// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/detail/mandates.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_manipulation/static_if.hpp"
#include "arene/base/type_traits/give_cv_to.hpp"

namespace {

using arene::base::dynamic_extent;
using arene::base::extents;
using arene::base::mdspan;
using arene::base::linalg::possibly_addable_v;
using arene::base::linalg::possibly_comparable_v;
using arene::base::linalg::possibly_multipliable_v;

/// @brief Replace the indicated rank of an mdspan with @c dynamic_extent while preserving everything else about it
/// @{
template <typename Mdspan, std::size_t RankIdx>
struct with_dynamic_extent_impl {};

template <typename ElementType, typename IndexType, typename Layout, typename Accessor, std::size_t CurrentExtent>
struct with_dynamic_extent_impl<
    mdspan<ElementType, arene::base::extents<IndexType, CurrentExtent>, Layout, Accessor>,
    0U> {
  using type = mdspan<ElementType, arene::base::extents<IndexType, dynamic_extent>, Layout, Accessor>;
};

template <
    typename ElementType,
    typename IndexType,
    typename Layout,
    typename Accessor,
    std::size_t Extent0,
    std::size_t Extent1>
struct with_dynamic_extent_impl<
    mdspan<ElementType, arene::base::extents<IndexType, Extent0, Extent1>, Layout, Accessor>,
    0U> {
  using type = mdspan<ElementType, arene::base::extents<IndexType, dynamic_extent, Extent1>, Layout, Accessor>;
};

template <
    typename ElementType,
    typename IndexType,
    typename Layout,
    typename Accessor,
    std::size_t Extent0,
    std::size_t Extent1>
struct with_dynamic_extent_impl<
    mdspan<ElementType, arene::base::extents<IndexType, Extent0, Extent1>, Layout, Accessor>,
    1U> {
  using type = mdspan<ElementType, arene::base::extents<IndexType, Extent0, dynamic_extent>, Layout, Accessor>;
};

template <typename Mdspan, std::size_t RankIdx>
using with_dynamic_extent =
    arene::base::give_cv_to_t<Mdspan, typename with_dynamic_extent_impl<std::remove_cv_t<Mdspan>, RankIdx>::type>;
/// @}

/// @brief Make the indicated rank of an mdspan one bigger while preserving everything else about it
/// @{
template <typename Mdspan, std::size_t RankIdx>
struct with_bigger_extent_impl {};

template <typename ElementType, typename IndexType, typename Layout, typename Accessor, std::size_t CurrentExtent>
struct with_bigger_extent_impl<
    mdspan<ElementType, arene::base::extents<IndexType, CurrentExtent>, Layout, Accessor>,
    0U> {
  using type = mdspan<ElementType, arene::base::extents<IndexType, CurrentExtent + 1U>, Layout, Accessor>;
};

template <
    typename ElementType,
    typename IndexType,
    typename Layout,
    typename Accessor,
    std::size_t Extent0,
    std::size_t Extent1>
struct with_bigger_extent_impl<
    mdspan<ElementType, arene::base::extents<IndexType, Extent0, Extent1>, Layout, Accessor>,
    0U> {
  using type = mdspan<ElementType, arene::base::extents<IndexType, Extent0 + 1U, Extent1>, Layout, Accessor>;
};

template <
    typename ElementType,
    typename IndexType,
    typename Layout,
    typename Accessor,
    std::size_t Extent0,
    std::size_t Extent1>
struct with_bigger_extent_impl<
    mdspan<ElementType, arene::base::extents<IndexType, Extent0, Extent1>, Layout, Accessor>,
    1U> {
  using type = mdspan<ElementType, arene::base::extents<IndexType, Extent0, Extent1 + 1U>, Layout, Accessor>;
};

template <typename Mdspan, std::size_t RankIdx>
using with_bigger_extent =
    arene::base::give_cv_to_t<Mdspan, typename with_bigger_extent_impl<std::remove_cv_t<Mdspan>, RankIdx>::type>;
/// @}

template <typename T>
class LinalgMandatesTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgMandatesTest, arene::base::linalg_testing::linalg_object_types, );

/// @test All linalg objects are possibly-addable to themselves
TYPED_TEST(LinalgMandatesTest, AlwaysAddableToSelf) {
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, TypeParam, TypeParam>);
}

/// @test All linalg objects are possibly-comparable to themselves
TYPED_TEST(LinalgMandatesTest, AlwaysComparableToSelf) {
  STATIC_ASSERT_TRUE(possibly_comparable_v<TypeParam, TypeParam>);
}

template <typename T>
class LinalgVectorMandatesTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgVectorMandatesTest, arene::base::linalg_testing::linalg_vector_types, );

/// @test Linalg vectors are statically compatible with ones with the same or dynamic extents
TYPED_TEST(LinalgVectorMandatesTest, CompatibleStaticExtents) {
  constexpr bool is_dynamic{TypeParam::static_extent(0U) == dynamic_extent};

  using arene::base::linalg::compatible_static_extents;
  using dynamic_vector = with_dynamic_extent<TypeParam, 0U>;
  using bigger_vector = with_bigger_extent<TypeParam, 0U>;

  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<TypeParam, TypeParam>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<TypeParam, dynamic_vector>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<dynamic_vector, TypeParam>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<TypeParam, bigger_vector>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<bigger_vector, TypeParam>(0U, 0U)));

  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, TypeParam>(0U, 0U));
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, dynamic_vector>(0U, 0U));
  STATIC_ASSERT_TRUE(compatible_static_extents<dynamic_vector, TypeParam>(0U, 0U));
  STATIC_ASSERT_EQ((compatible_static_extents<TypeParam, bigger_vector>(0U, 0U)), is_dynamic);
  STATIC_ASSERT_EQ((compatible_static_extents<bigger_vector, TypeParam>(0U, 0U)), is_dynamic);
}

/// @test Linalg vectors are always possibly-addable to vectors with dynamic extent
TYPED_TEST(LinalgVectorMandatesTest, AlwaysAddableToDynamicExtent) {
  using dynamic_vector = with_dynamic_extent<TypeParam, 0U>;

  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_vector, TypeParam, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, dynamic_vector, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, TypeParam, dynamic_vector>);

  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, dynamic_vector, dynamic_vector>);
  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_vector, TypeParam, dynamic_vector>);
  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_vector, dynamic_vector, TypeParam>);
}

/// @test Linalg vectors are always possibly-comparable to vectors with dynamic extent
TYPED_TEST(LinalgVectorMandatesTest, AlwaysComparableToDynamicExtent) {
  using dynamic_vector = with_dynamic_extent<TypeParam, 0U>;

  STATIC_ASSERT_TRUE(possibly_comparable_v<dynamic_vector, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_comparable_v<TypeParam, dynamic_vector>);
}

/// @test Linalg vectors are not possibly-addable to vectors with different static extents
TYPED_TEST(LinalgVectorMandatesTest, NotAddableToBiggerExtent) {
  constexpr bool is_dynamic{TypeParam::static_extent(0) == dynamic_extent};
  using bigger_vector = with_bigger_extent<TypeParam, 0U>;

  STATIC_ASSERT_EQ((possibly_addable_v<bigger_vector, TypeParam, TypeParam>), is_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, bigger_vector, TypeParam>), is_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, TypeParam, bigger_vector>), is_dynamic);

  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, bigger_vector, bigger_vector>), is_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<bigger_vector, TypeParam, bigger_vector>), is_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<bigger_vector, bigger_vector, TypeParam>), is_dynamic);
}

/// @test Linalg vectors are not possibly-comparable to vectors with different static extents
TYPED_TEST(LinalgVectorMandatesTest, NotComparableToBiggerExtent) {
  constexpr bool is_dynamic{TypeParam::static_extent(0) == dynamic_extent};
  using bigger_vector = with_bigger_extent<TypeParam, 0U>;

  STATIC_ASSERT_EQ((possibly_comparable_v<bigger_vector, TypeParam>), is_dynamic);
  STATIC_ASSERT_EQ((possibly_comparable_v<TypeParam, bigger_vector>), is_dynamic);
}

/// @test Linalg vectors are all possibly-left-multipliable with their own type as output via a matrix which is square
/// or dynamically-sized
TYPED_TEST(LinalgVectorMandatesTest, LeftMultipliable) {
  using square_matrix = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0), TypeParam::static_extent(0)> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, square_matrix, TypeParam>);

  using dynamic_width_matrix =
      mdspan<typename TypeParam::element_type, extents<std::size_t, TypeParam::static_extent(0), dynamic_extent> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, dynamic_width_matrix, TypeParam>);

  using dynamic_height_matrix =
      mdspan<typename TypeParam::element_type, extents<std::size_t, dynamic_extent, TypeParam::static_extent(0)> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, dynamic_height_matrix, TypeParam>);
}

/// @test Linalg vectors are all possibly-right-multipliable with their own type as output via a matrix which is square
/// or dynamically-sized
TYPED_TEST(LinalgVectorMandatesTest, RightMultipliable) {
  using square_matrix = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0), TypeParam::static_extent(0)> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<square_matrix, TypeParam, TypeParam>);

  using dynamic_width_matrix =
      mdspan<typename TypeParam::element_type, extents<std::size_t, TypeParam::static_extent(0), dynamic_extent> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<dynamic_width_matrix, TypeParam, TypeParam>);

  using dynamic_height_matrix =
      mdspan<typename TypeParam::element_type, extents<std::size_t, dynamic_extent, TypeParam::static_extent(0)> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<dynamic_height_matrix, TypeParam, TypeParam>);
}

template <typename T>
class LinalgMatrixMandatesTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgMatrixMandatesTest, arene::base::linalg_testing::linalg_matrix_types, );

/// @test Linalg matrices are statically compatible with ones with the same or dynamic extents
TYPED_TEST(LinalgMatrixMandatesTest, CompatibleStaticExtents) {
  constexpr bool dynamic_height{TypeParam::static_extent(0U) == dynamic_extent};
  constexpr bool dynamic_width{TypeParam::static_extent(1U) == dynamic_extent};
  constexpr bool any_dynamic{dynamic_height || dynamic_width};
  constexpr bool possibly_square{(TypeParam::static_extent(0) == TypeParam::static_extent(1)) || any_dynamic};

  using arene::base::linalg::compatible_static_extents;
  using dynamic_height_matrix = with_dynamic_extent<TypeParam, 0U>;
  using bigger_height_matrix = with_bigger_extent<TypeParam, 0U>;
  using dynamic_width_matrix = with_dynamic_extent<TypeParam, 1U>;
  using bigger_width_matrix = with_bigger_extent<TypeParam, 1U>;

  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<TypeParam, TypeParam>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<TypeParam, dynamic_height_matrix>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<dynamic_height_matrix, TypeParam>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<TypeParam, bigger_height_matrix>(0U, 0U)));
  STATIC_ASSERT_TRUE(noexcept(compatible_static_extents<bigger_height_matrix, TypeParam>(0U, 0U)));

  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, TypeParam>(0U, 0U));
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, dynamic_height_matrix>(0U, 0U));
  STATIC_ASSERT_TRUE(compatible_static_extents<dynamic_height_matrix, TypeParam>(0U, 0U));
  STATIC_ASSERT_EQ((compatible_static_extents<TypeParam, bigger_height_matrix>(0U, 0U)), dynamic_height);
  STATIC_ASSERT_EQ((compatible_static_extents<bigger_height_matrix, TypeParam>(0U, 0U)), dynamic_height);
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, dynamic_width_matrix>(0U, 0U));
  STATIC_ASSERT_TRUE(compatible_static_extents<dynamic_width_matrix, TypeParam>(0U, 0U));
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, bigger_width_matrix>(0U, 0U));
  STATIC_ASSERT_TRUE(compatible_static_extents<bigger_width_matrix, TypeParam>(0U, 0U));

  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, TypeParam>(1U, 1U));
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, dynamic_height_matrix>(1U, 1U));
  STATIC_ASSERT_TRUE(compatible_static_extents<dynamic_height_matrix, TypeParam>(1U, 1U));
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, bigger_height_matrix>(1U, 1U));
  STATIC_ASSERT_TRUE(compatible_static_extents<bigger_height_matrix, TypeParam>(1U, 1U));
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, dynamic_width_matrix>(1U, 1U));
  STATIC_ASSERT_TRUE(compatible_static_extents<dynamic_width_matrix, TypeParam>(1U, 1U));
  STATIC_ASSERT_EQ((compatible_static_extents<TypeParam, bigger_width_matrix>(1U, 1U)), dynamic_width);
  STATIC_ASSERT_EQ((compatible_static_extents<bigger_width_matrix, TypeParam>(1U, 1U)), dynamic_width);

  STATIC_ASSERT_EQ((compatible_static_extents<TypeParam, TypeParam>(0U, 1U)), possibly_square);
  STATIC_ASSERT_EQ((compatible_static_extents<TypeParam, dynamic_height_matrix>(0U, 1U)), any_dynamic);
  STATIC_ASSERT_TRUE(compatible_static_extents<dynamic_height_matrix, TypeParam>(0U, 1U));
  STATIC_ASSERT_EQ((compatible_static_extents<TypeParam, bigger_height_matrix>(0U, 1U)), any_dynamic);
  STATIC_ASSERT_EQ(
      (compatible_static_extents<bigger_height_matrix, TypeParam>(0U, 1U)),
      (dynamic_width || TypeParam::static_extent(0U) + 1U == TypeParam::static_extent(1U))
  );
  STATIC_ASSERT_TRUE(compatible_static_extents<TypeParam, dynamic_width_matrix>(0U, 1U));
  STATIC_ASSERT_EQ((compatible_static_extents<dynamic_width_matrix, TypeParam>(0U, 1U)), any_dynamic);
  STATIC_ASSERT_EQ(
      (compatible_static_extents<TypeParam, bigger_width_matrix>(0U, 1U)),
      (dynamic_height || TypeParam::static_extent(0U) == TypeParam::static_extent(1U) + 1U)
  );
  STATIC_ASSERT_EQ((compatible_static_extents<bigger_width_matrix, TypeParam>(0U, 1U)), any_dynamic);
}

/// @test Linalg matrices are always possibly-addable to matrices with one or more dynamic extents
TYPED_TEST(LinalgMatrixMandatesTest, AlwaysAddableToDynamicExtent) {
  using dynamic_height_matrix = with_dynamic_extent<TypeParam, 0U>;

  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_height_matrix, TypeParam, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, dynamic_height_matrix, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, TypeParam, dynamic_height_matrix>);

  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, dynamic_height_matrix, dynamic_height_matrix>);
  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_height_matrix, TypeParam, dynamic_height_matrix>);
  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_height_matrix, dynamic_height_matrix, TypeParam>);

  using dynamic_width_matrix = with_dynamic_extent<TypeParam, 1U>;

  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_width_matrix, TypeParam, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, dynamic_width_matrix, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, TypeParam, dynamic_width_matrix>);

  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, dynamic_width_matrix, dynamic_width_matrix>);
  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_width_matrix, TypeParam, dynamic_width_matrix>);
  STATIC_ASSERT_TRUE(possibly_addable_v<dynamic_width_matrix, dynamic_width_matrix, TypeParam>);

  using fully_dynamic_matrix = with_dynamic_extent<with_dynamic_extent<TypeParam, 0U>, 1U>;

  STATIC_ASSERT_TRUE(possibly_addable_v<fully_dynamic_matrix, TypeParam, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, fully_dynamic_matrix, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, TypeParam, fully_dynamic_matrix>);

  STATIC_ASSERT_TRUE(possibly_addable_v<TypeParam, fully_dynamic_matrix, fully_dynamic_matrix>);
  STATIC_ASSERT_TRUE(possibly_addable_v<fully_dynamic_matrix, TypeParam, fully_dynamic_matrix>);
  STATIC_ASSERT_TRUE(possibly_addable_v<fully_dynamic_matrix, fully_dynamic_matrix, TypeParam>);
}

/// @test Linalg matrices are always possibly-comparable to matrices with one or more dynamic extents
TYPED_TEST(LinalgMatrixMandatesTest, AlwaysComparableToDynamicExtent) {
  using dynamic_height_matrix = with_dynamic_extent<TypeParam, 0U>;

  STATIC_ASSERT_TRUE(possibly_comparable_v<dynamic_height_matrix, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_comparable_v<TypeParam, dynamic_height_matrix>);

  using dynamic_width_matrix = with_dynamic_extent<TypeParam, 1U>;

  STATIC_ASSERT_TRUE(possibly_comparable_v<dynamic_width_matrix, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_comparable_v<TypeParam, dynamic_width_matrix>);

  using fully_dynamic_matrix = with_dynamic_extent<with_dynamic_extent<TypeParam, 0U>, 1U>;

  STATIC_ASSERT_TRUE(possibly_comparable_v<fully_dynamic_matrix, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_comparable_v<TypeParam, fully_dynamic_matrix>);
}

/// @test Linalg matrices are not possibly-addable to matrices with different static height
TYPED_TEST(LinalgMatrixMandatesTest, NotAddableToBiggerHeight) {
  constexpr bool dynamic_height{TypeParam::static_extent(0) == dynamic_extent};
  using bigger_height_matrix = with_bigger_extent<TypeParam, 0U>;

  STATIC_ASSERT_EQ((possibly_addable_v<bigger_height_matrix, TypeParam, TypeParam>), dynamic_height);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, bigger_height_matrix, TypeParam>), dynamic_height);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, TypeParam, bigger_height_matrix>), dynamic_height);

  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, bigger_height_matrix, bigger_height_matrix>), dynamic_height);
  STATIC_ASSERT_EQ((possibly_addable_v<bigger_height_matrix, TypeParam, bigger_height_matrix>), dynamic_height);
  STATIC_ASSERT_EQ((possibly_addable_v<bigger_height_matrix, bigger_height_matrix, TypeParam>), dynamic_height);
}

/// @test Linalg matrices are not possibly-addable to matrices with different static width
TYPED_TEST(LinalgMatrixMandatesTest, NotAddableToBiggerWidth) {
  constexpr bool dynamic_width{TypeParam::static_extent(1) == dynamic_extent};
  using bigger_width_matrix = with_bigger_extent<TypeParam, 1U>;

  STATIC_ASSERT_EQ((possibly_addable_v<bigger_width_matrix, TypeParam, TypeParam>), dynamic_width);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, bigger_width_matrix, TypeParam>), dynamic_width);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, TypeParam, bigger_width_matrix>), dynamic_width);

  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, bigger_width_matrix, bigger_width_matrix>), dynamic_width);
  STATIC_ASSERT_EQ((possibly_addable_v<bigger_width_matrix, TypeParam, bigger_width_matrix>), dynamic_width);
  STATIC_ASSERT_EQ((possibly_addable_v<bigger_width_matrix, bigger_width_matrix, TypeParam>), dynamic_width);
}

/// @test Linalg matrices are not possibly-addable to matrices with both different static height and static width
TYPED_TEST(LinalgMatrixMandatesTest, NotAddableToGenerallyBigger) {
  constexpr bool dynamic_height{TypeParam::static_extent(0) == dynamic_extent};
  constexpr bool dynamic_width{TypeParam::static_extent(1) == dynamic_extent};
  constexpr bool fully_dynamic{dynamic_height && dynamic_width};
  using generally_bigger_matrix = with_bigger_extent<with_bigger_extent<TypeParam, 0U>, 1U>;

  STATIC_ASSERT_EQ((possibly_addable_v<generally_bigger_matrix, TypeParam, TypeParam>), fully_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, generally_bigger_matrix, TypeParam>), fully_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, TypeParam, generally_bigger_matrix>), fully_dynamic);

  STATIC_ASSERT_EQ((possibly_addable_v<TypeParam, generally_bigger_matrix, generally_bigger_matrix>), fully_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<generally_bigger_matrix, TypeParam, generally_bigger_matrix>), fully_dynamic);
  STATIC_ASSERT_EQ((possibly_addable_v<generally_bigger_matrix, generally_bigger_matrix, TypeParam>), fully_dynamic);
}

/// @test Linalg matrices are not possibly-comparable to matrices with one or more bigger static extents
TYPED_TEST(LinalgMatrixMandatesTest, NotComparableToBiggerExtent) {
  constexpr bool dynamic_height{TypeParam::static_extent(0) == dynamic_extent};
  constexpr bool dynamic_width{TypeParam::static_extent(1) == dynamic_extent};
  constexpr bool fully_dynamic{dynamic_height && dynamic_width};
  using bigger_height_matrix = with_bigger_extent<TypeParam, 0U>;
  using bigger_width_matrix = with_bigger_extent<TypeParam, 1U>;
  using generally_bigger_matrix = with_bigger_extent<with_bigger_extent<TypeParam, 0U>, 1U>;

  STATIC_ASSERT_EQ((possibly_comparable_v<bigger_height_matrix, TypeParam>), dynamic_height);
  STATIC_ASSERT_EQ((possibly_comparable_v<TypeParam, bigger_height_matrix>), dynamic_height);

  STATIC_ASSERT_EQ((possibly_comparable_v<bigger_width_matrix, TypeParam>), dynamic_width);
  STATIC_ASSERT_EQ((possibly_comparable_v<TypeParam, bigger_width_matrix>), dynamic_width);

  STATIC_ASSERT_EQ((possibly_comparable_v<generally_bigger_matrix, TypeParam>), fully_dynamic);
  STATIC_ASSERT_EQ((possibly_comparable_v<TypeParam, generally_bigger_matrix>), fully_dynamic);
}

/// @test For matrices, left-multiplication with vectors works as expected
TYPED_TEST(LinalgMatrixMandatesTest, VectorLeftMultipliable) {
  using left_multipliable_vector =
      mdspan<typename TypeParam::element_type, extents<std::size_t, TypeParam::static_extent(0)> >;
  using output_vector = mdspan<typename TypeParam::element_type, extents<std::size_t, TypeParam::static_extent(1)> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<left_multipliable_vector, TypeParam, output_vector>);

  using dynamic_vector = with_dynamic_extent<left_multipliable_vector, 0U>;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<dynamic_vector, TypeParam, output_vector>);
  STATIC_ASSERT_TRUE(possibly_multipliable_v<left_multipliable_vector, TypeParam, dynamic_vector>);
  STATIC_ASSERT_TRUE(possibly_multipliable_v<dynamic_vector, TypeParam, dynamic_vector>);

  using big_left_vector = with_bigger_extent<left_multipliable_vector, 0U>;
  using big_output_vector = with_bigger_extent<output_vector, 0U>;

  STATIC_ASSERT_EQ(
      (possibly_multipliable_v<big_left_vector, TypeParam, output_vector>),
      (TypeParam::static_extent(0) == dynamic_extent)
  );
  STATIC_ASSERT_EQ(
      (possibly_multipliable_v<left_multipliable_vector, TypeParam, big_output_vector>),
      (TypeParam::static_extent(1) == dynamic_extent)
  );
  STATIC_ASSERT_EQ(
      (possibly_multipliable_v<big_left_vector, TypeParam, big_output_vector>),
      (TypeParam::static_extent(0) == dynamic_extent) && (TypeParam::static_extent(1) == dynamic_extent)
  );
}

/// @test For matrices, right-multiplication with vectors works as expected
TYPED_TEST(LinalgMatrixMandatesTest, VectorRightMultipliable) {
  using right_multipliable_vector =
      mdspan<typename TypeParam::element_type, extents<std::size_t, TypeParam::static_extent(1)> >;
  using output_vector = mdspan<typename TypeParam::element_type, extents<std::size_t, TypeParam::static_extent(0)> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, right_multipliable_vector, output_vector>);

  using dynamic_vector = with_dynamic_extent<right_multipliable_vector, 0U>;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, dynamic_vector, output_vector>);
  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, right_multipliable_vector, dynamic_vector>);
  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, dynamic_vector, dynamic_vector>);

  using big_right_vector = with_bigger_extent<right_multipliable_vector, 0U>;
  using big_output_vector = with_bigger_extent<output_vector, 0U>;

  STATIC_ASSERT_EQ(
      (possibly_multipliable_v<TypeParam, big_right_vector, output_vector>),
      (TypeParam::static_extent(1) == dynamic_extent)
  );
  STATIC_ASSERT_EQ(
      (possibly_multipliable_v<TypeParam, right_multipliable_vector, big_output_vector>),
      (TypeParam::static_extent(0) == dynamic_extent)
  );
  STATIC_ASSERT_EQ(
      (possibly_multipliable_v<TypeParam, big_right_vector, big_output_vector>),
      (TypeParam::static_extent(0) == dynamic_extent) && (TypeParam::static_extent(1) == dynamic_extent)
  );
}

/// @test Matrix-matrix multiplications work as expected
TYPED_TEST(LinalgMatrixMandatesTest, MatrixMultipliable) {
  using transposed_matrix = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(1), TypeParam::static_extent(0)> >;
  using height_height_matrix = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(0), TypeParam::static_extent(0)> >;
  using width_width_matrix = mdspan<
      typename TypeParam::element_type,
      extents<std::size_t, TypeParam::static_extent(1), TypeParam::static_extent(1)> >;

  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, transposed_matrix, height_height_matrix>);
  STATIC_ASSERT_TRUE(possibly_multipliable_v<transposed_matrix, TypeParam, width_width_matrix>);

  STATIC_ASSERT_TRUE(possibly_multipliable_v<TypeParam, width_width_matrix, TypeParam>);
  STATIC_ASSERT_TRUE(possibly_multipliable_v<height_height_matrix, TypeParam, TypeParam>);

  constexpr bool dynamic_height{TypeParam::static_extent(0) == dynamic_extent};
  constexpr bool dynamic_width{TypeParam::static_extent(1) == dynamic_extent};
  constexpr bool square{TypeParam::static_extent(0) == TypeParam::static_extent(1) || dynamic_height || dynamic_width};

  STATIC_ASSERT_EQ((possibly_multipliable_v<TypeParam, transposed_matrix, width_width_matrix>), square);
  STATIC_ASSERT_EQ((possibly_multipliable_v<transposed_matrix, TypeParam, height_height_matrix>), square);

  STATIC_ASSERT_EQ((possibly_multipliable_v<TypeParam, height_height_matrix, TypeParam>), square);
  STATIC_ASSERT_EQ((possibly_multipliable_v<width_width_matrix, TypeParam, TypeParam>), square);
}

}  // namespace
