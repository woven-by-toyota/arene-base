// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace {

/// @brief A huge runtime size to use for testing that large dimensions of an @c extents can cause overflow
template <typename IndexType>
constexpr IndexType huge_dynamic_extent{std::numeric_limits<IndexType>::max()};

// Helper functions *not* for integer sequences --------

/// @brief Get an instance of @c TypeParam::extents_type where dynamic extents are just big enough to overflow
/// @tparam Extents An @c extents type
/// @return An instance of @c Extents where dynamic extents are just big enough to overflow (or as big as possible)
template <typename Extents>
constexpr auto make_overflowing_extents() noexcept -> Extents {
  using index_type = typename Extents::index_type;
  using rank_type = typename Extents::rank_type;

  // This puts 1s in for all the dynamic extents so extent_product becomes a product of just the static extents.
  auto static_extents =
      test::make_extents_impl<Extents>(index_type{1U}, std::make_index_sequence<Extents::rank_dynamic()>{});
  if (Extents::rank_dynamic() == rank_type{}) {
    return static_extents;  // If there are no dynamic dimensions there's nothing to adjust, so just return this.
  }

  arene::base::layout_detail::dim_range<rank_type> const all_dims{rank_type{}, Extents::rank()};
  auto const static_product = arene::base::layout_detail::extent_product(static_extents, all_dims);
  if (static_product.overflowed || static_product.value == index_type{}) {
    // Either the static extents overflow on their own or one is 0 so they can't overflow. Either way, nothing to do.
    return static_extents;
  }

  // Compute the value that should be set to each dynamic extent in order to just barely overflow
  constexpr auto max = std::numeric_limits<index_type>::max();
  auto dyn_extent =
      static_cast<index_type>(static_cast<rank_type>(max / static_product.value) / Extents::rank_dynamic());
  if (dyn_extent < max) {
    dyn_extent = static_cast<index_type>(dyn_extent + index_type{1});
  }

  return test::make_extents_impl<Extents>(dyn_extent, std::make_index_sequence<Extents::rank_dynamic()>{});
}

/// @brief Get an instance of @c TypeParam::extents_type where dynamic extents are all huge so using it will overflow
/// @tparam Extents An @c extents type
/// @return An instance of @c Extents where dynamic extents are expanded to @c huge_dynamic_extent
template <typename Extents>
constexpr auto make_huge_extents() noexcept -> Extents {
  return test::make_extents_impl<Extents>(
      huge_dynamic_extent<typename Extents::index_type>,
      std::make_index_sequence<Extents::rank_dynamic()>{}
  );
}

// Helper functions for integer sequences --------------

/// @brief An @c integer_sequence where each member is the minimum out-of-bounds index of its respective dimension
/// @tparam Extents An @c extents type for which we're generating an index pack
/// @{
template <typename Extents, typename RankIndexSequence = std::make_index_sequence<Extents::rank()>>
struct sequence_of_out_of_bounds_values_impl;

template <typename Extents, std::size_t... RankIndices>
struct sequence_of_out_of_bounds_values_impl<Extents, std::index_sequence<RankIndices...>> {
  using type =
      std::integer_sequence<typename Extents::index_type, test::expected_runtime_extent<Extents>(RankIndices)...>;
};

template <typename Extents>
using sequence_of_out_of_bounds_values = typename sequence_of_out_of_bounds_values_impl<Extents>::type;
/// @}

// Test fixture setup ----------------------------------

template <typename TypeParam>
class LayoutTest : public testing::Test {
  /// @brief Use the given mapping's function call operator with the given index pack and return the result
  /// @tparam Indices A pack of indices to use as arguments for the function call operator
  /// @param mapping A layout mapping object which maps packs of indices to one-dimensional indices
  /// @return The one-dimensional index mapped to by the multi-dimensional @c Indices
  template <typename Type, Type... Indices>
  constexpr auto map_from_index_pack(TypeParam const& mapping, std::integer_sequence<Type, Indices...>) noexcept
      -> decltype(mapping(Indices...)) {
    static_assert(noexcept(mapping(Indices...)), "Function calls on a mapping should always be noexcept");
    return mapping(Indices...);
  }

