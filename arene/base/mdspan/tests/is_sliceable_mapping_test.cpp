// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/is_sliceable_mapping.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/detail/all_valid_submdspan_slice_types_for.hpp"
#include "arene/base/mdspan/detail/do_submdspan_mapping.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/submdspan_mapping_result.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using test::basic_layout_mapping;
using test::basic_mapping_policy;

template <typename TypeParam>
class LayoutTest : public test::layout_test_base<TypeParam> {};

template <typename TypeParam>
class LayoutTest2 : public test::layout_test_base<TypeParam> {};

TYPED_TEST_SUITE(LayoutTest, test::layout_types, );

/// @test A type that is not a layout mapping does not satisfy the `is_sliceable_mapping_v` type trait
TEST(LayoutTest, NonLayoutMappingIsNotSliceable) { STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<int>); }

/// @test All of the built-in layouts have a mapping which satisfies the `is_sliceable_mapping_v` type trait
TYPED_TEST(LayoutTest, LayoutMappingIsSliceable) { STATIC_ASSERT_TRUE(arene::base::is_sliceable_mapping_v<TypeParam>); }

/// @test verify that is_sliceable_mapping_v is false when the layout mapping does not implement submdspan_mapping
TEST(LayoutTest, NoSubmdspanMapping) {
  // basic_layout_mapping must itself be a valid layout mapping, so that the assertions below fail
  // is_sliceable_mapping_v specifically because it lacks a submdspan_mapping overload, and not because it fails the
  // layout mapping requirements.
  STATIC_ASSERT_TRUE(arene::base::is_layout_mapping_v<basic_layout_mapping<arene::base::extents<std::size_t>>>);
  STATIC_ASSERT_TRUE(arene::base::is_layout_mapping_v<basic_layout_mapping<arene::base::extents<std::int32_t>>>);
  STATIC_ASSERT_TRUE(
      (arene::base::is_layout_mapping_v<basic_layout_mapping<arene::base::extents<std::int32_t, 1, 2, 3, 4>>>)
  );

  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<basic_layout_mapping<arene::base::extents<std::size_t>>>);
  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<basic_layout_mapping<arene::base::extents<std::int32_t>>>);
  STATIC_ASSERT_FALSE(
      (arene::base::is_sliceable_mapping_v<basic_layout_mapping<arene::base::extents<std::int32_t, 1, 2, 3, 4>>>)
  );
}

/// @brief Layout whose submdspan_mapping unconditionally returns an int
/// @tparam Extents layout extents type
template <typename Extents>
class invalid_submdspan_mapping_layout {
 public:
  using rank_type = std::size_t;
  using extents_type = Extents;
  using index_type = typename Extents::index_type;

  using layout_type = basic_mapping_policy<invalid_submdspan_mapping_layout>;

  auto operator==(invalid_submdspan_mapping_layout const&) const -> bool;

  auto extents() const -> extents_type const&;
  template <typename... Indices>
  auto operator()(Indices...) const -> index_type;

  auto required_span_size() const -> index_type;
  auto is_unique() const -> bool;
  auto is_exhaustive() const -> bool;
  auto is_strided() const -> bool;

  auto stride(rank_type rank) const -> index_type;

  static constexpr auto is_always_unique() -> bool { return false; }
  static constexpr auto is_always_exhaustive() -> bool { return false; }
  static constexpr auto is_always_strided() -> bool { return false; }

  // submdspan_mapping is supposed to return specializations of
  // submdspan_mapping_result. Return an int instead to verify this type of
  // incorrectness is detected.
  template <
      class... CanonicalSliceSpecifiers,
      arene::base::constraints<
          std::enable_if_t<sizeof...(CanonicalSliceSpecifiers) == extents_type::rank()>,
          std::enable_if_t<arene::base::mdspan_detail::
                               all_valid_submdspan_slice_types_for_v<extents_type, CanonicalSliceSpecifiers...>>> =
          nullptr>
  friend constexpr auto submdspan_mapping(invalid_submdspan_mapping_layout const&, CanonicalSliceSpecifiers...) noexcept
      -> int {
    return {};
  }
};

