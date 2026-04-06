// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/fill.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"

namespace {

template <typename TypeParam>
class LayoutStrideTest : public test::layout_test_base<TypeParam> {};

TYPED_TEST_SUITE(LayoutStrideTest, test::layout_stride_types, );

/// @test The default `layout_stride` constructor has the default strides, i.e. those of a `layout_right::mapping`
CONSTEXPR_TYPED_TEST(LayoutStrideTest, DefaultConstructor) {
  CONSTEXPR_ASSERT(std::is_nothrow_default_constructible<TypeParam>::value);

  TypeParam const layout;
  typename TypeParam::extents_type const extents{};
  std::size_t stride_so_far{1UL};
  // layout_stride iterates backward, from right to left; strides should increase
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

/// @test A `layout_stride` can be constructed from a set of `extents` and an `array` of strides
CONSTEXPR_TYPED_TEST(LayoutStrideTest, ArrayConstructor) {
  using extents_type = typename TypeParam::extents_type;
  using array_type = arene::base::array<typename TypeParam::index_type, extents_type::rank()>;
  CONSTEXPR_ASSERT(std::is_nothrow_constructible<TypeParam, extents_type const&, array_type const&>::value);

  constexpr auto extents = test::make_expanded_extents<extents_type>();
  // We'll construct the strides of a layout_left (the default being layout_right).
  constexpr array_type strides{test::make_left_strides(extents)};

  // Compare a left-strided layout_stride to a layout_left with the same extents.
  TypeParam const layout_from_array(extents, strides);
  arene::base::layout_left::mapping<extents_type> const left_layout(extents);
  for (std::size_t dim = 0UL; dim < extents_type::rank(); ++dim) {
    CONSTEXPR_ASSERT_EQ(layout_from_array.stride(dim), left_layout.stride(dim));
  }
}

/// @test A `layout_stride` can be constructed from a set of `extents` and a `span` of strides
CONSTEXPR_TYPED_TEST(LayoutStrideTest, SpanConstructor) {
  using extents_type = typename TypeParam::extents_type;
  using span_type = arene::base::span<typename TypeParam::index_type const, extents_type::rank()>;
  CONSTEXPR_ASSERT(std::is_nothrow_constructible<TypeParam, extents_type const&, span_type>::value);

  auto const extents = test::make_expanded_extents<extents_type>();
  // We'll construct the strides of a layout_left (the default being layout_right).
  auto strides = test::make_left_strides(extents);
  span_type const stride_span(strides);

  // Compare a left-strided layout_stride to a layout_left with the same extents.
  TypeParam const layout_from_span(extents, stride_span);
  arene::base::layout_left::mapping<extents_type> const left_layout(extents);
  for (std::size_t dim = 0UL; dim < extents_type::rank(); ++dim) {
    CONSTEXPR_ASSERT_EQ(layout_from_span.stride(dim), left_layout.stride(dim));
  }
}

/// @test Conversion works from `layout_left` to `layout_stride`
TYPED_TEST(LayoutStrideTest, ImplicitConversionFromLayoutLeft) {
  using left_mapping = arene::base::layout_left::mapping<typename TypeParam::extents_type>;
  STATIC_ASSERT_TRUE(std::is_convertible<left_mapping, TypeParam>::value);

  constexpr auto extents = test::make_expanded_extents<typename TypeParam::extents_type>();
  left_mapping const left(extents);
  TypeParam const from_left(left);

  for (typename TypeParam::rank_type dim{}; dim < extents.rank(); ++dim) {
    EXPECT_EQ(from_left.stride(dim), left.stride(dim));
  }
}

/// @test Conversion works from `layout_right` to `layout_stride`
TYPED_TEST(LayoutStrideTest, ImplicitConversionFromLayoutRight) {
  using right_mapping = arene::base::layout_right::mapping<typename TypeParam::extents_type>;
  STATIC_ASSERT_TRUE(std::is_convertible<right_mapping, TypeParam>::value);

  constexpr auto extents = test::make_expanded_extents<typename TypeParam::extents_type>();
  right_mapping const right(extents);
  TypeParam const from_right(right);

  for (typename TypeParam::rank_type dim{}; dim < extents.rank(); ++dim) {
    EXPECT_EQ(from_right.stride(dim), right.stride(dim));
  }
}

/// @test Conversion works correctly from types with the static mapping properties having various different values
TYPED_TEST(LayoutStrideTest, ConversionFromTypesWithDifferentStaticProperties) {
  using extents_type = typename TypeParam::extents_type;

  using other_mapping = test::user_defined_layout<true, true, true>::mapping<extents_type>;
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<TypeParam, other_mapping>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<other_mapping, TypeParam>::value);

  using not_strided = test::user_defined_layout<false, true, true>::mapping<extents_type>;
  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, not_strided>::value);

