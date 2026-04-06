// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/integer_sequences/sequential_values.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/testing/gtest.hpp"

// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"

namespace {

template <typename Mdspan>
class MdspanDeathTestSuite : public testing::Test {};

TYPED_TEST_SUITE(MdspanDeathTestSuite, arene::base::testing::mdspan_types, );

/// @test @c mdspan converting constructor is a precondition violation if a source extent doesn't match a target fixed
/// extent
CONDITIONAL_TYPED_TEST(
    MdspanDeathTestSuite,
    MdspanConvertingConstructorIsPreconditionViolationIfExtentDoesNotMatch,
    TypeParam::rank() > 0 && TypeParam::rank_dynamic() != TypeParam::rank()
) {
  using target_mdspan_type = TypeParam;
  using source_extents_type = arene::base::dextents<typename TypeParam::index_type, TypeParam::rank()>;
  using source_mdspan_type = arene::base::testing::rebind_mdspan_extents_t<target_mdspan_type, source_extents_type>;

  // Test successful conversion with matching extents
  auto success = source_mdspan_type{nullptr, arene::base::testing::all_extents_data_v<target_mdspan_type>};
  std::ignore = target_mdspan_type{success};

  // Test that mismatched extents cause death for each dimension that has a static extent
  for (auto dim : arene::base::sequential_values<typename TypeParam::rank_type, TypeParam::rank()>) {
    auto static_extent = TypeParam::static_extent(dim);
    auto mismatched_extents = arene::base::testing::all_extents_data_v<target_mdspan_type>;
    mismatched_extents[dim] = typename TypeParam::index_type{99};
    auto source = source_mdspan_type{nullptr, mismatched_extents};

    if (static_extent != arene::base::dynamic_extent) {
      ASSERT_DEATH(target_mdspan_type{source}, "Precondition");

      // This function called by the target_mdspan_type constructor in the above ASSERT_DEATH, but it won't be counted
      // in coverage unless it is explcitly called here with the same arguments.
      ASSERT_DEATH(
          arene::base::mdspan_detail::validate_extents_compatibility<target_mdspan_type>(source.extents()),
          "Precondition"
      );

    } else {
      // Dynamic extent: should succeed with any value
      std::ignore = target_mdspan_type{source};
    }
  }
}

/// @test accessing a multidimensional index out of bounds of the extents is a precondition violation
TYPED_TEST(MdspanDeathTestSuite, OutOfBoundsExtents) {
  using mdspan_type = arene::base::testing::mdspan_transform_element_type_t<TypeParam, std::add_const_t>;

  static constexpr auto data = arene::base::array<typename mdspan_type::value_type, 256>{};
  constexpr auto extents = arene::base::testing::make_extents<mdspan_type>();
  constexpr auto mdspan = mdspan_type{data.data(), extents};

  static_assert(  //
      mdspan.mapping().required_span_size() <= data.size(),
      "'data' is not correctly sized for this test"
  );

  for (auto dim : arene::base::sequential_values<std::size_t, mdspan_type::rank()>) {
    auto const index = [&extents, dim] {
      auto idx = arene::base::mdspan_detail::to_array(extents);
      ++idx[dim];
      return idx;
    }();

    ASSERT_DEATH(mdspan(index), "Precondition");
  }
}

/// @test providing negative index values is a precondition violation
TYPED_TEST(MdspanDeathTestSuite, NegativeIndices) {
  using mdspan_type = arene::base::testing::mdspan_transform_element_type_t<TypeParam, std::add_const_t>;

  static constexpr auto data = arene::base::array<typename mdspan_type::value_type, 256>{};
  constexpr auto extents = arene::base::testing::make_extents<mdspan_type>();
  constexpr auto mdspan = mdspan_type{data.data(), extents};

  static_assert(  //
      mdspan.mapping().required_span_size() <= data.size(),
      "'data' is not correctly sized for this test"
  );

  for (auto dim : arene::base::sequential_values<std::size_t, mdspan_type::rank()>) {
    auto const index = [dim] {
      auto idx = arene::base::array<std::int64_t, mdspan_type::rank()>{};
      idx[dim] = std::int64_t{-1};
      return idx;
    }();

    ASSERT_DEATH(mdspan(index), "Precondition");
  }
}

}  // namespace
