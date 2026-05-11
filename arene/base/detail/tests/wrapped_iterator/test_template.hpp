// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_WRAPPED_ITERATOR_TEST_TEMPLATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_WRAPPED_ITERATOR_TEST_TEMPLATE_HPP_

#include <gtest/gtest.h>

#include "arene/base/detail/tests/wrapped_iterator/helpers.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace wrapped_iterator_tests {

// NOLINTBEGIN(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic) We want to explicitly test the behavior for raw pointers to c-arrays

template <typename T>
class WrappedIteratorMemberTypesTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(WrappedIteratorMemberTypesTest);

/// @test The `value_type` for `wrapped_iterator` is the same as for the iterator being wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorMemberTypesTest, ValueTypeIsSameAsIteratorTraitsForWrappedIterator) {
  ::testing::StaticAssertTypeEq<
      typename wrapped_iterator_with_default_passkey<TypeParam>::value_type,
      typename std::iterator_traits<TypeParam>::value_type>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<wrapped_iterator_with_default_passkey<TypeParam>>::value_type,
      typename std::iterator_traits<TypeParam>::value_type>();
}

/// @test The `reference` for `wrapped_iterator` is the same as for the iterator being wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorMemberTypesTest, ReferenceIsSameAsIteratorTraitsForWrappedIterator) {
  ::testing::StaticAssertTypeEq<
      typename wrapped_iterator_with_default_passkey<TypeParam>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<wrapped_iterator_with_default_passkey<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

/// @test The `pointer` for `wrapped_iterator` is the same as for the iterator being wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorMemberTypesTest, PointerIsSameAsIteratorTraitsForWrappedIterator) {
  ::testing::StaticAssertTypeEq<
      typename wrapped_iterator_with_default_passkey<TypeParam>::pointer,
      typename std::iterator_traits<TypeParam>::pointer>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<wrapped_iterator_with_default_passkey<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

/// @test The `difference_type` for `wrapped_iterator` is the same as for the iterator being wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorMemberTypesTest, DifferenceTypeIsSameAsIteratorTraitsForWrappedIterator) {
  ::testing::StaticAssertTypeEq<
      typename wrapped_iterator_with_default_passkey<TypeParam>::difference_type,
      typename std::iterator_traits<TypeParam>::difference_type>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<wrapped_iterator_with_default_passkey<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

/// @test The `iterator_category` for `wrapped_iterator` is the same as for the iterator being wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorMemberTypesTest, IteratorCategoryIsSameAsIteratorTraitsForWrappedIterator) {
  ::testing::StaticAssertTypeEq<
      typename wrapped_iterator_with_default_passkey<TypeParam>::iterator_category,
      typename std::iterator_traits<TypeParam>::iterator_category>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<wrapped_iterator_with_default_passkey<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

REGISTER_TYPED_TEST_SUITE_P(
    WrappedIteratorMemberTypesTest,
    ValueTypeIsSameAsIteratorTraitsForWrappedIterator,
    ReferenceIsSameAsIteratorTraitsForWrappedIterator,
    PointerIsSameAsIteratorTraitsForWrappedIterator,
    DifferenceTypeIsSameAsIteratorTraitsForWrappedIterator,
    IteratorCategoryIsSameAsIteratorTraitsForWrappedIterator
);

template <typename T>
class WrappedIteratorReturnTypeEquivalenceTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(WrappedIteratorReturnTypeEquivalenceTest);

/// @test The return type of the dereference operator for `wrapped_iterator` is the same as for the iterator being
/// wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, Dereference) {
  ::testing::StaticAssertTypeEq<
      decltype(*std::declval<wrapped_iterator_with_default_passkey<TypeParam>&>()),
      typename wrapped_iterator_with_default_passkey<TypeParam>::reference>();
  ::testing::StaticAssertTypeEq<
      decltype(*std::declval<wrapped_iterator_with_default_passkey<TypeParam> const&>()),
      typename wrapped_iterator_with_default_passkey<TypeParam>::reference>();
}

/// @test The return type of the arrow operator for `wrapped_iterator` is the same as for the iterator being wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, Arrow) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam>&>().operator->()),
      typename wrapped_iterator_with_default_passkey<TypeParam>::pointer>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam> const&>().operator->()),
      typename wrapped_iterator_with_default_passkey<TypeParam>::pointer>();
}

