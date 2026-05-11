// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/tests/wrapped_iterator/test_template.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace wrapped_iterator_tests {

// NOLINTBEGIN(hicpp-avoid-c-arrays) Explicitly testing behavior with c-arrays.

/// A span of iterator types including raw pointers and complex iterators, which does not include any stdlib types not
/// provided by stdlib_choice.
/// @note std::iterator_list<>::iterator is a pointer in //stdlib
using nostdlib_iterator_types = arene::base::type_lists::remove_duplicates_t<::testing::Types<
    int*,
    int const*,
    char*,
    char const*,
    std::initializer_list<int>::iterator,
    std::initializer_list<int>::const_iterator,
    testing::demoted_iterator<int*>,
    testing::demoted_iterator<int const*>>>;

INSTANTIATE_TYPED_TEST_SUITE_P(NostdlibTypes, WrappedIteratorMemberTypesTest, nostdlib_iterator_types, );
INSTANTIATE_TYPED_TEST_SUITE_P(NostdlibTypes, WrappedIteratorReturnTypeEquivalenceTest, nostdlib_iterator_types, );

using nostdlib_container_types = ::testing::Types<int[10], int const[10], char[10], char const[10]>;

INSTANTIATE_TYPED_TEST_SUITE_P(NostdlibTypes, WrappedIteratorOperationEquivalenceTest, nostdlib_container_types, );

// NOLINTEND(hicpp-avoid-c-arrays)

}  // namespace wrapped_iterator_tests
