// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/copy_elements.hpp"

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
#include "testlibs/utilities/configurable_value.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::test_array;

/// @brief Rebind an @c mdspan type to have a @c const element type, giving the read-only source counterpart of a
/// writeable destination span
template <typename Mdspan>
using const_source_t = arene::base::testing::mdspan_transform_element_type_t<Mdspan, std::add_const_t>;

/// @brief An element type unrelated to any in the test suite, used to build a destination whose reference cannot be
/// assigned from the source reference
struct foreign_element {};

/// @brief Unary trait mapping any element type to @c foreign_element, for rebinding an @c mdspan to a foreign,
/// non-assignable element type
template <typename>
using to_foreign_element = foreign_element;

/// @brief A typed test fixture covering representative writeable @c mdspan types across ranks 0-3, static and dynamic
template <typename T>
class LinalgCopyElementsTypedTest : public ::testing::Test {};

using copy_span_types = ::testing::Types<
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t>>,            // rank 0
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 4>>,         // rank 1 static
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 1>>,              // rank 1 dynamic
    arene::base::mdspan<float, arene::base::extents<std::size_t, 3, 4>>,             // rank 2 static
    arene::base::mdspan<std::uint32_t, arene::base::dextents<std::size_t, 2>>,       // rank 2 dynamic
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2, 3>>>;  // rank 3 static

TYPED_TEST_SUITE(LinalgCopyElementsTypedTest, copy_span_types, );

/// @test Ensure @c copy_elements is invocable for matching source and destination spans of every rank, whether the
/// source is @c const or writeable
CONSTEXPR_TYPED_TEST(LinalgCopyElementsTypedTest, InvocableForMatchingSpans) {
  using out_span_t = TypeParam;
  using in_span_t = const_source_t<TypeParam>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::copy_elements), in_span_t, out_span_t>));
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::copy_elements), out_span_t, out_span_t>));
}

/// @test After @c copy_elements over randomly generated inputs, the destination equals the source elementwise, for
/// spans of every rank
ARENE_TYPED_PROPERTY(
    LinalgCopyElementsTypedTest,
    RandomInputsMatchSourceElementwise,
    (test_array<TypeParam> source_arr)
) {
  test_array<TypeParam> dest_arr{};

  TypeParam const source = source_arr;
  TypeParam const dest = dest_arr;

  linalg::copy_elements(source, dest);

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(source, dest));
}

/// @test Ensure cannot invoke @c copy_elements if the span arguments are not mdspans
CONSTEXPR_TEST(LinalgCopyElements, CannotInvokeWithNonMdspans) {
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::copy_elements), int, int>));
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::
           is_invocable_v<decltype(linalg::copy_elements), arene::base::array<int, 5>, arene::base::array<int, 5>>)
  );
}

/// @test Ensure can invoke @c copy_elements between compatible static and dynamic extents
CONSTEXPR_TEST(LinalgCopyElements, CanInvokeWithCompatibleDynamicExtents) {
  using in_span_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using out_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::copy_elements), in_span_t, out_span_t>));
}

/// @test Ensure cannot invoke @c copy_elements if the span extents are not compatible or ranks differ
CONSTEXPR_TEST(LinalgCopyElements, CannotInvokeWithIncompatibleExtents) {
  using in_span_t = arene::base::mdspan<std::int32_t const, arene::base::extents<std::size_t, 1, 2, 3>>;
  using mismatched_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 7, 2, 4>>;
  using lower_rank_span_t = arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 1, 2>>;

  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::copy_elements), in_span_t, mismatched_span_t>));
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::copy_elements), in_span_t, lower_rank_span_t>));
}

/// @test Ensure cannot invoke @c copy_elements when the source reference cannot be assigned to the output reference,
/// for spans of every rank
CONSTEXPR_TYPED_TEST(LinalgCopyElementsTypedTest, CannotInvokeWithUnassignableOutput) {
  using in_span_t = const_source_t<TypeParam>;
  // A destination with a const element type has a non-assignable reference.
  using const_out_span_t = const_source_t<TypeParam>;
  // A destination with a foreign element type cannot be assigned from the source reference.
  using foreign_out_span_t = arene::base::testing::mdspan_transform_element_type_t<TypeParam, to_foreign_element>;

  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::copy_elements), in_span_t, const_out_span_t>));
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(linalg::copy_elements), in_span_t, foreign_out_span_t>));
}