/// @test The return type of the index operator for `wrapped_iterator` is the same as for the iterator being wrapped
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, Index) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam>&>()[1]),
      typename wrapped_iterator_with_default_passkey<TypeParam>::reference>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam> const&>()[1]),
      typename wrapped_iterator_with_default_passkey<TypeParam>::reference>();
}

/// @test The return type of the preincrement operator for `wrapped_iterator` is a reference to `wrapped_iterator`
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, PreIncrement) {
  ::testing::StaticAssertTypeEq<
      decltype(++std::declval<wrapped_iterator_with_default_passkey<TypeParam>>()),
      wrapped_iterator_with_default_passkey<TypeParam>&>();
}

/// @test The return type of the postincrement operator for `wrapped_iterator` is `wrapped_iterator`
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, PostIncrement) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam>>()++),
      wrapped_iterator_with_default_passkey<TypeParam>>();
}

/// @test The return type of the predecrement operator for `wrapped_iterator` is a reference to `wrapped_iterator`
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, PreDecrement) {
  ::testing::StaticAssertTypeEq<
      decltype(--std::declval<wrapped_iterator_with_default_passkey<TypeParam>>()),
      wrapped_iterator_with_default_passkey<TypeParam>&>();
}

/// @test The return type of the postdecrement operator for `wrapped_iterator` is `wrapped_iterator`
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, PostDecrement) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam>>()--),
      wrapped_iterator_with_default_passkey<TypeParam>>();
}

/// @test The return type of adding an offset to a `wrapped_iterator` is `wrapped_iterator`
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, Addition) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam>>() + 1),
      wrapped_iterator_with_default_passkey<TypeParam>>();
  ::testing::StaticAssertTypeEq<
      decltype(1 + std::declval<wrapped_iterator_with_default_passkey<TypeParam>>()),
      wrapped_iterator_with_default_passkey<TypeParam>>();
}

/// @test The return type of subtracting an offset from a `wrapped_iterator` is `wrapped_iterator`, the return type of
/// subtracting two `wrapped_iterator`s is `difference_type`
/// @tparam TypeParam The iterator type being wrapped
TYPED_TEST_P(WrappedIteratorReturnTypeEquivalenceTest, Subtraction) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam>>() - 1),
      wrapped_iterator_with_default_passkey<TypeParam>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<wrapped_iterator_with_default_passkey<TypeParam>>() - std::declval<wrapped_iterator_with_default_passkey<TypeParam>>()),
      typename wrapped_iterator_with_default_passkey<TypeParam>::difference_type>();
}

REGISTER_TYPED_TEST_SUITE_P(
    WrappedIteratorReturnTypeEquivalenceTest,
    Dereference,
    Arrow,
    Index,
    PreIncrement,
    PostIncrement,
    PreDecrement,
    PostDecrement,
    Addition,
    Subtraction
);

template <typename T>
class WrappedIteratorOperationEquivalenceTest : public ::testing::Test {
 public:
  test_container<T> container{};
  using wrapped_iterator = wrapped_iterator_with_default_passkey<decltype(std::begin(std::declval<T&>()))>;
  using const_wrapped_iterator = wrapped_iterator_with_default_passkey<decltype(std::cbegin(std::declval<T const&>()))>;
};

TYPED_TEST_SUITE_P(WrappedIteratorOperationEquivalenceTest);

/// @test `wrapped_iterator` can be default-constructed
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, CanBeDefaultConstructed) {
  std::ignore = typename TestFixture::wrapped_iterator{};
  std::ignore = typename TestFixture::const_wrapped_iterator{};
}

/// @test Default-constructed `wrapped_iterator`s with the same wrapped type are equal to each other
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, DefaultConstructedIteratorsAreEquivalent) {
  EXPECT_EQ(typename TestFixture::wrapped_iterator{}, typename TestFixture::wrapped_iterator{});
  EXPECT_EQ(typename TestFixture::const_wrapped_iterator{}, typename TestFixture::const_wrapped_iterator{});
}

