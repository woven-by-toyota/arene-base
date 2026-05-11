// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/submdspan.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/transform.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/functional/bind_back.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/mdspan/tests/throwing_slice_conversions.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/minus.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/apply.hpp"
#include "arene/base/tuple/detail/tuple_transform.hpp"
#include "arene/base/tuple/tuple_cat.hpp"
#include "arene/base/type_list/cartesian_product.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/flat_map.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_manipulation/repeat_type.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_tuple_like.hpp"
// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

namespace {

/// @brief Compute the required span size for a layout mapping type with expanded test extents
/// @tparam Mapping The layout mapping type
template <class Mapping>
constexpr auto required_test_buffer_size() noexcept -> std::size_t {
  return static_cast<std::size_t>(arene::base::testing::make_mapping_with_extents<Mapping>(
                                      test::make_expanded_extents<typename Mapping::extents_type>()
  )
                                      .required_span_size());
}

/// @brief Zero-initialized data buffer sized to the required span size for a given mapping type
/// @tparam N buffer size
template <std::size_t N>
constexpr auto test_buffer = arene::base::array<int, N>{};

/// @brief Construct a source mdspan for the given layout mapping type from a correctly-sized test buffer
/// @tparam Mapping The layout mapping type
template <class Mapping>
constexpr auto make_test_src() noexcept
    -> arene::base::mdspan<int const, typename Mapping::extents_type, typename Mapping::layout_type> {
  auto const exts = test::make_expanded_extents<typename Mapping::extents_type>();
  auto const map = arene::base::testing::make_mapping_with_extents<Mapping>(exts);
  return {test_buffer<required_test_buffer_size<Mapping>()>.data(), map};
}

/// @brief Return a copy of an array with the first @c Count elements removed
/// @tparam Count the number of leading elements to drop
/// @tparam T the element type
/// @tparam N the original array size (must be >= @c Count)
/// @tparam Is index pack for the retained elements
template <std::size_t Count, class T, std::size_t N, std::size_t... Is>
constexpr auto drop_front_impl(arene::base::array<T, N> const& arr, std::index_sequence<Is...>) noexcept
    -> arene::base::array<T, N - Count> {
  return {{arr[Is + Count]...}};
}

/// @brief Return a copy of an array with the first @c Count elements removed
/// @tparam Count the number of leading elements to drop
/// @tparam T the element type
/// @tparam N the original array size (must be >= @c Count)
template <std::size_t Count, class T, std::size_t N>
constexpr auto drop_front(arene::base::array<T, N> const& arr) noexcept -> arene::base::array<T, N - Count> {
  return drop_front_impl<Count>(arr, std::make_index_sequence<N - Count>{});
}

/// @brief Wrap a non-tuple-like argument in @c std::make_tuple; forward a tuple-like one unchanged
/// @{
template <class T, arene::base::constraints<std::enable_if_t<arene::base::is_tuple_like_v<T>>> = nullptr>
constexpr auto as_tuple(T&& arg) noexcept {
  return std::forward<T>(arg);
}

template <class T, arene::base::constraints<std::enable_if_t<!arene::base::is_tuple_like_v<T>>> = nullptr>
constexpr auto as_tuple(T&& arg) noexcept {
  return std::make_tuple(std::forward<T>(arg));
}
/// @}

/// @brief Concatenate slice arguments into a single tuple, auto-wrapping non-tuple-like scalars
/// @tparam Args mix of tuple-like slice packs and scalar slices
/// @param args slice packs and/or individual slices, in the order they appear in the submdspan call
template <class... Args>
constexpr auto concat_slices(Args&&... args) noexcept {
  return arene::base::tuple_cat(as_tuple(std::forward<Args>(args))...);
}

/// @brief Trait that is @c true when @p Result 's accessor type matches @p Src 's accessor's @c offset_policy
/// @tparam Src source mdspan type
/// @tparam Result submdspan result mdspan type
template <class Src, class Result>
constexpr bool result_accessor_matches_offset_policy_v =
    std::is_same<typename Result::accessor_type, typename Src::accessor_type::offset_policy>::value;

/// @brief Map a (source extent, slice) pair to the resulting submdspan extent value
struct slice_to_result_extent {
  template <class IndexType>
  constexpr auto operator()(IndexType src_ext, arene::base::full_extent_t) const noexcept -> IndexType {
    return src_ext;
  }

