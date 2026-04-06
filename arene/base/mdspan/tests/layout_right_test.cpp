// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

template <typename TypeParam>
class LayoutRightTest : public test::layout_test_base<TypeParam> {};

TYPED_TEST_SUITE(LayoutRightTest, test::layout_right_types, );

/// @test The default layout_right constructor has the default strides, i.e. those of a default-constructed extent
CONSTEXPR_TYPED_TEST(LayoutRightTest, DefaultConstructor) {
  CONSTEXPR_ASSERT(std::is_nothrow_default_constructible<TypeParam>::value);

  TypeParam const layout;
  typename TypeParam::extents_type const extents{};
  std::size_t stride_so_far{1UL};
  // layout_right iterates backward, from right to left; strides should increase
  for (std::size_t i = TypeParam::extents_type::rank(); i > 0UL; --i) {
    CONSTEXPR_ASSERT_EQ(layout.extents().extent(i - 1UL), extents.extent(i - 1UL));
    CONSTEXPR_ASSERT_EQ(static_cast<std::size_t>(layout.stride(i - 1UL)), stride_so_far);
    stride_so_far *= static_cast<std::size_t>(extents.extent(i - 1UL));
    if (extents.extent(i - 1UL) == typename TypeParam::index_type{}) {
      // In our stride-setting algorithm, encountering a 0 extent resets the stride_so_far to 1.
      stride_so_far = typename TypeParam::index_type{1};
    }
  }
}

/// @test The extents-taking layout_right constructor has correct (right-justified) strides
CONSTEXPR_TYPED_TEST(LayoutRightTest, ConstructFromExtents) {
  CONSTEXPR_ASSERT(std::is_nothrow_constructible<TypeParam, typename TypeParam::extents_type const&>::value);
  CONSTEXPR_ASSERT(std::is_convertible<typename TypeParam::extents_type const&, TypeParam>::value);

  auto const extents = test::make_expanded_extents<typename TypeParam::extents_type>();
  TypeParam const mapping(extents);
  std::size_t stride_so_far{1UL};
  // layout_right iterates backward, from right to left; strides should increase
  for (std::size_t i = TypeParam::extents_type::rank(); i > 0UL; --i) {
    CONSTEXPR_ASSERT_EQ(mapping.extents().extent(i - 1UL), extents.extent(i - 1UL));
    CONSTEXPR_ASSERT_EQ(static_cast<std::size_t>(mapping.stride(i - 1UL)), stride_so_far);
    stride_so_far *= static_cast<std::size_t>(extents.extent(i - 1UL));
    if (extents.extent(i - 1UL) == typename TypeParam::index_type{}) {
      // In our stride-setting algorithm, encountering a 0 extent resets the stride_so_far to 1.
      stride_so_far = typename TypeParam::index_type{1};
    }
  }
}

/// @test The constructor from a layout_right with different extents is implicit when the extents construction is
CONSTEXPR_TYPED_TEST(LayoutRightTest, ConstructFromOtherLayoutRightMapping) {
  using conv_extents_type = arene::base::testing::implicitly_convertible_extents<typename TypeParam::extents_type>;
  using conv_mapping_type = arene::base::layout_right::mapping<conv_extents_type>;
  CONSTEXPR_ASSERT(std::is_nothrow_constructible<TypeParam, conv_mapping_type const&>::value);
  CONSTEXPR_ASSERT(std::is_convertible<conv_mapping_type const&, TypeParam>::value);

  conv_mapping_type const conv_mapping;
  TypeParam const mapping(conv_mapping);

  for (typename TypeParam::rank_type dim{}; dim < TypeParam::extents_type::rank(); ++dim) {
    CONSTEXPR_ASSERT_EQ(mapping.stride(dim), conv_mapping.stride(dim));
  }
}

/// @test The constructor from a layout_right with different extents is explicit when the extents construction is
CONSTEXPR_TYPED_TEST(LayoutRightTest, ConstructFromNonConvertibleLayoutRightMapping) {
  using non_conv_extents_type = arene::base::testing::explicitly_convertible_extents<typename TypeParam::extents_type>;
  using non_conv_mapping_type = arene::base::layout_right::mapping<non_conv_extents_type>;
  CONSTEXPR_ASSERT((std::is_nothrow_constructible<TypeParam, non_conv_mapping_type const&>::value));
  CONSTEXPR_ASSERT(!(std::is_convertible<non_conv_mapping_type const&, TypeParam>::value));

  non_conv_mapping_type const non_conv_mapping;
  TypeParam const mapping(non_conv_mapping);

  for (typename TypeParam::rank_type dim{}; dim < TypeParam::extents_type::rank(); ++dim) {
    CONSTEXPR_ASSERT_EQ(mapping.stride(dim), non_conv_mapping.stride(dim));
  }
}

