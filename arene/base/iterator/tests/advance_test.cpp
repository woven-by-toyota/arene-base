// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/advance.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/copy_n.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <forward_list>
#include <list>
#include <vector>
#endif

namespace {
using ::arene::base::advance;
using ::arene::base::constraints;
using ::arene::base::is_invocable_v;
namespace iterator_tests = ::arene::base::iterator_tests;

/// @test @c ::arene::base::advance should not satisfy @c is_invocable_v if the input iterator does not at least satisfy
/// @c is_input_iterator_v .
TEST(AdvanceInvocability, IsNotInvocableForTypeNotAtLeastInputIterator) {
  struct not_an_iterator {};
  STATIC_ASSERT_FALSE((is_invocable_v<decltype(advance), int&, int>));
  STATIC_ASSERT_FALSE((is_invocable_v<decltype(advance), not_an_iterator&, int>));
}

/// @test @c ::arene::base::advance should satisfy @c is_invocable_v if the input iterator is input_iterator
TEST(AdvanceInvocability, IsInvocableForInputIterator) {
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(advance), iterator_tests::forward_iterator_facade<>&, int>));
}
/// @test @c ::arene::base::advance should satisfy @c is_invocable_v if the input iterator is bidirectional_iterator
TEST(AdvanceInvocability, IsInvocableForBiDirectionalIterator) {
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(advance), iterator_tests::input_iterator_facade<>&, int>));
}
/// @test @c ::arene::base::advance should satisfy @c is_invocable_v if the input iterator is random_access_iterator
TEST(AdvanceInvocability, IsInvocableForRandomAccessIterator) {
  STATIC_ASSERT_TRUE((is_invocable_v<decltype(advance), iterator_tests::random_access_iterator_facade<>&, int>));
}

/// @test Given a random access iterator, @c arene::base::advance is noexcept if the iterator's @c operator+= is
/// noexcept.
TEST(AdvanceNoexcept, ForRandomAccessIteratorIsTrueIfIncrementAssignIsNoexcept) {
  STATIC_ASSERT_TRUE(
      noexcept(::arene::base::advance(std::declval<iterator_tests::random_access_iterator_facade<>&>(), 1))
  );
  STATIC_ASSERT_FALSE(noexcept(::arene::base::advance(
      std::declval<iterator_tests::random_access_iterator_facade<iterator_tests::throwing_increment_assign>&>(),
      1
  )));
}
/// @test Given a bidirectional iterator, @c arene::base::advance is noexcept if the iterator's @c operator++ and
/// @c operator-- are noexcept.
TEST(AdvanceNoexcept, ForBidirectionalIteratorIsTrueIfPostIncrementAndPostDecrementAreNoexcept) {
  STATIC_ASSERT_TRUE(
      noexcept(::arene::base::advance(std::declval<iterator_tests::bidirectional_iterator_facade<>&>(), 1))
  );
  STATIC_ASSERT_FALSE(noexcept(::arene::base::advance(
      std::declval<iterator_tests::bidirectional_iterator_facade<iterator_tests::throwing_preincrement>&>(),
      1
  )));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::advance(
      std::declval<iterator_tests::bidirectional_iterator_facade<iterator_tests::throwing_predecrement>&>(),
      1
  )));
}
/// @test Given an input iterator, @c arene::base::advance is noexcept if the iterator's @c operator++ is noexcept.
TEST(AdvanceNoexcept, ForInputIteratorIsTrueIfPostIncrementIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(::arene::base::advance(std::declval<iterator_tests::input_iterator_facade<>&>(), 1)));
  STATIC_ASSERT_FALSE(noexcept(::arene::base::advance(
      std::declval<iterator_tests::input_iterator_facade<iterator_tests::throwing_preincrement>&>(),
      1
  )));
}