  template <class IndexType, class Offset, class Extent, class Stride>
  constexpr auto operator()(IndexType, arene::base::extent_slice<Offset, Extent, Stride> slice) const noexcept
      -> IndexType {
    return static_cast<IndexType>(slice.extent);
  }

  template <class IndexType, class First, class Last, class Stride>
  constexpr auto operator()(IndexType, arene::base::range_slice<First, Last, Stride> slice) const noexcept
      -> IndexType {
    return static_cast<IndexType>(slice.last - slice.first);
  }
};

/// @brief Map a slice to the index at which its submdspan view begins along that dimension
template <class IndexType>
struct slice_to_offset {
  constexpr auto operator()(arene::base::full_extent_t) const noexcept -> IndexType { return IndexType{}; }

  template <class Offset, class Extent, class Stride>
  constexpr auto operator()(arene::base::extent_slice<Offset, Extent, Stride> slice) const noexcept -> IndexType {
    return static_cast<IndexType>(slice.offset);
  }

  template <class First, class Last, class Stride>
  constexpr auto operator()(arene::base::range_slice<First, Last, Stride> slice) const noexcept -> IndexType {
    return static_cast<IndexType>(slice.first);
  }
};

/// @brief Functor that transforms an extent value into a full-range @c range_slice
/// @tparam IndexType the index type of the extents
template <class IndexType>
struct make_full_range_range_slice {
  using range_slice_type = arene::base::range_slice<IndexType, IndexType, std::integral_constant<IndexType, 1>>;

  constexpr auto operator()(IndexType ext) const noexcept -> range_slice_type { return {IndexType{}, ext}; }
};

/// @brief Functor that transforms an extent value into an empty @c range_slice at the end of the dimension
/// @tparam IndexType the index type of the extents
template <class IndexType>
struct make_empty_end_range_slice {
  using range_slice_type = arene::base::range_slice<IndexType, IndexType, std::integral_constant<IndexType, 1>>;

  constexpr auto operator()(IndexType ext) const noexcept -> range_slice_type { return {ext, ext}; }
};

/// @brief Functor that transforms an extent value into a single-element @c extent_slice at the last index
/// @tparam IndexType the index type of the extents
template <class IndexType>
struct make_last_element_extent_slice {
  using extent_slice_type = arene::base::extent_slice<IndexType, IndexType, std::integral_constant<IndexType, 1>>;

  constexpr auto operator()(IndexType ext) const noexcept -> extent_slice_type {
    ARENE_PRECONDITION(ext > 0);
    return {static_cast<IndexType>(ext - IndexType{1}), IndexType{1}};
  }
};

/// @brief Functor that transforms an extent value into a single-element @c range_slice at the last index
/// @tparam IndexType the index type of the extents
template <class IndexType>
struct make_last_element_range_slice {
  using range_slice_type = arene::base::range_slice<IndexType, IndexType, std::integral_constant<IndexType, 1>>;

  constexpr auto operator()(IndexType ext) const noexcept -> range_slice_type {
    ARENE_PRECONDITION(ext > 0);
    return {static_cast<IndexType>(ext - IndexType{1}), ext};
  }
};

/// @brief Custom integral-constant-like type for testing submdspan slice handling
/// @tparam Type the value type
/// @tparam Value the compile-time constant value
template <class Type, Type Value>
struct integral_constant_like {
  static constexpr auto value = Value;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator Type() const noexcept { return value; }
};

/// @brief Custom type implicitly convertible to an integer, for testing submdspan with convertible slice types
/// @tparam Type the target conversion type
template <class Type>
struct convertible_to {
  Type val;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator Type() const noexcept { return val; }
};

template <class T>
// NOLINTNEXTLINE(modernize-use-transparent-functors)
constexpr auto minus_one = arene::base::bind_back(std::minus<T>{}, T{1});

template <class Mapping>
class SubmdspanTypedTest : public test::layout_test_base<Mapping> {};

TYPED_TEST_SUITE(SubmdspanTypedTest, test::layout_types, );

/// @test @c submdspan with full-range @c extent_slice on each dimension covers entire src mdspan
CONSTEXPR_TYPED_TEST(SubmdspanTypedTest, FullRangeExtentSlicesCoversSrcMdspan) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  using slice_maker = arene::base::testing::make_full_range_extent_slice<index_type>;

  auto const full_extent_slices =
      arene::base::transform(slice_maker{}, arene::base::mdspan_detail::to_array(src.extents()));

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      full_extent_slices
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(src.data_handle(), result.data_handle());
  CONSTEXPR_ASSERT_EQ(src.extents(), result.extents());
}

