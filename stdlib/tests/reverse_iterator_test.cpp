// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/reverse_iterator.hpp"

#include "stdlib/include/iterator"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test check that @c arene::base::reverse_iterator can be used without a toolchain provided C++ standard library
TEST(ReverseIteratorTest, Usable) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int values[] = {1, 2, 3};

  auto iter = ::arene::base::make_reverse_iterator(std::end(values));

  ASSERT_EQ(3, *iter);
  ASSERT_EQ(2, *++iter);
  ASSERT_EQ(1, *++iter);
}

}  // namespace
