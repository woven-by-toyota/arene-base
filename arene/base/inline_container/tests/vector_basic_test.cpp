// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gmock/gmock.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/inline_container/tests/testing_types.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "stdlib/include/iterator"
#include "testlibs/utilities/counting_wrapper.hpp"
#include "testlibs/utilities/has_broken_less_than.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

template <typename Map, typename... Args>
using try_construct_result = decltype(Map::try_construct(std::declval<Args>()...));

using ::arene::base::inline_vector;
using counting_wrapper = ::testing::counting_wrapper<int>;

using test_string = arene::base::inline_string<50>;

constexpr std::size_t standard_capacity = 25;

template <typename T>
using standard_capacity_vector = inline_vector<T, standard_capacity>;

/////////////////////////////
// Pathological test types //
/////////////////////////////

struct not_move_constructible {
  not_move_constructible(not_move_constructible&&) = delete;
  not_move_constructible(not_move_constructible const&) = delete;
  auto operator=(not_move_constructible&&) -> not_move_constructible& = default;
  auto operator=(not_move_constructible const&) -> not_move_constructible& = default;
  ~not_move_constructible() = default;
};

struct not_move_assignable {
  not_move_assignable(not_move_assignable&&) = default;
  not_move_assignable(not_move_assignable const&) = default;
  auto operator=(not_move_assignable&&) -> not_move_assignable& = delete;
  auto operator=(not_move_assignable const&) -> not_move_assignable& = delete;
  ~not_move_assignable() = default;
};

struct nothrow_swappable {};
ARENE_MAYBE_UNUSED void swap(nothrow_swappable&, nothrow_swappable&) noexcept {}

struct throw_swappable {};
ARENE_MAYBE_UNUSED void swap(throw_swappable&, throw_swappable&) {}

struct non_default_constructible {
  explicit non_default_constructible(int) {}
};

template <typename T>
struct non_throwing_input_iterator {
  using iterator_category = std::input_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = T&;
  using value_type = T;
  using pointer = void;

  auto operator*() noexcept -> reference;

  auto operator++() noexcept -> non_throwing_input_iterator&;
  auto operator++(int) noexcept -> non_throwing_input_iterator;
  auto operator==(non_throwing_input_iterator const&) noexcept -> bool;
  auto operator!=(non_throwing_input_iterator const&) noexcept -> bool;
};

// TODO Use as many of these pathological types as TypeParams in the normal parameterized tests as possible
/////////////////////////////
// Pathological type tests //
/////////////////////////////

/// @test Given a default-constructed `inline_vector` holding instances of a type that is not assignable, a new element
/// can be inserted via `push_back`, such that after insertion the size has increased
TEST(InlineVector, CanPushBackNotAssignableValues) {
  struct not_assignable {
    not_assignable(not_assignable const&) = default;
    auto operator=(not_assignable const&) -> not_assignable& = delete;

    not_assignable(not_assignable&&) = default;
    auto operator=(not_assignable&&) -> not_assignable& = delete;
    ~not_assignable() = default;
  };

  standard_capacity_vector<not_assignable> vec;
  ASSERT_TRUE(vec.empty());
  vec.push_back(not_assignable{});
  ASSERT_EQ(vec.size(), 1);
}

/// @test An `inline_vector` holding elements of a non-default-constructible type can be constructed from an initializer
/// list in a `constexpr` context, and the elements are those from the initializer list
TEST(InlineVector, CanConstructFromInitializerListWithNoDefaultConstructible) {
  struct not_default_constructible {
    explicit not_default_constructible(int that_i)
        : value(that_i) {}

    int value;
  };

  constexpr int value1 = 10;
  constexpr int value2 = 20;
  constexpr int value3 = 42;
  constexpr int value4 = 99;
  standard_capacity_vector<not_default_constructible> vec{
      not_default_constructible(value1),
      not_default_constructible(value2),
      not_default_constructible(value3),
      not_default_constructible(value4)
  };

  EXPECT_EQ(vec.size(), 4);
  EXPECT_EQ(vec[0].value, value1);
  EXPECT_EQ(vec[1].value, value2);
  EXPECT_EQ(vec[2].value, value3);
  EXPECT_EQ(vec[3].value, value4);
}

/// @test `inline_vector` is not move constructible if the element type is not move constructible, and not move
/// assignable if the element type is not move assignable
TEST(InlineVector, NotMoveableIfTypeNotMoveable) {
  static_assert(
      !std::is_move_constructible<inline_vector<not_move_constructible, 10>>::value,
      "Not move constructible"
  );
  static_assert(!std::is_move_assignable<inline_vector<not_move_constructible, 10>>::value, "Not move assignable");

  static_assert(std::is_move_constructible<inline_vector<not_move_assignable, 10>>::value, "Is move constructible");
  static_assert(!std::is_move_assignable<inline_vector<not_move_assignable, 10>>::value, "Not move assignable");
}

