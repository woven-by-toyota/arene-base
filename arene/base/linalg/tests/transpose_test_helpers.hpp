// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TRANSPOSE_TEST_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TRANSPOSE_TEST_HELPERS_HPP_

#include "arene/base/linalg/layout_transpose.hpp"
#include "arene/base/mdspan/detail/reverse_extents.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"

namespace arene {
namespace base {
namespace linalg_testing {

/// @brief The transposed mapping type that wraps the nested mapping @c NestedMapping
template <class NestedMapping>
using transposed_mapping_for = typename linalg::layout_transpose<typename NestedMapping::layout_type>::template mapping<
    mdspan_detail::reverse_extents_t<typename NestedMapping::extents_type>>;

/// @brief Build the nested mapping over expanded test extents
/// @tparam NestedMapping The (un-transposed) mapping type to build
template <class NestedMapping>
constexpr auto make_nested() noexcept -> NestedMapping {
  return testing::make_mapping_with_extents<NestedMapping>(
      test::make_expanded_extents<typename NestedMapping::extents_type>()
  );
}

/// @brief Build the transposed mapping wrapping @c make_nested<NestedMapping>()
/// @tparam NestedMapping The (un-transposed) mapping type to transpose
template <class NestedMapping>
constexpr auto make_transposed() noexcept -> transposed_mapping_for<NestedMapping> {
  return transposed_mapping_for<NestedMapping>{make_nested<NestedMapping>()};
}

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_TRANSPOSE_TEST_HELPERS_HPP_