/// @test @c submdspan with full-range @c range_slice on each dimension covers entire src mdspan
CONSTEXPR_TYPED_TEST(SubmdspanTypedTest, FullRangeRangeSlicesCoversSrcMdspan) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  using slice_maker = make_full_range_range_slice<index_type>;

  auto const full_range_slices =
      arene::base::transform(slice_maker{}, arene::base::mdspan_detail::to_array(src.extents()));

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      full_range_slices
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(src.data_handle(), result.data_handle());
  CONSTEXPR_ASSERT_EQ(src.extents(), result.extents());
}

/// @test @c submdspan with max-index slices reduces rank to zero at the correct offset
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    MaxIndexSlicesCollapseToRankZero,
    arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  auto const max_indices =
      arene::base::transform(minus_one<index_type>, arene::base::mdspan_detail::to_array(src.extents()));

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      max_indices
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), 0U);
  CONSTEXPR_ASSERT_EQ(result.data_handle(), &arene::base::apply(src, max_indices));
}

/// @test @c submdspan with single-element @c extent_slice at max index on each dimension offsets to the last element
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    MaxOffsetExtentSlicesPointToLastElement,
    arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  using slice_maker = make_last_element_extent_slice<index_type>;

  auto const slices = arene::base::transform(slice_maker{}, arene::base::mdspan_detail::to_array(src.extents()));

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      slices
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  auto const expected_extents = arene::base::tuple_detail::tuple_transform_to_array<index_type>(
      arene::base::mdspan_detail::to_array(src.extents()),
      slices,
      slice_to_result_extent{}
  );
  CONSTEXPR_ASSERT_EQ(arene::base::mdspan_detail::to_array(result.extents()), expected_extents);

  auto const max_indices =
      arene::base::tuple_detail::tuple_transform_to_array<index_type>(slices, slice_to_offset<index_type>{});
  CONSTEXPR_ASSERT_EQ(result.data_handle(), &arene::base::apply(src, max_indices));
}

/// @test @c submdspan with single-element @c range_slice at max index on each dimension offsets to the last element
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    MaxOffsetRangeSlicesPointToLastElement,
    arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  using slice_maker = make_last_element_range_slice<index_type>;

  auto const slices = arene::base::transform(slice_maker{}, arene::base::mdspan_detail::to_array(src.extents()));

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      slices
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  auto const expected_extents = arene::base::tuple_detail::tuple_transform_to_array<index_type>(
      arene::base::mdspan_detail::to_array(src.extents()),
      slices,
      slice_to_result_extent{}
  );
  CONSTEXPR_ASSERT_EQ(arene::base::mdspan_detail::to_array(result.extents()), expected_extents);

  auto const max_indices =
      arene::base::tuple_detail::tuple_transform_to_array<index_type>(slices, slice_to_offset<index_type>{});
  CONSTEXPR_ASSERT_EQ(result.data_handle(), &arene::base::apply(src, max_indices));
}

/// @test @c submdspan with full_extent on leading dims and mid-range @c extent_slice on last dim offsets correctly
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    MidRangeExtentSliceOnLastWithFullExtentOnLeading,
    TypeParam::extents_type::rank() >= 2U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;
  using extent_slice_type = arene::base::extent_slice<index_type, index_type, std::integral_constant<index_type, 1>>;

  auto const src = make_test_src<TypeParam>();

  constexpr auto leading_rank = extents_type::rank() - 1U;
  auto const last_ext = src.extents().extent(leading_rank);
  auto const trailing = extent_slice_type{index_type{1}, static_cast<index_type>(last_ext - index_type{1})};
  auto const slices = concat_slices(arene::base::testing::make_full_extents<leading_rank>(), trailing);
  auto const result = arene::base::apply(arene::base::bind_front(arene::base::submdspan, src), slices);
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  auto const expected_extents = arene::base::tuple_detail::tuple_transform_to_array<index_type>(
      arene::base::mdspan_detail::to_array(src.extents()),
      slices,
      slice_to_result_extent{}
  );
  CONSTEXPR_ASSERT_EQ(arene::base::mdspan_detail::to_array(result.extents()), expected_extents);

  auto const offset_indices =
      arene::base::tuple_detail::tuple_transform_to_array<index_type>(slices, slice_to_offset<index_type>{});
  CONSTEXPR_ASSERT_EQ(result.data_handle(), &arene::base::apply(src, offset_indices));
}

