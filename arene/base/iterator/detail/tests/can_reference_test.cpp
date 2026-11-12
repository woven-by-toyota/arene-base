// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/detail/can_reference.hpp"

#include <gtest/gtest.h>

namespace {

using arene::base::iterator_detail::can_reference_v;

template <typename T, bool Referenceable>
struct CanReferenceTestCase {
  using type = T;
  static constexpr bool referenceable = Referenceable;
};

template <typename T>
class CanReferenceTest : public ::testing::Test {};

using cases = ::testing::Types<
    CanReferenceTestCase<int, true>,
    CanReferenceTestCase<int&, true>,
    CanReferenceTestCase<int&&, true>,
    CanReferenceTestCase<int*, true>,
    CanReferenceTestCase<int const, true>,
    CanReferenceTestCase<int const&, true>,
    CanReferenceTestCase<int const&&, true>,
    CanReferenceTestCase<int const*, true>,
    CanReferenceTestCase<void(), true>,
    CanReferenceTestCase<void(int), true>,
    CanReferenceTestCase<int(int, float, double), true>,
    CanReferenceTestCase<void, false>,
    CanReferenceTestCase<void*, true>,
    CanReferenceTestCase<void const, false>,
    CanReferenceTestCase<void() const, false>,
    CanReferenceTestCase<void()&, false>,
    CanReferenceTestCase<void()&&, false>,
    CanReferenceTestCase<void const*, true>>;

TYPED_TEST_SUITE(CanReferenceTest, cases, );

/// @test Given an object, reference, pointer, function, or non-referenceable function type `T`, when
/// `can_reference_v<T>` is evaluated, then it should be `true` if `T&` is well-formed, else `false`.
TYPED_TEST(CanReferenceTest, CanReference) {
  using type = typename TypeParam::type;
  constexpr bool expected = TypeParam::referenceable;
  EXPECT_EQ(can_reference_v<type>, expected);
}

}  // namespace
