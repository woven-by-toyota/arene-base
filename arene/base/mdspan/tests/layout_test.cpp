// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/layout.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/is_layout_mapping.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/void_t.hpp"

namespace {

/// @brief A type trait to check if the given type parameter has the typedefs required for a @c layout or not
/// @{
template <typename T, typename = void>
struct has_layout_typedefs : std::false_type {};
template <typename T>
struct has_layout_typedefs<
    T,
    arene::base::void_t<
        typename T::extents_type,
        typename T::index_type,
        typename T::size_type,
        typename T::rank_type,
        typename T::layout_type>> : std::true_type {};
/// @}

/// @brief A dummy type representing an index that can't be converted into anything else
struct non_convertible_index {};

/// @brief A dummy type representing an index that can be explicitly converted into the given type
/// @tparam T The type which this can be converted into
template <typename T, bool Noexcept>
struct explicitly_convertible_to {
  T value;

  explicit constexpr operator T() const noexcept(Noexcept) { return value; }
};

/// @brief A dummy type representing an index that can be implicitly converted into the given type
/// @tparam T The type which this can be converted into
template <typename T, bool Noexcept>
struct implicitly_convertible_to {
  T value;

  // NOLINTNEXTLINE(hicpp-explicit-conversions) This is a test class that exists entirely to test implicit conversions
  constexpr operator T() const noexcept(Noexcept) { return value; }
};

// Test fixture setup ----------------------------------

template <typename TypeParam>
class LayoutTest : public test::layout_test_base<TypeParam> {};

TYPED_TEST_SUITE(LayoutTest, test::layout_types, );

/// @test Layout mappings are trivially copyable
TYPED_TEST(LayoutTest, LayoutIsTrivial) {
  STATIC_ASSERT_TRUE(std::is_trivially_copyable<TypeParam>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_copy_constructible<TypeParam>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_copy_assignable<TypeParam>::value);
}

/// @test Layout mappings has all of the required typedefs
TYPED_TEST(LayoutTest, LayoutHasTypedefs) { STATIC_ASSERT_TRUE(has_layout_typedefs<TypeParam>::value); }

/// @test All of the built-in layouts satisfy the `is_layout_mapping_v` type trait
TYPED_TEST(LayoutTest, LayoutSatisfiesTypeTrait) { STATIC_ASSERT_TRUE(arene::base::is_layout_mapping_v<TypeParam>); }

/// @test `required_span_size()` returns the expected special values in cases where there are no extents or one is 0
TYPED_TEST(LayoutTest, RequiredSpanSizesForExceptionalCases) {
  STATIC_ASSERT_TRUE(noexcept(TypeParam{}.required_span_size()));

  // If the rank is 0, the required span size should be 1; it may also be 1 even if the rank is not 0.
  STATIC_ASSERT_TRUE((TypeParam::extents_type::rank() != 0) || (TypeParam{}.required_span_size() == 1));

  // The required span size should 0 if and only if at least one of the extents is 0.
  STATIC_ASSERT_EQ(test::any_extent_is_0(TypeParam{}.extents()), TypeParam{}.required_span_size() == 0);

  // The "normal" case of required_span_size() is tested below as part of the tests of the function call operator.
}

/// @test In-bounds indices are mapped to the correct places
TYPED_TEST(LayoutTest, InBoundsMappingsAreCorrect) {
  using index_type = typename TypeParam::index_type;

  STATIC_ASSERT_TRUE(noexcept(TypeParam{}.extents()));

  if (!test::any_extent_is_0(TypeParam{}.extents())) {
    auto const last_index = static_cast<index_type>(TypeParam{}.required_span_size() - index_type{1});
    EXPECT_EQ(TestFixture::map_from_zero_indices(), 0);
    EXPECT_EQ(TestFixture::map_from_max_indices(), last_index);
  }
}

/// @test In-bounds mapping also works with types that are implicitly convertible to TypeParam::index_type
TYPED_TEST(LayoutTest, InBoundsMappingsWorkWithConvertibleIndices) {
  if (!test::any_extent_is_0(TypeParam{}.extents())) {
    EXPECT_EQ(TestFixture::template map_from_zero_indices<std::uint16_t>(), 0);
    EXPECT_EQ(TestFixture::template map_from_zero_indices<bool>(), 0);
  }
}

/// @test Function call operator is only usable on a pack whose size is exactly equal to the rank of the extents
TYPED_TEST(LayoutTest, MappingIndicesConstrainedToRank) {
  EXPECT_TRUE(TestFixture::template can_map_from_n_size_ts<TypeParam::extents_type::rank()>());

  if (TypeParam::extents_type::rank() != 0U) {
    // We need to clamp this to avoid instantiating can_map_from_n_size_ts<-1UL>, which would crash the compiler.
    constexpr std::size_t rank_minus_one =
        std::max(TypeParam::extents_type::rank(), typename TypeParam::rank_type{1U}) - 1U;
    EXPECT_FALSE(TestFixture::template can_map_from_n_size_ts<0U>());
    EXPECT_FALSE(TestFixture::template can_map_from_n_size_ts<rank_minus_one>());
  }

  EXPECT_FALSE(TestFixture::template can_map_from_n_size_ts<TypeParam::extents_type::rank() + 1U>());
}

/// @test Function call operator is only usable on a pack where all indices are nothrow-convertible to index_type
TYPED_TEST(LayoutTest, MappingIndicesMustBeNothrowConvertible) {
  EXPECT_EQ(
      TestFixture::template can_map_from_indices_with_type<non_convertible_index>(),
      TypeParam::extents_type::rank() == 0U
  );

  using explicit_noexcept = explicitly_convertible_to<typename TypeParam::index_type, true>;
  using implicit_noexcept = implicitly_convertible_to<typename TypeParam::index_type, true>;
  using explicit_throwing = explicitly_convertible_to<typename TypeParam::index_type, false>;
  using implicit_throwing = implicitly_convertible_to<typename TypeParam::index_type, false>;

  // If conversion is noexcept, then the call works if and only if the conversion is *implicit* (or the pack is empty)
  EXPECT_EQ(
      TestFixture::template can_map_from_indices_with_type<explicit_noexcept>(),
      TypeParam::extents_type::rank() == 0U
  );
  EXPECT_TRUE(TestFixture::template can_map_from_indices_with_type<implicit_noexcept>());

  // If conversion is not noexcept, then the call only works if the pack is empty, regardless of explicit/implicitness
  EXPECT_EQ(
      TestFixture::template can_map_from_indices_with_type<explicit_throwing>(),
      TypeParam::extents_type::rank() == 0U
  );
  EXPECT_EQ(
      TestFixture::template can_map_from_indices_with_type<implicit_throwing>(),
      TypeParam::extents_type::rank() == 0U
  );
}

/// @test The static assertion that the bounds of a layout's Extents don't overflow works correctly
CONSTEXPR_TYPED_TEST(LayoutTest, CheckIndicesForOverflow) {
  // This function is only ever run inside of a static assert so we don't get coverage for it unless we run it manually.
  CONSTEXPR_ASSERT(!arene::base::layout_detail::mapped_indices_overflow<typename TypeParam::extents_type>());
}

/// @brief Make a dynamic copy of the given extents
/// @tparam Extents The @c extents type to be copied
/// @tparam Dims A pack of the dimensions in @c Extents
/// @param extents The @c Extents instance to be copied
/// @return An @c extents object where all extents are dynamic with their values set to whatever is in @c extents
template <typename Extents, typename Extents::rank_type... Dims>
constexpr auto
make_dynamic_copy(Extents const& extents, std::integer_sequence<typename Extents::rank_type, Dims...>) noexcept
    -> arene::base::dextents<typename Extents::index_type, Extents::rank()> {
  return arene::base::dextents<typename Extents::index_type, Extents::rank()>{extents.extent(Dims)...};
}

/// @brief Make a dynamic copy of the given extents
/// @tparam Extents The @c extents type to be copied
/// @param extents The @c Extents instance to be copied
/// @return An @c extents object where all extents are dynamic with their values set to whatever is in @c extents
template <typename Extents>
constexpr auto make_dynamic_copy(Extents const& extents) noexcept
    -> arene::base::dextents<typename Extents::index_type, Extents::rank()> {
  return make_dynamic_copy(extents, std::make_integer_sequence<typename Extents::rank_type, Extents::rank()>{});
}

template <
    typename Mapping,
    arene::base::constraints<
        std::enable_if_t<std::is_same<typename Mapping::layout_type, arene::base::layout_stride>::value>> = nullptr>
constexpr auto make_mapping_with_extents(typename Mapping::extents_type const& extents) noexcept -> Mapping {
  return {extents, arene::base::layout_detail::right_strides(extents)};
}

template <
    typename Mapping,
    arene::base::constraints<
        std::enable_if_t<!std::is_same<typename Mapping::layout_type, arene::base::layout_stride>::value>> = nullptr>
constexpr auto make_mapping_with_extents(typename Mapping::extents_type const& extents) noexcept -> Mapping {
  return {extents};
}

/// @test All layouts support equality comparison with themselves and with other specializations with different extents
CONSTEXPR_TYPED_TEST(LayoutTest, SelfEqualityOperator) {
  CONSTEXPR_ASSERT_EQ(TypeParam{}, TypeParam{});
  CONSTEXPR_ASSERT_FALSE(TypeParam{} != TypeParam{});

  constexpr auto extents = test::make_expanded_extents<typename TypeParam::extents_type>();
  constexpr auto mapping = make_mapping_with_extents<TypeParam>(extents);

  using distinct_extents_type = arene::base::dextents<typename TypeParam::index_type, TypeParam::extents_type::rank()>;
  using distinct_mapping_type = typename TypeParam::layout_type::template mapping<distinct_extents_type>;
  constexpr distinct_extents_type distinct_extents = make_dynamic_copy(extents);
  constexpr auto distinct_mapping = make_mapping_with_extents<distinct_mapping_type>(distinct_extents);

  CONSTEXPR_ASSERT_EQ(mapping, distinct_mapping);
  CONSTEXPR_ASSERT_EQ(distinct_mapping, mapping);
  CONSTEXPR_ASSERT_FALSE(mapping != distinct_mapping);
  CONSTEXPR_ASSERT_FALSE(distinct_mapping != mapping);
}

/// @test All layouts support non-throwing equality comparison with their own type
CONSTEXPR_TYPED_TEST(LayoutTest, SelfEqualityConstraints) {
  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<TypeParam, TypeParam>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<TypeParam, TypeParam>);
}

