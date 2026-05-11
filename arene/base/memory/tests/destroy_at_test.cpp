// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/memory/destroy_at.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/new.hpp"  // IWYU pragma: keep
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::array;
using arene::base::byte;
using arene::base::destroy_at;

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicit global instance counter
// Utility class that records the number of calls to the constructor and destructor.
class TestUtility {
 public:
  TestUtility() { ++count_ctor; }
  ~TestUtility() { ++count_dtor; }

  static std::int32_t count_ctor;
  static std::int32_t count_dtor;
  static void reset() {
    count_ctor = 0;
    count_dtor = 0;
  }

  TestUtility(TestUtility const&) = delete;
  TestUtility(TestUtility&&) = delete;
  auto operator=(TestUtility const&) -> TestUtility& = delete;
  auto operator=(TestUtility&&) -> TestUtility& = delete;
};

std::int32_t TestUtility::count_ctor{0};
std::int32_t TestUtility::count_dtor{0};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables) explicit global instance counter

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) Explicitly testing placement construction into buffers.
/// @test `destroy_at` destroys the object at the input pointer for non-array types.
TEST(DestroyAt, DestroysObjectAtInputPtrForNonArrayType) {
  array<byte, sizeof(TestUtility)> buffer{};

  TestUtility::count_ctor = 0;
  TestUtility::count_dtor = 0;
  ::new (buffer.data()) TestUtility();
  ASSERT_EQ(TestUtility::count_ctor, 1);
  destroy_at(reinterpret_cast<TestUtility*>(buffer.data()));
  ASSERT_EQ(TestUtility::count_dtor, 1);

  ::new (buffer.data()) TestUtility();
  ASSERT_EQ(TestUtility::count_ctor, 2);
  destroy_at(reinterpret_cast<TestUtility*>(buffer.data()));
  ASSERT_EQ(TestUtility::count_dtor, 2);
}

/// @test `destroy_at` recursively destroys all objects for nested array types.
TEST(DestroyAt, RecursivelyDestroysObjectsForNestedArrayType) {
  TestUtility::reset();
  constexpr std::size_t count = 10;
  TestUtility arr[count][count];  // NOLINT(hicpp-avoid-c-arrays) explicitly testing behavior of c-arrays

  ASSERT_EQ(TestUtility::count_ctor, count * count);

  destroy_at(&arr);

  ASSERT_EQ(TestUtility::count_dtor, count * count);
}

/// @test `destroy_at` is always `noexcept`.
TEST(DestroyAt, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(destroy_at(std::declval<int*>())));
  STATIC_ASSERT_TRUE(noexcept(destroy_at(std::declval<arene::base::array<std::int32_t, 5>*>())));
  STATIC_ASSERT_TRUE(noexcept(destroy_at(std::declval<TestUtility*>())));
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) Explicitly testing placement construction into buffers.

}  // namespace