/// @test An `inline_vector` can be constructed from an iterator range specified using input iterators, creating a
/// vector holding copies of the elements from the source range, with a size equal to the number of elements in the
/// source range
TEST(InlineVector, CanConstructFromInputIteratorRange) {
  using arr = arene::base::array<test_string, 4>;
  arr const source{"hello", "foo", "bar", "baz"};
  using input_iter = testing::demoted_iterator<arr::const_iterator, std::input_iterator_tag>;

  standard_capacity_vector<test_string> const vec{input_iter(source.begin()), input_iter(source.end())};

  EXPECT_THAT(vec, ::testing::ElementsAreArray(source));
}

/// @test Two instances of `inline_vector` with an element type that only supports equality comparison, can be compared
/// for equality and inequality
TEST(InlineVectorCompareEqualityOnlyValue, CanCompareForEqualityAndInequality) {
  class only_equality {
   public:
    std::int32_t value;

    constexpr auto operator==(only_equality const& rhs) const -> bool { return value == rhs.value; }
  };
  using vector_t = standard_capacity_vector<only_equality>;

  STATIC_ASSERT_NE((vector_t{{1}, {2}, {3}}), (vector_t{{4}, {5}, {6}}));
  STATIC_ASSERT_NE((vector_t{{1}, {2}, {3}}), (vector_t{{1}, {2}, {3}, {4}}));
  STATIC_ASSERT_FALSE(vector_t{{1}, {2}, {3}} != vector_t{{1}, {2}, {3}});
  STATIC_ASSERT_FALSE(vector_t{{1}, {2}, {3}} == vector_t{{4}, {5}, {6}});
  STATIC_ASSERT_FALSE(vector_t{{1}, {2}, {3}} == vector_t{{1}, {2}, {3}, {4}});
  STATIC_ASSERT_EQ((vector_t{{1}, {2}, {3}}), (vector_t{{1}, {2}, {3}}));
}

/// @test `noexcept` specification of swap for two `inline_vector`s matches that of the element type
TEST(InlineVector, SwapNoexceptMatchesTypes) {
  using vec1 = standard_capacity_vector<nothrow_swappable>;
  using vec2 = standard_capacity_vector<throw_swappable>;

  static_assert(arene::base::is_nothrow_swappable_v<vec1>, "Nothrow swappable");
  static_assert(!arene::base::is_nothrow_swappable_v<vec2>, "Throwing swap");
  static_assert(!arene::base::is_nothrow_swappable_v<throw_swappable>, "Throwing swap");
  static_assert(noexcept(std::declval<vec1&>().swap(std::declval<vec1&>())), "Nothrow swappable");
  static_assert(!noexcept(std::declval<vec2&>().swap(std::declval<vec2&>())), "Throwing swap");
}

/// @test `try_construct` with a number of elements is not considered for overload resolution if the element type of the
/// `inline_vector` is not default-constructible.
TEST(InlineVector, CannotTryConstructWithSizeForANonDefaultConstructibleType) {
  using ivec = inline_vector<non_default_constructible, standard_capacity>;

  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<try_construct_result, ivec, std::size_t>);
}

/// @test The `inline_vector` constructor with a number of elements is not considered for overload resolution if the
/// element type of the `inline_vector` is not default-constructible.
TEST(InlineVector, CannotConstructWithSizeForANonDefaultConstructibleType) {
  using ivec = inline_vector<non_default_constructible, standard_capacity>;

  STATIC_ASSERT_FALSE(std::is_constructible<ivec, std::size_t>::value);
}

/// @test Invoking `try_construct` with an iterator range denoted by input iterators returns an `inline_vector` with the
/// same elements as the source range
TEST(InlineVector, CanTryConstructFromInputIteratorRange) {
  using arr = arene::base::array<test_string, 4>;
  arr const source{"hello", "foo", "bar", "baz"};
  using input_iter = testing::demoted_iterator<arr::const_iterator, std::input_iterator_tag>;

  auto vec = standard_capacity_vector<test_string>::try_construct(input_iter(source.begin()), input_iter(source.end()));
  ASSERT_TRUE(vec.has_value());

  EXPECT_THAT(*vec, ::testing::ElementsAreArray(source));
}

