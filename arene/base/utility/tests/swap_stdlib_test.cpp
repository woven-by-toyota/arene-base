// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <vector>

#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/byte/byte.hpp"
#include "arene/base/utility/swap.hpp"

namespace {

// These two test cases use types which are not implemented in the Arene Base Standard Library, so we only compile them
// when the platform Standard Library is in use.

/// @test Validates that @c arene::base::swap does not cause ambiguous overload resolution in the implementation of
/// @c std::unique_ptr<T>::reset() when @c T is a type in the @c arene::base namespace.
TEST(Swap, NoAmbigiousOverloadResolutionWithUniquePtr) {
  using ::arene::base::literals::operator""_byte;
  std::unique_ptr<arene::base::byte> byte_ptr = std::make_unique<::arene::base::byte>(10_byte);
  std::unique_ptr<arene::base::byte> other;
  byte_ptr.reset(other.release());  // NOLINT(misc-uniqueptr-reset-release) explicitly testing the behavior of reset.
}

/// @test check that the specialization of @c std::swap for @c std::vector is called
TEST(Swap, WithStdVector) {
  auto vec1 = std::vector<int>{1, 2, 3};
  auto vec2 = std::vector<int>{4};

  auto* const data1 = vec1.data();
  auto* const data2 = vec2.data();

  ::arene::base::swap(vec1, vec2);

  ASSERT_EQ(data1, vec2.data());
  ASSERT_EQ(data2, vec1.data());
}

}  // namespace