 protected:
  /// @brief Map a pack of out-of-bounds values for each dimension of this @c mapping to a one-dimensional output index
  /// @return The one-dimensional output index corresponding to an input of all out-of-bounds indices
  /// @note This will probably crash with a precondition violation instead of returning
  constexpr auto map_from_out_of_bounds_indices() noexcept -> typename TypeParam::index_type {
    return map_from_index_pack(TypeParam{}, sequence_of_out_of_bounds_values<typename TypeParam::extents_type>{});
  }
};

// Tests applied only to layout_left::mapping

template <typename TypeParam>
class LayoutLeftDeathTest : public LayoutTest<TypeParam> {};

TYPED_TEST_SUITE(LayoutLeftDeathTest, test::layout_left_types, );

// Tests applied only to layout_right::mapping

template <typename TypeParam>
class LayoutRightDeathTest : public LayoutTest<TypeParam> {};

TYPED_TEST_SUITE(LayoutRightDeathTest, test::layout_right_types, );

// Tests applied only to layout_stride::mapping

template <typename TypeParam>
class LayoutStrideDeathTest : public LayoutTest<TypeParam> {};

TYPED_TEST_SUITE(LayoutStrideDeathTest, test::layout_stride_types, );

// Generic tests applied to all layouts

template <typename TypeParam>
class LayoutDeathTest : public LayoutTest<TypeParam> {};

TYPED_TEST_SUITE(LayoutDeathTest, test::layout_types, );

// Actual test cases -----------------------------------

/// @test The extents-taking layout_left constructor crashes when given extents that would overflow
TYPED_TEST(LayoutLeftDeathTest, ConstructFromExtentsCrashesOnOverflow) {
  auto const extents = make_huge_extents<typename TypeParam::extents_type>();
  arene::base::layout_detail::dim_range<typename TypeParam::rank_type> const dims{
      0U,
      std::max(extents.rank(), 1UL) - 1UL
  };
  bool const product_overflows = arene::base::layout_detail::extent_product(extents, dims).overflowed;

  if (product_overflows) {
    ASSERT_DEATH(TypeParam{extents}, "Precondition");
  } else {
    std::ignore = TypeParam{extents};
  }
}

/// @test Constructing from a set of extents whose product is too big to store crashes with a precondition violation
TYPED_TEST(LayoutLeftDeathTest, ConstructFromLayoutLeftWithTooBigExtentsCrashes) {
  // Make a mapping with an extents which overflows for TypeParam::extents_type but not for big_extents_type
  using big_extents_type = arene::base::testing::explicitly_convertible_extents<typename TypeParam::extents_type>;
  big_extents_type const extents = make_overflowing_extents<typename TypeParam::extents_type>();
  arene::base::layout_left::mapping<big_extents_type> const big_mapping(extents);

  if (TypeParam::extents_type::rank_dynamic() >= 1U && big_mapping.required_span_size() != 0U) {
    ASSERT_DEATH(TypeParam{big_mapping}, "Precondition");
  } else {
    std::ignore = TypeParam{big_mapping};
  }
}

/// @test Constructing from a layout_right whose extent product is too big to store crashes
TEST(LayoutLeftDeathTest, ConstructFromLayoutRightWithTooBigExtentsCrashes) {
  // This error condition can only happen when there is exactly one extent and that extent is dynamic.

  using small_extents_type = arene::base::extents<std::uint32_t, arene::base::dynamic_extent>;
  using big_extents_type = arene::base::extents<std::uint64_t, arene::base::dynamic_extent>;
  big_extents_type const extents = make_huge_extents<big_extents_type>();
  arene::base::layout_right::mapping<big_extents_type> const big_mapping(extents);

  ASSERT_DEATH(arene::base::layout_left::mapping<small_extents_type>{big_mapping}, "Precondition");
}

