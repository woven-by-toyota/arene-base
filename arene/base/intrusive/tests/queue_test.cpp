// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/intrusive/queue.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/intrusive/singly_linked_node.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"

namespace {

using ::arene::base::intrusive::queue;
using ::arene::base::intrusive::singly_linked_node;

constexpr std::size_t array_size{10};

// This is a workaround to silence the warning about using a moved-from object, which we need in order to test behavior
// after move.
template <class T>
void silence_use_after_move(T&) {}

class user_type : public singly_linked_node<> {
 public:
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr user_type(int value = 0)
      : value_{value} {}
  int value_{0};
  constexpr auto value() const noexcept -> int { return value_; }
};

class derived_user_type : public user_type {};

template <typename T>
class IntrusiveQueueTypedTest : public testing::Test {
 public:
  using type = T;
};

using tested_types = ::testing::Types<user_type, derived_user_type>;

TYPED_TEST_SUITE(IntrusiveQueueTypedTest, tested_types, );

/// @test `pop` retrieves the elements in the order pushed
TYPED_TEST(IntrusiveQueueTypedTest, PushPop) {
  queue<user_type> test_queue;
  static_assert(sizeof(queue<user_type>) == 2 * sizeof(void*) + sizeof(std::size_t), "Wrong size of intrusive queue");
  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);

  using type_under_test = std::remove_reference_t<typename TestFixture::type>;

  arene::base::array<type_under_test, array_size> buffer{};  // Prepare 10 objects
  for (auto& elem : buffer) {
    ASSERT_FALSE(elem.is_linked());
    test_queue.push(elem);
    ASSERT_TRUE(elem.is_linked());
  }

  ASSERT_FALSE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 10);

  for (size_t idx = 0; idx < buffer.size(); ++idx) {
    ASSERT_EQ(&test_queue.front(), &buffer[idx]);
    test_queue.pop();
    ASSERT_FALSE(buffer[idx].is_linked());
  }

  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);
}

/// @test `front` returns a reference to the first element, `back` returns a reference to the last element
TYPED_TEST(IntrusiveQueueTypedTest, FrontBack) {
  queue<user_type> test_queue;
  using type_under_test = std::remove_reference_t<typename TestFixture::type>;
  type_under_test node;
  test_queue.push(node);
  ASSERT_EQ(&test_queue.front(), &node);
  ASSERT_EQ(&test_queue.back(), &node);
  test_queue.pop();

  arene::base::array<type_under_test, array_size> buffer{};  // Prepare 10 objects
  for (auto& elem : buffer) {
    test_queue.push(elem);
  }

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(&test_queue.front(), &buffer[0]);
  ASSERT_EQ(&test_queue.back(), &buffer[9]);
}

/// @test Moving a `queue` moves the elements
TYPED_TEST(IntrusiveQueueTypedTest, MoveOperations) {
  queue<user_type> test_queue;

  using type_under_test = std::remove_reference_t<typename TestFixture::type>;
  arene::base::array<type_under_test, array_size> buffer{};  // Prepare 10 objects

  for (auto& elem : buffer) {
    test_queue.push(elem);
  }

  ASSERT_EQ(test_queue.size(), 10);

  // Move constructor
  queue<user_type> queue2{std::move(test_queue)};
  silence_use_after_move(test_queue);
  ASSERT_EQ(queue2.size(), 10);
  ASSERT_EQ(test_queue.size(), 0);
  for (size_t idx = 0; idx < buffer.size(); ++idx) {
    ASSERT_EQ(&queue2.front(), &buffer[idx]);
    queue2.pop();
  }

  // Push elements back
  for (auto& elem : buffer) {
    queue2.push(elem);
  }

  // Move assignment
  test_queue = std::move(queue2);
  silence_use_after_move(queue2);
  ASSERT_EQ(queue2.size(), 0);
  ASSERT_EQ(test_queue.size(), 10);
  for (size_t idx = 0; idx < buffer.size(); ++idx) {
    ASSERT_EQ(&test_queue.front(), &buffer[idx]);
    test_queue.pop();
  }
}

