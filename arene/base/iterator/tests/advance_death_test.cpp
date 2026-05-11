// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/iterator/advance.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"

namespace {
using ::arene::base::advance;
namespace iterator_tests = ::arene::base::iterator_tests;

template <typename T>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct AdvanceTest
    : protected iterator_tests::backing_data<T>
    , public ::testing::Test {};

template <typename T>
class AdvanceInputIteratorDeathTest : public AdvanceTest<T> {};

TYPED_TEST_SUITE(AdvanceInputIteratorDeathTest, iterator_tests::only_incrementable_iterator_containers, );
/// @test Given a negative step and a forward or input iterator, @c arene::base::advance produces an
/// @c ARENE_PRECONDITION violation.
TYPED_TEST(AdvanceInputIteratorDeathTest, CallingWithNegativeValueIsPreconditionViolation) {
  auto itr = this->begin();
  ASSERT_DEATH(advance(itr, -1), "Precondition violation");
}

}  // namespace
