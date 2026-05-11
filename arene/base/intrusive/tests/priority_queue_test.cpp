// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/intrusive/priority_queue.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/intrusive/binary_tree_node.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/greater.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/reverse.hpp"
#include "arene/base/stdlib_choice/sort.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"

namespace {

using ::arene::base::intrusive::binary_tree_node;
using ::arene::base::intrusive::priority_queue;

// This is a workaround to silence the warning about using a moved-from object, which we need in order to test behavior
// after move.
template <class T>
void silence_use_after_move(T&) {}

class user_type : public binary_tree_node<> {
 public:
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr user_type(std::int32_t value)
      : value_{value} {}
  std::int32_t value_{0};

  friend constexpr auto operator<(user_type const& lhs, user_type const& rhs) noexcept -> bool {
    return lhs.value_ < rhs.value_;
  }

  ARENE_MAYBE_UNUSED friend constexpr auto operator>(user_type const& lhs, user_type const& rhs) noexcept -> bool {
    return lhs.value_ > rhs.value_;
  }
};
class derived_user_type : public user_type {
  using user_type::user_type;
};

struct default_test_tag {};

using base_hook = binary_tree_node<default_test_tag>;

class user_type_custom_type : public base_hook {
 public:
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  user_type_custom_type(std::int32_t value)
      : value_{value} {}
  std::int32_t value_{0};