/// @test Self move-assignment does nothing
TYPED_TEST(IntrusiveQueueTypedTest, SelfMoveAssignment) {
  queue<user_type> test_queue;

  using type_under_test = std::remove_reference_t<typename TestFixture::type>;
  arene::base::array<type_under_test, array_size> buffer{};  // Prepare 10 objects

  for (auto& elem : buffer) {
    test_queue.push(elem);
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_WSELFMOVE("Test code: self-assignment is on purpose");
  // Justification: Self move assignment on purpose
  // NOLINTNEXTLINE(clang-diagnostic-self-move)
  test_queue = std::move(test_queue);
  ARENE_IGNORE_END();
  ASSERT_EQ(test_queue.size(), 10);
  for (size_t idx = 0; idx < buffer.size(); ++idx) {
    ASSERT_EQ(&test_queue.front(), &buffer[idx]);
    test_queue.pop();
  }
}

/// @test An instance of a derived class can be added to a `queue`
TEST(IntrusiveQueue, DerivedClass) {
  queue<derived_user_type> test_queue;
  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);

  arene::base::array<derived_user_type, array_size> buffer{};  // Prepare 10 objects
  for (auto& elem : buffer) {
    ASSERT_FALSE(elem.is_linked());
    test_queue.push(elem);
    ASSERT_TRUE(elem.is_linked());
  }

  ASSERT_FALSE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 10);

  for (auto& val : buffer) {
    ASSERT_EQ(&test_queue.front(), &val);
    test_queue.pop();
    ASSERT_FALSE(val.is_linked());
  }

  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);
}

class tag {};

class user_type_custom_hook : public singly_linked_node<tag> {};

/// @test Elements can be added to a queue with a custom tag type
TEST(IntrusiveQueue, CustomHook) {
  queue<user_type_custom_hook, tag> test_queue;
  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);

  arene::base::array<user_type_custom_hook, array_size> buffer{};  // Prepare 10 objects
  for (auto& elem : buffer) {
    test_queue.push(elem);
  }
  ASSERT_FALSE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 10);

  for (auto& val : buffer) {
    ASSERT_EQ(&test_queue.front(), &val);
    test_queue.pop();
  }

  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);
}

class tag1 {};
class tag2 {};

// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
class user_type_multiple_hook
    : public singly_linked_node<tag1>
    , public singly_linked_node<tag2> {};

/// @test Elements with multiple tag types can be added to one queue for each tag type
TEST(IntrusiveQueue, MultipleHook) {
  queue<user_type_multiple_hook, tag1> test_queue;
  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);

  arene::base::array<user_type_multiple_hook, array_size> buffer{};  // Prepare 10 objects
  for (auto& elem : buffer) {
    test_queue.push(elem);
  }
  ASSERT_FALSE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 10);

  for (auto& val : buffer) {
    ASSERT_EQ(&test_queue.front(), &val);
    test_queue.pop();
  }

  ASSERT_TRUE(test_queue.empty());
  ASSERT_EQ(test_queue.size(), 0);
}