/// @test @c submdspan with full_extent on leading dims and mid-range @c range_slice on last dim offsets correctly
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    MidRangeRangeSliceOnLastWithFullExtentOnLeading,
    TypeParam::extents_type::rank() >= 2U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;
  using range_slice_type = arene::base::range_slice<index_type, index_type, std::integral_constant<index_type, 1>>;

  auto const src = make_test_src<TypeParam>();

  constexpr auto leading_rank = extents_type::rank() - 1U;
  auto const last_ext = src.extents().extent(leading_rank);
  auto const trailing = range_slice_type{index_type{1}, last_ext};
  auto const slices = concat_slices(arene::base::testing::make_full_extents<leading_rank>(), trailing);
  auto const result = arene::base::apply(arene::base::bind_front(arene::base::submdspan, src), slices);
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  auto const expected_extents = arene::base::tuple_detail::tuple_transform_to_array<index_type>(
      arene::base::mdspan_detail::to_array(src.extents()),
      slices,
      slice_to_result_extent{}
  );
  CONSTEXPR_ASSERT_EQ(arene::base::mdspan_detail::to_array(result.extents()), expected_extents);

  auto const offset_indices =
      arene::base::tuple_detail::tuple_transform_to_array<index_type>(slices, slice_to_offset<index_type>{});
  CONSTEXPR_ASSERT_EQ(result.data_handle(), &arene::base::apply(src, offset_indices));
}

/// @test @c submdspan with index on first dim and full-range @c extent_slice on remaining dims reduces rank
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    IndexFirstDimWithExtentSlicesOnRestReducesRank,
    TypeParam::extents_type::rank() >= 2U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  using slice_maker = arene::base::testing::make_full_range_extent_slice<index_type>;

  auto const all_slices = arene::base::transform(slice_maker{}, arene::base::mdspan_detail::to_array(src.extents()));

  auto const result = arene::base::apply(
      arene::base::bind_front(arene::base::submdspan, src),
      concat_slices(index_type{0}, drop_front<1>(all_slices))
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  constexpr auto trailing_rank = extents_type::rank() - 1U;
  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), trailing_rank);
  CONSTEXPR_ASSERT_EQ(result.data_handle(), src.data_handle());
  for (std::size_t dim = 0U; dim < trailing_rank; ++dim) {
    CONSTEXPR_ASSERT_EQ(result.extents().extent(dim), src.extents().extent(dim + 1U));
  }
}

/// @test @c submdspan with index on first dim and full-range @c range_slice on remaining dims reduces rank
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    IndexFirstDimWithRangeSlicesOnRestReducesRank,
    TypeParam::extents_type::rank() >= 2U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  using slice_maker = make_full_range_range_slice<index_type>;

  auto const all_slices = arene::base::transform(slice_maker{}, arene::base::mdspan_detail::to_array(src.extents()));

  auto const result = arene::base::apply(
      arene::base::bind_front(arene::base::submdspan, src),
      concat_slices(index_type{0}, drop_front<1>(all_slices))
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  constexpr auto trailing_rank = extents_type::rank() - 1U;

  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), trailing_rank);
  CONSTEXPR_ASSERT_EQ(result.data_handle(), src.data_handle());
  for (std::size_t dim = 0U; dim < trailing_rank; ++dim) {
    CONSTEXPR_ASSERT_EQ(result.extents().extent(dim), src.extents().extent(dim + 1U));
  }
}

/// @test @c submdspan with empty @c extent_slice on last dim produces a zero-extent dimension
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    EmptyExtentSliceOnLastDimProducesZeroExtent,
    TypeParam::extents_type::rank() >= 1U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;
  using extent_slice_type = arene::base::extent_slice<index_type, index_type, std::integral_constant<index_type, 1>>;

  auto const src = make_test_src<TypeParam>();

  constexpr auto leading_rank = extents_type::rank() - 1U;
  auto const trailing = extent_slice_type{index_type{0}, index_type{0}};
  auto const slices = concat_slices(arene::base::testing::make_full_extents<leading_rank>(), trailing);
  auto const result = arene::base::apply(arene::base::bind_front(arene::base::submdspan, src), slices);
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  auto const expected_extents = arene::base::tuple_detail::tuple_transform_to_array<index_type>(
      arene::base::mdspan_detail::to_array(src.extents()),
      slices,
      slice_to_result_extent{}
  );
  CONSTEXPR_ASSERT_EQ(arene::base::mdspan_detail::to_array(result.extents()), expected_extents);
  CONSTEXPR_ASSERT_EQ(result.data_handle(), src.data_handle());
}