/// @test Default-constructed `wrapped_iterator`s can be advanced 0 steps
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, DefaultConstructedIteratorsCanBeAdvancedZeroSteps) {
  EXPECT_EQ(typename TestFixture::wrapped_iterator{} += 0, typename TestFixture::wrapped_iterator{});
  EXPECT_EQ(typename TestFixture::const_wrapped_iterator{} += 0, typename TestFixture::const_wrapped_iterator{});
}

/// @test `base` returns the iterator being wrapped
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, BaseReturnsUnderlyingIterator) {
  decltype(auto) raw_itr = this->container.begin();
  auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  EXPECT_EQ(itr.base(), raw_itr);
}

/// @test Dereferencing a `wrapped_iterator` returns the result of dereferencing the iterator being wrapped
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, Dereference) {
  for (decltype(auto) raw_itr = this->container.begin(); raw_itr != this->container.end(); ++raw_itr) {
    auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
    EXPECT_EQ(*itr, *raw_itr);
  }
}

/// @test Dereferencing a `wrapped_iterator` constructed from a const_iterator returns the result of dereferencing the
/// iterator being wrapped
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, ConstDereference) {
  for (decltype(auto) raw_itr = this->container.cbegin(); raw_itr != this->container.cend(); ++raw_itr) {
    auto const itr = typename TestFixture::const_wrapped_iterator{default_passkey{}, raw_itr};
    EXPECT_EQ(*itr, *raw_itr);
  }
}

/// @test Using the arrow operator on a `wrapped_iterator` returns the result of the arrow operator on the iterator
/// being wrapped
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, Arrow) {
  for (decltype(auto) raw_itr = this->container.begin(); raw_itr != this->container.end(); ++raw_itr) {
    auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
    EXPECT_EQ(*(itr.operator->()), *raw_itr);
  }
}

/// @test Using the arrow operator on a `wrapped_iterator` produced for a const_iterator returns the result of the arrow
/// operator on the iterator being wrapped
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, ConstArrow) {
  for (decltype(auto) raw_itr = this->container.cbegin(); raw_itr != this->container.cend(); ++raw_itr) {
    auto const itr = typename TestFixture::const_wrapped_iterator{default_passkey{}, raw_itr};
    EXPECT_EQ(*(itr.operator->()), *raw_itr);
  }
}

/// @test Indexing from a `wrapped_iterator` returns the result of indexing from the iterator being wrapped
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, Index) {
  decltype(auto) raw_itr = this->container.begin();
  auto const distance = ::arene::base::distance(raw_itr, this->container.end());

  auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  EXPECT_EQ(itr[0], raw_itr[0]);
  EXPECT_EQ(itr[distance - 1], raw_itr[distance - 1]);
  EXPECT_EQ(itr[distance / 2], raw_itr[distance / 2]);
}

/// @test Indexing from a `wrapped_iterator` produced from a const_iterator returns the result of indexing from the
/// iterator being wrapped
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, ConstIndex) {
  decltype(auto) raw_itr = this->container.cbegin();
  auto const distance = ::arene::base::distance(raw_itr, this->container.cend());

  auto const itr = typename TestFixture::const_wrapped_iterator{default_passkey{}, raw_itr};
  EXPECT_EQ(itr[0], raw_itr[0]);
  EXPECT_EQ(itr[distance - 1], raw_itr[distance - 1]);
  EXPECT_EQ(itr[distance / 2], raw_itr[distance / 2]);
}

/// @test Preincrementing a `wrapped_iterator` increments the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, PreIncrement) {
  decltype(auto) raw_itr = this->container.begin();
  auto itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  EXPECT_EQ((++itr).base(), ++raw_itr);
  EXPECT_EQ(itr.base(), raw_itr);
}

/// @test Postincrementing a `wrapped_iterator` increments the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, PostIncrement) {
  decltype(auto) raw_itr = this->container.begin();
  auto itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  EXPECT_EQ((itr++).base(), raw_itr++);
  EXPECT_EQ(itr.base(), raw_itr);
}

/// @test Predecrementing a `wrapped_iterator` decrements the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, PreDecrement) {
  decltype(auto) raw_itr = this->container.begin();
  auto itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  EXPECT_EQ((--itr).base(), --raw_itr);
  EXPECT_EQ(itr.base(), raw_itr);
}

