// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/linalg/layout_transpose.hpp"
#include "arene/base/linalg/tests/transpose_test_helpers.hpp"
#include "arene/base/mdspan/detail/do_submdspan_mapping.hpp"
#include "arene/base/mdspan/detail/reverse_extents.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/is_sliceable_mapping.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/submdspan.hpp"
#include "arene/base/mdspan/submdspan_mapping_result.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::extents;
using arene::base::full_extent;
using arene::base::layout_stride;
using arene::base::linalg::layout_transpose;
using arene::base::linalg_testing::transposed_mapping_for;
using arene::base::mdspan_detail::do_submdspan_mapping;
using arene::base::mdspan_detail::reverse_extents_t;
using arene::base::testing::make_test_src;

/// @brief A range slice with a runtime first/last and unit stride, used to select a contiguous sub-range
using range_slice = arene::base::range_slice<std::size_t>;

/// @brief An @c extent_slice with a compile-time stride of 2, used to force a non-unit-stride (layout_stride) result
using strided_slice = arene::base::extent_slice<std::size_t, std::size_t, std::integral_constant<std::size_t, 2>>;

/// @brief A 12-element buffer of distinct values 0..11 at namespace scope, so its address is a constant expression
constexpr arene::base::array<int, 12> distinct_buffer{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};

/// @brief Whether the first type in a slice specifier pack is @c arene::base::full_extent_t
/// @tparam Slices The slice specifier pack; an empty pack yields @c false
/// @{
template <class... Slices>
struct first_slice_is_full_extent : std::false_type {};

template <class First, class... Rest>
struct first_slice_is_full_extent<First, Rest...> : std::is_same<First, arene::base::full_extent_t> {};
/// @}

/// @brief A sliceable user-defined layout whose forwarding @c submdspan_mapping is @c noexcept only when the *first*
/// slice specifier is @c full_extent_t, so its noexcept-ness depends on slice *order*
///
/// It inherits from @c test::user_defined_layout 's mapping for all of the layout-mapping boilerplate (the
/// @c layout_right base, extents, strides, and that base's own forwarding @c submdspan_mapping) and overrides only the
/// @c noexcept specifier to be order-sensitive. Because @c layout_transpose reverses the two slices before forwarding
/// them to the nested mapping, a transpose of this layout is @c noexcept exactly when the transpose's @c noexcept
/// clause reverses the slices the same way its call does, so it is a regression check that the clause and the call
/// agree on slice order.
class order_sensitive_submdspan_layout {
 public:
  /// @brief A mapping reusing @c test::user_defined_layout with an order-sensitive @c submdspan_mapping @c noexcept
  /// @tparam Extents The @c extents type of this mapping
  template <typename Extents>
  class mapping : public test::user_defined_layout<true, true, true>::mapping<Extents> {
    using base_type = typename test::user_defined_layout<true, true, true>::mapping<Extents>;

   public:
    using base_type::base_type;
    using layout_type = order_sensitive_submdspan_layout;

    /// @brief Access the inherited @c user_defined_layout base mapping
    /// @param src The derived mapping whose base subobject to access
    /// @return A const reference to the inherited base mapping
    ///
    /// The explicit @c base_type return type lets the forwarding @c submdspan_mapping name the base overload in an
    /// unevaluated context without completing @c base_type, which the transpose instantiates eagerly.
    static constexpr auto as_base(mapping const& src) noexcept -> base_type const& {
      return static_cast<base_type const&>(src);
    }

    /// @brief Forward @c submdspan_mapping to the inherited base mapping, @c noexcept only if the first slice is
    /// @c full_extent_t
    /// @tparam Slices The slice specifier types
    /// @param src The mapping to slice
    /// @param slices The slice specifiers
    /// @return The result of slicing the inherited base mapping
    template <class... Slices>
    friend constexpr auto submdspan_mapping(mapping const& src, Slices... slices) noexcept(
        first_slice_is_full_extent<Slices...>::value
    ) -> decltype(submdspan_mapping(as_base(src), slices...)) {
      return submdspan_mapping(as_base(src), slices...);
    }
  };
};