/// @test Invoking `try_construct` with an iterator range denoted by input iterators that has more elements than the
/// capacity of the destination returns an empty `optional`
TEST(InlineVector, TryConstructingFromOverLongInputRangeReturnsEmpty) {
  using arr = arene::base::array<test_string, 26>;
  arr const source{"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                   "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

  ASSERT_GT(source.size(), standard_capacity);

  using input_iter = testing::demoted_iterator<arr::const_iterator, std::input_iterator_tag>;

  auto vec = standard_capacity_vector<test_string>::try_construct(input_iter(source.begin()), input_iter(source.end()));
  ASSERT_FALSE(vec.has_value());
}

/// @test Invoking `try_construct` with a brace-enclosed initializer list returns an `inline_vector` with the
/// same elements as the source list
TEST(InlineVector, CanTryConstructFromBraceInitializerList) {
  auto vec = standard_capacity_vector<test_string>::try_construct({"hello", "foo", "bar", "baz"});
  ASSERT_TRUE(vec.has_value());

  arene::base::array<test_string, 4> const expected{"hello", "foo", "bar", "baz"};
  EXPECT_THAT(*vec, ::testing::ElementsAreArray(expected));
}

/// @test Invoking `try_construct` with a brace-enclosed initializer list that has more elements than the capacity of
/// the destination returns an empty `optional`
TEST(InlineVector, TryConstructingFromOverLongBraceInitializerListReturnsEmpty) {
  auto vec = standard_capacity_vector<test_string>::try_construct({"a", "b", "c", "d", "e", "f", "g", "h", "i",
                                                                   "j", "k", "l", "m", "n", "o", "p", "q", "r",
                                                                   "s", "t", "u", "v", "w", "x", "y", "z"});
  ASSERT_FALSE(vec.has_value());
}

/// @test Two instances of `inline_vector` can be compared for equality even when `value_type` has a broken `operator<`.
TEST(InlineVector, CanCompareForEquality) {
  standard_capacity_vector<testing::has_broken_less_than> const vec{0, 1, 2};
  standard_capacity_vector<testing::has_broken_less_than> const different_contents{3, 4, 5};
  standard_capacity_vector<testing::has_broken_less_than> const longer{0, 1, 2, 3};

  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization) We're doing this copy to check that it's the same
  standard_capacity_vector<testing::has_broken_less_than> const copy{vec};

  EXPECT_TRUE(vec != different_contents);
  EXPECT_TRUE(vec != longer);
  EXPECT_FALSE(vec != copy);
  EXPECT_FALSE(vec == different_contents);
  EXPECT_FALSE(vec == longer);
  EXPECT_TRUE(vec == copy);
}

////////////////////////////
// counting_wrapper tests //
////////////////////////////

/// @test When invoked with an rvalue, `push_back` moves from the supplied value when constructing the new element in
/// the vector
TEST(InlineVectorOpTracker, PushbackMovesFromRvalue) {
  counting_wrapper::count = {};
  standard_capacity_vector<counting_wrapper> vec;
  vec.push_back(counting_wrapper{});
  EXPECT_EQ(counting_wrapper::count.default_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 1);
  vec.push_back(counting_wrapper{});
  EXPECT_EQ(counting_wrapper::count.default_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 2);
}

/// @test When erasing an element in the middle of an `inline_vector`, the following elements are moved into place via
/// move-assignment, and the final element destroyed
TEST(InlineVectorOpTracker, EraseMovesAndDestroys) {
  standard_capacity_vector<counting_wrapper> vec;
  constexpr std::size_t count = 10;
  for (std::size_t idx = 0; idx < count; ++idx) {
    vec.push_back(counting_wrapper{});
  }
  counting_wrapper::count = {};

  vec.erase(vec.begin() + 3);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 6);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 1);
}

