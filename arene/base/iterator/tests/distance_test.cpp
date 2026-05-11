// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/distance.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/prev.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/copy_n.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>
#endif

namespace {

// For testing arene::base::span.
template <typename T, std::size_t S>
struct proxy_span {
  arene::base::iterator_tests::demoted_iterator_array<std::random_access_iterator_tag, T, S> data;
  arene::base::span<T, S> span;

  proxy_span(std::initializer_list<T> content)
      : data(content),
        span(data) {}

  using difference_type = typename arene::base::span<T, S>::difference_type;

  using const_iterator = typename arene::base::span<T, S>::iterator;
  using iterator = typename arene::base::span<T, S>::iterator;

  using const_reverse_iterator = typename arene::base::span<T, S>::reverse_iterator;
  using reverse_iterator = typename arene::base::span<T, S>::reverse_iterator;

  auto begin() const -> iterator { return span.begin(); }
  auto end() const -> iterator { return span.end(); }

  auto rbegin() const -> reverse_iterator { return span.rbegin(); }
  auto rend() const -> reverse_iterator { return span.rend(); }
};

std::size_t const size = 7;
std::ptrdiff_t const offset = 3;

// Test distance on containers that support forward iterators.

namespace iterator_tests = ::arene::base::iterator_tests;

template <typename Container>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct DistanceTest
    : protected iterator_tests::backing_data<Container>
    , public ::testing::Test {
  std::ptrdiff_t const offset = 3;
};

using forward_iterator_containers = ::testing::Types<
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    std::forward_list<int>,
    std::unordered_set<int>,
    std::list<int>,
    std::set<int>,
    std::deque<int>,
    std::array<int, size>,
    std::vector<int>,
#endif
    arene::base::array<int, size>,
    arene::base::iterator_tests::demoted_iterator_array<std::forward_iterator_tag, int, size>,
    proxy_span<int, size>>;

template <typename Container>
struct ForwardIteratorDistanceTest : ::testing::Test {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  Container const container{1, 2, 3, 4, 5, 6, 7};
  using const_iterator = typename Container::const_iterator;
};

TYPED_TEST_SUITE(ForwardIteratorDistanceTest, forward_iterator_containers, );

/// @test Confirm that the iterator type for the supplied container is a forward iterator
/// @tparam TypeParam The type of the container
TYPED_TEST(ForwardIteratorDistanceTest, IsForwardIterator) {
  static_assert(arene::base::is_forward_iterator_v<decltype(this->container.begin())>, "Must be a forward iterator");
}

/// @test `distance` returns zero if `first` equals `last`
/// @tparam TypeParam The type of the container
TYPED_TEST(ForwardIteratorDistanceTest, ReturnsZeroForFirstEqualLast) {
  ASSERT_EQ(arene::base::distance(this->container.begin(), this->container.begin()), 0);
  ASSERT_EQ(arene::base::distance(this->container.end(), this->container.end()), 0);
}

/// @test `distance` returns the size of the container if invoked with the results of `begin` and `end`
/// @tparam TypeParam The type of the container
TYPED_TEST(ForwardIteratorDistanceTest, ReturnsSize) {
  ASSERT_EQ(arene::base::distance(this->container.begin(), this->container.end()), size);
}

/// @test `distance` invoked with the result of `begin` and an iterator in the middle of the container returns the
/// offset from the result of `begin` to that middle iterator.
/// @tparam TypeParam The type of the container
TYPED_TEST(ForwardIteratorDistanceTest, DistanceFromBeginToMiddleIsOffset) {
  // NOLINTNEXTLINE(hicpp-use-auto,modernize-use-auto)
  auto const middle = arene::base::next(this->container.begin(), offset);
  ASSERT_EQ(arene::base::distance(this->container.begin(), middle), offset);
}

/// @test `distance` invoked with an iterator in the middle of the container and the result of `end` returns the size of
/// the container minus the offset from the result of `begin` to that middle iterator.
/// @tparam TypeParam The type of the container
TYPED_TEST(ForwardIteratorDistanceTest, DistanceFromMiddleToEndIsSizeMinusOffset) {
  // NOLINTNEXTLINE(hicpp-use-auto,modernize-use-auto)
  auto const middle = arene::base::next(this->container.begin(), offset);
  ASSERT_EQ(arene::base::distance(middle, this->container.end()), static_cast<std::ptrdiff_t>(size) - offset);
}

// Test distance on containers that support bidirectional iterators.

template <typename Container>
struct BidirectionalIteratorDistanceTest : ::testing::Test {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  Container const container{1, 2, 3, 4, 5, 6, 7};
  using const_iterator = typename Container::const_iterator;
  using const_reverse_iterator = typename Container::const_reverse_iterator;
};

using bidirectional_iterator_containers = ::testing::Types<
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    std::vector<int>,
    std::list<int>,
    std::set<int>,
    std::deque<int>,
    std::array<int, size>,
#endif
    arene::base::array<int, size>,
    arene::base::iterator_tests::demoted_iterator_array<std::bidirectional_iterator_tag, int, size>,
    proxy_span<int, size>>;

TYPED_TEST_SUITE(BidirectionalIteratorDistanceTest, bidirectional_iterator_containers, );

/// @test Confirm that the iterator type for the container is a bidirectional iterator
/// @tparam TypeParam The type of the container
TYPED_TEST(BidirectionalIteratorDistanceTest, IsBidirectionalIterator) {
  static_assert(
      arene::base::is_bidirectional_iterator_v<decltype(this->container.begin())>,
      "Must be a bidirectional iterator"
  );
}

template <typename Container>
class DistanceFirstBeforeLastTest : public DistanceTest<Container> {};

TYPED_TEST_SUITE(DistanceFirstBeforeLastTest, iterator_tests::at_least_forward_iterator_containers, );

/// @test Given two forward iterators from the same sequence where both iterators point to the same location in the
/// sequence, @c arene::base::distance returns @c 0 .
TYPED_TEST(DistanceFirstBeforeLastTest, ReturnsZeroForFirstEqualLast) {
  EXPECT_EQ(arene::base::distance(this->begin(), this->begin()), 0);
  EXPECT_EQ(arene::base::distance(this->end(), this->end()), 0);
}
/// @test Given two forward iterators from the same sequence where the first iterator points to a location earlier
/// in the sequence than the second, @c arene::base::distance returns the number of steps between the positions as a
/// positive value.
TYPED_TEST(DistanceFirstBeforeLastTest, ReturnsPositiveNumberOfStepsFromFirstToLastWhenFirstIsBeforeLast) {
  EXPECT_EQ(arene::base::distance(this->begin(), this->end()), this->ssize());
  EXPECT_EQ(arene::base::distance(::arene::base::next(this->begin()), ::arene::base::next(this->begin(), 2)), 1);
}

template <typename Iter>
extern constexpr bool has_random_access_tag_v =
    std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>::value;

template <typename Iter, arene::base::constraints<std::enable_if_t<!has_random_access_tag_v<Iter>>> = nullptr>
auto find_distance(Iter lhs, Iter rhs) -> std::ptrdiff_t {
  std::ptrdiff_t count{0};
  for (; lhs != rhs; ++lhs) {
    ++count;
  }
  return count;
}

template <typename Iter, arene::base::constraints<std::enable_if_t<has_random_access_tag_v<Iter>>> = nullptr>
auto find_distance(Iter lhs, Iter rhs) -> std::ptrdiff_t {
  return rhs - lhs;
}

/// @test Given two forward iterators from the same sequence where the first iterator points to a location earlier
/// in the sequence than the second, @c arene::base::distance is equivalent to @c std::distance ,
TYPED_TEST(DistanceFirstBeforeLastTest, IsEquivalentToStdDistance) {
  EXPECT_EQ(arene::base::distance(this->begin(), this->end()), find_distance(this->begin(), this->end()));
}

template <typename Container>
class DistanceFirstAfterLastTest : public DistanceTest<Container> {};

using random_access_iterator_containers = ::testing::Types<
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    std::deque<int>,
    std::array<int, size>,
    std::vector<int>,
#endif
    arene::base::array<int, size>,
    arene::base::iterator_tests::demoted_iterator_array<std::random_access_iterator_tag, int, size>,
    proxy_span<int, size>>;

template <typename Container>
struct RandomAccessIteratorDistanceTest : ::testing::Test {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  Container const container{1, 2, 3, 4, 5, 6, 7};

