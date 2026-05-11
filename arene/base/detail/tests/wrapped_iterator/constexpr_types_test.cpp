// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/tests/wrapped_iterator/helpers.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace wrapped_iterator_tests {

// NOLINTBEGIN(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic) Explicitly testing c-arrays

template <typename T>
class WrappedIteratorOperationEquivalenceConstexprTest : public ::testing::Test {
 public:
  using wrapped_iterator = wrapped_iterator_with_default_passkey<decltype(std::begin(std::declval<T const&>()))>;
};

using constexpr_container_types = ::testing::Types<int[10], int const[10], char[10], char const[10]>;

TYPED_TEST_SUITE(WrappedIteratorOperationEquivalenceConstexprTest, constexpr_container_types, );

/// @test `base` returns the underlying iterator in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, baseReturnsUnderlyingIterator) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin()).base(),
      container.cbegin()
  );
}

/// @test Dereferencing a `wrapped_iterator` returns the result of dereferencing the underlying iterator in a
/// `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, Dereference) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      *(typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin())),
      *container.cbegin()
  );
}

/// @test The arrow operator on a `wrapped_iterator` returns the result of the arrow operator on the underlying iterator
/// in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, Arrow) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      *(typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin()).operator->()),
      *container.cbegin()
  );
}

/// @test Indexing from a `wrapped_iterator` returns the result of indexing from the underlying iterator in a
/// `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, Index) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin())[0],
      container.cbegin()[0]
  );
}

/// Constexpr wrappers around mutating operators
/// @{
template <typename Itr>
constexpr auto do_post_increment(Itr itr) {
  return ++itr;
}

template <typename Itr>
constexpr auto do_pre_increment(Itr itr) {
  return itr++;
}

template <typename Itr>
constexpr auto do_post_decrement(Itr itr) {
  return --itr;
}

template <typename Itr>
constexpr auto do_pre_decrement(Itr itr) {
  return itr--;
}
/// @}

/// @test Preincrement on a `wrapped_iterator` increments the underlying iterator in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, PreIncrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_post_increment(typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin())).base(),
      do_post_increment(container.cbegin())
  );
}

/// @test Postincrement on a `wrapped_iterator` increments the underlying iterator in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, PostIncrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_pre_increment(typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin())).base(),
      do_pre_increment(container.cbegin())
  );
}

/// @test Predecrement on a `wrapped_iterator` decrements the underlying iterator in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, PreDecrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_post_decrement(typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin() + 1)).base(),
      do_post_decrement(container.cbegin() + 1)
  );
}

/// @test Postdecrement on a `wrapped_iterator` decrements the underlying iterator in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, PostDecrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_pre_decrement(typename TestFixture::wrapped_iterator{default_passkey{}, container.cbegin() + 1}).base(),
      do_pre_decrement(container.cbegin() + 1)
  );
}

/// @test Adding a delta to a `wrapped_iterator` returns a `wrapped_iterator` holding the result of adding the delta to
/// the wrapped iterator in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, AdditionWithDelta) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      (typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin()) + 1).base(),
      container.cbegin() + 1
  );
}

/// @test Subtracting a delta from a `wrapped_iterator` returns a `wrapped_iterator` holding the result of subtracting
/// the delta from the wrapped iterator in a `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, SubtractionWithDelta) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      (typename TestFixture::wrapped_iterator(default_passkey{}, container.cbegin() + 1) - 1).base(),
      (container.cbegin() + 1) - 1
  );
}

/// @test Subtracting a `wrapped_iterator` from another returns the result of subtracting the wrapped iterators in a
/// `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, SubtractionWithIterator) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      ((typename TestFixture::wrapped_iterator{default_passkey{}, container.cbegin()} + 1) -
       typename TestFixture::wrapped_iterator{default_passkey{}, container.cbegin()}),
      (container.cbegin() + 1) - container.cbegin()
  );
}

/// Constexpr wrappers around mutating operators
/// @{
template <typename Itr>
constexpr auto do_increment_assign(Itr itr, typename std::iterator_traits<Itr>::difference_type delta) {
  return itr += delta;
}

template <typename Itr>
constexpr auto do_decrement_assign(Itr itr, typename std::iterator_traits<Itr>::difference_type delta) {
  return itr -= delta;
}
/// @}

/// @test Adding a delta to a `wrapped_iterator` with `+=` adds the delta to the wrapped iterator in a `constexpr`
/// context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, IncrementAssign) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_increment_assign(typename TestFixture::wrapped_iterator{default_passkey{}, container.cbegin()}, 1).base(),
      do_increment_assign(container.cbegin(), 1)
  );
}

/// @test Subtracting a delta from a `wrapped_iterator` with `-=` subtracts the delta to the wrapped iterator in a
/// `constexpr` context
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST(WrappedIteratorOperationEquivalenceConstexprTest, DecrementAssign) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_decrement_assign(typename TestFixture::wrapped_iterator{default_passkey{}, container.cbegin()} + 1, 1).base(),
      do_decrement_assign(container.cbegin() + 1, 1)
  );
}

// NOLINTEND(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic)

}  // namespace wrapped_iterator_tests
