// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/deque.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/inline_container/circular_buffer.hpp"
#include "arene/base/inline_container/testing/deque.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/testing/gtest.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

namespace {

using ::arene::base::inline_deque;
using counting_wrapper = ::testing::counting_wrapper<int>;

////////////////////////////
// counting_wrapper tests //
////////////////////////////

using counting_wrapper_deque = inline_deque<counting_wrapper, 32>;

/// @test `emplace_front` and `emplace_back` construct values in place from their arguments without copying or moving
TEST(InlineDequeCountingWrapper, EmplaceFrontAndBackConstructValuesInPlace) {
  counting_wrapper::count = {};
  counting_wrapper_deque deque;
  deque.emplace_front(0);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.value_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
  deque.emplace_back(1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.value_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);

  EXPECT_EQ(deque.size(), 2U);
  deque.pop_back();
  deque.pop_back();
  EXPECT_TRUE(deque.empty());
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.value_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 2);
}

/// @test When invoked with an rvalue, `push_front` moves from the supplied value when constructing the new element
/// in the `inline_deque`. Then, removing an element with `pop_back`, only causes the destruction of one element
TEST(InlineDequeCountingWrapper, PushFrontMovesFromRvalueAndPopBackOnlyDestroys) {
  counting_wrapper::count = {};
  counting_wrapper_deque deque;
  deque.push_front(counting_wrapper{});
  EXPECT_EQ(counting_wrapper::count.default_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 1);
  deque.push_front(counting_wrapper{});
  EXPECT_EQ(counting_wrapper::count.default_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 2);
  deque.pop_back();
  EXPECT_EQ(counting_wrapper::count.default_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 3);
}

/// @test When invoked with an rvalue, `push_back` moves from the supplied value when constructing the new element
/// in the `inline_deque`. Then, removing an element with `pop_front`, only causes the destruction of one element
TEST(InlineDequeCountingWrapper, PushBackMovesFromRvalueAndPopBackOnlyDestroys) {
  counting_wrapper::count = {};
  counting_wrapper_deque deque;
  deque.push_back(counting_wrapper{});
  EXPECT_EQ(counting_wrapper::count.default_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 1);
  deque.push_back(counting_wrapper{});
  EXPECT_EQ(counting_wrapper::count.default_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 2);
  deque.pop_front();
  EXPECT_EQ(counting_wrapper::count.default_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 3);
}

/// @test When invoked with an lvalue, `push_front` copies from the supplied value when constructing the new element
TEST(InlineDequeCountingWrapper, PushFrontCopiesFromLvalue) {
  counting_wrapper_deque deque;
  counting_wrapper const value{};
  counting_wrapper::count = {};

  deque.push_front(value);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
  deque.push_front(value);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test When invoked with an lvalue, `push_back` copies from the supplied value when constructing the new element
TEST(InlineDequeCountingWrapper, PushBackCopiesFromLvalue) {
  counting_wrapper_deque deque;
  counting_wrapper const value{};
  counting_wrapper::count = {};

  deque.push_back(value);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
  deque.push_back(value);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_deque` copy constructor copies the elements from the original deque
TEST(InlineDequeCountingWrapper, CopyingDequeCopiesElements) {
  constexpr std::size_t count = 3;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  auto const copy(deque);

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, count);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_deque` move constructor moves the stored elements from the original deque
TEST(InlineDequeCountingWrapper, MovingConstructMovesElements) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  counting_wrapper_deque const moved(std::move(deque));

  EXPECT_EQ(moved.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count);
}

/// @test The `inline_deque` copy-assignment operator does nothing for self-assignment
TEST(InlineDequeCountingWrapper, CopyAssignFromSelfDoesNothing) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};
  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wself-assign-overloaded", "Testing self assignment");
  deque = deque;
  ARENE_IGNORE_END();
  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_deque` copy-assignment operator uses copy assignment to existing elements in the destination and
/// copy construction for additional elements in the source
TEST(InlineDequeCountingWrapper, CopyAssignOverSmallerCopiesElements) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper_deque copy;
  constexpr std::size_t smaller_count = 10;
  for (std::size_t i = 0; i < smaller_count; i++) {
    copy.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(copy.size(), smaller_count);

  copy = deque;

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, count - smaller_count);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, smaller_count);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_deque` copy-assignment operator uses copy assignment to existing elements and destroys excess
/// elements in the destination
TEST(InlineVectorCountingWrapper, CopyAssignOverLargerDestroysExcess) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper_deque copy;
  constexpr std::size_t larger_count = 18;
  for (std::size_t i = 0; i < larger_count; i++) {
    copy.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(copy.size(), larger_count);

  copy = deque;

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, count);
  EXPECT_EQ(counting_wrapper::count.dtor, larger_count - count);
}