/// @test @c submdspan with empty compile-time @c extent_slice on last dim produces a zero-extent dimension
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    EmptyStaticExtentSliceOnLastDimProducesZeroExtent,
    TypeParam::extents_type::rank() >= 1U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;
  using extent_slice_type = arene::base::extent_slice<
      std::integral_constant<index_type, 0>,
      std::integral_constant<index_type, 0>,
      std::integral_constant<index_type, 1>>;

  auto const src = make_test_src<TypeParam>();

  constexpr auto leading_rank = extents_type::rank() - 1U;
  auto const slices = concat_slices(arene::base::testing::make_full_extents<leading_rank>(), extent_slice_type{});
  auto const result = arene::base::apply(arene::base::bind_front(arene::base::submdspan, src), slices);
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  auto const expected_extents = arene::base::tuple_detail::tuple_transform_to_array<index_type>(
      arene::base::mdspan_detail::to_array(src.extents()),
      slices,
      slice_to_result_extent{}
  );
  CONSTEXPR_ASSERT_EQ(arene::base::mdspan_detail::to_array(result.extents()), expected_extents);
  CONSTEXPR_ASSERT_EQ(result.data_handle(), src.data_handle());
}

/// @test @c submdspan with empty @c range_slice on last dim produces a zero-extent dimension
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    EmptyRangeSliceOnLastDimProducesZeroExtent,
    TypeParam::extents_type::rank() >= 1U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;
  using range_slice_type = arene::base::range_slice<index_type, index_type, std::integral_constant<index_type, 1>>;

  auto const src = make_test_src<TypeParam>();

  constexpr auto leading_rank = extents_type::rank() - 1U;
  auto const trailing = range_slice_type{index_type{0}, index_type{0}};
  auto const slices = concat_slices(arene::base::testing::make_full_extents<leading_rank>(), trailing);
  auto const result = arene::base::apply(arene::base::bind_front(arene::base::submdspan, src), slices);
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  auto const expected_extents = arene::base::tuple_detail::tuple_transform_to_array<index_type>(
      arene::base::mdspan_detail::to_array(src.extents()),
      slices,
      slice_to_result_extent{}
  );
  CONSTEXPR_ASSERT_EQ(arene::base::mdspan_detail::to_array(result.extents()), expected_extents);
  CONSTEXPR_ASSERT_EQ(result.data_handle(), src.data_handle());
}

/// @test @c submdspan with empty @c range_slice at end of each dimension produces a zero-size result
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    EmptyRangeSlicesAtEndProduceZeroSize,
    TypeParam::extents_type::rank() >= 1U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  auto const end_slices = arene::base::transform(
      make_empty_end_range_slice<index_type>{},
      arene::base::mdspan_detail::to_array(src.extents())
  );

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      end_slices
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), extents_type::rank());
  CONSTEXPR_ASSERT_EQ(result.size(), std::size_t{0});
}

/// @test @c submdspan with empty @c range_slice on remaining dims and index on first dim produces a zero-size result
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    IndexWithEmptyRangeSlicesProducesZeroSize,
    TypeParam::extents_type::rank() >= 2U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  auto const end_slices = arene::base::transform(
      make_empty_end_range_slice<index_type>{},
      arene::base::mdspan_detail::to_array(src.extents())
  );

  auto const result = arene::base::apply(
      arene::base::bind_front(arene::base::submdspan, src),
      concat_slices(index_type{0}, drop_front<1>(end_slices))
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  constexpr auto trailing_rank = extents_type::rank() - 1U;
  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), trailing_rank);
  CONSTEXPR_ASSERT_EQ(result.size(), std::size_t{0});
}

/// @test @c submdspan with full_extent on leading dimensions and an index on the last reduces rank by one
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanTypedTest,
    IndexOnLastDimWithFullExtentOnLeadingReducesRank,
    TypeParam::extents_type::rank() >= 2U &&
        arene::base::testing::all_positive_static_extents(typename TypeParam::extents_type{})
) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename extents_type::index_type;

  auto const src = make_test_src<TypeParam>();

  constexpr auto leading_rank = extents_type::rank() - 1U;
  auto const result = arene::base::apply(
      arene::base::bind_front(arene::base::submdspan, src),
      concat_slices(arene::base::testing::make_full_extents<leading_rank>(), index_type{0})
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), leading_rank);
  CONSTEXPR_ASSERT_EQ(result.data_handle(), src.data_handle());
  for (std::size_t dim = 0U; dim < leading_rank; ++dim) {
    CONSTEXPR_ASSERT_EQ(result.extents().extent(dim), src.extents().extent(dim));
  }
}