  friend auto operator<(user_type_custom_type const& lhs, user_type_custom_type const& rhs) noexcept -> bool {
    return lhs.value_ < rhs.value_;
  }
  ARENE_MAYBE_UNUSED friend auto operator>(user_type_custom_type const& lhs, user_type_custom_type const& rhs) noexcept
      -> bool {
    return lhs.value_ > rhs.value_;
  }
};

struct tag1 {};
struct tag2 {};

using base_hook1 = binary_tree_node<tag1>;
using base_hook2 = binary_tree_node<tag2>;
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
class user_type_multiple_tag
    : public base_hook1
    , public base_hook2 {
 public:
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  user_type_multiple_tag(std::int32_t value)
      : value_{value} {}
  std::int32_t value_{0};

  friend auto operator<(user_type_multiple_tag const& lhs, user_type_multiple_tag const& rhs) noexcept -> bool {
    return lhs.value_ < rhs.value_;
  }
  ARENE_MAYBE_UNUSED friend auto
  operator>(user_type_multiple_tag const& lhs, user_type_multiple_tag const& rhs) noexcept -> bool {
    return lhs.value_ > rhs.value_;
  }
};

/// @test Instances of a class derived from the `priority_queue` element type can be added to the queue
TEST(IntrusivePriorityQueue, DerivedClass) {
  // To test is the queue can handle derived types
  // Not a typed test
  priority_queue<derived_user_type> queue;
  arene::base::array<derived_user_type, 10> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  for (std::size_t idx = 0; idx < values.size(); idx++) {
    ASSERT_FALSE(values[idx].is_linked());
    queue.push(values[idx]);
    ASSERT_TRUE(values[idx].is_linked());
    ASSERT_EQ(queue.size(), idx + 1);
  }

  for (std::int32_t idx = 0; !queue.empty(); idx++) {
    auto const& top = queue.top();
    ASSERT_EQ(queue.top().value_, 10 - idx);
    queue.pop();
    ASSERT_FALSE(top.is_linked());
  }

  ASSERT_TRUE(queue.empty());
}

// Typed test definition
template <typename T>
class IntrusivePriorityQueueTypedTest : public testing::Test {
 public:
  using type = std::remove_reference_t<T>;
  using value_type = typename T::value_type;
};

using tested_types = ::testing::Types<
    priority_queue<user_type>,
    priority_queue<user_type, std::greater<>>,
    priority_queue<derived_user_type>,
    priority_queue<user_type_custom_type, std::less<>, default_test_tag>,
    priority_queue<user_type_multiple_tag, std::less<>, tag1>,
    priority_queue<user_type_multiple_tag, std::less<>, tag2>>;

TYPED_TEST_SUITE(IntrusivePriorityQueueTypedTest, tested_types, );

/// @test A default-constructed `priority_queue` is empty
TYPED_TEST(IntrusivePriorityQueueTypedTest, DefaultConstructedIsEmpty) {
  typename TestFixture::type const queue;
  ASSERT_TRUE(queue.empty());
}

/// @test Elements can be pusehd to a `priority_queue`, and then popped in priority order
TYPED_TEST(IntrusivePriorityQueueTypedTest, PushPop) {
  typename TestFixture::type queue;

  using tag = typename TestFixture::type::tag;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 10> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::sort(values.begin(), values.end(), value_compare{});

  for (std::size_t idx = 0; idx < values.size(); idx++) {
    ASSERT_FALSE(values[idx].binary_tree_node<tag>::is_linked());
    queue.push(values[idx]);
    ASSERT_TRUE(values[idx].binary_tree_node<tag>::is_linked());
    ASSERT_EQ(queue.size(), idx + 1);
  }

  for (std::size_t idx = 0; !queue.empty(); idx++) {
    auto const& top = queue.top();
    ASSERT_EQ(queue.top().value_, values[9U - idx].value_);
    queue.pop();
    ASSERT_FALSE(top.binary_tree_node<tag>::is_linked());
  }

  // Push again but only use pop this time
  for (std::size_t idx = 0; idx < values.size(); idx++) {
    queue.push(values[idx]);
  }

  for (std::size_t idx = 0; !queue.empty(); idx++) {
    ASSERT_EQ(queue.top().value_, values[9U - idx].value_);
    queue.pop();
  }

  ASSERT_TRUE(queue.empty());
}

/// @test Pushes and pops can be interleaved on a `priority_queue`
TYPED_TEST(IntrusivePriorityQueueTypedTest, MultiplePush) {
  typename TestFixture::type queue;

  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 10> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::sort(values.begin(), values.end(), value_compare{});

  // Push 5 elements, don't use for loop
  queue.push(values[0]);
  queue.push(values[1]);
  queue.push(values[2]);
  queue.push(values[3]);
  queue.push(values[4]);

  ASSERT_EQ(queue.size(), 5);

  // Pop 3 elements and check the value
  ASSERT_EQ(queue.top().value_, values[4].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[3].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[2].value_);
  queue.pop();

  ASSERT_EQ(queue.size(), 2);

  // Push another 5 elements
  queue.push(values[5]);
  queue.push(values[6]);
  queue.push(values[7]);
  queue.push(values[8]);
  queue.push(values[9]);

  ASSERT_EQ(queue.size(), 7);

  // Pop all the elements and check the value
  ASSERT_EQ(queue.top().value_, values[9].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[8].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[7].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[6].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[5].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[1].value_);
  queue.pop();
  ASSERT_EQ(queue.top().value_, values[0].value_);
  queue.pop();
  ASSERT_TRUE(queue.empty());
}

/// @test Multiple elements with the same value can be added to a `priority_queue`, and they are popped together
TYPED_TEST(IntrusivePriorityQueueTypedTest, PushPopSameValue) {
  typename TestFixture::type queue;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 12> values = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
  arene::base::array<int, 12> expected = {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1};
  std::sort(expected.begin(), expected.end(), value_compare{});
  std::reverse(expected.begin(), expected.end());

  for (std::size_t idx = 0; idx < values.size(); idx++) {
    queue.push(values[idx]);
  }
  ASSERT_EQ(queue.size(), values.size());

  for (auto& value : expected) {
    ASSERT_EQ(queue.top().value_, value);
    queue.pop();
  }

  ASSERT_TRUE(queue.empty());
}

/// @test Move construction of a `priority_queue` transfers the elements to the new queue
TYPED_TEST(IntrusivePriorityQueueTypedTest, MoveConstruct) {
  typename TestFixture::type queue;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 12> values = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
  arene::base::array<int, 12> expected = {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1};
  std::sort(expected.begin(), expected.end(), value_compare{});
  std::reverse(expected.begin(), expected.end());

  for (std::size_t idx = 0; idx < values.size(); idx++) {
    queue.push(values[idx]);
  }
  ASSERT_EQ(queue.size(), values.size());

  typename TestFixture::type queue2{std::move(queue)};
  silence_use_after_move(queue);
  ASSERT_TRUE(queue.empty());
  ASSERT_EQ(queue2.size(), values.size());

  for (auto& value : expected) {
    ASSERT_EQ(queue2.top().value_, value);
    queue2.pop();
  }
}

/// @test Move assingment of a `priority_queue` transfers the elements
TYPED_TEST(IntrusivePriorityQueueTypedTest, MoveAssignment) {
  typename TestFixture::type queue;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 12> values = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
  arene::base::array<int, 12> expected = {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1};
  std::sort(expected.begin(), expected.end(), value_compare{});
  std::reverse(expected.begin(), expected.end());

  for (std::size_t idx = 0; idx < values.size(); idx++) {
    queue.push(values[idx]);
  }
  ASSERT_EQ(queue.size(), values.size());

  typename TestFixture::type queue2;
  queue2 = std::move(queue);
  silence_use_after_move(queue);
  ASSERT_TRUE(queue.empty());
  ASSERT_EQ(queue2.size(), values.size());

  for (auto& value : expected) {
    ASSERT_EQ(queue2.top().value_, value);
    queue2.pop();
  }
}

/// @test Self-move-assignment of a `priority_queue` does nothing
TYPED_TEST(IntrusivePriorityQueueTypedTest, SelfMoveAssignment) {
  typename TestFixture::type queue;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 12> values = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2};
  arene::base::array<int, 12> expected = {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1};
  std::sort(expected.begin(), expected.end(), value_compare{});
  std::reverse(expected.begin(), expected.end());