/// @test Constructing an `inline_vector` with a count creates the specified number of default-constructed elements, and
/// sets the `size` of the vector to that count.
TEST(InlineVectorOpTracker, CanConstructWithSize) {
  counting_wrapper::count = {};
  constexpr std::size_t count = 3;
  standard_capacity_vector<counting_wrapper> const vec(count);
  STATIC_ASSERT_TRUE(noexcept(standard_capacity_vector<counting_wrapper>(count)));

  EXPECT_EQ(vec.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, count);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Constructing an `inline_vector` with a count and a value creates the specified number of elements
/// copy-constructed from the supplied value, and sets the `size` of the vector to that count.
TEST(InlineVectorOpTracker, CanConstructWithSizeAndValue) {
  counting_wrapper const source{};
  counting_wrapper::count = {};
  constexpr std::size_t count = 14;
  standard_capacity_vector<counting_wrapper> const vec(count, source);

  EXPECT_EQ(vec.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, count);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_vector` copy constructor copies the stored elements and size from the original vector.
TEST(InlineVectorOpTracker, CopyingVectorCopiesElements) {
  constexpr std::size_t count = 3;
  standard_capacity_vector<counting_wrapper> const vec(count);
  counting_wrapper::count = {};

  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  auto const copy(vec);

  EXPECT_EQ(vec.size(), count);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, count);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_vector` move constructor moves the stored elements from the original vector
TEST(InlineVectorOpTracker, MovingVectorMovesElements) {
  constexpr std::size_t count = 14;
  standard_capacity_vector<counting_wrapper> vec(count);
  counting_wrapper::count = {};

  standard_capacity_vector<counting_wrapper> const moved(std::move(vec));

  // NOLINTNEXTLINE(bugprone-use-after-move)
  EXPECT_EQ(vec.size(), 0);
  EXPECT_EQ(moved.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count);
}

/// @test The `inline_vector` copy-assignment operator does nothing for self-assignment
TEST(InlineVectorOpTracker, CopyAssignFromSelfDoesNothing) {
  constexpr std::size_t count = 14;
  standard_capacity_vector<counting_wrapper> vec(count);
  counting_wrapper::count = {};
  auto& self = vec;
  self = vec;
  EXPECT_EQ(vec.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_vector` copy-assignment operator uses copy assignment for elements from the source where there is
/// already a corresponding element in the destination, and copy construction for elements that do not have a
/// corresponding element in the destination
TEST(InlineVectorOpTracker, CopyAssignOverSmallerVectorCopiesElements) {
  constexpr std::size_t count = 14;
  constexpr std::size_t smaller_count = 10;
  standard_capacity_vector<counting_wrapper> const vec(count);
  standard_capacity_vector<counting_wrapper> copy(smaller_count);
  counting_wrapper::count = {};

  copy = vec;

  EXPECT_EQ(vec.size(), count);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, count - smaller_count);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, smaller_count);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_vector` copy-assignment operator destroys the excess elements in the destination when the source
/// vector has fewer elements than the destination vector
TEST(InlineVectorOpTracker, CopyAssignOverLargerVectorDestroysExcess) {
  constexpr std::size_t count = 14;
  constexpr std::size_t larger_count = 18;
  standard_capacity_vector<counting_wrapper> const vec(count);
  standard_capacity_vector<counting_wrapper> copy(larger_count);
  counting_wrapper::count = {};

  copy = vec;

  EXPECT_EQ(vec.size(), count);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, count);
  EXPECT_EQ(counting_wrapper::count.dtor, larger_count - count);
}

/// @test The `inline_vector` move-assignment operator does nothing for self-assignment
TEST(InlineVectorOpTracker, MoveAssignFromSelfDoesNothing) {
  constexpr std::size_t count = 14;
  standard_capacity_vector<counting_wrapper> vec(count);
  counting_wrapper::count = {};

  auto& self = vec;
  self = std::move(vec);

  // NOLINTNEXTLINE(bugprone-use-after-move)
  EXPECT_EQ(vec.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test The `inline_vector` move-assignment operator uses move assignment for elements from the source where there is
/// already a corresponding element in the destination, and move construction for elements that do not have a
/// corresponding element in the destination
TEST(InlineVectorOpTracker, MoveAssignOverSmallerVectorMovesElements) {
  constexpr std::size_t count = 14;
  constexpr std::size_t smaller_count = 10;
  standard_capacity_vector<counting_wrapper> vec(count);
  standard_capacity_vector<counting_wrapper> copy(smaller_count);
  counting_wrapper::count = {};

  copy = std::move(vec);

  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
  EXPECT_EQ(vec.size(), 0);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count - smaller_count);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, smaller_count);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count);
}

/// @test The `inline_vector` move-assignment operator destroys the excess elements in the destination when the source
/// vector has fewer elements than the destination vector
TEST(InlineVectorOpTracker, MoveAssignOverLargerVectorDestroysExcess) {
  constexpr std::size_t count = 14;
  constexpr std::size_t larger_count = 18;
  standard_capacity_vector<counting_wrapper> vec(count);
  standard_capacity_vector<counting_wrapper> copy(larger_count);
  counting_wrapper::count = {};

  copy = std::move(vec);

  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
  EXPECT_EQ(vec.size(), 0);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, count);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, larger_count);
}

/// @test Invoking `pop_back` on a non-empty `inline_vector` destroys the last element and reduces the size by one.
TEST(InlineVectorOpTracker, CanPopBack) {
  constexpr std::size_t count = 14;
  standard_capacity_vector<counting_wrapper> vec(count);
  counting_wrapper::count = {};

  vec.pop_back();

  EXPECT_EQ(vec.size(), count - 1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 1);
}