/// @test @c submdspan is not invocable with wrong number of slices
TYPED_TEST(SubmdspanTypedTest, NotInvocableWithWrongNumberOfSlices) {
  using extents_type = typename TypeParam::extents_type;
  using src_type = arene::base::mdspan<int const, extents_type, typename TypeParam::layout_type>;

  CONSTEXPR_ASSERT(
      arene::base::type_lists::concat_t<
          arene::base::is_invocable<decltype(arene::base::submdspan), src_type>,
          arene::base::repeat_type_t<extents_type::rank(), arene::base::type_list, arene::base::full_extent_t>>::value
  );
  CONSTEXPR_ASSERT_FALSE(
      arene::base::type_lists::concat_t<
          arene::base::is_invocable<decltype(arene::base::submdspan), src_type>,
          arene::base::repeat_type_t<extents_type::rank() + 1U, arene::base::type_list, arene::base::full_extent_t>>::
          value
  );
}

// Constraint tests for non-sliceable mappings

/// @test @c submdspan should not be invocable with a layout whose mapping lacks a @c submdspan_mapping overload
TEST(SubmdspanConstraintTest, NotInvocableWithMappingLackingSubmdspanMapping) {
  using layout_type = test::user_defined_layout<true, true, true>;

  using rank1 = arene::base::mdspan<int const, arene::base::extents<std::size_t, 3>, layout_type>;
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(arene::base::submdspan), rank1, arene::base::full_extent_t>)
  );

  using rank2 = arene::base::mdspan<int const, arene::base::extents<std::size_t, 3, 4>, layout_type>;
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(arene::base::submdspan), rank2, std::size_t, arene::base::full_extent_t>)
  );
}

/// @test @c submdspan should not be invocable with a non-unique mapping
TEST(SubmdspanConstraintTest, NotInvocableWithNonUniqueMapping) {
  using layout_type = test::user_defined_layout<true, true, false>;
  using src_type = arene::base::mdspan<int const, arene::base::extents<std::size_t, 3>, layout_type>;

  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(arene::base::submdspan), src_type, arene::base::full_extent_t>)
  );
}

/// @test @c submdspan should not be invocable with a non-strided mapping
TEST(SubmdspanConstraintTest, NotInvocableWithNonStridedMapping) {
  using layout_type = test::user_defined_layout<false, true, true>;
  using src_type = arene::base::mdspan<int const, arene::base::extents<std::size_t, 3>, layout_type>;

  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(arene::base::submdspan), src_type, arene::base::full_extent_t>)
  );
}

/// @brief A layout whose @c submdspan_mapping overload returns a non-@c submdspan_mapping_result type
class bad_submdspan_mapping_return_layout {
 public:
  /// @brief Mapping that inherits all layout mapping requirements from @c user_defined_layout but adds a
  /// @c submdspan_mapping hidden friend that returns @c int instead of @c submdspan_mapping_result
  template <typename Extents>
  class mapping : public test::user_defined_layout<true, true, true>::template mapping<Extents> {
   public:
    using layout_type = bad_submdspan_mapping_return_layout;

    /// @brief Returns @c int instead of @c submdspan_mapping_result — intentionally wrong return type
    template <class... Slices>
    friend constexpr auto submdspan_mapping(mapping const&, Slices...) noexcept -> int {
      return 0;
    }
  };
};

/// @test @c submdspan should not be invocable with a mapping whose @c submdspan_mapping does not return
/// @c submdspan_mapping_result
TEST(SubmdspanConstraintTest, NotInvocableWithBadSubmdspanMappingReturnType) {
  using src_type =
      arene::base::mdspan<int const, arene::base::extents<std::size_t, 3>, bad_submdspan_mapping_return_layout>;

  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(arene::base::submdspan), src_type, arene::base::full_extent_t>)
  );
}

/// @brief Test parameters combining a layout mapping type with a submdspan slice type
/// @tparam Mapping the layout mapping type
/// @tparam SliceType the canonical slice type
template <class Mapping, class SliceType>
struct slice_parameterized_test_case {
  using mapping_type = Mapping;
  using slice_type = SliceType;
};

