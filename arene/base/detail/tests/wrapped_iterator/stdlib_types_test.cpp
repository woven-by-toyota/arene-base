// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/detail/tests/wrapped_iterator/helpers.hpp"
#include "arene/base/detail/tests/wrapped_iterator/test_template.hpp"

namespace wrapped_iterator_tests {

template <>
class test_container_storage<std::string> {
 public:
  std::string data_{"1234567890"};
};

/// A span of iterator types that require stdlib types not backported by stdlib_choice.
using stdlib_iterator_types = ::testing::Types<
    std::string::iterator,
    std::string::const_iterator,
    std::vector<int>::iterator,
    std::vector<int>::const_iterator,
    std::array<int, 10>::iterator,
    std::array<int, 10>::const_iterator>;

INSTANTIATE_TYPED_TEST_SUITE_P(StdlibTypes, WrappedIteratorMemberTypesTest, stdlib_iterator_types, );
INSTANTIATE_TYPED_TEST_SUITE_P(StdlibTypes, WrappedIteratorReturnTypeEquivalenceTest, stdlib_iterator_types, );

using stdlib_container_types = ::testing::Types<std::string, std::vector<int>, std::array<int, 10>>;

INSTANTIATE_TYPED_TEST_SUITE_P(StdlibTypes, WrappedIteratorOperationEquivalenceTest, stdlib_container_types, );

/// @test A `wrapped_iterator` can be constructed from a `wrapped_iterator` with a different underlying iterator type if
/// the underlying iterator of the `wrapped_iterator` being constructed can be constructed from the underlying iterator
/// of the argument
TEST(WrappedIteratorConversion, ValidWithCompatibleUnderlying) {
  std::vector<int> values{1, 2, 3, 4};
  ASSERT_EQ(
      wrapped_iterator_with_default_passkey<int const*>(
          wrapped_iterator_with_default_passkey<int*>(default_passkey{}, values.data())
      )
          .base(),
      values.data()
  );
}

}  // namespace wrapped_iterator_tests