/// @test @c copy_elements widens each element when the output element type is wider than the input
CONSTEXPR_TEST(LinalgCopyElements, WideningConversionToOutputElement) {
  using in_span_t = arene::base::mdspan<std::int16_t const, arene::base::extents<std::size_t, 4>>;
  using out_span_t = arene::base::mdspan<std::int64_t, arene::base::extents<std::size_t, 4>>;

  arene::base::array<std::int16_t, 4U> const source{1, 2, 3, 4};
  arene::base::array<std::int64_t, 4U> dest{};

  linalg::copy_elements(in_span_t{source.data()}, out_span_t{dest.data()});

  arene::base::array<std::int64_t, 4U> const expected{1, 2, 3, 4};
  CONSTEXPR_ASSERT_EQ(dest, expected);
}

/// @test @c copy_elements on empty spans is a no-op and does not crash, while the same instantiation still copies a
/// non-empty span, so both the empty and non-empty paths are exercised for a single @c copy_elements instantiation
CONSTEXPR_TEST(LinalgCopyElements, HandlesEmptyAndNonEmptySpans) {
  using in_span_t = arene::base::mdspan<
      float const,
      arene::base::extents<std::uint32_t, arene::base::dynamic_extent, 6, arene::base::dynamic_extent>>;
  using out_span_t = arene::base::
      mdspan<float, arene::base::extents<std::uint32_t, 5, arene::base::dynamic_extent, arene::base::dynamic_extent>>;

  // Empty span: copy_elements takes the early-return path and must be a no-op.
  arene::base::array<float, 0> const empty_source_data{};
  arene::base::array<float, 0> empty_dest_data{};

  arene::base::array<std::uint32_t, 3> const empty_extents{5, 6, 0};
  in_span_t const empty_source{empty_source_data.data(), empty_extents};
  out_span_t const empty_dest{empty_dest_data.data(), empty_extents};

  linalg::copy_elements(empty_source, empty_dest);

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(empty_source, empty_dest));

  // Non-empty span of the same instantiation: copy_elements enters the loop body and copies every element.
  arene::base::array<float, static_cast<std::size_t>(5U * 6U * 2U)> source_data{};
  for (std::size_t idx{}; idx < source_data.size(); ++idx) {
    source_data[idx] = static_cast<float>(idx) + 0.5F;
  }
  arene::base::array<float, static_cast<std::size_t>(5U * 6U * 2U)> dest_data{};

  arene::base::array<std::uint32_t, 3> const extents{5, 6, 2};
  in_span_t const source{source_data.data(), extents};
  out_span_t const dest{dest_data.data(), extents};

  linalg::copy_elements(source, dest);

  CONSTEXPR_ASSERT(arene::base::linalg::equal_elements(source, dest));
}

/// @test @c copy_elements over a fully aliased span leaves the elements unchanged, for spans of every rank
ARENE_TYPED_PROPERTY(LinalgCopyElementsTypedTest, FullyAliasedCopyIsIdentity, (test_array<TypeParam> source_arr)) {
  test_array<TypeParam> expected_arr = source_arr;  // snapshot the random contents before the aliased copy

  TypeParam const aliased = source_arr;  // one span used as both source and destination
  TypeParam const expected = expected_arr;

  linalg::copy_elements(aliased, aliased);

  ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(aliased, expected));
}

/// @test @c noexcept-ness of @c copy_elements follows the noexcept-ness of the underlying assignment
CONSTEXPR_TEST(LinalgCopyElements, NoexceptFollowsUnderlyingAssignment) {
  using non_throwing = ::testing::configurable_value<std::int32_t>;
  // copy_elements assigns from an lvalue reference, so the destination's copy-assignment is what governs noexcept.
  using throwing_assign = ::testing::configurable_value<std::int32_t, ::testing::throws_on::copy_assign>;

  using span_t = arene::base::mdspan<non_throwing, arene::base::extents<std::size_t, 1, 2, 3>>;
  CONSTEXPR_ASSERT((arene::base::is_nothrow_invocable_v<decltype(linalg::copy_elements), span_t, span_t>));

  using throwing_span_t = arene::base::mdspan<throwing_assign, arene::base::extents<std::size_t, 1, 2, 3>>;
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(linalg::copy_elements), throwing_span_t, throwing_span_t>));
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_nothrow_invocable_v<decltype(linalg::copy_elements), throwing_span_t, throwing_span_t>)
  );
}

}  // namespace