/// @test Invoking the `resize` member function on an empty vector sets the size of the vector to the specified count,
/// and constructs the specified number of elements.
TEST(InlineVectorOpTracker, ResizingEmptyVectorConstructsElements) {
  counting_wrapper::count = {};
  standard_capacity_vector<counting_wrapper> vec;
  constexpr std::size_t count = 5;
  vec.resize(count);
  EXPECT_EQ(vec.size(), count);

  EXPECT_EQ(counting_wrapper::count.default_ctor, count);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Invoking the `resize` member function on a vector with fewer elements than the specified size sets the size of
/// the vector to the specified count, and default-constructs a number of new elements equal to the difference between
/// the specified size and the previous size
TEST(InlineVectorOpTracker, ResizingFromSmallToLargeConstructsElements) {
  constexpr std::size_t initial_count = 5;
  standard_capacity_vector<counting_wrapper> vec(initial_count);
  constexpr std::size_t count = 17;
  counting_wrapper::count = {};
  vec.resize(count);
  EXPECT_EQ(vec.size(), count);

  EXPECT_EQ(counting_wrapper::count.default_ctor, count - initial_count);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Invoking the `resize` member function on a vector with more elements than the specified size sets the size of
/// the vector to the specified count, and destroys the trailing elements, where the number of destroyed elements is the
/// difference between the old size and the new size
TEST(InlineVectorOpTracker, ResizingFromLargeToSmallDestructsElements) {
  constexpr std::size_t initial_count = 17;
  standard_capacity_vector<counting_wrapper> vec(initial_count);
  constexpr std::size_t count = 5;
  counting_wrapper::count = {};
  vec.resize(count);
  EXPECT_EQ(vec.size(), count);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, initial_count - count);
}

/// @test Invoking `resize` with a count of zero on an `inline_vector` with a capacity of zero does nothing
TEST(InlineVectorOpTracker, ResizeZeroCapacityVecToZeroIsOk) {
  inline_vector<counting_wrapper, 0> vec{};
  counting_wrapper::count = {};
  vec.resize(0);
  EXPECT_EQ(vec.size(), 0);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Given an `inline_vector` holding existing elements, assigning to that vector with an initializer list holding
/// more elements copy assigns over the existing elements, and then copy-constructs new elements to increase the size of
/// the vector to that of the initializer list
TEST(InlineVectorOpTracker, CopyAssignOverSmallerVectorFromInitListCopiesElements) {
  constexpr std::size_t count = 14;
  constexpr std::size_t smaller_count = 10;
  std::initializer_list<counting_wrapper> const values{
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{}
  };
  standard_capacity_vector<counting_wrapper> copy(smaller_count);
  counting_wrapper::count = {};

  copy = values;

  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, count - smaller_count);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, smaller_count);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Given an `inline_vector` holding existing elements, assigning to that vector with an initializer list holding
/// fewer elements copy assigns over the existing elements up to the size of the initializer list, and then destroys a
/// number of elements equal to the difference between the old size of the vector and the size of the initializer list
TEST(InlineVectorOpTracker, CopyAssignOverLargerVectorFromInitListDestroysExcess) {
  constexpr std::size_t count = 14;
  constexpr std::size_t larger_count = 18;
  std::initializer_list<counting_wrapper> const values{
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{},
      counting_wrapper{}
  };
  standard_capacity_vector<counting_wrapper> copy(larger_count);
  counting_wrapper::count = {};

  copy = values;

  EXPECT_EQ(values.size(), count);
  EXPECT_EQ(copy.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, count);
  EXPECT_EQ(counting_wrapper::count.dtor, larger_count - count);
}

/// @test Invoking `swap` without a namespace qualification on two `inline_vector` instances uses `swap` to exchange the
/// values up to the size of the smaller vector, then move-constructs the elements from the larger vector over to the
/// end of the smaller vector, and destroys the corresponding elements from the larger vector, so the elements from one
/// have been moved to the other, and vice-versa.
TEST(InlineVectorOpTracker, ADLSwapSwapsAndMovesElements) {
  using vec = standard_capacity_vector<counting_wrapper>;

  constexpr std::size_t count1 = 12;
  constexpr std::size_t count2 = 5;
  vec value1(count1);
  vec value2(count2);

  counting_wrapper::count = {};
  swap(value1, value2);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.swap, count2);

  EXPECT_EQ(value1.size(), count2);
  EXPECT_EQ(value2.size(), count1);

  counting_wrapper::count = {};
  swap(value1, value2);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.swap, count2);

  EXPECT_EQ(value1.size(), count1);
  EXPECT_EQ(value2.size(), count2);
}

