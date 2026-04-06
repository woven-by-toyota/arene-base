// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/submdspan_extents.hpp"

#include <gtest/gtest.h>

#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/tests/submdspan_slices_handling_test_common.hpp"  //  IWYU pragma: keep
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"

namespace {
using ::arene::base::extents;
using ::arene::base::submdspan_extents;

template <class T>
struct SubmdspanExtents : testing::Test {};

using IndexTypes = ::testing::Types<int, std::size_t>;

TYPED_TEST_SUITE(SubmdspanExtents, IndexTypes, );

/// @test @c submdspan_extents is not invocable if the number of
/// slices does not match the rank of the extents
TYPED_TEST(SubmdspanExtents, InvocableIfSameSize) {
  using index_type = TypeParam;
  static_assert_invocable_if_same_size<decltype(submdspan_extents), index_type>();
}

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,fuchsia-statically-constructed-objects)
auto const canonicalize_with_extents =
    [](auto... slices) noexcept(  //
        noexcept(arene::base::submdspan_extents(extents<int, 3, 3>{}, std::declval<decltype(slices)>()...))
    ) { return arene::base::submdspan_extents(extents<int, 3, 3>{}, slices...); };

struct test_config {
  static decltype((canonicalize_with_extents)) function_under_test;
};

decltype((canonicalize_with_extents)) test_config::function_under_test = canonicalize_with_extents;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables,fuchsia-statically-constructed-objects)

using test_config_type = ::testing::Types<test_config>;

INSTANTIATE_TYPED_TEST_SUITE_P(SubmdspanExtents, SubmdspanSliceHandlingCommon, test_config_type, );

}  // namespace