  using not_exhaustive = test::user_defined_layout<true, false, true>::mapping<extents_type>;
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<TypeParam, not_exhaustive>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<not_exhaustive, TypeParam>::value);

  using not_unique = test::user_defined_layout<true, true, false>::mapping<extents_type>;
  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, not_unique>::value);
}

/// @test Conversion works correctly from types with various kinds of non-matching extents in a known mapping
TYPED_TEST(LayoutStrideTest, ConversionFromTypesWithDifferentExtentsAndKnownMapping) {
  using extents_type = typename TypeParam::extents_type;
  using implicit_extents = arene::base::testing::implicitly_convertible_extents<extents_type>;
  using explicit_extents = arene::base::testing::explicitly_convertible_extents<extents_type>;
  // This is non-convertible merely due to having more dimensions than any of our TypeParams.
  using non_convertible_extents = arene::base::extents<std::size_t, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>;
  static_assert(!std::is_constructible<extents_type, non_convertible_extents>::value, "Add more dims if this fails");

  // This combination (known mapping, implicitly convertible) is the only way to get implicit conversion to be allowed.
  using implicit_mapping = arene::base::layout_left::mapping<implicit_extents>;
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<TypeParam, implicit_mapping>::value);
  STATIC_ASSERT_TRUE(std::is_convertible<implicit_mapping, TypeParam>::value);

  using explicit_mapping = arene::base::layout_left::mapping<explicit_extents>;
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<TypeParam, explicit_mapping>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<explicit_mapping, TypeParam>::value);

  using non_convertible_mapping = arene::base::layout_left::mapping<non_convertible_extents>;
  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, non_convertible_mapping>::value);
}

/// @test Conversion works correctly from types with various kinds of non-matching extents in a user-defined mapping
TYPED_TEST(LayoutStrideTest, ConversionFromTypesWithDifferentExtentsAndUnknownMapping) {
  using extents_type = typename TypeParam::extents_type;
  using implicit_extents = arene::base::testing::implicitly_convertible_extents<extents_type>;
  using explicit_extents = arene::base::testing::explicitly_convertible_extents<extents_type>;
  // This is non-convertible merely due to having more dimensions than any of our TypeParams.
  using non_convertible_extents = arene::base::extents<std::size_t, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>;
  static_assert(!std::is_constructible<extents_type, non_convertible_extents>::value, "Add more dims if this fails");

  // This is not implicitly convertible even though the extents are, because the mapping isn't a known type.
  using implicit_mapping = test::user_defined_layout<true, true, true>::mapping<implicit_extents>;
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<TypeParam, implicit_mapping>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<implicit_mapping, TypeParam>::value);

  using explicit_mapping = test::user_defined_layout<true, true, true>::mapping<explicit_extents>;
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<TypeParam, explicit_mapping>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<explicit_mapping, TypeParam>::value);

  using non_convertible_mapping = test::user_defined_layout<true, true, true>::mapping<non_convertible_extents>;
  STATIC_ASSERT_FALSE(std::is_constructible<TypeParam, non_convertible_mapping>::value);
}