  // Test for self move assignment
  for (std::size_t idx = 0; idx < values.size(); idx++) {
    queue.push(values[idx]);
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_WSELFMOVE("Test code: self-assignment is on purpose");
  // Justification: Self move assignment on purpose
  // NOLINTNEXTLINE(clang-diagnostic-self-move)
  queue = std::move(queue);
  ARENE_IGNORE_END();
  ASSERT_EQ(queue.size(), values.size());
  ASSERT_FALSE(queue.empty());

  for (auto& value : expected) {
    ASSERT_EQ(queue.top().value_, value);
    queue.pop();
  }
}

/// @test The middle of 3 nodes can be explicitly removed via `erase`, and the remaining elements are popped in order
TYPED_TEST(IntrusivePriorityQueueTypedTest, SwapSiblingNodes) {
  typename TestFixture::type queue;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 3> values = {1, 2, 3};
  arene::base::array<int, 2> expected = {3, 1};  // 2 will always be removed
  std::sort(expected.begin(), expected.end(), value_compare{});
  std::reverse(expected.begin(), expected.end());

  for (std::size_t idx = 0; idx < values.size(); idx++) {
    queue.push(values[idx]);
  }

  // Remove 2. One of the steps is to swap 2 and 3.
  queue.erase(values[1]);

  ASSERT_EQ(queue.size(), 2);

  for (auto& value : expected) {
    ASSERT_EQ(queue.top().value_, value);
    queue.pop();
  }
}

/// @test Random test with a fixed seed
TEST(IntrusivePriorityQueueRandom, PushPop) {
  constexpr std::size_t size = 128U;
  // 128 randomly-generated values between -42 and 42
  arene::base::array<user_type, size> elements{
      -20, 20,  33,  -26, 31,  -3,  40,  40, -27, -33, -13, 1,   34,  -37, -1,  -37, 31,  -13, -22, -24, 9,  -42,
      2,   -29, 4,   41,  -35, 11,  -18, -5, 6,   13,  37,  26,  38,  -40, -31, -40, -25, -16, 0,   30,  13, -24,
      1,   12,  21,  29,  -25, 40,  -30, 14, -8,  0,   13,  -5,  28,  -42, 35,  26,  -26, -4,  13,  -33, 11, -29,
      24,  -17, -20, 30,  9,   -28, 13,  31, -38, -7,  25,  19,  -21, 21,  -38, -40, 36,  -2,  -8,  -35, 24, -33,
      40,  3,   34,  18,  7,   17,  4,   5,  25,  29,  -41, -12, 7,   15,  41,  24,  16,  41,  3,   37,  16, 1,
      32,  -36, -38, 17,  33,  5,   -11, -6, 22,  -22, -1,  9,   -37, 7,   41,  -31, 36,  4,
  };

  priority_queue<user_type> queue;
  for (user_type& value : elements) {
    queue.push(value);
  }
  ASSERT_EQ(queue.size(), size);

  arene::base::array<user_type, size> sorted_elements(elements);
  std::sort(sorted_elements.begin(), sorted_elements.end(), std::greater<>());

  for (std::size_t idx = 0; idx < size; idx++) {
    // Make sure the top is always the highest-sorted element
    ASSERT_EQ(queue.top().value_, sorted_elements[idx].value_);
    queue.pop();
  }
  ASSERT_TRUE(queue.empty());
}

/// @test Erasing the last element reduces the size by 1
TYPED_TEST(IntrusivePriorityQueueTypedTest, EraseLastElement) {
  typename TestFixture::type queue;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 2> values = {0, 1};
  std::sort(values.begin(), values.end(), value_compare{});
  std::reverse(values.begin(), values.end());

  for (auto& val : values) {
    queue.push(val);
  }

  ASSERT_EQ(queue.size(), values.size());
  queue.erase(values[1]);

  ASSERT_EQ(queue.size(), 1);
}

/// @test This is a special case found during permutation testing, where a node may move up during the erase process;
/// ensure that the values are still popped in the correct order
TYPED_TEST(IntrusivePriorityQueueTypedTest, EraseSpecialCase) {
  typename TestFixture::type queue;
  using value_t = typename TestFixture::type::value_type;
  using value_compare = typename TestFixture::type::value_compare;

  arene::base::array<value_t, 10> values = {0, 1, 6, 7, 8, 2, 3, 4, 5, 9};
  arene::base::array<value_t, 9> expected = {9, 8, 7, 6, 5, 4, 3, 2, 0};  // Missing 1
  std::sort(expected.begin(), expected.end(), value_compare{});
  std::reverse(expected.begin(), expected.end());

  for (auto& val : values) {
    queue.push(val);
  }
  ASSERT_EQ(queue.size(), values.size());

  queue.erase(values[1]);  // Remove 1 from the priority queue

  for (auto const& val : expected) {
    ASSERT_EQ(queue.top().value_, val.value_);
    queue.pop();
  }
}

/// @test Invoking `top` or `pop` on an empty `priority_queue` terminates the program with a precondition violation
TYPED_TEST(IntrusivePriorityQueueTypedTest, NonEmptyPrecondition) {
  typename TestFixture::type priority_queue;

  ASSERT_DEATH(priority_queue.top(), "!empty()");
  ASSERT_DEATH(priority_queue.pop(), "!empty()");
}
/// @test Pushing a node that already belongs to a queue terminates the program with a precondition violation
TYPED_TEST(IntrusivePriorityQueueTypedTest, PushElementTwice) {
  // Push type already in a priority queue
  typename TestFixture::type queue;

  // Pushing a node that already belongs to a queue should fail
  typename TestFixture::type::value_type value01{1};
  typename TestFixture::type::value_type value02{2};
  queue.push(value01);
  queue.push(value02);
  ASSERT_DEATH(queue.push(value01), ".*");
  ASSERT_DEATH(queue.push(value02), ".*");
}

/// @test Erasing an element node from an empty queue terminates the program with a precondition violation
TYPED_TEST(IntrusivePriorityQueueTypedTest, EraseFromEmptyQueue) {
  // Erasing element not currently in the priority queue
  typename TestFixture::type queue;
  typename TestFixture::type::value_type value01{1};
  ASSERT_DEATH(queue.erase(value01), "!empty()");
}

/// @test Erasing an element node that is not in the queue terminates the program with a precondition violation
TYPED_TEST(IntrusivePriorityQueueTypedTest, EraseElementNotPresent) {
  typename TestFixture::type queue;
  typename TestFixture::type::value_type value01{1};
  typename TestFixture::type::value_type value02{2};  // Doesn't belong to any priority queue
  queue.push(value01);
  ASSERT_DEATH(queue.erase(value02), "contains.*");
  queue.push(value02);
  queue.erase(value02);
}

/// @test Special test that tests pushing a type deriving from multiple bases
/// to different priority queues.
TEST(IntrusivePriorityQueue, MultipleTag) {
  priority_queue<user_type_multiple_tag, std::less<>, tag1> queue1;

  priority_queue<user_type_multiple_tag, std::less<>, tag2> queue2;

  arene::base::array<user_type_multiple_tag, 10> values = {
      user_type_multiple_tag(1),
      user_type_multiple_tag(2),
      user_type_multiple_tag(3),
      user_type_multiple_tag(4),
      user_type_multiple_tag(5),
      user_type_multiple_tag(6),
      user_type_multiple_tag(7),
      user_type_multiple_tag(8),
      user_type_multiple_tag(9),
      user_type_multiple_tag(10)
  };

  // Add elements to queue1
  for (std::size_t idx = 0; idx < values.size(); idx++) {
    ASSERT_FALSE(values[idx].base_hook1::is_linked());
    queue1.push(values[idx]);
    ASSERT_TRUE(values[idx].base_hook1::is_linked());
    ASSERT_EQ(queue1.size(), idx + 1);
  }

  // Add elements to queue2
  for (std::size_t idx = 0; idx < values.size(); idx++) {
    ASSERT_FALSE(values[idx].base_hook2::is_linked());
    queue2.push(values[idx]);
    ASSERT_TRUE(values[idx].base_hook2::is_linked());
    ASSERT_EQ(queue2.size(), idx + 1);
  }

  // Pop elements from queue1
  for (std::int32_t idx = 0; !queue1.empty(); idx++) {
    auto const& top = queue1.top();
    ASSERT_EQ(queue1.top().value_, 10 - idx);
    queue1.pop();
    ASSERT_FALSE(top.base_hook1::is_linked());
  }

  // Pop elements from queue2
  for (std::int32_t idx = 0; !queue2.empty(); idx++) {
    auto const& top = queue2.top();
    ASSERT_EQ(queue2.top().value_, 10 - idx);
    queue2.pop();
    ASSERT_FALSE(top.base_hook2::is_linked());
  }

  ASSERT_TRUE(queue1.empty());
  ASSERT_TRUE(queue2.empty());
}

namespace intrusive_priority_queue_constexpr_test {
constexpr auto top_and_pop(priority_queue<user_type>& queue) noexcept {
  auto value = queue.top().value_;
  queue.pop();
  return value;
}

constexpr auto case_1() {
  priority_queue<user_type> queue;
  user_type value1{1};
  user_type value2{2};
  user_type value3{3};
  user_type value4{4};
  user_type value5{4};
  user_type value6{3};
  user_type value7{2};
  user_type value8{1};

  queue.push(value1);
  queue.push(value2);
  queue.push(value3);
  queue.push(value4);
  queue.push(value5);
  queue.push(value6);
  queue.push(value7);
  queue.push(value8);

  auto size = queue.size();
  auto popped_value0 = top_and_pop(queue);
  auto popped_value1 = top_and_pop(queue);
  auto popped_value2 = top_and_pop(queue);
  auto popped_value3 = top_and_pop(queue);
  auto popped_value4 = top_and_pop(queue);
  auto popped_value5 = top_and_pop(queue);
  auto popped_value6 = top_and_pop(queue);
  auto popped_value7 = top_and_pop(queue);
  auto size_after = queue.size();

  return std::make_tuple(
      size,
      popped_value0,
      popped_value1,
      popped_value2,
      popped_value3,
      popped_value4,
      popped_value5,
      popped_value6,
      popped_value7,
      size_after
  );
}

constexpr auto case_2() {
  // Case involving erasing elements
  priority_queue<user_type> queue;
  user_type value1{1};
  user_type value2{2};
  user_type value3{3};
  user_type value4{4};
  user_type value5{4};
  user_type value6{3};
  user_type value7{2};
  user_type value8{1};

  queue.push(value1);
  queue.push(value2);
  queue.push(value3);
  queue.push(value4);
  queue.push(value5);
  queue.push(value6);
  queue.push(value7);
  queue.push(value8);

  queue.erase(value2);
  queue.erase(value4);
  queue.erase(value6);
  queue.erase(value8);

  auto size = queue.size();
  auto popped_value0 = top_and_pop(queue);
  auto popped_value1 = top_and_pop(queue);
  auto popped_value2 = top_and_pop(queue);
  auto popped_value3 = top_and_pop(queue);
  auto size_after = queue.size();

  return std::make_tuple(size, popped_value0, popped_value1, popped_value2, popped_value3, size_after);
}

constexpr auto case_3() {
  // Case involving popping/erasing elements and pushing them back
  priority_queue<user_type> queue;

  user_type value1{1};
  user_type value2{2};
  user_type value3{3};
  user_type value4{4};
  user_type value5{4};
  user_type value6{3};
  user_type value7{2};
  user_type value8{1};

  // Push 1, 2, 3, 4
  queue.push(value1);
  queue.push(value2);
  queue.push(value3);
  queue.push(value4);

  // Pop
  auto size_1 = queue.size();
  auto popped_value1 = top_and_pop(queue);  // 4
  auto popped_value2 = top_and_pop(queue);  // 3
  auto popped_value3 = top_and_pop(queue);  // 2
  auto popped_value4 = top_and_pop(queue);  // 1

  // Push more
  queue.push(value5);
  queue.push(value6);
  queue.push(value7);
  queue.push(value8);

  auto size_2 = queue.size();
  auto popped_value5 = top_and_pop(queue);  // 4
  auto popped_value6 = top_and_pop(queue);  // 3
  auto popped_value7 = top_and_pop(queue);  // 2
  auto popped_value8 = top_and_pop(queue);  // 1
  auto size_after = queue.size();

  return std::make_tuple(
      size_1,
      popped_value1,
      popped_value2,
      popped_value3,
      popped_value4,
      size_2,
      popped_value5,
      popped_value6,
      popped_value7,
      popped_value8,
      size_after
  );
}

constexpr auto case_4() {
  // Case involving popping and pushing the same element
  priority_queue<user_type> queue;
  user_type obj1{1};
  user_type obj2{2};
  user_type obj3{3};
  queue.push(obj1);
  queue.push(obj2);
  queue.push(obj3);

  queue.erase(obj1);
  queue.erase(obj3);

  queue.push(obj1);
  queue.push(obj3);

  auto size = queue.size();
  auto value1 = top_and_pop(queue);  // 3
  auto value2 = top_and_pop(queue);  // 2
  auto value3 = top_and_pop(queue);  // 1

  return std::make_tuple(size, value1, value2, value3);
}
}  // namespace intrusive_priority_queue_constexpr_test

/// @test `priority_queue` can be used in a `constexpr` context
TEST(IntrusivePriorityQueue, ConstexprTest) {
  // Putting static asserts in GTEST block just to keep track
  // of the tests.
  static_assert(
      intrusive_priority_queue_constexpr_test::case_1() == std::make_tuple(8U, 4, 4, 3, 3, 2, 2, 1, 1, 0U),
      "Constexpr test failed"
  );
  static_assert(
      intrusive_priority_queue_constexpr_test::case_2() == std::make_tuple(4U, 4, 3, 2, 1, 0U),
      "Constexpr test failed"
  );
  static_assert(
      intrusive_priority_queue_constexpr_test::case_3() == std::make_tuple(4U, 4, 3, 2, 1, 4U, 4, 3, 2, 1, 0U),
      "Constexpr test failed"
  );
  static_assert(
      intrusive_priority_queue_constexpr_test::case_4() == std::make_tuple(3U, 3, 2, 1),
      "Constexpr test failed"
  );
}
}  // namespace