template <class NestedMapping>
class LayoutTransposeSubmdspanTest : public ::testing::Test {};

TYPED_TEST_SUITE(LayoutTransposeSubmdspanTest, test::rank2_standard_layout_mappings, );

/// @test @c submdspan of a @c layout_transpose with full-range @c extent_slice on each dimension returns the transposed
/// mapping
CONSTEXPR_TYPED_TEST(LayoutTransposeSubmdspanTest, FullExtentSliceReTransposesResult) {
  auto const nested = make_test_src<TypeParam>().mapping();
  transposed_mapping_for<TypeParam> const transposed{nested};

  auto const transposed_result = do_submdspan_mapping(transposed, full_extent, full_extent);
  auto const nested_result = do_submdspan_mapping(nested, full_extent, full_extent);

  using nested_layout = typename decltype(nested_result.mapping)::layout_type;
  using transposed_layout = typename decltype(transposed_result.mapping)::layout_type;
  static_assert(
      std::is_same<transposed_layout, layout_transpose<nested_layout>>::value,
      "rank-2 result must be re-transposed around the nested sub-mapping's layout"
  );

  CONSTEXPR_ASSERT_EQ(transposed_result.offset, nested_result.offset);
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.extents().extent(0), nested_result.mapping.extents().extent(1));
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.extents().extent(1), nested_result.mapping.extents().extent(0));
}

/// @test @c submdspan of a @c layout_transpose with a rank-reducing (integral) slice on the first dimension drops the
/// transpose and passes the nested result through unchanged
CONSTEXPR_TYPED_TEST(LayoutTransposeSubmdspanTest, IntegralSliceOnDimension0DropsTranspose) {
  using index_type = typename TypeParam::index_type;

  auto const nested = make_test_src<TypeParam>().mapping();
  transposed_mapping_for<TypeParam> const transposed{nested};

  auto const transposed_result = do_submdspan_mapping(transposed, index_type{0}, full_extent);
  auto const nested_result = do_submdspan_mapping(nested, full_extent, index_type{0});

  static_assert(
      std::is_same<decltype(transposed_result.mapping), decltype(nested_result.mapping)>::value,
      "rank-1 result must pass through as the nested sub-mapping, not a layout_transpose"
  );
  CONSTEXPR_ASSERT_EQ(decltype(transposed_result.mapping)::extents_type::rank(), 1U);

  CONSTEXPR_ASSERT_EQ(transposed_result.offset, nested_result.offset);
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.extents().extent(0), nested_result.mapping.extents().extent(0));
}

/// @test @c submdspan of a @c layout_transpose with a rank-reducing (integral) slice on the second dimension drops the
/// transpose and passes the nested result through unchanged
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    LayoutTransposeSubmdspanTest,
    IntegralSliceOnDimension1DropsTranspose,
    arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using index_type = typename TypeParam::index_type;

  auto const nested = make_test_src<TypeParam>().mapping();
  transposed_mapping_for<TypeParam> const transposed{nested};

  auto const transposed_result = do_submdspan_mapping(transposed, full_extent, index_type{0});
  auto const nested_result = do_submdspan_mapping(nested, index_type{0}, full_extent);

  static_assert(
      std::is_same<decltype(transposed_result.mapping), decltype(nested_result.mapping)>::value,
      "rank-1 result must pass through as the nested sub-mapping, not a layout_transpose"
  );
  CONSTEXPR_ASSERT_EQ(decltype(transposed_result.mapping)::extents_type::rank(), 1U);

  CONSTEXPR_ASSERT_EQ(transposed_result.offset, nested_result.offset);
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.extents().extent(0), nested_result.mapping.extents().extent(0));
}

