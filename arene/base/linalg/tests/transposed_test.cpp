// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/transposed.hpp"

#include <gtest/gtest.h>

#include "arene/base/linalg/layout_transpose.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/detail/reverse_extents.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"

namespace {

using arene::base::extents;
using arene::base::is_instantiation_of_v;
using arene::base::layout_left;
using arene::base::layout_right;
using arene::base::layout_stride;
using arene::base::mdspan;
using arene::base::linalg::layout_transpose;
using arene::base::linalg::transposed;
using arene::base::linalg_testing::test_array;
using arene::base::mdspan_detail::reverse_extents_t;

/// @brief An owning @c test_array whose view has the extents and layout of the given rank-2 mapping type
///
/// The backing buffer lives inside the returned object, so the aliased @c transposed view is valid for as long as the
/// storage is in scope. This replaces the earlier reliance on a shared global buffer.
template <class Mapping>
using matrix_storage = test_array<mdspan<int const, typename Mapping::extents_type, typename Mapping::layout_type>>;

/// @brief The layout @c transposed produces for a standard input layout: left <-> right, stride -> stride
/// @{
template <class Layout>
struct transposed_layout;

template <>
struct transposed_layout<layout_left> {
  using type = layout_right;
};

template <>
struct transposed_layout<layout_right> {
  using type = layout_left;
};

template <>
struct transposed_layout<layout_stride> {
  using type = layout_stride;
};

template <class Layout>
using transposed_layout_t = typename transposed_layout<Layout>::type;
/// @}

template <typename Mapping>
class LinalgTransposedTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgTransposedTest, test::rank2_standard_layout_mappings, );

/// @test @c transposed of a standard layout swaps the result extents and layout
CONSTEXPR_TYPED_TEST(LinalgTransposedTest, ExtentsAndLayoutAreTransposed) {
  matrix_storage<TypeParam> storage{};
  auto const matrix = storage.mdspan();
  auto const result = transposed(matrix);

  ::testing::
      StaticAssertTypeEq<typename decltype(result)::layout_type, transposed_layout_t<typename TypeParam::layout_type>>(
      );
  ::testing::
      StaticAssertTypeEq<typename decltype(result)::extents_type, reverse_extents_t<typename TypeParam::extents_type>>(
      );
  CONSTEXPR_ASSERT_EQ(result.extent(0U), matrix.extent(1U));
  CONSTEXPR_ASSERT_EQ(result.extent(1U), matrix.extent(0U));
}

/// @test @c transposed of a standard layout swaps the strides
CONSTEXPR_TYPED_TEST(LinalgTransposedTest, StridesAreSwapped) {
  matrix_storage<TypeParam> storage{};
  auto const matrix = storage.mdspan();
  auto const result = transposed(matrix);

  CONSTEXPR_ASSERT_EQ(result.mapping().stride(0U), matrix.mapping().stride(1U));
  CONSTEXPR_ASSERT_EQ(result.mapping().stride(1U), matrix.mapping().stride(0U));
}

/// @test @c transposed of a standard layout transpose the input data
///
/// @note The aliasing loop never iterates when an extent is 0, which gcc8 rejects in a constant expression, so the
/// compile-time evaluation is gated to the types whose expanded extents are all non-zero.
CONDITIONALLY_CONSTEXPR_TYPED_TEST(
    LinalgTransposedTest,
    ElementsAreTransposed,
    (!test::any_extent_is_0(test::make_expanded_extents<typename TypeParam::extents_type>()))
) {
  matrix_storage<TypeParam> storage{};
  auto const matrix = storage.mdspan();
  auto const result = transposed(matrix);

  using index_type = typename decltype(matrix)::index_type;
  for (index_type row{}; row != matrix.extent(0U); ++row) {
    for (index_type column{}; column != matrix.extent(1U); ++column) {
      CONSTEXPR_ASSERT_EQ(&matrix(row, column), &result(column, row));
    }
  }
}

/// @brief A non-standard layout which @c transposed does not have special handling for, resulting in returning a @c
/// layout_transpose
///
/// It behaves exactly like @c layout_right
class custom_layout {
 public:
  template <class Extents>
  class mapping : public layout_right::mapping<Extents> {
   public:
    using base_mapping_type = layout_right::mapping<Extents>;
    using base_mapping_type::base_mapping_type;
    using layout_type = custom_layout;

    /// @brief Construct from a base @c layout_right mapping
    constexpr explicit mapping(base_mapping_type const& base) noexcept
        : base_mapping_type{base} {}
  };
};

/// @test @c transposed of a non-standard layout is wrapped in layout_transpose
CONSTEXPR_TEST(LinalgTransposedUntypedTest, CustomLayoutIsWrapped) {
  matrix_storage<custom_layout::mapping<extents<std::size_t, 2, 3>>> storage{};
  auto const matrix = storage.mdspan();
  auto const result = transposed(matrix);

  ::testing::StaticAssertTypeEq<typename decltype(result)::layout_type, layout_transpose<custom_layout>>();
  CONSTEXPR_ASSERT_EQ(result.extent(0U), matrix.extent(1U));
  CONSTEXPR_ASSERT_EQ(result.extent(1U), matrix.extent(0U));

  using index_type = typename decltype(matrix)::index_type;
  for (index_type row{}; row != matrix.extent(0U); ++row) {
    for (index_type column{}; column != matrix.extent(1U); ++column) {
      CONSTEXPR_ASSERT_EQ(&matrix(row, column), &result(column, row));
    }
  }
}

/// @test @c transposed of a layout_transpose input unwraps it to the nested layout
CONSTEXPR_TEST(LinalgTransposedUntypedTest, LayoutTransposeIsUnwrapped) {
  matrix_storage<custom_layout::mapping<extents<std::size_t, 2, 3>>> storage{};
  auto const matrix = storage.mdspan();
  auto const wrapped = transposed(matrix);
  auto const result = transposed(wrapped);

  static_assert(
      is_instantiation_of_v<typename decltype(wrapped)::layout_type, layout_transpose>,
      "the intermediate view must be a layout_transpose"
  );
  ::testing::StaticAssertTypeEq<typename decltype(result)::layout_type, custom_layout>();
  // transposed(transposed(matrix)) recovers the original extents and aliasing.
  CONSTEXPR_ASSERT_EQ(result.extent(0U), matrix.extent(0U));
  CONSTEXPR_ASSERT_EQ(result.extent(1U), matrix.extent(1U));

  using index_type = typename decltype(wrapped)::index_type;
  for (index_type row{}; row != wrapped.extent(0U); ++row) {
    for (index_type column{}; column != wrapped.extent(1U); ++column) {
      CONSTEXPR_ASSERT_EQ(&wrapped(row, column), &result(column, row));
    }
  }
}

}  // namespace