/// @test Observing the strides via @c strides()[i] and @c stride(i) always gives the same answer
TYPED_TEST(LayoutStrideTest, StrideObserversAreEquivalent) {
  constexpr TypeParam layout;
  STATIC_ASSERT_TRUE(noexcept(layout.strides()));
  STATIC_ASSERT_TRUE(noexcept(layout.stride(0)));

  // Pulled out to make sure it gets called even for rank 0
  constexpr auto consteval_strides = layout.strides();
  auto const runtime_strides = layout.strides();
  for (std::size_t i = 0UL; i < TypeParam::extents_type::rank(); ++i) {
    EXPECT_EQ(consteval_strides[i], layout.stride(i));
    EXPECT_EQ(runtime_strides[i], layout.stride(i));
  }
}

/// @test Required span size is zero if any extent is zero
CONSTEXPR_TEST(LayoutStrideTest, RequiredSpanSizeIsZeroIfExtentsSpaceIsZero) {
  using extents_type = arene::base::extents<std::int8_t, 2, 2, arene::base::dynamic_extent>;

  auto const mapping = arene::base::layout_stride::mapping<extents_type>{
      extents_type{2, 2, 0},
      // these strides are large enough to cause overflow when accumulating the
      // first two dimensions
      arene::base::array<std::int8_t, extents_type::rank>{64, 64, 64}
  };

  CONSTEXPR_ASSERT_EQ(0U, mapping.required_span_size());
}

/// @test Static observers are available and correct for layout_stride::mapping
TYPED_TEST(LayoutStrideTest, StaticObserversAreCorrect) {
  STATIC_ASSERT_EQ(TypeParam::is_always_unique(), true);
  STATIC_ASSERT_EQ(TypeParam::is_always_exhaustive(), false);
  STATIC_ASSERT_EQ(TypeParam::is_always_strided(), true);

  STATIC_ASSERT_TRUE(noexcept(TypeParam::is_always_unique()));
  STATIC_ASSERT_TRUE(noexcept(TypeParam::is_always_exhaustive()));
  STATIC_ASSERT_TRUE(noexcept(TypeParam::is_always_strided()));
}

/// @test Instance observers are available and correct for layout_stride::mapping
TYPED_TEST(LayoutStrideTest, InstanceObserversAreCorrect) {
  ASSERT_EQ(TypeParam{}.is_unique(), true);
  ASSERT_EQ(TypeParam{}.is_exhaustive(), true);
  ASSERT_EQ(TypeParam{}.is_strided(), true);

  ASSERT_TRUE(noexcept(TypeParam{}.is_unique()));
  ASSERT_TRUE(noexcept(TypeParam{}.is_exhaustive()));
  ASSERT_TRUE(noexcept(TypeParam{}.is_strided()));
}

/// @test @c is_exhaustive is @c true when the size of the multidimentional index space is zero
CONSTEXPR_TEST(LayoutStrideTest, IsExhaustiveIsTrueForZeroSize) {
  using extents_type = arene::base::dextents<std::uint32_t, 1>;

  auto const map = arene::base::layout_stride::mapping<extents_type>  //
      {                                                               //
       extents_type{},
       arene::base::array<std::uint32_t, 1>{1}
      };

  CONSTEXPR_ASSERT(map.is_exhaustive());
}

/// @test The equality and inequality operators for `layout_stride` work for built-in types
CONSTEXPR_TYPED_TEST(LayoutStrideTest, EqualityOperator) {
  using left_type = arene::base::layout_left::mapping<typename TypeParam::extents_type>;
  using right_type = arene::base::layout_right::mapping<typename TypeParam::extents_type>;
  constexpr left_type left{};
  constexpr right_type right{};

  bool left_and_right_are_same{true};
  for (typename TypeParam::rank_type dim{}; dim < TypeParam::extents_type::rank(); ++dim) {
    if (left.stride(dim) != right.stride(dim)) {
      left_and_right_are_same = false;
      break;
    }
  }

  // Default-constructed layout_stride::mapping is equal to itself and to a default-constructed layout_right::mapping;
  // if layout_left and layout_right are the same, then default layout_left::mapping is equal too.
  CONSTEXPR_ASSERT_EQ(TypeParam{}, TypeParam{});
  CONSTEXPR_ASSERT_EQ(TypeParam{} == TypeParam{left}, left_and_right_are_same);
  CONSTEXPR_ASSERT_EQ(TypeParam{}, TypeParam{right});

  CONSTEXPR_ASSERT_EQ(TypeParam{} == left, left_and_right_are_same);
  CONSTEXPR_ASSERT_EQ(TypeParam{}, right);
  CONSTEXPR_ASSERT_EQ(left == TypeParam{}, left_and_right_are_same);
  CONSTEXPR_ASSERT_EQ(right, TypeParam{});

  CONSTEXPR_ASSERT_FALSE(TypeParam{} != TypeParam{});
  CONSTEXPR_ASSERT_EQ(TypeParam{} != TypeParam{left}, !left_and_right_are_same);
  CONSTEXPR_ASSERT_FALSE(TypeParam{} != TypeParam{right});

  CONSTEXPR_ASSERT_EQ(TypeParam{} != left, !left_and_right_are_same);
  CONSTEXPR_ASSERT_FALSE(TypeParam{} != right);
  CONSTEXPR_ASSERT_EQ(left != TypeParam{}, !left_and_right_are_same);
  CONSTEXPR_ASSERT_FALSE(right != TypeParam{});
}