/// @test The constructor from a layout_stride crashes with a precondition when the input is not left-strided
TYPED_TEST(LayoutLeftDeathTest, ConstructFromLayoutStrideWithWrongStridesCrashes) {
  constexpr std::size_t rank{TypeParam::extents_type::rank()};
  using stride_mapping_type = arene::base::layout_stride::mapping<typename TypeParam::extents_type>;
  ASSERT_TRUE((std::is_nothrow_constructible<TypeParam, stride_mapping_type const&>::value));
  ASSERT_EQ((std::is_convertible<stride_mapping_type const&, TypeParam>::value), rank == 0U);

  // A default-constructed layout_stride is right-strided, so this is invalid if an invalid one can be constructed.
  stride_mapping_type const source;

  // For some extents (e.g. rank 0 or 1, or any extent whose output space is size 0) it's not actually possible to
  // construct a striding that fails to be left-strided. So we need to inspect source to see if it's invalid in reality.
  TypeParam const left_strided_mapping;
  bool should_crash{false};
  for (typename TypeParam::rank_type dim{}; dim < rank; ++dim) {
    if (source.stride(dim) != left_strided_mapping.stride(dim)) {
      should_crash = true;
      break;
    }
  }

  if (should_crash) {
    ASSERT_DEATH(TypeParam{source}, "Precondition");
  } else {
    std::ignore = TypeParam{source};
  }
}

/// @test The extents-taking layout_right constructor crashes when given extents that would overflow
TYPED_TEST(LayoutRightDeathTest, ConstructFromExtentsCrashesOnOverflow) {
  auto const extents = make_huge_extents<typename TypeParam::extents_type>();
  arene::base::layout_detail::dim_range<typename TypeParam::rank_type> const dims{1U, extents.rank()};
  bool const product_overflows = arene::base::layout_detail::extent_product(extents, dims).overflowed;

  if (product_overflows) {
    ASSERT_DEATH(TypeParam{extents}, "Precondition");
  } else {
    std::ignore = TypeParam{extents};
  }
}

/// @test Constructing from a set of extents whose product is too big to store crashes with a precondition violation
TYPED_TEST(LayoutRightDeathTest, ConstructFromLayoutRightWithTooBigExtentsCrashes) {
  // Make a mapping with an extents which overflows for TypeParam::extents_type but not for big_extents_type
  using big_extents_type = arene::base::testing::explicitly_convertible_extents<typename TypeParam::extents_type>;
  big_extents_type const extents = make_overflowing_extents<typename TypeParam::extents_type>();
  arene::base::layout_right::mapping<big_extents_type> const big_mapping(extents);

  if (TypeParam::extents_type::rank_dynamic() >= 1U && big_mapping.required_span_size() != 0U) {
    ASSERT_DEATH(TypeParam{big_mapping}, "Precondition");
  } else {
    std::ignore = TypeParam{big_mapping};
  }
}

/// @test Constructing from a layout_left whose extent product is too big to store crashes
TEST(LayoutRightDeathTest, ConstructFromLayoutLeftWithTooBigExtentsCrashes) {
  // This error condition can only happen when there is exactly one extent and that extent is dynamic.

  using small_extents_type = arene::base::extents<std::uint32_t, arene::base::dynamic_extent>;
  using big_extents_type = arene::base::extents<std::uint64_t, arene::base::dynamic_extent>;
  big_extents_type const extents = make_huge_extents<big_extents_type>();
  arene::base::layout_left::mapping<big_extents_type> const big_mapping(extents);

  ASSERT_DEATH(arene::base::layout_right::mapping<small_extents_type>{big_mapping}, "Precondition");
}

/// @test A `layout_stride` constructed with a negative stride is a precondition violation
TYPED_TEST(LayoutStrideDeathTest, StridesMustNotBeNegative) {
  using extents_type = typename TypeParam::extents_type;

  auto const extents = test::make_expanded_extents<extents_type>();
  // For the stride array, we use a small signed type that will be implicitly convertible to index_type.
  auto strides = test::make_left_strides<extents_type, std::int16_t>(extents);
  arene::base::span<std::int16_t const, extents_type::rank()> const stride_span(strides);

  // If it's possible to make a negative stride, we do so and construction must fail; otherwise it must succeed.
  if (extents_type::rank() > 0UL) {
    strides[0] = std::int16_t{-1};
    ASSERT_DEATH(TypeParam(extents, strides), "Precondition");
    ASSERT_DEATH(TypeParam(extents, stride_span), "Precondition");
  } else {
    std::ignore = TypeParam(extents, strides);
    std::ignore = TypeParam(extents, stride_span);
  }
}