/// @test @c submdspan of a @c layout_transpose with two integral slices collapses to a rank-0 result at the same offset
/// as the nested mapping
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    LayoutTransposeSubmdspanTest,
    TwoIntegralSlicesCollapseToRankZero,
    arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using index_type = typename TypeParam::index_type;

  auto const nested = make_test_src<TypeParam>().mapping();
  transposed_mapping_for<TypeParam> const transposed{nested};

  // Distinct indices so a mistaken index swap would change the offset. Reversed for the nested comparison.
  auto const transposed_result = do_submdspan_mapping(transposed, index_type{1}, index_type{0});
  auto const nested_result = do_submdspan_mapping(nested, index_type{0}, index_type{1});

  CONSTEXPR_ASSERT_EQ(decltype(transposed_result.mapping)::extents_type::rank(), 0U);
  CONSTEXPR_ASSERT_EQ(transposed_result.offset, nested_result.offset);
}

/// @test @c submdspan of a @c layout_transpose with a non-unit-stride @c extent_slice degrades the nested sub-mapping
/// to @c layout_stride, and the rank-2 result is re-transposed around that @c layout_stride sub-mapping
CONSTEXPR_TEST(LayoutTransposeSubmdspan, NonUnitStrideSliceReTransposesLayoutStride) {
  using nested_extents = extents<std::size_t, 3U, 6U>;
  using transposed_extents = extents<std::size_t, 6U, 3U>;
  arene::base::layout_right::mapping<nested_extents> const nested{};
  layout_transpose<arene::base::layout_right>::mapping<transposed_extents> const transposed{nested};

  auto const transposed_result = do_submdspan_mapping(transposed, strided_slice{0, 3}, full_extent);
  auto const nested_result = do_submdspan_mapping(nested, full_extent, strided_slice{0, 3});

  using nested_layout = typename decltype(nested_result.mapping)::layout_type;
  using transposed_layout = typename decltype(transposed_result.mapping)::layout_type;
  static_assert(std::is_same<nested_layout, layout_stride>::value, "nested result must degrade to layout_stride");
  static_assert(
      std::is_same<transposed_layout, layout_transpose<layout_stride>>::value,
      "a degraded rank-2 result must be re-transposed around layout_stride"
  );

  CONSTEXPR_ASSERT_EQ(transposed_result.offset, nested_result.offset);
  // The re-transpose swaps both the extents and the strides of the nested layout_stride sub-mapping.
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.extents().extent(0), nested_result.mapping.extents().extent(1));
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.extents().extent(1), nested_result.mapping.extents().extent(0));
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.stride(0), nested_result.mapping.stride(1));
  CONSTEXPR_ASSERT_EQ(transposed_result.mapping.stride(1), nested_result.mapping.stride(0));
}

/// @test A rank-2 @c submdspan of a transposed mdspan aliases the same elements as transposed indexing of the parent
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    LayoutTransposeSubmdspanTest,
    EndToEndRank2AliasesParent,
    arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using index_type = typename TypeParam::index_type;
  using nested_src_type = decltype(make_test_src<TypeParam>());
  using parent_extents = reverse_extents_t<typename TypeParam::extents_type>;
  using parent_type = arene::base::mdspan<
      int const,
      parent_extents,
      layout_transpose<typename TypeParam::layout_type>,
      typename nested_src_type::accessor_type>;

  auto const nested_src = make_test_src<TypeParam>();
  transposed_mapping_for<TypeParam> const layout{nested_src.mapping()};
  parent_type const parent{nested_src.data_handle(), layout};

  // Take the sub-range of transposed rows [1, extent0) and all transposed columns.
  auto const sub = arene::base::submdspan(parent, range_slice{index_type{1}, parent.extent(0)}, full_extent);

  CONSTEXPR_ASSERT_EQ(sub.extent(0), static_cast<index_type>(parent.extent(0) - index_type{1}));
  CONSTEXPR_ASSERT_EQ(sub.extent(1), parent.extent(1));
  for (index_type i{}; i < sub.extent(0); ++i) {
    for (index_type j{}; j < sub.extent(1); ++j) {
      CONSTEXPR_ASSERT_EQ(&sub(i, j), &parent(static_cast<index_type>(i + index_type{1}), j));
    }
  }
}