/// @test verify that is_sliceable_mapping_v is false when the layout mapping implements submdspan_mapping incorrectly
TEST(LayoutTest, InvalidSubmdspanMapping) {
  // submdspan_mapping must be invocable for this layout, so that the assertions below fail is_sliceable_mapping_v
  // specifically because submdspan_mapping returns the wrong type (an int) rather than because it is not invocable.
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<
                      decltype(arene::base::mdspan_detail::do_submdspan_mapping),
                      invalid_submdspan_mapping_layout<arene::base::extents<std::size_t>>>));

  // rank 0
  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<
                      invalid_submdspan_mapping_layout<arene::base::extents<std::size_t>>>);
  // rank 1
  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<
                      invalid_submdspan_mapping_layout<arene::base::extents<std::size_t, 3>>>);
  // rank > 1
  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<
                      invalid_submdspan_mapping_layout<arene::base::extents<std::size_t, 3, 4, 5, 6>>>);
}

/// @brief Layout whose submdspan_mapping unconditionally returns a
/// submdspan_mapping_result whose rank does not depend on the number of
/// non-collapsing slice specifiers.
/// @tparam Extents layout extents type
template <typename Extents>
class wrong_rank_submdspan_mapping_layout {
 public:
  using rank_type = std::size_t;
  using extents_type = Extents;
  using index_type = typename Extents::index_type;

  using layout_type = basic_mapping_policy<wrong_rank_submdspan_mapping_layout>;

  auto operator==(wrong_rank_submdspan_mapping_layout const&) const -> bool;

  auto extents() const -> extents_type const&;
  template <typename... Indices>
  auto operator()(Indices...) const -> index_type;

  auto required_span_size() const -> index_type;
  auto is_unique() const -> bool;
  auto is_exhaustive() const -> bool;
  auto is_strided() const -> bool;

  auto stride(rank_type rank) const -> index_type;

  static constexpr auto is_always_unique() -> bool { return false; }
  static constexpr auto is_always_exhaustive() -> bool { return false; }
  static constexpr auto is_always_strided() -> bool { return false; }

  template <
      class... CanonicalSliceSpecifiers,
      arene::base::constraints<
          std::enable_if_t<sizeof...(CanonicalSliceSpecifiers) == extents_type::rank()>,
          std::enable_if_t<arene::base::mdspan_detail::
                               all_valid_submdspan_slice_types_for_v<extents_type, CanonicalSliceSpecifiers...>>> =
          nullptr>
  friend constexpr auto
  submdspan_mapping(wrong_rank_submdspan_mapping_layout const&, CanonicalSliceSpecifiers...) noexcept
      -> arene::base::submdspan_mapping_result<arene::base::layout_left::mapping<extents_type>> {
    return {};
  }
};

/// @test 4.3 define a layout with a mapping which defines a submdspan_mapping which has incorrect rank
TEST(LayoutTest, WrongRankSubmdspanMapping) {
  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<
                      wrong_rank_submdspan_mapping_layout<arene::base::extents<std::size_t, 1>>>);
}

/// @brief Layout whose submdspan_mapping returns a submdspan_mapping_result whose mapping has an index_type that does
/// not match the source mapping's index_type.
/// @tparam Extents layout extents type
///
/// This is fixed at rank 0 so that both the "invalid slices are ill-formed" clause and the rank clauses hold trivially
/// (0 == 0), isolating the index_type-match requirement as the sole reason the layout is not sliceable.
template <typename Extents>
class wrong_index_type_submdspan_mapping_layout {
  static_assert(Extents::rank() == 0U, "helper is only meaningful at rank 0");