/// @test Elements with multiple tags types can be added to a separate queue for each tag type, and the behaviour of
/// each queue  is independent of the status of the element in the other queue
TEST(IntrusiveQueue, MultipleQueueTest) {
  using base_hook1 = singly_linked_node<tag1>;
  using base_hook2 = singly_linked_node<tag2>;
  queue<user_type_multiple_hook, tag1> queue1;
  queue<user_type_multiple_hook, tag2> queue2;

  ASSERT_TRUE(queue1.empty());
  ASSERT_TRUE(queue2.empty());
  ASSERT_EQ(queue1.size(), 0);
  ASSERT_EQ(queue2.size(), 0);

  arene::base::array<user_type_multiple_hook, array_size> buffer{};  // Prepare 10 objects

  // Add elements to queue1
  for (auto& elem : buffer) {
    ASSERT_FALSE(elem.base_hook1::is_linked());
    ASSERT_FALSE(elem.base_hook2::is_linked());
    queue1.push(elem);
    ASSERT_TRUE(elem.base_hook1::is_linked());
    ASSERT_FALSE(elem.base_hook2::is_linked());
  }

  ASSERT_FALSE(queue1.empty());
  ASSERT_TRUE(queue2.empty());
  ASSERT_EQ(queue1.size(), 10);
  ASSERT_EQ(queue2.size(), 0);

  // Add elements to queue2
  for (auto& elem : buffer) {
    queue2.push(elem);
    ASSERT_TRUE(elem.base_hook2::is_linked());
  }

  ASSERT_FALSE(queue1.empty());
  ASSERT_FALSE(queue2.empty());
  ASSERT_EQ(queue1.size(), 10);
  ASSERT_EQ(queue2.size(), 10);

  // Pop elements from queue1
  for (auto& val : buffer) {
    ASSERT_EQ(&queue1.front(), &val);
    queue1.pop();
    ASSERT_FALSE(val.base_hook1::is_linked());
    ASSERT_TRUE(val.base_hook2::is_linked());
  }

  ASSERT_TRUE(queue1.empty());
  ASSERT_FALSE(queue2.empty());
  ASSERT_EQ(queue1.size(), 0);
  ASSERT_EQ(queue2.size(), 10);

  // Pop elements from queue2
  for (auto& val : buffer) {
    ASSERT_EQ(&queue2.front(), &val);
    queue2.pop();
    ASSERT_FALSE(val.base_hook2::is_linked());
  }

  ASSERT_TRUE(queue1.empty());
  ASSERT_TRUE(queue2.empty());
  ASSERT_EQ(queue1.size(), 0);
  ASSERT_EQ(queue2.size(), 0);
}

template <typename T>
class IntrusiveQueueTypedDeathTest : public testing::Test {
 public:
  using type = T;
};

using death_tested_types = ::testing::Types<
    queue<user_type>,
    queue<derived_user_type>,
    queue<user_type_multiple_hook, tag1>,
    queue<user_type_multiple_hook, tag2> >;

TYPED_TEST_SUITE(IntrusiveQueueTypedDeathTest, death_tested_types, );

/// @test Invoking `front`, `back` or `pop` on an empty queue terminates the program with a precondition violation
TYPED_TEST(IntrusiveQueueTypedDeathTest, NonEmptyQueuePreconditions) {
  typename TestFixture::type test_queue;

  ASSERT_DEATH(test_queue.front(), "!empty()");
  ASSERT_DEATH(test_queue.back(), "!empty()");
  ASSERT_DEATH(test_queue.pop(), "!empty()");
}

/// @test Invoking `push` to add a node that is already in another queue terminates the program with a precondition
/// violation
TYPED_TEST(IntrusiveQueueTypedDeathTest, NoRepeatedItems) {
  typename TestFixture::type test_queue;
  // Push a node belonging to another queue
  typename TestFixture::type queue2;
  typename TestFixture::type::value_type node;
  typename TestFixture::type::value_type node2;
  queue2.push(node);
  queue2.push(node2);

  ASSERT_DEATH(test_queue.push(node), "!value.linked_node::is_linked()");
}

/// @test Invoking `push` to add a node that is already in this queue terminates the program with a precondition
/// violation
TYPED_TEST(IntrusiveQueueTypedDeathTest, DeathTests) {
  typename TestFixture::type test_queue;
  // Push a node already linked
  typename TestFixture::type::value_type node;
  test_queue.push(node);
  ASSERT_DEATH(test_queue.push(node), "!value.linked_node::is_linked()");
}

constexpr auto front_and_pop(queue<user_type>& test_queue) noexcept {
  auto value = test_queue.front().value_;
  test_queue.pop();
  return value;
}