/// @brief Assert that a rank-2 @c submdspan taken with a @c range_slice on both transposed dimensions aliases the
/// expected parent elements, failing on the specific property that mismatched
/// @param parent The transposed parent mdspan to slice
/// @param first0 The inclusive start of the half-open range along transposed dimension 0
/// @param first1 The inclusive start of the half-open range along transposed dimension 1
///
/// The sub-ranges are <c>[first0, extent0)</c> and <c>[first1, extent1)</c>, so both offsets are exercised while the
/// sub-view stays in bounds. Comparing against @c parent(i + first0, j + first1) with independent offsets verifies that
/// each transposed dimension's offset is applied to the correct nested dimension. This is a macro rather than a
/// function so each property is checked with @c CONSTEXPR_ASSERT_EQ and a failure reports the offending line instead of
/// a collapsed @c bool.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ASSERT_RANGE_SLICES_ALIAS_PARENT(parent, first0, first1)                                                \
  do { /* NOLINT(cppcoreguidelines-avoid-do-while) */                                                           \
    using slice_index_type = decltype((parent).extent(0));                                                      \
    auto const sub = arene::base::submdspan(                                                                    \
        (parent),                                                                                               \
        range_slice{(first0), (parent).extent(0)},                                                              \
        range_slice{(first1), (parent).extent(1)}                                                               \
    );                                                                                                          \
    CONSTEXPR_ASSERT_EQ(sub.extent(0), static_cast<slice_index_type>((parent).extent(0) - (first0)));           \
    CONSTEXPR_ASSERT_EQ(sub.extent(1), static_cast<slice_index_type>((parent).extent(1) - (first1)));           \
    for (slice_index_type i{}; i < sub.extent(0); ++i) {                                                        \
      for (slice_index_type j{}; j < sub.extent(1); ++j) {                                                      \
        CONSTEXPR_ASSERT_EQ(                                                                                    \
            &sub(i, j),                                                                                         \
            &(parent)(static_cast<slice_index_type>(i + (first0)), static_cast<slice_index_type>(j + (first1))) \
        );                                                                                                      \
      }                                                                                                         \
    }                                                                                                           \
  } while (false)

/// @test A rank-2 @c submdspan taken with a @c range_slice on both transposed dimensions aliases the parent for every
/// combination of zero and non-zero offsets
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    LayoutTransposeSubmdspanTest,
    EndToEndRank2RangeSlicesVaryingOffsets,
    arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using index_type = typename TypeParam::index_type;
  using nested_src_type = decltype(make_test_src<TypeParam>());
  using parent_extents = reverse_extents_t<typename TypeParam::extents_type>;
  using parent_type = arene::base::mdspan<
      int const,
      parent_extents,
      layout_transpose<typename TypeParam::layout_type>,
      typename nested_src_type::accessor_type>;

  auto const nested_src = make_test_src<TypeParam>();
  transposed_mapping_for<TypeParam> const layout{nested_src.mapping()};
  parent_type const parent{nested_src.data_handle(), layout};

  constexpr index_type zero{0};
  constexpr index_type one{1};
  ASSERT_RANGE_SLICES_ALIAS_PARENT(parent, zero, zero);
  ASSERT_RANGE_SLICES_ALIAS_PARENT(parent, zero, one);
  ASSERT_RANGE_SLICES_ALIAS_PARENT(parent, one, zero);
  ASSERT_RANGE_SLICES_ALIAS_PARENT(parent, one, one);
}

#undef ASSERT_RANGE_SLICES_ALIAS_PARENT