/// @brief The list of full-extent-like slice test variants
using full_extent_slice_types =
    arene::base::type_list<arene::base::full_extent_t, arene::base::testing::convertible_to_full_extent>;

/// @brief Cross product of test::layout_types × full-extent-like slice types
using full_extent_slice_test_types = arene::base::type_lists::
    cartesian_product_as_t<slice_parameterized_test_case, test::layout_types, full_extent_slice_types>;

template <class Params>
class SubmdspanFullExtentSliceTypedTest : public test::layout_test_base<typename Params::mapping_type> {};

TYPED_TEST_SUITE(SubmdspanFullExtentSliceTypedTest, full_extent_slice_test_types, );

/// @test @c submdspan with all full-extent-like slices returns an mdspan with the same extents
CONSTEXPR_TYPED_TEST(SubmdspanFullExtentSliceTypedTest, FullExtentSlicesReturnMdspanWithSameExtents) {
  using mapping_type = typename TypeParam::mapping_type;
  using extents_type = typename mapping_type::extents_type;
  using slice_type = typename TypeParam::slice_type;

  auto const src = make_test_src<mapping_type>();

  auto const result = arene::base::apply(
      arene::base::bind_front(arene::base::submdspan, src),
      arene::base::testing::make_full_extents<extents_type::rank(), slice_type>()
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(src.data_handle(), result.data_handle());
  CONSTEXPR_ASSERT_EQ(src.extents(), result.extents());
}

/// @brief The list of index-like slice test variants
using index_slice_types = arene::base::type_list<
    std::size_t,
    std::integral_constant<std::size_t, 0>,
    integral_constant_like<std::size_t, 0>,
    convertible_to<std::size_t>,
    std::int32_t,
    std::integral_constant<std::int32_t, 0>,
    integral_constant_like<std::int32_t, 0>,
    convertible_to<std::int32_t>>;

/// @brief Cross product of test::layout_types × slice types
using index_slice_test_types = arene::base::type_lists::
    cartesian_product_as_t<slice_parameterized_test_case, test::layout_types, index_slice_types>;

template <class Params>
class SubmdspanIndexSliceTypedTest : public test::layout_test_base<typename Params::mapping_type> {};

TYPED_TEST_SUITE(SubmdspanIndexSliceTypedTest, index_slice_test_types, );

/// @test @c submdspan with all zero index-like slices reduces rank to zero
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanIndexSliceTypedTest,
    AllZeroIndexSlicesCollapseToRankZero,
    arene::base::testing::all_positive_static_extents(typename TypeParam::mapping_type::extents_type{})
) {
  using mapping_type = typename TypeParam::mapping_type;
  using extents_type = typename mapping_type::extents_type;
  using index_type = typename extents_type::index_type;
  using slice_type = typename TypeParam::slice_type;

  auto const src = make_test_src<mapping_type>();

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      arene::base::array<slice_type, extents_type::rank()>{}
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), 0U);
  auto const zero_indices = arene::base::array<index_type, extents_type::rank()>{};
  CONSTEXPR_ASSERT_EQ(result.data_handle(), &arene::base::apply(src, zero_indices));
}

/// @brief Functor that transforms an extent value into a single-element pair-like slice at the last index @c {ext-1,
/// ext}
/// @tparam PairType the pair-like type to construct
template <class PairType>
struct make_last_element_pair_like {
  using first_type = std::tuple_element_t<0, PairType>;
  using second_type = std::tuple_element_t<1, PairType>;

  template <class IndexType>
  constexpr auto operator()(IndexType ext) const noexcept -> PairType {
    return PairType{static_cast<first_type>(ext - IndexType{1}), static_cast<second_type>(ext)};
  }
};

/// @brief For a given mapping, produce the type_list of all pair-like slice test variants
/// @tparam Mapping the layout mapping type
template <class Mapping>
using pair_like_slice_variants = arene::base::type_list<
    slice_parameterized_test_case<Mapping, std::pair<typename Mapping::index_type, typename Mapping::index_type>>,
    slice_parameterized_test_case<Mapping, std::tuple<typename Mapping::index_type, typename Mapping::index_type>>,
    slice_parameterized_test_case<Mapping, arene::base::array<typename Mapping::index_type, 2>>,
    slice_parameterized_test_case<Mapping, std::pair<std::size_t, std::size_t>>>;

/// @brief Cartesian product of test::layout_types × pair-like slice types
using pair_like_slice_test_types = arene::base::type_lists::flat_map_t<test::layout_types, pair_like_slice_variants>;