/// @test A `layout_stride` constructed with a negative stride is a precondition violation
TYPED_TEST(LayoutStrideDeathTest, StridesMustNotBeZero) {
  using extents_type = typename TypeParam::extents_type;

  auto const extents = test::make_expanded_extents<extents_type>();
  // For the stride array, we use a small signed type that will be implicitly convertible to index_type.
  auto strides = test::make_left_strides<extents_type, std::int16_t>(extents);
  arene::base::span<std::int16_t const, extents_type::rank()> const stride_span(strides);

  // If it's possible to make a zero stride, we do so and construction must fail; otherwise it must succeed.
  if (extents_type::rank() > 0UL) {
    strides[0] = std::int16_t{0};
    ASSERT_DEATH(TypeParam(extents, strides), "Precondition");
    ASSERT_DEATH(TypeParam(extents, stride_span), "Precondition");
  } else {
    std::ignore = TypeParam(extents, strides);
    std::ignore = TypeParam(extents, stride_span);
  }
}

/// @test A `layout_stride` constructed with a set of strides which are not a unique mapping is a precondition violation
/// @note The precondition check is *not* complete. It only catches *some* cases that lead to non-uniqueness.
TYPED_TEST(LayoutStrideDeathTest, StridesMustBeUnique) {
  using extents_type = typename TypeParam::extents_type;
  using index_type = typename TypeParam::index_type;

  constexpr auto extents = test::make_expanded_extents<extents_type>();

  // Making all of the strides 1 will certainly fail to be one-to-one as long as the rank is 2 or higher and none of the
  // extents are 0.
  arene::base::array<index_type, extents_type::rank()> strides{};
  for (std::size_t dim = 0UL; dim < extents_type::rank(); ++dim) {
    strides[dim] = index_type{1};
  }
  arene::base::span<index_type const, extents_type::rank()> const stride_span(strides);

  using layout_right = arene::base::layout_right::mapping<extents_type>;
  constexpr index_type mapped_space_size{layout_right(extents).required_span_size()};
  constexpr bool non_unique_possible{(extents.rank() > 1UL) && (mapped_space_size != index_type{})};

  // If it's possible to make a non-unique striding, we do so and construction must fail; otherwise it must succeed.
  if (non_unique_possible) {
    ASSERT_DEATH(TypeParam(extents, strides), "Precondition");
    ASSERT_DEATH(TypeParam(extents, stride_span), "Precondition");
  } else {
    std::ignore = TypeParam(extents, strides);
    std::ignore = TypeParam(extents, stride_span);
  }
}

// A flag determining the
enum class broken_flag : unsigned { nothing = 0b0U, zero_stride = 0b1U, huge_size = 0b10U, non_zero_offset = 0b100U };

constexpr auto operator&(broken_flag left, broken_flag right) noexcept -> bool {
  return (static_cast<unsigned>(left) & static_cast<unsigned>(right)) != 0U;
}

// This layout class is needed so that the mapping will satisfy is_layout_mapping_v
class broken_layout {
 public:
  template <typename Extents>
  class mapping;
};

/// @brief A user-defined mapping type that can be configured to fail the conversion preconditions in various ways.
//
/// If set to <c>zero_stride</c>, it reports that all of its strides are 0.
/// If set to <c>huge_size</c>, it reports that all strides and @c required_span_size are the maximum possible value.
/// If set to <c>non_zero_offset</c>, it reports that the image of mapping <c>(0, 0, 0...)</c> is not 0.
template <typename Extents>
class broken_layout::mapping {
 public:
  using extents_type = Extents;
  using index_type = typename extents_type::index_type;
  using rank_type = typename extents_type::rank_type;
  using layout_type = broken_layout;