/// @test The equality and inequality operators for `layout_stride` with `layout_left` and `layout_right` are noexcept
CONSTEXPR_TYPED_TEST(LayoutStrideTest, EqualityOperatorNoexceptWithLeftAndRight) {
  using left_type = arene::base::layout_left::mapping<typename TypeParam::extents_type>;
  using right_type = arene::base::layout_right::mapping<typename TypeParam::extents_type>;

  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<TypeParam, left_type>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<TypeParam, left_type>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<left_type, TypeParam>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<left_type, TypeParam>);

  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<TypeParam, right_type>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<TypeParam, right_type>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<right_type, TypeParam>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<right_type, TypeParam>);
}

/// @test The constraints on the equality and inequality operators for `layout_stride` work in SFINAE
CONSTEXPR_TYPED_TEST(LayoutStrideTest, EqualityOperatorConstraints) {
  using extents_type = typename TypeParam::extents_type;
  using passes_constraints = test::equality_constraints_configurable<true, true, true>::mapping<extents_type>;

  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<TypeParam, passes_constraints>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<TypeParam, passes_constraints>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_equality_comparable_v<passes_constraints, TypeParam>);
  CONSTEXPR_ASSERT(arene::base::is_nothrow_inequality_comparable_v<passes_constraints, TypeParam>);

  using not_layout_mapping_alike = test::equality_constraints_configurable<false, true, true>::mapping<extents_type>;
  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<TypeParam, not_layout_mapping_alike>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<TypeParam, not_layout_mapping_alike>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<not_layout_mapping_alike, TypeParam>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<not_layout_mapping_alike, TypeParam>);

  using has_different_rank = test::equality_constraints_configurable<true, false, true>::mapping<extents_type>;
  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<TypeParam, has_different_rank>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<TypeParam, has_different_rank>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<has_different_rank, TypeParam>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<has_different_rank, TypeParam>);

  using not_strided = test::equality_constraints_configurable<true, true, false>::mapping<extents_type>;
  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<TypeParam, not_strided>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<TypeParam, not_strided>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_equality_comparable_v<not_strided, TypeParam>);
  CONSTEXPR_ASSERT_FALSE(arene::base::is_inequality_comparable_v<not_strided, TypeParam>);
}

/// @brief Get an instance of @c Extents where dynamic extents are all equal to @c value
/// @tparam Extents An @c extents type
/// @param value The value to use for all dynamic extents
/// @return An instance of @c Extents where dynamic extents are set to @c value
template <typename Extents>
constexpr auto make_extents_with_value(typename Extents::index_type value) noexcept {
  return test::make_extents_impl<Extents>(value, std::make_index_sequence<Extents::rank_dynamic()>{});
}

// Just a magic value that's different from anything in the extents under test
template <typename IndexType>
constexpr IndexType unique_extent{17236};

// A class which is comparable to TypeParam but which will let us change the parameters so they're unequal
template <bool DifferentExtents, bool DifferentStrides, bool DifferentOffsets>
class comparable {
 public:
  template <typename Extents>
  class mapping {
   public:
    using extents_type = Extents;
    using rank_type = typename extents_type::rank_type;
    using index_type = typename extents_type::index_type;
    using layout_type = comparable;

