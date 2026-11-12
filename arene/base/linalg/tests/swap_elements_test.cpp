// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/swap_elements.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/swap.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief An element type unrelated to any in the test suite, used to build a destination whose reference cannot be
/// assigned from the source reference
struct foreign_element {};

/// @brief A wrapped version of T which is swappable with T despite being a distinct type
template <typename T>
struct wrapped_value {
  T value;

  friend constexpr auto operator==(wrapped_value const& left, wrapped_value const& right) -> bool {
    return left.value == right.value;
  }

  friend constexpr auto swap(wrapped_value<T>& wrapped, T& unwrapped) -> void {
    arene::base::swap(wrapped.value, unwrapped);
  }

  friend constexpr auto swap(T& unwrapped, wrapped_value<T>& wrapped) -> void {
    arene::base::swap(unwrapped, wrapped.value);
  }
};

/// @brief A wrapped version of T which is swappable with T on one side only
template <typename T>
struct incompletely_swappable {
  T value;

  // Swap is only implemented on one side -- this doesn't satisfy is_swappable_with so the algorithm is not invocable
  friend constexpr auto swap(incompletely_swappable<T>& wrapped, T& unwrapped) -> void {
    arene::base::swap(wrapped.value, unwrapped);
  }
};

/// @brief Unary traits for rebinding the element types of mdspans
template <typename>
using to_foreign_element = foreign_element;

template <typename T>
using to_wrapped_value = wrapped_value<T>;

template <typename T>
using to_incompletely_swappable = incompletely_swappable<T>;

/// @brief A typed test fixture covering representative writeable @c mdspan types across ranks 0-3, static and dynamic
template <typename T>
class LinalgSwapElementsTypedTest : public ::testing::Test {};

using swap_span_types = ::testing::Types<
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t>>,            // rank 0
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 4>>,         // rank 1 static
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>,              // rank 1 dynamic
    arene::base::mdspan<float, arene::base::extents<std::size_t, 3, 4>>,             // rank 2 static
    arene::base::mdspan<std::uint32_t, arene::base::dextents<std::size_t, 2>>,       // rank 2 dynamic
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>>;  // rank 3 static

TYPED_TEST_SUITE(LinalgSwapElementsTypedTest, swap_span_types, );

/// @test Ensure @c swap_elements is invocable for matching left and right spans of every rank
CONSTEXPR_TYPED_TEST(LinalgSwapElementsTypedTest, InvocableForMatchingSpans) {
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), TypeParam, TypeParam>));
}

/// @test After @c swap_elements over randomly generated inputs, the contents are swapped, for spans of every rank
ARENE_TYPED_PROPERTY(
    LinalgSwapElementsTypedTest,
    RandomInputsCanBeSwapped,
    (test_array<TypeParam> const original_left_arr, test_array<TypeParam> const original_right_arr)
) {
  test_array<TypeParam> left_arr{original_left_arr};
  test_array<TypeParam> right_arr{original_right_arr};

  TypeParam const left = left_arr;
  TypeParam const right = right_arr;

  linalg::swap_elements(left, right);

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(left, original_right_arr.mdspan()));
  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(right, original_left_arr.mdspan()));
}

/// @test Ensure cannot invoke @c swap_elements if the span arguments are not mdspans
CONSTEXPR_TEST(LinalgSwapElements, CannotInvokeWithNonMdspans) {
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::swap_elements), int&, int&>));
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::
           is_invocable_v<decltype(linalg::swap_elements), arene::base::array<int, 5>&, arene::base::array<int, 5>&>)
  );
}

/// @test Ensure can invoke @c swap_elements between compatible static and dynamic extents
CONSTEXPR_TEST(LinalgSwapElements, CanInvokeWithCompatibleDynamicExtents) {
  using left_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using right_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), left_span_t, right_span_t>));
}