constexpr std::size_t container_size = 5;
constexpr std::initializer_list<int> values = {1, 2, 3, 4, 5};
static_assert(values.size() == container_size, "values and container_size must have the same size");
using all_iterator_types = ::testing::Types<
    arene::base::array<int, 10>,
    arene::base::array<int, 10> const,
    arene::base::iterator_tests::demoted_iterator_array<std::bidirectional_iterator_tag>,
    arene::base::iterator_tests::demoted_iterator_array<std::bidirectional_iterator_tag> const,
    arene::base::iterator_tests::demoted_iterator_array<std::forward_iterator_tag>,
    arene::base::iterator_tests::demoted_iterator_array<std::forward_iterator_tag> const
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::vector<int>,
    std::vector<int> const,
    std::list<int>,
    std::list<int> const,
    std::forward_list<int>,
    std::forward_list<int> const
#endif
    >;

using bidirectional_iterator_types = ::testing::Types<
    arene::base::array<int, 10>,
    arene::base::array<int, 10> const,
    arene::base::iterator_tests::demoted_iterator_array<std::bidirectional_iterator_tag>,
    arene::base::iterator_tests::demoted_iterator_array<std::bidirectional_iterator_tag> const
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
    ,
    std::vector<int>,
    std::vector<int> const,
    std::list<int>,
    std::list<int> const
#endif
    >;

template <typename T>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct AdvanceTest
    : protected iterator_tests::backing_data<T>
    , public ::testing::Test {
  using f_pdiff_t =
      typename std::iterator_traits<decltype(std::declval<iterator_tests::backing_data<T>>().begin())>::difference_type;
};

template <typename T>
class AdvancePositiveTest : public AdvanceTest<T> {};

TYPED_TEST_SUITE(AdvancePositiveTest, iterator_tests::at_least_forward_iterator_containers, );

template <typename Iterator>
void do_increment(Iterator& iter, std::ptrdiff_t steps) {
  for (; steps > 0; --steps) {
    ++iter;
  }
}

template <typename Iterator>
void do_decrement(Iterator& iter, std::ptrdiff_t steps) {
  for (; steps < 0; ++steps) {
    --iter;
  }
}

/// @test Given an at least forward iterator, @c arene::base::advance is equivalent to @c std::advance for a positive
/// step.
TYPED_TEST(AdvancePositiveTest, IsEquivalentToStdAdvance) {
  using pdiff_t = typename TestFixture::f_pdiff_t;
  auto const steps = static_cast<pdiff_t>(container_size / 2);
  auto arene_itr = this->begin();
  auto std_itr = this->begin();
  advance(arene_itr, steps);
  do_increment(std_itr, steps);
  EXPECT_EQ(arene_itr, std_itr);
  advance(arene_itr, static_cast<pdiff_t>(container_size) - steps);
  do_increment(std_itr, static_cast<pdiff_t>(container_size) - steps);
  EXPECT_EQ(arene_itr, std_itr);
  EXPECT_EQ(arene_itr, this->end());
}

template <typename T>
class AdvanceNegativeTest : public AdvanceTest<T> {};

TYPED_TEST_SUITE(AdvanceNegativeTest, iterator_tests::at_least_bidirectional_iterator_containers, );
/// @test Given at least a bidirectional iterator, @c arene::base::advance is equivalent to @c std::advance for a
/// negative step.
TYPED_TEST(AdvanceNegativeTest, IsEquivalentToStdAdvance) {
  using pdiff_t = typename TestFixture::f_pdiff_t;
  auto const steps = -static_cast<pdiff_t>(container_size / 2);
  auto arene_itr = this->end();
  auto std_itr = this->end();
  advance(arene_itr, steps);
  do_decrement(std_itr, steps);
  EXPECT_EQ(arene_itr, std_itr);
  advance(arene_itr, -static_cast<pdiff_t>(container_size) - steps);
  do_decrement(std_itr, -static_cast<pdiff_t>(container_size) - steps);
  EXPECT_EQ(arene_itr, std_itr);
  EXPECT_EQ(arene_itr, this->begin());
}

constexpr auto advance_to(
    std::initializer_list<int> lst,
    typename std::iterator_traits<decltype(lst)::iterator>::difference_type index
) {
  auto const* itr = lst.begin();
  advance(itr, index);
  return itr;
}
/// @test Validates @c arene::base::next is invocable in a constexpr context.
TEST(AdvanceConstexpr, CanBeCalledInAConstexprContext) {
  STATIC_ASSERT_EQ((*advance_to({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 4)), 5);
}

}  // namespace