  /// @brief Extents for the returned result: same (empty) shape but a deliberately mismatched index_type
  using result_extents_type = arene::base::extents<std::int32_t>;
  static_assert(
      !std::is_same<typename Extents::index_type, typename result_extents_type::index_type>::value,
      "the result index_type must differ from the source index_type for this helper to test what it claims"
  );

 public:
  using rank_type = std::size_t;
  using extents_type = Extents;
  using index_type = typename Extents::index_type;

  using layout_type = basic_mapping_policy<wrong_index_type_submdspan_mapping_layout>;

  auto operator==(wrong_index_type_submdspan_mapping_layout const&) const -> bool;

  auto extents() const -> extents_type const&;
  template <typename... Indices>
  auto operator()(Indices...) const -> index_type;

  auto required_span_size() const -> index_type;
  auto is_unique() const -> bool;
  auto is_exhaustive() const -> bool;
  auto is_strided() const -> bool;

  auto stride(rank_type rank) const -> index_type;

  static constexpr auto is_always_unique() -> bool { return false; }
  static constexpr auto is_always_exhaustive() -> bool { return false; }
  static constexpr auto is_always_strided() -> bool { return false; }

  template <
      class... CanonicalSliceSpecifiers,
      arene::base::constraints<
          std::enable_if_t<sizeof...(CanonicalSliceSpecifiers) == extents_type::rank()>,
          std::enable_if_t<arene::base::mdspan_detail::
                               all_valid_submdspan_slice_types_for_v<extents_type, CanonicalSliceSpecifiers...>>> =
          nullptr>
  friend constexpr auto
  submdspan_mapping(wrong_index_type_submdspan_mapping_layout const&, CanonicalSliceSpecifiers...) noexcept
      -> arene::base::submdspan_mapping_result<arene::base::layout_left::mapping<result_extents_type>> {
    return {};
  }
};

/// @test verify that is_sliceable_mapping_v is false when the result mapping's index_type does not match the source's
TEST(LayoutTest, WrongIndexTypeSubmdspanMapping) {
  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<
                      wrong_index_type_submdspan_mapping_layout<arene::base::extents<std::size_t>>>);
}

/// @brief Layout whose submdspan_mapping is unconstrained, so it accepts invalid slice specifiers as well as valid
/// ones. Shared with the layout_transpose tests, which wrap it at rank 2.
using test::unconstrained_submdspan_mapping_layout;

/// @test verify that is_sliceable_mapping_v is false when submdspan_mapping accepts invalid slice specifiers
///
/// The extents are fixed at rank 1 with a static extent of 0 so that both the all-@c full_extent_t slice list and the
/// collapsing slice list reduce to a single non-collapsing slice. The fixed rank-1 result therefore satisfies every
/// other sliceable requirement, isolating the "invalid slices must be ill-formed" clause as the sole reason this
/// layout is not sliceable.
TEST(LayoutTest, UnconstrainedSubmdspanMapping) {
  STATIC_ASSERT_FALSE(arene::base::is_sliceable_mapping_v<
                      unconstrained_submdspan_mapping_layout<arene::base::extents<std::size_t, 0>>>);
}

// --- Direct tests of the is_valid_submdspan_mapping_result detail trait ---
//
// These white-box tests exercise arene::base::is_sliceable_mapping_detail::is_valid_submdspan_mapping_result
// directly. It validates a single submdspan_mapping_result against an expected index_type and an expected result rank
// (the number of non-collapsing slices), and is the building block that is_sliceable_mapping_v evaluates for both the
// all-full_extent slice list and the collapsing slice list. Testing it directly isolates each requirement (result
// shape, rank, index_type) without having to hand-craft a layout whose submdspan_mapping produces the exact result
// under test.

namespace detail = arene::base::is_sliceable_mapping_detail;

template <class Extents>
using left_result = arene::base::submdspan_mapping_result<arene::base::layout_left::mapping<Extents>>;

