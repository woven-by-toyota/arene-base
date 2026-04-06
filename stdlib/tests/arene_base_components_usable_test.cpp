// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test Verify that the use of components from arene-base in stdlib does not
/// introduce a cyclic dependency. There is a fine-grained dependency graph that
/// allows components in stdlib to depend on components in arene-base which may
/// in turn depend on other components in stdlib.
///
TEST(AreneBaseUsableInStdLib, HasBasicInputIteratorOperationTraitUsable) {
  using arene::base::is_input_iterator_v;

  static_assert(is_input_iterator_v<char*>, "");
  static_assert(is_input_iterator_v<char const*>, "");
  static_assert(!is_input_iterator_v<int>, "");
}

}  // namespace