  static constexpr auto is_always_strided() noexcept -> bool { return true; }
  static constexpr auto is_always_exhaustive() noexcept -> bool { return true; }
  static constexpr auto is_always_unique() noexcept -> bool { return true; }

  static constexpr auto is_strided() noexcept -> bool { return true; }
  static constexpr auto is_exhaustive() noexcept -> bool { return true; }
  static constexpr auto is_unique() noexcept -> bool { return true; }

  constexpr mapping(Extents const& extents, broken_flag flags) noexcept
      : extents_(extents),
        flags_(flags) {}

  constexpr auto extents() const noexcept -> Extents const& { return extents_; }

  constexpr auto stride(rank_type) const noexcept -> index_type {
    if (flags_ & broken_flag::zero_stride) {
      return {0};
    }
    if (flags_ & broken_flag::huge_size) {
      return std::numeric_limits<index_type>::max();
    }
    return {1};
  }

  constexpr auto required_span_size() const noexcept -> index_type {
    return (flags_ & broken_flag::huge_size) ? std::numeric_limits<index_type>::max() : index_type{1};
  }

  // This will only be called with index 0 to determine the offset, so it ignores its parameters for simplicity
  template <typename... Ts>
  constexpr auto operator()(Ts...) const noexcept -> index_type {
    return (flags_ & broken_flag::non_zero_offset) ? index_type{1} : index_type{0};
  }

  // Equality operator is needed so that this mapping will satisfy is_layout_mapping_v; it's not used in these tests
  friend constexpr auto operator==(mapping const&, mapping const&) noexcept -> bool { return true; }

 private:
  Extents extents_;
  broken_flag flags_;
};

/// @test The conversion function from an arbitrary strided layout to a layout_stride fails with a precondition when
/// one or more of the strides are zero
TYPED_TEST(LayoutStrideDeathTest, ConversionRequiresPositiveStrides) {
  using extents_type = typename TypeParam::extents_type;
  broken_layout::mapping<extents_type> const source(
      test::make_expanded_extents<extents_type>(),
      broken_flag::zero_stride
  );

  if (extents_type::rank() != typename extents_type::rank_type{}) {
    ASSERT_DEATH(TypeParam{source}, "Precondition");
  } else {
    std::ignore = TypeParam{source};
  }
}

/// @test The conversion function from an arbitrary strided layout to a layout_stride fails with a precondition when
/// the required span size is too large to represent
TYPED_TEST(LayoutStrideDeathTest, ConversionRequiresSizeBeInBounds) {
  using big_extents_type = arene::base::testing::explicitly_convertible_extents<typename TypeParam::extents_type>;
  broken_layout::mapping<big_extents_type> const source(
      test::make_expanded_extents<big_extents_type>(),
      broken_flag::huge_size
  );

  ASSERT_DEATH(TypeParam{source}, "Precondition");
}

/// @test The conversion function from an arbitrary strided layout to a layout_stride fails with a precondition when
/// the offset of the original mapping is not zero
TYPED_TEST(LayoutStrideDeathTest, ConversionRequiresOffsetBeZero) {
  using extents_type = typename TypeParam::extents_type;
  broken_layout::mapping<extents_type> const source(
      test::make_expanded_extents<extents_type>(),
      broken_flag::non_zero_offset
  );

  if (extents_type::rank() != typename extents_type::rank_type{}) {
    ASSERT_DEATH(TypeParam{source}, "Precondition");
  } else {
    std::ignore = TypeParam{source};
  }
}

/// @test Checking a stride with index at least the rank of the extents crashes with a precondition violation
TYPED_TEST(LayoutDeathTest, OutOfBoundsStrideCheckCrashes) {
  ASSERT_DEATH(TypeParam{}.stride(TypeParam::extents_type::rank()), "Precondition");
}

/// @test Out-of-bounds indices cause a precondition violation
TYPED_TEST(LayoutDeathTest, OutOfBoundsMappingsCrash) {
  if (TypeParam::extents_type::rank() != 0U) {
    ASSERT_DEATH(TestFixture::map_from_out_of_bounds_indices(), "Precondition");
  }
}

}  // namespace