/// @test the primary template is false for a first argument that is not a submdspan_mapping_result specialization
TEST(IsValidSubmdspanMappingResult, FalseForNonSubmdspanMappingResult) {
  STATIC_ASSERT_FALSE((detail::is_valid_submdspan_mapping_result<int, std::size_t, 1U>));
}

/// @test true when the result rank, index_type, and expected non-collapsing slice count all agree
TEST(IsValidSubmdspanMappingResult, TrueWhenResultMatchesExpectedRank) {
  // rank 0: empty result, zero non-collapsing slices
  STATIC_ASSERT_TRUE(
      (detail::is_valid_submdspan_mapping_result<left_result<arene::base::extents<std::size_t>>, std::size_t, 0U>)
  );

  // rank 1 result, one non-collapsing slice
  STATIC_ASSERT_TRUE(
      (detail::is_valid_submdspan_mapping_result<left_result<arene::base::extents<std::size_t, 3>>, std::size_t, 1U>)
  );

  // rank 2 result, two non-collapsing slices
  STATIC_ASSERT_TRUE(
      (detail::is_valid_submdspan_mapping_result<left_result<arene::base::extents<std::size_t, 3, 4>>, std::size_t, 2U>)
  );
}

/// @test false when the result rank does not equal the expected number of non-collapsing slices
TEST(IsValidSubmdspanMappingResult, FalseForRankMismatch) {
  // expect two non-collapsing slices but a rank-1 result
  STATIC_ASSERT_FALSE(
      (detail::is_valid_submdspan_mapping_result<left_result<arene::base::extents<std::size_t, 3>>, std::size_t, 2U>)
  );

  // expect one non-collapsing slice but a rank-2 result
  STATIC_ASSERT_FALSE(
      (detail::is_valid_submdspan_mapping_result<left_result<arene::base::extents<std::size_t, 3, 4>>, std::size_t, 1U>)
  );
}

/// @test false when the result mapping's index_type does not match the expected index_type
TEST(IsValidSubmdspanMappingResult, FalseForIndexTypeMismatch) {
  STATIC_ASSERT_FALSE(
      (detail::is_valid_submdspan_mapping_result<left_result<arene::base::extents<std::int32_t, 3>>, std::size_t, 1U>)
  );
}

/// @test the non-collapsing slice mask marks full_extent_t and extent_slice specializations as non-collapsing and
/// everything else (e.g. an index) as collapsing
TEST(NonCollapsingSliceMask, ClassifiesSliceTypes) {
  constexpr auto mask =
      detail::non_collapsing_slice_mask<arene::base::full_extent_t, int, arene::base::extent_slice<int>>;
  STATIC_ASSERT_TRUE(mask[0]);   // full_extent_t: non-collapsing
  STATIC_ASSERT_FALSE(mask[1]);  // int: collapsing
  STATIC_ASSERT_TRUE(mask[2]);   // extent_slice: non-collapsing
}

/// @test non_collapsing_slice_count returns the number of non-collapsing slices in a type_list of slice types
TEST(NonCollapsingSliceCount, CountsNonCollapsingSlices) {
  // empty list: zero
  STATIC_ASSERT_TRUE((detail::non_collapsing_slice_count<arene::base::type_list<>> == 0U));

  // all non-collapsing (full_extent_t and extent_slice)
  STATIC_ASSERT_TRUE(
      (detail::non_collapsing_slice_count<
           arene::base::type_list<arene::base::full_extent_t, arene::base::extent_slice<int>>> == 2U)
  );

  // mixed: two non-collapsing (full_extent_t, extent_slice) and one collapsing (int)
  STATIC_ASSERT_TRUE(
      (detail::non_collapsing_slice_count<
           arene::base::type_list<arene::base::full_extent_t, int, arene::base::extent_slice<int>>> == 2U)
  );

  // all collapsing (indices): zero
  STATIC_ASSERT_TRUE((detail::non_collapsing_slice_count<arene::base::type_list<int, int>> == 0U));
}

}  // namespace