/// @test Invoking the `swap` member function on an `inline_vector` vector instance, passing a second `inline_vector`
/// instance uses `swap` to exchange the values up to the size of the smaller vector, then move-constructs the elements
/// from the larger vector over to the end of the smaller vector, and destroys the corresponding elements from the
/// larger vector, so the elements from one have been moved to the other, and vice-versa.
TEST(InlineVectorOpTracker, SwapSwapsAndMovesElements) {
  using vec = standard_capacity_vector<counting_wrapper>;

  constexpr std::size_t count1 = 12;
  constexpr std::size_t count2 = 5;
  vec value1(count1);
  vec value2(count2);

  counting_wrapper::count = {};
  value1.swap(value2);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.swap, count2);

  EXPECT_EQ(value1.size(), count2);
  EXPECT_EQ(value2.size(), count1);

  counting_wrapper::count = {};
  value1.swap(value2);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count1 - count2);
  EXPECT_EQ(counting_wrapper::count.swap, count2);

  EXPECT_EQ(value1.size(), count1);
  EXPECT_EQ(value2.size(), count2);
}

/// @test Invoking `insert` with an iterator referring to an existing element of an `inline_vector` and a source
/// iterator range denoted by a pair of iterators move constructs elements from the existing sequence into the empty
/// space at the end of the vector, copy constructs elements from the inserted sequence into the empty space where there
/// are no existing elements, and copy assigns elements from the inserted sequence over the existing elements that do
/// exist.
TEST(InlineVectorOpTracker, InsertFromIteratorRangeInMiddleMinimizesMoves) {
  using vec = standard_capacity_vector<counting_wrapper>;

  constexpr std::size_t count1 = 5;
  constexpr std::ptrdiff_t offset = 3;
  constexpr std::size_t count2 = 7;
  vec value1(count1);
  vec const value2(count2);
  counting_wrapper::count = {};
  value1.insert(value1.begin() + offset, value2.begin(), value2.end());
  EXPECT_EQ(value1.size(), count1 + count2);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count1 - static_cast<std::size_t>(offset));
  EXPECT_EQ(counting_wrapper::count.copy_ctor, count2 - (count1 - static_cast<std::size_t>(offset)));
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, (count1 - static_cast<std::size_t>(offset)));
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
  EXPECT_EQ(counting_wrapper::count.swap, 0);
}

/// @test Invoking `insert` with an iterator referring to an existing element of an `inline_vector` and a source
/// iterator range denoted by a pair of iterators, where the inserted range is shorter than the number of elements in
/// the original vector after the point of insertion, move constructs a number of elements equal to the length of the
/// inserted range from the existing sequence into the empty space at the end of the vector, move-assigns the remaining
/// elements from the existing vector after the insertion point over the existing elements following that point, prior
/// to the original end of the vector, and copy assigns elements from the inserted sequence over the original elements
/// at the insert location.
TEST(InlineVectorOpTracker, InsertFromIteratorRangeInMiddleMinimizesMovesWithShortInsert) {
  using vec = standard_capacity_vector<counting_wrapper>;

  constexpr std::size_t count1 = 15;
  constexpr std::ptrdiff_t offset = 3;
  constexpr std::size_t count2 = 7;
  vec value1(count1);
  vec const value2(count2);
  counting_wrapper::count = {};
  value1.insert(value1.begin() + offset, value2.begin(), value2.end());
  EXPECT_EQ(value1.size(), count1 + count2);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count2);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, (count1 - static_cast<std::size_t>(offset)) - count2);
  EXPECT_EQ(counting_wrapper::count.copy_assign, count2);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
  EXPECT_EQ(counting_wrapper::count.swap, 0);
}

/// @test An `inline_vector` can be move-constructed from an `inline_vector` with a larger capacity, provided the number
/// of elements in the source vector is less than the capacity of the target vector, and the elements are
/// move-constructed.
TEST(InlineVectorOpTracker, CanMoveConstructFromBiggerCapacityVector) {
  constexpr std::size_t count = 11;
  inline_vector<counting_wrapper, standard_capacity> value1(count);
  constexpr std::size_t capacity2 = 12;
  counting_wrapper::count = {};
  inline_vector<counting_wrapper, capacity2> const value2(std::move(value1));

  EXPECT_EQ(value2.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
  EXPECT_EQ(counting_wrapper::count.swap, 0);
}

/// @test An `inline_vector` can be move-assigned from an `inline_vector` with a larger capacity, provided the number
/// of elements in the source vector is less than the capacity of the target vector, and the elements are
/// move-constructed where the index is equal to or greater than the original size of the destination vector, and
/// move-assigned where the index is less than the original size of the destination vector.
TEST(InlineVectorOpTracker, CanMoveAssignFromBiggerCapacityVector) {
  constexpr std::size_t count = 11;
  inline_vector<counting_wrapper, standard_capacity> value1(count);
  constexpr std::size_t capacity2 = 12;
  constexpr std::size_t old_count = 3;
  inline_vector<counting_wrapper, capacity2> value2(old_count);
  counting_wrapper::count = {};
  value2 = std::move(value1);

  EXPECT_EQ(value2.size(), count);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, count - old_count);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, old_count);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
  EXPECT_EQ(counting_wrapper::count.swap, 0);
}