/// @test A rank-2 @c submdspan of a transposed custom (non-standard) layout yields the expected element values
CONSTEXPR_TEST(LayoutTransposeSubmdspan, EndToEndCustomNestedLayout) {
  using nested_layout = test::user_defined_layout</*Strided=*/true, /*Exhaustive=*/true, /*Unique=*/true>;
  using nested_extents = extents<std::size_t, 3U, 4U>;
  using nested_mapping = nested_layout::mapping<nested_extents>;

  nested_mapping const nested{/*is_strided=*/true};
  layout_transpose<nested_layout>::mapping<extents<std::size_t, 4U, 3U>> const layout{nested};
  arene::base::mdspan<int const, extents<std::size_t, 4U, 3U>, layout_transpose<nested_layout>> const parent{
      distinct_buffer.data(),
      layout
  };

  // The custom layout is sliceable because it is wrapped in a layout_transpose.
  static_assert(
      arene::base::is_sliceable_mapping_v<layout_transpose<nested_layout>::mapping<extents<std::size_t, 4U, 3U>>>,
      "a transposed custom layout must be sliceable"
  );

  auto const sub = arene::base::submdspan(parent, full_extent, range_slice{0, 2});

  CONSTEXPR_ASSERT_EQ(sub.extent(0), std::size_t{4});
  CONSTEXPR_ASSERT_EQ(sub.extent(1), std::size_t{2});
  for (std::size_t i{}; i < sub.extent(0); ++i) {
    for (std::size_t j{}; j < sub.extent(1); ++j) {
      CONSTEXPR_ASSERT_EQ(sub(i, j), parent(i, j));
    }
  }
}

/// @test @c submdspan of a @c layout_transpose with an empty @c range_slice on one transposed dimension produces a
/// rank-2 view with a zero extent on that dimension while preserving the other extent
CONSTEXPR_TYPED_TEST(LayoutTransposeSubmdspanTest, EmptyRangeSliceProducesZeroExtent) {
  using index_type = typename TypeParam::index_type;
  using nested_src_type = decltype(make_test_src<TypeParam>());
  using parent_extents = reverse_extents_t<typename TypeParam::extents_type>;
  using parent_type = arene::base::mdspan<
      int const,
      parent_extents,
      layout_transpose<typename TypeParam::layout_type>,
      typename nested_src_type::accessor_type>;

  auto const nested_src = make_test_src<TypeParam>();
  transposed_mapping_for<TypeParam> const layout{nested_src.mapping()};
  parent_type const parent{nested_src.data_handle(), layout};

  // An empty [0, 0) range on transposed dimension 0 zeroes that extent; the full range keeps dimension 1.
  auto const sub = arene::base::submdspan(parent, range_slice{index_type{0}, index_type{0}}, full_extent);

  CONSTEXPR_ASSERT_EQ(decltype(sub)::extents_type::rank(), 2U);
  CONSTEXPR_ASSERT_EQ(sub.extent(0), index_type{0});
  CONSTEXPR_ASSERT_EQ(sub.extent(1), parent.extent(1));
  CONSTEXPR_ASSERT_EQ(sub.size(), std::size_t{0});
}

/// @test A @c layout_transpose of a non-sliceable nested layout is itself not sliceable
CONSTEXPR_TEST(LayoutTransposeSubmdspan, NonSliceableNestedLayoutIsNotSliceable) {
  using nested_layout = test::basic_layout_mapping<extents<std::size_t, 3U, 4U>>::layout_type;
  using transposed_mapping = layout_transpose<nested_layout>::mapping<extents<std::size_t, 4U, 3U>>;

  static_assert(
      !arene::base::is_sliceable_mapping_v<transposed_mapping>,
      "a transpose of a non-sliceable layout must not be sliceable"
  );
}