/// @test Postdecrementing a `wrapped_iterator` decrements the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, PostDecrement) {
  decltype(auto) raw_itr = this->container.begin();
  auto itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  EXPECT_EQ((itr--).base(), raw_itr--);
  EXPECT_EQ(itr.base(), raw_itr);
}

/// @test Adding a delta to a `wrapped_iterator` returns a `wrapped_iterator` holding the result of adding the delta to
/// the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, AdditionWithDelta) {
  decltype(auto) raw_itr = this->container.begin();
  auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::wrapped_iterator>::difference_type delta = 0;
       delta <= container_size;
       ++delta) {
    EXPECT_EQ((itr + delta).base(), raw_itr + delta);
  }
}

/// @test Subtracting a delta from a `wrapped_iterator` returns a `wrapped_iterator` holding the result of subtracting
/// the delta from the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, SubtractionWithDelta) {
  decltype(auto) raw_itr = this->container.begin();
  auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::wrapped_iterator>::difference_type count = 0;
       count <= container_size;
       ++count) {
    auto const delta = container_size - count;
    EXPECT_EQ((itr - delta).base(), raw_itr - delta);
  }
}

/// @test Subtracting a `wrapped_iterator` from another returns the result of subtracting the wrapped iterators
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, SubtractionWithIterator) {
  decltype(auto) raw_itr = this->container.begin();
  auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::wrapped_iterator>::difference_type offset = 0;
       offset <= container_size;
       ++offset) {
    auto const further_raw_itr = raw_itr + offset;
    auto const further_itr = itr + offset;
    EXPECT_EQ(further_itr - itr, further_raw_itr - raw_itr);
    EXPECT_EQ(itr - further_itr, raw_itr - further_raw_itr);
  }
}

/// @test Subtracting a default-constructed `wrapped_iterator` from another returns zero
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, SubtractionWithTwoDefaultCtordIteratorsIsZero) {
  EXPECT_EQ(typename TestFixture::wrapped_iterator{} - typename TestFixture::wrapped_iterator{}, 0);
}

/// @test Adding a delta to a `wrapped_iterator` with `+=` adds the delta to the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, IncrementAssign) {
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::wrapped_iterator>::difference_type delta = 0;
       delta <= container_size;
       ++delta) {
    decltype(auto) raw_itr = this->container.begin();
    auto itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
    EXPECT_EQ((itr += delta).base(), raw_itr += delta);
    EXPECT_EQ(itr.base(), raw_itr);
  }
}

/// @test Subtracting a delta from a `wrapped_iterator` with `-=` subtracts the delta from the wrapped iterator
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, DecrementAssign) {
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::wrapped_iterator>::difference_type count = 0;
       count <= container_size;
       ++count) {
    auto const delta = container_size - count;
    decltype(auto) raw_itr = this->container.begin();
    auto itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
    EXPECT_EQ((itr -= delta).base(), raw_itr -= delta);
    EXPECT_EQ(itr.base(), raw_itr);
  }
}

/// @test The result of comparing two `wrapped_iterator`s is the same as the result of comparing the underlying
/// iterators
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, Comparison) {
  for (decltype(auto) raw_itr = this->container.begin() + 1; raw_itr != this->container.end() - 1; ++raw_itr) {
    auto const itr = typename TestFixture::wrapped_iterator{default_passkey{}, raw_itr};
    auto const itr_greater = itr + 1;
    auto const itr_less = itr - 1;
    auto const raw_itr_greater = raw_itr + 1;
    auto const raw_itr_less = raw_itr - 1;

    EXPECT_EQ((itr == itr), (raw_itr == raw_itr));
    EXPECT_EQ((itr == itr_greater), (raw_itr == raw_itr_greater));
    EXPECT_EQ((itr == itr_less), (raw_itr == raw_itr_less));

    EXPECT_EQ((itr != itr), (raw_itr != raw_itr));
    EXPECT_EQ((itr != itr_greater), (raw_itr != raw_itr_greater));
    EXPECT_EQ((itr != itr_less), (raw_itr != raw_itr_less));

    EXPECT_EQ((itr < itr), (raw_itr < raw_itr));
    EXPECT_EQ((itr < itr_greater), (raw_itr < raw_itr_greater));
    EXPECT_EQ((itr < itr_less), (raw_itr < raw_itr_less));

    EXPECT_EQ((itr <= itr), (raw_itr <= raw_itr));
    EXPECT_EQ((itr <= itr_greater), (raw_itr <= raw_itr_greater));
    EXPECT_EQ((itr <= itr_less), (raw_itr <= raw_itr_less));

    EXPECT_EQ((itr > itr), (raw_itr > raw_itr));
    EXPECT_EQ((itr > itr_greater), (raw_itr > raw_itr_greater));
    EXPECT_EQ((itr > itr_less), (raw_itr > raw_itr_less));

    EXPECT_EQ((itr >= itr), (raw_itr >= raw_itr));
    EXPECT_EQ((itr >= itr_greater), (raw_itr >= raw_itr_greater));
    EXPECT_EQ((itr >= itr_less), (raw_itr >= raw_itr_less));
  }
}