/// @test An `inline_vector` on the left side of an assignment statement assigns the first elements and destroys any
/// excess elements.
TEST(InlineVectorOpTracker, MoveAssignDestroysOldElements) {
  constexpr std::size_t count = 11;
  inline_vector<counting_wrapper, standard_capacity> value(count);
  counting_wrapper::count = {};

  value = inline_vector<counting_wrapper, standard_capacity>(1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 1);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 1);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, count);
  EXPECT_EQ(counting_wrapper::count.swap, 0);
}

/// @test Invoking `try_construct` with a number of elements and a source value constructs an `inline_vector` holding
/// the specified number of elements that are copy-constructed from the supplied value
TEST(InlineVectorOpTracker, CanConstructWithSizeAndSourceViaTryConstruct) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;
  constexpr std::size_t size1 = 10;
  counting_wrapper const source;
  STATIC_ASSERT_TRUE(std::is_same<decltype(ivec::try_construct(size1, source)), arene::base::optional<ivec>>::value);
  STATIC_ASSERT_TRUE(noexcept(ivec::try_construct(size1, source)));

  counting_wrapper::count = {};
  auto res = ivec::try_construct(size1, source);
  ASSERT_TRUE(res.has_value());
  ASSERT_FALSE(res->empty());

  ASSERT_EQ(res->size(), size1);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, size1);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Invoking `try_construct` with a number of elements and a source value, where the specified number of elements
/// exceeds the capacity of the vector, returns an empty `optional`.
TEST(InlineVectorOpTracker, ConstructWithExcessiveSizeAndSourceViaTryConstructFails) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;
  constexpr std::size_t size1 = standard_capacity + 1;
  counting_wrapper const source;
  STATIC_ASSERT_TRUE(std::is_same<decltype(ivec::try_construct(size1, source)), arene::base::optional<ivec>>::value);
  STATIC_ASSERT_TRUE(noexcept(ivec::try_construct(size1, source)));

  counting_wrapper::count = {};
  ASSERT_FALSE(ivec::try_construct(size1, source).has_value());

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Invoking `try_construct` with an rvalue `inline_vector` constructs a new `inline_vector`, and move-constructs
/// the elements from the source to the new vector.
TEST(InlineVectorOpTracker, CanMoveViaTryConstruct) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;

  ivec source(standard_capacity - 1);

  counting_wrapper::count = {};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  auto res = ivec::try_construct(std::move(source));
  ASSERT_EQ(res->size(), standard_capacity - 1);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_EQ(source.size(), 0);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, res->size());
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, res->size());
}

/// @test Constructing an `inline_vector` from an rvalue `inline_vector` move-constructs the elements from the source to
/// the new vector.
TEST(InlineVectorOpTracker, CanMoveViaConstruct) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;

  ivec source(standard_capacity - 1);

  counting_wrapper::count = {};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  auto res = ivec(std::move(source));
  ASSERT_EQ(res.size(), standard_capacity - 1);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_EQ(source.size(), 0);

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, res.size());
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, res.size());
}