/// @test The constructor from a layout_right is disabled when the ranks of the extents don't match
TYPED_TEST(LayoutRightTest, ConstructFromLayoutRightWithDifferentRankIsDisabled) {
  using diff_rank_extents = test::extents_with_different_rank<typename TypeParam::extents_type>;
  using diff_rank_mapping = arene::base::layout_right::mapping<diff_rank_extents>;
  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, diff_rank_mapping>::value);
}

/// @test The constructor from a layout_right is disabled when the static parts of the extents don't match
TYPED_TEST(LayoutRightTest, ConstructFromLayoutRightWithDifferentStaticExtentsIsDisabled) {
  using extents = typename TypeParam::extents_type;
  using diff_static_extents = test::extents_with_different_static_part<extents>;
  using diff_static_mapping = arene::base::layout_right::mapping<diff_static_extents>;

  // This conversion is only possible if the rank is 0, in which case the original and shifted extents are the same.
  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, diff_static_mapping>::value),
      ((extents::rank() == 0U) || (extents::rank() == extents::rank_dynamic()))
  );
}

/// @test The constructor from a layout_left with the same extents works if and only if rank <= 1
CONSTEXPR_TYPED_TEST(LayoutRightTest, ConstructFromLayoutLeftMapping) {
  constexpr std::size_t rank{TypeParam::extents_type::rank()};
  using left_mapping_type = arene::base::layout_left::mapping<typename TypeParam::extents_type>;
  CONSTEXPR_ASSERT_EQ((std::is_nothrow_constructible<TypeParam, left_mapping_type const&>::value), rank <= 1U);
  CONSTEXPR_ASSERT_EQ((std::is_convertible<left_mapping_type const&, TypeParam>::value), rank <= 1U);

  // Workaround for lack of constexpr if to make sure the block below always compiles
  using left_type_if_convertible = std::conditional_t<rank <= 1U, left_mapping_type, TypeParam>;

  if (rank <= typename TypeParam::rank_type{1U}) {
    left_type_if_convertible const left_mapping;
    TypeParam const converted{left_mapping};
    if (rank == typename TypeParam::rank_type{1U}) {
      // If there is a dimension, it should be 1 on both sides (this is why the conversion is allowed at all).
      CONSTEXPR_ASSERT_EQ(converted.stride(typename TypeParam::rank_type{}), typename TypeParam::index_type{1U});
    }

    using diff_rank_extents = test::extents_with_different_rank<typename TypeParam::extents_type>;
    using diff_rank_mapping = arene::base::layout_left::mapping<diff_rank_extents>;
    CONSTEXPR_ASSERT((!std::is_constructible<TypeParam, diff_rank_mapping>::value));

    using diff_static_extents = test::extents_with_different_static_part<typename TypeParam::extents_type>;
    using diff_static_mapping = arene::base::layout_left::mapping<diff_static_extents>;
    CONSTEXPR_ASSERT_EQ(
        (std::is_constructible<TypeParam, diff_static_mapping>::value),
        ((rank == 0U) || (rank == TypeParam::extents_type::rank_dynamic()))
    );
  }
}

/// @test The constructor from a layout_left with compatible extents works if and only if rank <= 1
TYPED_TEST(LayoutRightTest, ConstructFromLayoutLeftMappingWithOtherExtents) {
  constexpr std::size_t rank{TypeParam::extents_type::rank()};
  using conv_extents_type = arene::base::testing::implicitly_convertible_extents<typename TypeParam::extents_type>;
  using left_mapping_implicit_t = arene::base::layout_left::mapping<conv_extents_type>;
  ASSERT_EQ((std::is_nothrow_constructible<TypeParam, left_mapping_implicit_t const&>::value), rank <= 1U);
  ASSERT_EQ((std::is_convertible<left_mapping_implicit_t const&, TypeParam>::value), rank <= 1U);

  using non_conv_extents_type = arene::base::testing::explicitly_convertible_extents<typename TypeParam::extents_type>;
  using left_mapping_explicit_t = arene::base::layout_left::mapping<non_conv_extents_type>;
  ASSERT_EQ((std::is_nothrow_constructible<TypeParam, left_mapping_explicit_t const&>::value), rank <= 1U);
  ASSERT_TRUE(!(std::is_convertible<left_mapping_explicit_t const&, TypeParam>::value));

  // Workaround for lack of constexpr if to make sure the block below always compiles
  using imp_type_if_convertible = std::conditional_t<rank <= 1U, left_mapping_implicit_t, TypeParam>;
  using exp_type_if_convertible = std::conditional_t<rank <= 1U, left_mapping_explicit_t, TypeParam>;

  if (rank <= typename TypeParam::rank_type{1U}) {
    imp_type_if_convertible const left_mapping_implicit;
    TypeParam const imp_converted{left_mapping_implicit};
    if (rank == typename TypeParam::rank_type{1U}) {
      // If there is a dimension, it should be 1 on both sides (this is why the conversion is allowed at all).
      ASSERT_EQ(imp_converted.stride(typename TypeParam::rank_type{}), typename TypeParam::index_type{1U});
    }

    exp_type_if_convertible const left_mapping_explicit;
    TypeParam const exp_converted{left_mapping_explicit};
    if (rank == typename TypeParam::rank_type{1U}) {
      // If there is a dimension, it should be 1 on both sides (this is why the conversion is allowed at all).
      ASSERT_EQ(exp_converted.stride(typename TypeParam::rank_type{}), typename TypeParam::index_type{1U});
    }
  }
}

