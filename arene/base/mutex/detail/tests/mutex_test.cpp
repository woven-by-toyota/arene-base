// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mutex/detail/mutex.hpp"

#include <gtest/gtest.h>

#include "arene/base/mutex/detail/tests/mock_mutex.hpp"  // IWYU pragma: keep
#include "arene/base/mutex/detail/tests/mutex_test_base.hpp"

// Note: INSTANTIATE_TYPED_TEST_SUITE_P can't handle FQNs, but also needs ADL to work, so you have to instantiate the
// test in the same namespace as the test suite was declared in. Simple imports aren't enough, it has to actually be in
// the same namespace.
namespace arene {
namespace base {
namespace mutex_test {

using ::arene::base::mutex_detail::mutex;
using ::arene::base::mutex_test::mock_mutex;

INSTANTIATE_TYPED_TEST_SUITE_P(Mutex, MutexBaseTest, mutex<mock_mutex>, );

/// @test Ensure `selected_mutex` wraps `std::mutex`
TEST(SelectedMutex, IsMutexWrapper) {
  ::testing::StaticAssertTypeEq<::arene::base::mutex_detail::selected_mutex, mutex<std::mutex>>();
}

}  // namespace mutex_test
}  // namespace base
}  // namespace arene