/// @test Invoking `try_construct` with an rvalue `inline_vector` of a smaller capacity constructs a new
/// `inline_vector`, and move-constructs the elements from the source to the new vector.
TEST(InlineVectorOpTracker, CanMoveViaTryConstructFromSmaller) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;
  using smaller_vec = inline_vector<counting_wrapper, standard_capacity - 1>;
  smaller_vec values(standard_capacity - 1);

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<try_construct_result, ivec, ivec&&>);
  STATIC_ASSERT_TRUE(noexcept(ivec::try_construct(std::move(values))));
  counting_wrapper::count = {};
  auto res = ivec::try_construct(std::move(values));
  ASSERT_EQ(res->size(), standard_capacity - 1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, res->size());
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Constructing an `inline_vector` from an rvalue `inline_vector` of a smaller capacity constructs a new
/// `inline_vector`, and move-constructs the elements from the source to the new vector.
TEST(InlineVectorOpTracker, CanMoveViaConstructFromSmaller) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;
  using smaller_vec = inline_vector<counting_wrapper, standard_capacity - 1>;
  smaller_vec values(standard_capacity - 1);

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<try_construct_result, ivec, ivec&&>);
  STATIC_ASSERT_TRUE(noexcept(ivec(std::move(values))));
  counting_wrapper::count = {};
  auto res = ivec(std::move(values));
  ASSERT_EQ(res.size(), standard_capacity - 1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, res.size());
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Invoking `try_construct` with an rvalue `inline_vector` of a larger capacity but a size that is less than or
/// equal to the capacity of the destination type constructs a new `inline_vector`, and move-constructs the elements
/// from the source to the new vector.
TEST(InlineVectorOpTracker, CanMoveViaTryConstructFromLargerWithFewerElements) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;
  using larger_vec = inline_vector<counting_wrapper, standard_capacity + 1>;
  larger_vec values(standard_capacity - 1);

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<try_construct_result, ivec, ivec&&>);
  STATIC_ASSERT_TRUE(noexcept(ivec::try_construct(std::move(values))));
  counting_wrapper::count = {};
  auto res = ivec::try_construct(std::move(values));
  ASSERT_EQ(res->size(), standard_capacity - 1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, res->size());
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Constructing an `inline_vector` from an rvalue `inline_vector` with a larger capacity but a size that is
/// less than or equal to the capacity of the destination type move-constructs elements from the source vector
TEST(InlineVectorOpTracker, CanMoveViaConstructFromLargerWithFewerElements) {
  using ivec = inline_vector<counting_wrapper, standard_capacity>;
  using larger_vec = inline_vector<counting_wrapper, standard_capacity + 1>;
  larger_vec values(standard_capacity - 1);

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<try_construct_result, ivec, ivec&&>);
  STATIC_ASSERT_TRUE(noexcept(ivec(std::move(values))));
  counting_wrapper::count = {};
  auto res = ivec(std::move(values));
  ASSERT_EQ(res.size(), standard_capacity - 1);
  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, res.size());
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Invoking `try_construct` with an iterator range that has more elements than the capacity of the destination
/// returns an empty `optional`
TEST(InlineVectorOpTracker, TryConstructingFromOverLongBidirectionalRangeReturnsEmpty) {
  constexpr std::size_t capacity = 10;
  arene::base::array<counting_wrapper, capacity + 1> const source{};
  counting_wrapper::count = {};

  ASSERT_FALSE((inline_vector<counting_wrapper, capacity>::try_construct(source.begin(), source.end()).has_value()));

  EXPECT_EQ(counting_wrapper::count.default_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.copy_ctor, 0);
  EXPECT_EQ(counting_wrapper::count.move_assign, 0);
  EXPECT_EQ(counting_wrapper::count.copy_assign, 0);
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test Default-constructing and destroying an `inline_vector` of a user-defined type does not destroy any elements
TEST(InlineVectorOpTracker, NoObjectsDestroyedOnEmptyVector) {
  counting_wrapper::count = {};
  { standard_capacity_vector<counting_wrapper> const vec; }
  EXPECT_EQ(counting_wrapper::count.dtor, 0);
}

/// @test When an `inline_vector` of a user-defined type is destroyed, then the destructor is called for each element.
TEST(InlineVectorOpTracker, InsertedObjectsDestroyedWhenVectorDestroyed) {
  counting_wrapper const source;
  counting_wrapper::count = {};
  {
    standard_capacity_vector<counting_wrapper> vec;
    vec.push_back(source);
    vec.push_back(source);
    vec.push_back(source);
  }
  EXPECT_EQ(counting_wrapper::count.dtor, 3);
}

class trivial_class {};

class type_with_trivial_destructor {
  std::int32_t value_;

 public:
  explicit type_with_trivial_destructor(std::int32_t val)
      : value_(val) {}

  ARENE_MAYBE_UNUSED auto get_value() const noexcept -> std::int32_t { return value_; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class type_with_non_trivial_destructor {
 public:
  ~type_with_non_trivial_destructor() { std::ignore = 42; }
};

/// @test Verify that the destructor for @c inline_vector<T> is trivial if the destructor for @c T is
TEST(InlineVector, DestructorIsTrivialIfTypeIs) {
  STATIC_ASSERT_FALSE(std::is_trivially_default_constructible<type_with_trivial_destructor>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_destructible<int>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_destructible<trivial_class>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_destructible<type_with_trivial_destructor>::value);
  STATIC_ASSERT_FALSE(std::is_trivially_destructible<type_with_non_trivial_destructor>::value);

  STATIC_ASSERT_TRUE(std::is_trivially_destructible<standard_capacity_vector<int>>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_destructible<standard_capacity_vector<trivial_class>>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_destructible<standard_capacity_vector<type_with_trivial_destructor>>::value);
  STATIC_ASSERT_FALSE(std::is_trivially_destructible<standard_capacity_vector<type_with_non_trivial_destructor>>::value
  );
}

}  // namespace