/// @test The `inline_deque` move-assignment operator does nothing for self-assignment
TEST(InlineDequeCountingWrapper, MoveAssignFromSelfDoesNothing) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }

  counting_wrapper::count = {};

  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wself-move", "testing self assignment");
  deque = std::move(deque);
  ARENE_IGNORE_END();

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_deque` move-assignment operator uses move assignment to existing elements in the destination and
/// move construction for additional elements in the source
TEST(InlineDequeCountingWrapper, MoveAssignOverSmallerMovesElements) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper_deque moved;
  constexpr std::size_t smaller_count = 10;
  for (std::size_t i = 0; i < smaller_count; i++) {
    moved.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(moved.size(), smaller_count);

  moved = std::move(deque);

  EXPECT_EQ(moved.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count - smaller_count);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, smaller_count);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count);
}

/// @test The `inline_deque` move-assignment operator uses move assignment to existing elements and destroys excess
/// elements in the destination
TEST(InlineVectorCountingWrapper, MoveAssignOverLargerDestroysExcess) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper_deque moved;
  constexpr std::size_t larger_count = 18;
  for (std::size_t i = 0; i < larger_count; i++) {
    moved.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  EXPECT_EQ(deque.size(), count);
  EXPECT_EQ(moved.size(), larger_count);

  moved = std::move(deque);

  EXPECT_EQ(moved.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, count);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, larger_count);
}

/// @test Invoking `pop_back` on a non-empty `inline_deque` destroys an element and reduces the size by one
TEST(InlineDequeCountingWrapper, PopBackDestroysOneAndUpdatesSize) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  EXPECT_EQ(deque.size(), count);

  deque.pop_back();

  EXPECT_EQ(deque.size(), count - 1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 1);
}

/// @test Invoking `pop_front` on a non-empty `inline_deque` destroys an element and reduces the size by one
TEST(InlineDequeCountingWrapper, PopFrontDestroysOneAndUpdatesSize) {
  constexpr std::size_t count = 14;
  counting_wrapper_deque deque;
  for (std::size_t i = 0; i < count; i++) {
    deque.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  EXPECT_EQ(deque.size(), count);

  deque.pop_front();

  EXPECT_EQ(deque.size(), count - 1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 1);
}

/// @test An `inline_deque` on the left side of an assignment destroys any existing elements
TEST(InlineDequeCountingWrapper, MoveAssignDestroysOldElements) {
  constexpr std::size_t count = 11;
  counting_wrapper_deque value;
  for (std::size_t i = 0; i < count; i++) {
    value.push_front(counting_wrapper{});
  }
  counting_wrapper::count = {};

  value = counting_wrapper_deque{};
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count);
}

/// @test Default-constructing and destroying an `inline_deque` of a user-defined type does not destroy any elements
TEST(InlineDequeCountingWrapper, NoObjectsDestroyedOnEmptyDeque) {
  counting_wrapper::count = {};
  { counting_wrapper_deque const deque; }
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test When an `inline_deque` of a user-defined type is destroyed, then the destructor is called for each element
TEST(InlineDequeCountingWrapper, InsertedObjectsDestroyedWhenDequeDestroyed) {
  counting_wrapper const source;
  counting_wrapper::count = {};
  {
    counting_wrapper_deque deque;
    deque.push_front(source);
    deque.push_front(source);
    deque.push_front(source);
  }
  EXPECT_EQ(counting_wrapper::count.dtor, 3);
}

/// @test When a `circular_buffer` is already full and a new element is pushed in, an existing element is destroyed
TEST(InlineDequeCountingWrapper, CircularBufferOverwrites) {
  counting_wrapper const source;
  counting_wrapper::count = {};
  {
    arene::base::circular_buffer<counting_wrapper, 3U> queue;
    queue.push_back(source);
    queue.push_back(source);
    queue.push_back(source);
    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(counting_wrapper::count.copy_ctor, 3);
    EXPECT_EQ(counting_wrapper::count.dtor, 0);

    queue.push_back(source);
    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(counting_wrapper::count.copy_ctor, 4);
    EXPECT_EQ(counting_wrapper::count.dtor, 1);
  }
  EXPECT_EQ(counting_wrapper::count.dtor, 4);
}

}  // namespace

using arene::base::literals::operator""_asv;

using not_default_constructible =
    ::testing::configurable_value<int, ::testing::throws_on::nothing, ::testing::disable::default_construct>;

using non_copyable = ::testing::configurable_value<
    int,
    ::testing::throws_on::nothing,
    ::testing::disable::copy_construct | ::testing::disable::copy_assign>;

using noexcept_false = ::testing::configurable_value<std::size_t, ::testing::throws_on::everything>;

template <>
constexpr auto arene::base::testing::test_value_array<arene::base::string_view> =
    arene::base::to_array({"red"_asv, "blue"_asv, "green"_asv, "yellow"_asv});

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects) This test-only customization point must be (const) static.
template <>
auto const arene::base::testing::test_value_array<arene::base::inline_vector<arene::base::string_view, 3>> =
    arene::base::to_array<arene::base::inline_vector<arene::base::string_view, 3>>(
        {{"1"_asv}, {"1"_asv, "2"_asv}, {"1"_asv, "2"_asv, "3"_asv}}
    );

using TestTypes = ::testing::Types<
    arene::base::inline_deque<int, 25>,
    arene::base::inline_deque<double, 15>,
    arene::base::inline_deque<arene::base::string_view, 5>,
    arene::base::inline_deque<non_copyable, 10>,
    arene::base::inline_deque<not_default_constructible, 10>,
    arene::base::inline_deque<noexcept_false, 15>,
    arene::base::inline_deque<int, 2>,
    arene::base::inline_deque<double, 1>,
    arene::base::inline_deque<arene::base::inline_vector<arene::base::string_view, 3>, 3>,
    arene::base::circular_buffer<int, 10>,
    arene::base::circular_buffer<double, 8>,
    arene::base::circular_buffer<arene::base::string_view, 5>,
    arene::base::circular_buffer<non_copyable, 10>,
    arene::base::circular_buffer<not_default_constructible, 10>,
    arene::base::circular_buffer<noexcept_false, 15>,
    arene::base::circular_buffer<int, 2>,
    arene::base::circular_buffer<double, 1>,
    arene::base::circular_buffer<arene::base::inline_vector<arene::base::string_view, 3>, 3>>;

ARENE_INSTANTIATE_TESTS(Internal, InlineDeque, TestTypes);