/// @test All layouts support non-throwing equality comparison with different-extent specializations of the same rank
CONSTEXPR_TYPED_TEST(LayoutTest, SameRankEqualityWorks) {
  using same_rank_extents = arene::base::dextents<typename TypeParam::index_type, TypeParam::extents_type::rank()>;
  using same_rank_mapping = typename TypeParam::layout_type::template mapping<same_rank_extents>;

  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<TypeParam, same_rank_mapping>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<TypeParam, same_rank_mapping>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<same_rank_mapping, TypeParam>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<same_rank_mapping, TypeParam>);
}

/// @test All layouts correctly report that equality comparisons with extents of different ranks are not supported
CONSTEXPR_TYPED_TEST(LayoutTest, DifferentRankEqualityIsDisabled) {
  using different_rank_extents = test::extents_with_different_rank<typename TypeParam::extents_type>;
  using different_rank_mapping = typename TypeParam::layout_type::template mapping<different_rank_extents>;

  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<TypeParam, different_rank_mapping>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<TypeParam, different_rank_mapping>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<different_rank_mapping, TypeParam>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<different_rank_mapping, TypeParam>);
}

/// @test The static assertion that the bounds of a layout's Extents don't overflow can fail
TEST(LayoutUntypedTest, OverflowCheckCanFail) {
  constexpr std::size_t huge{std::numeric_limits<std::size_t>::max() - 1UL};
  using huge_extents = arene::base::extents<std::size_t, huge, huge, huge>;
  // This function is only ever run inside of a static assert so we don't get coverage for it unless we run it manually.
  EXPECT_TRUE(arene::base::layout_detail::mapped_indices_overflow<huge_extents>());
}

}  // namespace
