// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"

namespace {
namespace iterator_tests = ::arene::base::iterator_tests;

template <typename Container>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct NextTest
    : protected iterator_tests::backing_data<Container>
    , public ::testing::Test {};

template <typename Container>
class NextNegativeStepDeathTest : public NextTest<Container> {};

TYPED_TEST_SUITE(NextNegativeStepDeathTest, iterator_tests::only_incrementable_iterator_containers, );

/// @test Given a negative step and a forward or input iterator, @c arene::base::next produces an @c
/// ARENE_PRECONDITION violation.
TYPED_TEST(NextNegativeStepDeathTest, ForwardOrInputInputIteratorIsPreconditionViolationIfStepIsNegative) {
  ASSERT_DEATH(std::ignore = ::arene::base::next(this->begin(), -1), "Precondition");
}
}  // namespace