/// @test A @c layout_transpose of a nested layout that *does* provide a @c submdspan_mapping which violates the
/// sliceable requirements is itself not sliceable
///
/// The nested layout provides a reachable but non-conforming @c submdspan_mapping: it is unconstrained, so it accepts
/// invalid slice specifiers. Because @c layout_transpose forwards to the nested @c submdspan_mapping, @c
/// is_sliceable_mapping_v checks the nested overload transitively, so the defect propagates and leaves the transpose
/// non-sliceable even though the overload exists.
CONSTEXPR_TEST(LayoutTransposeSubmdspan, NonConformingNestedSubmdspanMappingIsNotSliceable) {
  using nested_layout = test::basic_mapping_policy<test::unconstrained_submdspan_mapping_layout>;
  using transposed_mapping = layout_transpose<nested_layout>::mapping<extents<std::size_t, 4U, 3U>>;

  static_assert(
      !arene::base::is_sliceable_mapping_v<transposed_mapping>,
      "a transpose of a nested layout that accepts invalid slices must not be sliceable"
  );
}

/// @test @c layout_transpose::submdspan_mapping propagates the nested layout's @c noexcept specification
///
/// A standard (@c layout_right) nested layout has a @c noexcept @c submdspan_mapping, so the transpose's must be
/// @c noexcept; a user-defined nested layout with a potentially-throwing @c submdspan_mapping must make the transpose's
/// potentially-throwing as well, rather than being unconditionally @c noexcept.
CONSTEXPR_TEST(LayoutTransposeSubmdspan, SubmdspanMappingPropagatesNestedNoexcept) {
  using nothrow_transposed = layout_transpose<arene::base::layout_right>::mapping<extents<std::size_t, 4U, 3U>>;
  using throwing_nested = test::user_defined_layout<
      /*Strided=*/true,
      /*Exhaustive=*/true,
      /*Unique=*/true,
      /*SubmdspanNoexcept=*/false>;
  using throwing_transposed = layout_transpose<throwing_nested>::mapping<extents<std::size_t, 4U, 3U>>;

  static_assert(
      noexcept(do_submdspan_mapping(std::declval<nothrow_transposed const&>(), full_extent, full_extent)),
      "a transpose of a noexcept nested submdspan_mapping must be noexcept"
  );
  static_assert(
      !noexcept(do_submdspan_mapping(std::declval<throwing_transposed const&>(), full_extent, full_extent)),
      "a transpose of a potentially-throwing nested submdspan_mapping must be potentially-throwing"
  );
}

/// @test @c layout_transpose::submdspan_mapping computes its @c noexcept specification from the *reversed* slice order,
/// matching the reversed nested call it performs
CONSTEXPR_TEST(LayoutTransposeSubmdspan, SubmdspanMappingNoexceptReversesSliceOrder) {
  using order_sensitive_transposed =
      layout_transpose<order_sensitive_submdspan_layout>::mapping<extents<std::size_t, 4U, 3U>>;
  using index_type = order_sensitive_transposed::index_type;

  CONSTEXPR_ASSERT(
      noexcept(do_submdspan_mapping(std::declval<order_sensitive_transposed const&>(), index_type{0}, full_extent))
  );
  CONSTEXPR_ASSERT_FALSE(
      noexcept(do_submdspan_mapping(std::declval<order_sensitive_transposed const&>(), full_extent, index_type{0}))
  );
}

/// @test @c submdspan of a transposed mapping is invocable with exactly two slices and not with any other count
CONSTEXPR_TEST(LayoutTransposeSubmdspan, InvocableWithExactlyTwoSlices) {
  using parent_type =
      arene::base::mdspan<int const, extents<std::size_t, 4U, 3U>, layout_transpose<arene::base::layout_right>>;

  CONSTEXPR_ASSERT((arene::base::is_invocable_v<
                    decltype(arene::base::submdspan),
                    parent_type,
                    arene::base::full_extent_t,
                    arene::base::full_extent_t>));
  // A transposed mapping is always rank 2, so one or three slices must be rejected by the rank constraint.
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(arene::base::submdspan), parent_type, arene::base::full_extent_t>)
  );
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<
                          decltype(arene::base::submdspan),
                          parent_type,
                          arene::base::full_extent_t,
                          arene::base::full_extent_t,
                          arene::base::full_extent_t>));
}

}  // namespace