    static constexpr auto is_always_strided() noexcept -> bool { return true; }
    static constexpr auto is_always_exhaustive() noexcept -> bool { return true; }
    static constexpr auto is_always_unique() noexcept -> bool { return true; }

    static constexpr auto is_strided() noexcept -> bool { return true; }
    static constexpr auto is_exhaustive() noexcept -> bool { return true; }
    static constexpr auto is_unique() noexcept -> bool { return true; }

    template <typename OtherMapping>
    explicit constexpr mapping(OtherMapping const& other_mapping) noexcept
        : extents_{DifferentExtents ? make_extents_with_value<extents_type>(unique_extent<index_type>) : other_mapping.extents()},
          strides_{} {
      if (DifferentStrides) {
        arene::base::fill(strides_.begin(), strides_.end(), unique_extent<index_type>);
      } else {
        auto const other_strides = other_mapping.strides();
        arene::base::copy(other_strides.begin(), other_strides.end(), strides_.begin());
      }
    }

    constexpr auto extents() const noexcept -> extents_type const& { return extents_; }

    constexpr auto stride(rank_type dim) const noexcept -> index_type { return strides_[dim]; }

    // This class is only used to test comparisons, so this operator() will only ever be called with all-0 arguments.
    template <typename... T>
    constexpr auto operator()(T&&...) const noexcept -> index_type {
      return DifferentOffsets ? index_type{1} : index_type{};
    }

    // This span size doesn't need to be entirely correct, but it should correctly check if the span size is 0 or not.
    constexpr auto required_span_size() const noexcept -> index_type {
      return (extents_.rank() == rank_type{} || test::any_extent_is_0(extents_)) ? index_type{} : index_type{1};
    }

    // Equality operator is needed so that this mapping will satisfy is_layout_mapping_v; it's not used in these tests
    friend constexpr auto operator==(mapping const&, mapping const&) noexcept -> bool { return true; }

   private:
    extents_type extents_;
    arene::base::array<index_type, extents_type::rank()> strides_;
  };
};

/// @test Various ways mappings can not be equal work as expected (unless the rank is 0, when all mapping are equal)
CONSTEXPR_TYPED_TEST(LayoutStrideTest, EqualityOperatorMismatches) {
  bool const rank_not_0{TypeParam::extents_type::rank() > 0U};

  using extents_type = typename TypeParam::extents_type;
  using dyn_extents_type = arene::base::dextents<typename TypeParam::index_type, TypeParam::extents_type::rank()>;

  TypeParam const mapping;
  comparable<false, false, false>::template mapping<extents_type> const same_contents(mapping);
  comparable<true, false, false>::template mapping<dyn_extents_type> const different_extents(mapping);
  comparable<false, true, false>::template mapping<extents_type> const different_strides(mapping);
  comparable<false, false, true>::template mapping<extents_type> const different_offsets(mapping);

  CONSTEXPR_ASSERT_EQ(mapping, same_contents);
  CONSTEXPR_ASSERT_EQ(same_contents, mapping);

  // It's only possible for mappings to have different extents when their extents' rank is not zero.
  CONSTEXPR_ASSERT_EQ(mapping != different_extents, rank_not_0);
  CONSTEXPR_ASSERT_EQ(different_extents != mapping, rank_not_0);

  // Similarly, it's only possible for mappings to have different strides when their extents' rank is not zero.
  CONSTEXPR_ASSERT_EQ(mapping != different_strides, rank_not_0);
  CONSTEXPR_ASSERT_EQ(different_strides != mapping, rank_not_0);

  // For the offset to matter, the size of the output space must not be 0 (i.e. rank > 0 and no extents are 0).
  CONSTEXPR_ASSERT_EQ(mapping != different_offsets, rank_not_0 && !test::any_extent_is_0(mapping.extents()));
  CONSTEXPR_ASSERT_EQ(different_offsets != mapping, rank_not_0 && !test::any_extent_is_0(mapping.extents()));
}

}  // namespace