  using const_iterator = typename Container::const_iterator;
  using const_reverse_iterator = typename Container::const_reverse_iterator;
};

TYPED_TEST_SUITE(RandomAccessIteratorDistanceTest, random_access_iterator_containers, );

/// @test Confirm that the iterator type for the container is a random access iterator
/// @tparam TypeParam The type of the container
TYPED_TEST(RandomAccessIteratorDistanceTest, IsRandomAccessIterator) {
  using container_iterator = typename TypeParam::const_iterator;
  static_assert(has_random_access_tag_v<container_iterator>, "Must be a random-access iterator");
}
TYPED_TEST_SUITE(DistanceFirstAfterLastTest, iterator_tests::random_access_iterator_containers, );
/// @test Given two random access iterators from the same sequence where the first iterator points to a location
/// later in the sequence than the second, @c arene::base::distance returns the number of steps between the
/// positions as a negative value.
TYPED_TEST(DistanceFirstAfterLastTest, ReturnsNegativeNumberOfStepsFromFirstToLastWhenFirstIsBeforeLast) {
  EXPECT_EQ(arene::base::distance(this->end(), this->begin()), -this->ssize());
  EXPECT_EQ(arene::base::distance(::arene::base::prev(this->end()), ::arene::base::prev(this->end(), 2)), -1);
}
/// @test Given two random access iterators from the same sequence where the first iterator points to a location
/// later in the sequence than the second, @c arene::base::distance is equivalent to @c std::distance ,
TYPED_TEST(DistanceFirstAfterLastTest, IsEquivalentToStdDistance) {
  EXPECT_EQ(arene::base::distance(this->end(), this->begin()), find_distance(this->end(), this->begin()));
}
/// @test `distance` returns the expected results when given pointers into an array
TEST(Distance, DistanceRawArrayIterator) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  int c_array[size]{};

  ASSERT_EQ(arene::base::distance(std::begin(c_array), std::end(c_array)), size);
  ASSERT_EQ(arene::base::distance(std::end(c_array), std::begin(c_array)), -static_cast<std::ptrdiff_t>(size));

  ASSERT_EQ(arene::base::distance(&(c_array[0]), &(c_array[offset])), offset);
}

/// @test Validates that @c arene::base::distance works in constexpr.
TEST(DistanceConstexpr, ReturnsNumberOfStepsFromFirstToLast) {
  constexpr arene::base::array<int, size> container{1, 2, 3, 4, 5, 6, 7};
  static_assert(
      arene::base::distance(container.begin(), container.end()) == size,
      "Expected distance to be size of container"
  );
}

}  // namespace