/// @test The constructor from a right-strided layout_stride works for all ranks, and is implicit when rank == 0
CONSTEXPR_TYPED_TEST(LayoutRightTest, ConstructFromLayoutStrideMapping) {
  constexpr std::size_t rank{TypeParam::extents_type::rank()};
  using stride_mapping_type = arene::base::layout_stride::mapping<typename TypeParam::extents_type>;
  CONSTEXPR_ASSERT((std::is_nothrow_constructible<TypeParam, stride_mapping_type const&>::value));
  CONSTEXPR_ASSERT_EQ((std::is_convertible<stride_mapping_type const&, TypeParam>::value), rank == 0U);

  stride_mapping_type const stride_mapping;
  TypeParam const converted{stride_mapping};
  for (typename TypeParam::rank_type dim{}; dim < rank; ++dim) {
    CONSTEXPR_ASSERT_EQ(stride_mapping.stride(dim), converted.stride(dim));
  }
}

/// @test Constructor from a layout_stride with different extents works and is always explicit
CONSTEXPR_TYPED_TEST(LayoutRightTest, ConstructFromLayoutStrideWithConvertibleExtents) {
  constexpr std::size_t rank{TypeParam::extents_type::rank()};
  using conv_extents_type = arene::base::testing::implicitly_convertible_extents<typename TypeParam::extents_type>;
  using conv_mapping_type = arene::base::layout_stride::mapping<conv_extents_type>;
  CONSTEXPR_ASSERT((std::is_nothrow_constructible<TypeParam, conv_mapping_type const&>::value));
  CONSTEXPR_ASSERT_EQ((std::is_convertible<conv_mapping_type const&, TypeParam>::value), rank == 0U);

  conv_mapping_type const conv_mapping;
  TypeParam const mapping(conv_mapping);

  for (typename TypeParam::rank_type dim{}; dim < rank; ++dim) {
    CONSTEXPR_ASSERT_EQ(mapping.stride(dim), conv_mapping.stride(dim));
  }
}

/// @test The constructor from a layout_stride is disabled when the ranks of the extents don't match
TYPED_TEST(LayoutRightTest, ConstructFromLayoutStrideWithDifferentRankIsDisabled) {
  using diff_rank_extents = test::extents_with_different_rank<typename TypeParam::extents_type>;
  using diff_rank_mapping = arene::base::layout_stride::mapping<diff_rank_extents>;
  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, diff_rank_mapping>::value);
}

/// @test The constructor from a layout_stride is disabled when the static parts of the extents don't match
TYPED_TEST(LayoutRightTest, ConstructFromLayoutStrideWithDifferentStaticExtentsIsDisabled) {
  using extents = typename TypeParam::extents_type;
  using diff_static_extents = test::extents_with_different_static_part<extents>;
  using diff_static_mapping = arene::base::layout_stride::mapping<diff_static_extents>;

  // This conversion is only possible if the rank is 0, in which case the original and shifted extents are the same.
  STATIC_ASSERT_EQ(
      (std::is_constructible<TypeParam, diff_static_mapping>::value),
      ((extents::rank() == 0U) || (extents::rank() == extents::rank_dynamic()))
  );
}

/// @test Static observers are available and correct for layout_right::mapping
TYPED_TEST(LayoutRightTest, StaticObserversAreCorrect) {
  STATIC_ASSERT_EQ(TypeParam::is_always_unique(), true);
  STATIC_ASSERT_EQ(TypeParam::is_always_exhaustive(), true);
  STATIC_ASSERT_EQ(TypeParam::is_always_strided(), true);

  STATIC_ASSERT_TRUE(noexcept(TypeParam::is_always_unique()));
  STATIC_ASSERT_TRUE(noexcept(TypeParam::is_always_exhaustive()));
  STATIC_ASSERT_TRUE(noexcept(TypeParam::is_always_strided()));
}

/// @test Instance observers are available and correct for layout_right::mapping
TYPED_TEST(LayoutRightTest, InstanceObserversAreCorrect) {
  ASSERT_EQ(TypeParam{}.is_unique(), true);
  ASSERT_EQ(TypeParam{}.is_exhaustive(), true);
  ASSERT_EQ(TypeParam{}.is_strided(), true);

  ASSERT_TRUE(noexcept(TypeParam{}.is_unique()));
  ASSERT_TRUE(noexcept(TypeParam{}.is_exhaustive()));
  ASSERT_TRUE(noexcept(TypeParam{}.is_strided()));
}

}  // namespace