/// @test Ensure cannot invoke @c swap_elements if the span extents are not compatible or ranks differ
CONSTEXPR_TEST(LinalgSwapElements, CannotInvokeWithIncompatibleExtents) {
  using left_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>;
  using mismatched_right_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 7, 2, 4>>;
  using lower_rank_right_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2>>;

  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(linalg::swap_elements), left_span_t, mismatched_right_span_t>)
  );
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(linalg::swap_elements), left_span_t, lower_rank_right_span_t>)
  );
}

/// @test Ensure cannot invoke @c swap_elements when one span's element type cannot be swapped, for spans of every rank
CONSTEXPR_TYPED_TEST(LinalgSwapElementsTypedTest, CannotInvokeWithUnswappableOutput) {
  using raw_span_t = TypeParam;
  // A span with a const element type has a non-swappable reference.
  using const_span_t = arene::base::testing::mdspan_transform_element_type_t<TypeParam, std::add_const_t>;
  // A span with a foreign element type cannot be swapped with the original reference type.
  using foreign_span_t = arene::base::testing::mdspan_transform_element_type_t<TypeParam, to_foreign_element>;

  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), raw_span_t, raw_span_t>));

  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::swap_elements), raw_span_t, const_span_t>));
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::swap_elements), const_span_t, raw_span_t>));

  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::swap_elements), raw_span_t, foreign_span_t>));
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::swap_elements), foreign_span_t, raw_span_t>));
}

/// @test Can invoke @c swap_elements when the left reference can be swapped with the right reference
CONSTEXPR_TYPED_TEST(LinalgSwapElementsTypedTest, CanInvokeWithAdlSwap) {
  using arene::base::testing::mdspan_transform_element_type_t;

  // A second span with an element type that is distinct from the original but swappable with it
  using wrapped_span_t = mdspan_transform_element_type_t<TypeParam, to_wrapped_value>;
  // A second span with an element type that has an ADL span with the original element type, but only on one side
  using incompletely_swappable_span_t = mdspan_transform_element_type_t<TypeParam, to_incompletely_swappable>;

  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), TypeParam, wrapped_span_t>));
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(linalg::swap_elements), TypeParam, incompletely_swappable_span_t>)
  );
}

/// @test @c swap_elements works when elements of the two mdspans are only swappable using an ADL customization
CONSTEXPR_TEST(LinalgSwapElements, AdlSwapOfElements) {
  using wval = wrapped_value<std::int64_t>;
  using wrapped_span_t = arene::base::mdspan<wval, arene::base::extents<std::size_t, 4>>;
  using unwrapped_span_t = arene::base::mdspan<std::int64_t, arene::base::extents<std::size_t, 4>>;

  arene::base::array<wval, 4U> left{wval{1}, wval{2}, wval{3}, wval{4}};
  arene::base::array<std::int64_t, 4U> right{5, 6, 7, 8};

  linalg::swap_elements(wrapped_span_t{left.data()}, unwrapped_span_t{right.data()});

  arene::base::array<wval, 4U> const expected_left{wval{5}, wval{6}, wval{7}, wval{8}};
  CONSTEXPR_ASSERT_EQ(left, expected_left);

  arene::base::array<std::int64_t, 4U> const expected_right{1, 2, 3, 4};
  CONSTEXPR_ASSERT_EQ(right, expected_right);

  // swap_elements is invocable on all combinations of the wrapped and unwrapped types
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), wrapped_span_t, unwrapped_span_t>));
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), unwrapped_span_t, wrapped_span_t>));
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), wrapped_span_t, wrapped_span_t>));
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::swap_elements), unwrapped_span_t, unwrapped_span_t>));
}

/// @test @c swap_elements on empty spans is a no-op and does not crash
CONSTEXPR_TEST(LinalgSwapElements, HandlesEmptyDynamicSpans) {
  using left_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using right_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  arene::base::array<float, 0> empty_left_data{};
  arene::base::array<float, 0> empty_right_data{};

  arene::base::array<std::uint32_t, 3> const empty_extents{5, 6, 0};
  left_span_t const empty_left{empty_left_data.data(), empty_extents};
  right_span_t const empty_right{empty_right_data.data(), empty_extents};

  linalg::swap_elements(empty_left, empty_right);

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(empty_left, empty_right));
}