/// @test The result of comparing two `wrapped_iterator`s produced from a const_iterator is the same as the result of
/// comparing the underlying iterators
/// @tparam TypeParam The type of a sequence container to use as a source of iterators
TYPED_TEST_P(WrappedIteratorOperationEquivalenceTest, ConstComparison) {
  for (decltype(auto) raw_itr = this->container.cbegin() + 1; raw_itr != this->container.cend() - 1; ++raw_itr) {
    auto const itr = typename TestFixture::const_wrapped_iterator{default_passkey{}, raw_itr};
    auto const itr_greater = itr + 1;
    auto const itr_less = itr - 1;
    auto const raw_itr_greater = raw_itr + 1;
    auto const raw_itr_less = raw_itr - 1;

    EXPECT_EQ((itr == itr), (raw_itr == raw_itr));
    EXPECT_EQ((itr == itr_greater), (raw_itr == raw_itr_greater));
    EXPECT_EQ((itr == itr_less), (raw_itr == raw_itr_less));

    EXPECT_EQ((itr != itr), (raw_itr != raw_itr));
    EXPECT_EQ((itr != itr_greater), (raw_itr != raw_itr_greater));
    EXPECT_EQ((itr != itr_less), (raw_itr != raw_itr_less));

    EXPECT_EQ((itr < itr), (raw_itr < raw_itr));
    EXPECT_EQ((itr < itr_greater), (raw_itr < raw_itr_greater));
    EXPECT_EQ((itr < itr_less), (raw_itr < raw_itr_less));

    EXPECT_EQ((itr <= itr), (raw_itr <= raw_itr));
    EXPECT_EQ((itr <= itr_greater), (raw_itr <= raw_itr_greater));
    EXPECT_EQ((itr <= itr_less), (raw_itr <= raw_itr_less));

    EXPECT_EQ((itr > itr), (raw_itr > raw_itr));
    EXPECT_EQ((itr > itr_greater), (raw_itr > raw_itr_greater));
    EXPECT_EQ((itr > itr_less), (raw_itr > raw_itr_less));

    EXPECT_EQ((itr >= itr), (raw_itr >= raw_itr));
    EXPECT_EQ((itr >= itr_greater), (raw_itr >= raw_itr_greater));
    EXPECT_EQ((itr >= itr_less), (raw_itr >= raw_itr_less));
  }
}

REGISTER_TYPED_TEST_SUITE_P(
    WrappedIteratorOperationEquivalenceTest,
    CanBeDefaultConstructed,
    DefaultConstructedIteratorsAreEquivalent,
    DefaultConstructedIteratorsCanBeAdvancedZeroSteps,
    BaseReturnsUnderlyingIterator,
    Dereference,
    ConstDereference,
    Arrow,
    ConstArrow,
    Index,
    ConstIndex,
    PreIncrement,
    PostIncrement,
    PreDecrement,
    PostDecrement,
    AdditionWithDelta,
    SubtractionWithDelta,
    SubtractionWithIterator,
    SubtractionWithTwoDefaultCtordIteratorsIsZero,
    IncrementAssign,
    DecrementAssign,
    Comparison,
    ConstComparison
);

// NOLINTEND(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic)

}  // namespace wrapped_iterator_tests
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_WRAPPED_ITERATOR_TEST_TEMPLATE_HPP_