constexpr auto case_1() {
  queue<user_type> test_queue;
  // Create a queue of 1,2,3,4,5,6,7,8
  user_type value1{1};
  user_type value2{2};
  user_type value3{3};
  user_type value4{4};
  user_type value5{5};
  user_type value6{6};
  user_type value7{7};
  user_type value8{8};
  test_queue.push(value1);
  test_queue.push(value2);
  test_queue.push(value3);
  test_queue.push(value4);
  test_queue.push(value5);
  test_queue.push(value6);
  test_queue.push(value7);
  test_queue.push(value8);

  auto size = test_queue.size();
  auto front_value = test_queue.front().value_;
  auto back_value = test_queue.back().value_;
  test_queue.pop();
  auto popped_value1 = front_and_pop(test_queue);
  auto popped_value2 = front_and_pop(test_queue);
  auto popped_value3 = front_and_pop(test_queue);
  auto popped_value4 = front_and_pop(test_queue);
  auto popped_value5 = front_and_pop(test_queue);
  auto popped_value6 = front_and_pop(test_queue);
  auto popped_value7 = front_and_pop(test_queue);

  return std::make_tuple(
      size,
      front_value,
      back_value,
      popped_value1,
      popped_value2,
      popped_value3,
      popped_value4,
      popped_value5,
      popped_value6,
      popped_value7,
      test_queue.empty()
  );
}

constexpr auto case_2() {
  queue<user_type> test_queue;
  // Create a queue of 1,2,3,4,5,6,7,8
  user_type value1{1};
  user_type value2{2};
  user_type value3{3};
  user_type value4{4};
  user_type value5{5};
  user_type value6{6};
  user_type value7{7};
  user_type value8{8};
  test_queue.push(value1);
  test_queue.push(value2);
  test_queue.push(value3);

  auto size_1 = test_queue.size();
  auto popped_value1 = front_and_pop(test_queue);
  auto popped_value2 = front_and_pop(test_queue);
  auto popped_value3 = front_and_pop(test_queue);

  // Push more
  test_queue.push(value4);
  test_queue.push(value5);
  test_queue.push(value6);
  test_queue.push(value7);
  test_queue.push(value8);

  auto size_2 = test_queue.size();
  auto popped_value4 = front_and_pop(test_queue);
  auto popped_value5 = front_and_pop(test_queue);
  auto popped_value6 = front_and_pop(test_queue);
  auto popped_value7 = front_and_pop(test_queue);
  auto popped_value8 = front_and_pop(test_queue);

  return std::make_tuple(
      size_1,
      size_2,
      popped_value1,
      popped_value2,
      popped_value3,
      popped_value4,
      popped_value5,
      popped_value6,
      popped_value7,
      popped_value8
  );
}

constexpr auto case_3() {
  queue<user_type> test_queue;
  user_type obj{42};
  test_queue.push(obj);
  auto size_1 = test_queue.size();
  auto value1 = front_and_pop(test_queue);
  auto size_2 = test_queue.size();
  test_queue.push(obj);  // Push again
  auto size_3 = test_queue.size();
  auto value2 = front_and_pop(test_queue);

  return std::make_tuple(size_1, size_2, size_3, value1, value2);
}

/// @test `queue` can be used in a `constexpr` context
TEST(IntrusiveQueue, Constexpr) {
  static_assert(case_1() == std::make_tuple(std::size_t{8}, 1, 8, 2, 3, 4, 5, 6, 7, 8, true), "Constexpr test failed");
  static_assert(
      case_2() == std::make_tuple(std::size_t{3}, std::size_t{5}, 1, 2, 3, 4, 5, 6, 7, 8),
      "Constexpr test failed"
  );
  static_assert(
      case_3() == std::make_tuple(std::size_t{1}, std::size_t{0}, std::size_t{1}, 42, 42),
      "Constexpr test failed"
  );
}

}  // namespace