/// @test @c swap_elements on non-empty dynamic spans causes the elements to be swapped
CONSTEXPR_TEST(LinalgSwapElements, HandlesNonEmptyDynamicSpans) {
  using left_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using right_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  arene::base::array<float, static_cast<std::size_t>(5U * 6U * 2U)> left_data{};
  arene::base::array<float, static_cast<std::size_t>(5U * 6U * 2U)> right_data{};
  for (std::size_t idx{}; idx < left_data.size(); ++idx) {
    left_data[idx] = static_cast<float>(idx) + 0.5F;
    right_data[idx] = static_cast<float>(idx) - 0.5F;
  }
  decltype(left_data) original_left_data{left_data};
  decltype(right_data) original_right_data{right_data};

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 2};
  left_span_t const left{left_data.data(), extents};
  right_span_t const right{right_data.data(), extents};

  linalg::swap_elements(left, right);

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(left, right_span_t{original_right_data.data(), extents}));
  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(right, left_span_t{original_left_data.data(), extents}));
}

/// @test @c swap_elements over a fully aliased span leaves the elements unchanged, for spans of every rank
ARENE_TYPED_PROPERTY(LinalgSwapElementsTypedTest, FullyAliasedSwapIsIdentity, (test_array<TypeParam> source_arr)) {
  test_array<TypeParam> expected_arr = source_arr;  // snapshot the random contents before the aliased swap

  TypeParam const aliased = source_arr;  // one span used as both source and destination
  TypeParam const expected = expected_arr;

  linalg::swap_elements(aliased, aliased);

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(aliased, expected));
}

// Two typedefs to be used in noexcept tests. The arrow is irrelevant, just used to make this a distinct type that's
// not used elsewhere. What's relevant is that throwing_arrow does not throw on assignments, and throwing_assign does.
using throwing_arrow = ::testing::configurable_value<std::int32_t, ::testing::throws_on::arrow>;
using throwing_assign = ::testing::configurable_value<std::int32_t, ::testing::throws_on::copy_assign>;

}  // namespace

namespace testing {
// A non-throwing swap for a type that has throwing assignment
constexpr auto swap(throwing_assign& left, throwing_assign& right) noexcept -> void {
  arene::base::swap(left.value, right.value);
}

// A throwing swap for a type that has non-throwing assignment
constexpr auto swap(throwing_arrow& left, throwing_arrow& right) noexcept(false) -> void {
  arene::base::swap(left.value, right.value);
}
}  // namespace testing

namespace {
/// @test @c noexcept-ness of @c swap_elements follows the noexcept-ness of the underlying swap
TEST(LinalgSwapElements, NoexceptFollowsUnderlyingSwap) {
  using non_throwing = ::testing::configurable_value<std::int32_t>;

  using span_t = arene::base::mdspan<non_throwing, arene::base::extents<std::size_t, 1, 2, 3>>;
  EXPECT_TRUE((arene::base::is_nothrow_invocable_v<decltype(linalg::swap_elements), span_t, span_t>));

  using non_throwing_span_t = arene::base::mdspan<throwing_assign, arene::base::extents<std::size_t, 1, 2, 3>>;
  // swap_elements is noexcept because _swap_ is noexcept, even though assign is not
  EXPECT_TRUE(
      (arene::base::is_nothrow_invocable_v<decltype(linalg::swap_elements), non_throwing_span_t, non_throwing_span_t>)
  );

  using throwing_span_t = arene::base::mdspan<throwing_arrow, arene::base::extents<std::size_t, 1, 2, 3>>;
  EXPECT_TRUE((arene::base::is_invocable_v<decltype(linalg::swap_elements), throwing_span_t, throwing_span_t>));
  // swap_elements is noexcept(false) because _swap_ is noexcept(false), even though assign is noexcept(true)
  EXPECT_FALSE((arene::base::is_nothrow_invocable_v<decltype(linalg::swap_elements), throwing_span_t, throwing_span_t>)
  );
}

}  // namespace