template <class Params>
class SubmdspanPairLikeSliceTypedTest : public test::layout_test_base<typename Params::mapping_type> {};

TYPED_TEST_SUITE(SubmdspanPairLikeSliceTypedTest, pair_like_slice_test_types, );

/// @test @c submdspan with single-element pair-like slices at max index on each dimension offsets to the last element
CONDITIONAL_CONSTEXPR_TYPED_TEST(
    SubmdspanPairLikeSliceTypedTest,
    MaxOffsetPairLikeSlicesPointToLastElement,
    arene::base::testing::all_positive_static_extents(typename TypeParam::mapping_type::extents_type{})
) {
  using mapping_type = typename TypeParam::mapping_type;
  using extents_type = typename mapping_type::extents_type;
  using index_type = typename extents_type::index_type;
  using slice_type = typename TypeParam::slice_type;

  auto const src = make_test_src<mapping_type>();

  auto const slices = arene::base::transform(
      make_last_element_pair_like<slice_type>{},
      arene::base::mdspan_detail::to_array(src.extents())
  );

  auto const result = arene::base::apply(  //
      arene::base::bind_front(arene::base::submdspan, src),
      slices
  );
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(decltype(result)::extents_type::rank(), extents_type::rank());

  auto const max_indices =
      arene::base::transform(minus_one<index_type>, arene::base::mdspan_detail::to_array(src.extents()));
  CONSTEXPR_ASSERT_EQ(result.data_handle(), &arene::base::apply(src, max_indices));
}

/// @test @c submdspan is @c noexcept(false) if canonicalization of any slice can throw
TEST(SubmdspanTest, Noexcept) {
  using src_type = arene::base::mdspan<int, arene::base::extents<int, 3, 3>>;

  auto const is_noexcept_with = [](auto... args) {
    return arene::base::is_nothrow_invocable_v<decltype(arene::base::submdspan), src_type, decltype(args)...>;
  };

  ASSERT_TRUE(is_noexcept_with(arene::base::full_extent, arene::base::full_extent));
  ASSERT_TRUE(is_noexcept_with(int{}, int{}));
  ASSERT_TRUE(is_noexcept_with(std::pair<int, int>{}, int{}));

  ASSERT_FALSE(is_noexcept_with(arene::base::testing::throwing_convertible_to_full_extent{}, int{}));
  ASSERT_FALSE(is_noexcept_with(int{}, arene::base::testing::throwing_convertible_to_full_extent{}));

  ASSERT_FALSE(is_noexcept_with(arene::base::testing::throwing_convertible_to_int{}, int{}));
  ASSERT_FALSE(is_noexcept_with(int{}, arene::base::testing::throwing_convertible_to_int{}));
  ASSERT_FALSE(is_noexcept_with(std::pair<int, arene::base::testing::throwing_convertible_to_int>{}, int{}));
  ASSERT_FALSE(is_noexcept_with(std::pair<arene::base::testing::throwing_convertible_to_int, int>{}, int{}));
}

/// @brief An accessor whose @c offset_policy is a distinct type (@c default_accessor) rather than itself
/// @tparam T the element type accessed
template <class T>
struct custom_offset_policy_accessor : arene::base::default_accessor<T> {
  using offset_policy = arene::base::default_accessor<T>;
};

/// @test @c submdspan rebinds the accessor to @c AccessorPolicy::offset_policy when the two differ
CONSTEXPR_TEST(SubmdspanTest, RebindsAccessorToOffsetPolicy) {
  using element_type = int const;
  using extents_type = arene::base::extents<int, 4, 3>;
  using accessor_type = custom_offset_policy_accessor<element_type>;
  using src_type = arene::base::mdspan<element_type, extents_type, arene::base::layout_left, accessor_type>;

  static_assert(
      !std::is_same<accessor_type, typename accessor_type::offset_policy>::value,
      "test requires the accessor's offset_policy differs from the accessor itself"
  );

  using mapping_type = typename src_type::mapping_type;
  auto const src = src_type{test_buffer<required_test_buffer_size<mapping_type>()>.data(), mapping_type{}};
  auto const result = arene::base::submdspan(src, arene::base::full_extent, int{1});
  static_assert(result_accessor_matches_offset_policy_v<decltype(src), decltype(result)>, "must rebind accessor");

  CONSTEXPR_ASSERT_EQ(result.data_handle(), &src(0, 1));
  CONSTEXPR_ASSERT_EQ(result.extents().extent(0), 4);
}

}  // namespace
