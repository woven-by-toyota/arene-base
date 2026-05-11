// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/reverse_iterator.hpp"

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_container/set.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list.hpp"

namespace {

// NOLINTBEGIN(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic) We want to explicitly test the behavior for raw pointers to c-arrays

using ::arene::base::reverse_iterator;

using ::arene::base::type_list;

namespace type_lists = ::arene::base::type_lists;

/// Helper for casting a type_list to testing::Types
template <typename Tl>
using as_testing_types = type_lists::apply_all_t<Tl, ::testing::Types>;

/// containers with bidirectional iterators
using bidirectional_iterator_containers =
    type_list<arene::base::inline_map<int, int, 10>, arene::base::inline_set<int, 10>>;

/// containers with random access iterators
using random_access_iterator_containers = type_list<
    int[10],
    int const[10],
    char[10],
    char const[10],
    arene::base::inline_vector<int, 10>,
    arene::base::array<int, 10>>;
namespace iterator_tests = ::arene::base::iterator_tests;

/// Metafunction to get the iterator type of a container
template <typename Container>
using iterator_type = decltype(std::begin(std::declval<Container&>()));
template <typename Container>
using iterator_type_tl = type_list<iterator_type<Container>>;

/// Metafunction to get the const iterator type of a container
template <typename Container>
using const_iterator_type = decltype(std::cbegin(std::declval<Container&>()));
template <typename Container>
using const_iterator_type_tl = type_list<const_iterator_type<Container>>;

/// iterators that are bidirectional
using bidirectional_iterators = type_lists::concat_t<
    type_lists::flat_map_t<iterator_tests::bidirectional_iterator_containers, iterator_type_tl>,
    type_lists::flat_map_t<iterator_tests::bidirectional_iterator_containers, const_iterator_type_tl>>;

/// iterators that are random access
using random_access_iterators = type_lists::concat_t<
    type_lists::flat_map_t<iterator_tests::random_access_iterator_containers, iterator_type_tl>,
    type_lists::flat_map_t<iterator_tests::random_access_iterator_containers, const_iterator_type_tl>>;

template <typename T>
class ReverseIteratorMemberTypesAreIteratorTraitsTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(ReverseIteratorMemberTypesAreIteratorTraitsTest);

/// @test Given an iterator, @c arene::base::reverse_iterator<Iterator>::value_type is equivalent to
/// @c std::iterator_traits<Iterator>::value_type .
TYPED_TEST_P(ReverseIteratorMemberTypesAreIteratorTraitsTest, ValueType) {
  ::testing::StaticAssertTypeEq<
      typename reverse_iterator<TypeParam>::value_type,
      typename std::iterator_traits<TypeParam>::value_type>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<reverse_iterator<TypeParam>>::value_type,
      typename std::iterator_traits<TypeParam>::value_type>();
}

/// @test Given an iterator, @c arene::base::reverse_iterator<Iterator>::referece is equivalent to
/// @c std::iterator_traits<Iterator>::reference .
TYPED_TEST_P(ReverseIteratorMemberTypesAreIteratorTraitsTest, Reference) {
  ::testing::StaticAssertTypeEq<
      typename reverse_iterator<TypeParam>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<reverse_iterator<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

/// @test Given an iterator, @c arene::base::reverse_iterator<Iterator>::pointer is equivalent to
/// @c std::iterator_traits<Iterator>::pointer .
TYPED_TEST_P(ReverseIteratorMemberTypesAreIteratorTraitsTest, Pointer) {
  ::testing::StaticAssertTypeEq<
      typename reverse_iterator<TypeParam>::pointer,
      typename std::iterator_traits<TypeParam>::pointer>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<reverse_iterator<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

/// @test Given an iterator, @c arene::base::reverse_iterator<Iterator>::difference_type is equivalent to
/// @c std::iterator_traits<Iterator>::difference_type .
TYPED_TEST_P(ReverseIteratorMemberTypesAreIteratorTraitsTest, DifferenceType) {
  ::testing::StaticAssertTypeEq<
      typename reverse_iterator<TypeParam>::difference_type,
      typename std::iterator_traits<TypeParam>::difference_type>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<reverse_iterator<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

/// @test Given an iterator, @c arene::base::reverse_iterator<Iterator>::iterator_category is equivalent to
/// @c std::iterator_traits<Iterator>::iterator_category .
TYPED_TEST_P(ReverseIteratorMemberTypesAreIteratorTraitsTest, IteratorCategory) {
  ::testing::StaticAssertTypeEq<
      typename reverse_iterator<TypeParam>::iterator_category,
      typename std::iterator_traits<TypeParam>::iterator_category>();
  ::testing::StaticAssertTypeEq<
      typename std::iterator_traits<reverse_iterator<TypeParam>>::reference,
      typename std::iterator_traits<TypeParam>::reference>();
}

REGISTER_TYPED_TEST_SUITE_P(
    ReverseIteratorMemberTypesAreIteratorTraitsTest,
    ValueType,
    Reference,
    Pointer,
    DifferenceType,
    IteratorCategory
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    Bidirectional,
    ReverseIteratorMemberTypesAreIteratorTraitsTest,
    bidirectional_iterators,
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    RandomAccess,
    ReverseIteratorMemberTypesAreIteratorTraitsTest,
    random_access_iterators,
);

template <typename T>
class ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest);

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator* 's return type
/// is @c arene::base::reverse_iterator<Iterator>::reference_type .
TYPED_TEST_P(ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest, Dereference) {
  ::testing::StaticAssertTypeEq<
      decltype(*std::declval<reverse_iterator<TypeParam>&>()),
      typename reverse_iterator<TypeParam>::reference>();
  ::testing::StaticAssertTypeEq<
      decltype(*std::declval<reverse_iterator<TypeParam> const&>()),
      typename reverse_iterator<TypeParam>::reference>();
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator-> 's return type
/// is @c arene::base::reverse_iterator<Iterator>::pointer .
TYPED_TEST_P(ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest, Arrow) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<reverse_iterator<TypeParam>&>().operator->()),
      typename reverse_iterator<TypeParam>::pointer>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<reverse_iterator<TypeParam> const&>().operator->()),
      typename reverse_iterator<TypeParam>::pointer>();
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator++ 's return type
/// is @c arene::base::reverse_iterator<Iterator>& .
TYPED_TEST_P(ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest, PreIncrement) {
  ::testing::StaticAssertTypeEq<decltype(++std::declval<reverse_iterator<TypeParam>>()), reverse_iterator<TypeParam>&>(
  );
}
/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator++(int) 's return
/// type is @c arene::base::reverse_iterator<Iterator>
TYPED_TEST_P(ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest, PostIncrement) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<reverse_iterator<TypeParam>>()++), reverse_iterator<TypeParam>>();
}
/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator-- 's return type
/// is @c arene::base::reverse_iterator<Iterator>&
TYPED_TEST_P(ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest, PreDecrement) {
  ::testing::StaticAssertTypeEq<decltype(--std::declval<reverse_iterator<TypeParam>>()), reverse_iterator<TypeParam>&>(
  );
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator--(int) 's return
/// type is @c arene::base::reverse_iterator<Iterator>
TYPED_TEST_P(ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest, PostDecrement) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<reverse_iterator<TypeParam>>()--), reverse_iterator<TypeParam>>();
}

REGISTER_TYPED_TEST_SUITE_P(
    ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest,
    Dereference,
    Arrow,
    PostIncrement,
    PreIncrement,
    PostDecrement,
    PreDecrement
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    Bidirectional,
    ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest,
    bidirectional_iterators,
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    RandomAccess,
    ReverseIteratorBidirectionalOperatorReturnTypeEquivalenceTest,
    random_access_iterators,
);

template <typename T>
class ReverseIteratorRandomAccessOperatorReturnTypeEquivalenceTest : public ::testing::Test {};

TYPED_TEST_SUITE(ReverseIteratorRandomAccessOperatorReturnTypeEquivalenceTest, random_access_iterators, );
/// @test Given at a random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator[] 's return type
/// is @c arene::base::reverse_iterator<Iterator>::reference
TYPED_TEST(ReverseIteratorRandomAccessOperatorReturnTypeEquivalenceTest, Index) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<reverse_iterator<TypeParam>&>()[1]),
      typename reverse_iterator<TypeParam>::reference>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<reverse_iterator<TypeParam> const&>()[1]),
      typename reverse_iterator<TypeParam>::reference>();
}
/// @test Given at a random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator+ 's return type
/// is @c arene::base::reverse_iterator<Iterator>
TYPED_TEST(ReverseIteratorRandomAccessOperatorReturnTypeEquivalenceTest, Addition) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<reverse_iterator<TypeParam>>() + 1), reverse_iterator<TypeParam>>(
  );
  ::testing::StaticAssertTypeEq<decltype(1 + std::declval<reverse_iterator<TypeParam>>()), reverse_iterator<TypeParam>>(
  );
}
/// @test Given at a random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator+(difference_type) 's
/// return type is @c arene::base::reverse_iterator<Iterator>
TYPED_TEST(ReverseIteratorRandomAccessOperatorReturnTypeEquivalenceTest, UnarySubtraction) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<reverse_iterator<TypeParam>>() - 1), reverse_iterator<TypeParam>>(
  );
}
/// @test Given at a random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator+(itr,itr) 's
/// return type is @c arene::base::reverse_iterator<Iterator>::difference_type
TYPED_TEST(ReverseIteratorRandomAccessOperatorReturnTypeEquivalenceTest, BinarySubtraction) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<reverse_iterator<TypeParam>>() - std::declval<reverse_iterator<TypeParam>>()),
      typename reverse_iterator<TypeParam>::difference_type>();
}

/// A container-like object that provides the normal iterator APIs backed by some actual container-like object.
template <typename Container>
class test_container : private iterator_tests::backing_data<Container> {
 public:
  using iterator_tests::backing_data<Container>::begin;
  using iterator_tests::backing_data<Container>::end;
  using iterator_tests::backing_data<Container>::ssize;
  constexpr auto cbegin() const { return std::cbegin(this->container_); }
  constexpr auto cend() const { return std::cend(this->container_); }
  constexpr auto size() const { return ::arene::base::distance(cbegin(), cend()); }
};

template <typename T>
class ReverseIteratorStdReverseIteratorEquivalenceTest : public ::testing::Test {
 public:
  test_container<T> container;
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  using std_riterator = std::reverse_iterator<iterator_type<test_container<T>>>;
  using const_std_riterator = std::reverse_iterator<const_iterator_type<test_container<T>>>;
#endif
  using riterator = reverse_iterator<iterator_type<test_container<T>>>;
  using const_riterator = reverse_iterator<const_iterator_type<test_container<T>>>;
};

template <typename T>
using ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest =
    ReverseIteratorStdReverseIteratorEquivalenceTest<T>;

TYPED_TEST_SUITE_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest);

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator> may be default
/// constructed.
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, CanBeDefaultConstructed) {
  EXPECT_NO_FATAL_FAILURE(std::ignore = typename TestFixture::riterator{});
  EXPECT_NO_FATAL_FAILURE(std::ignore = typename TestFixture::const_riterator{});
}

/// @test Given at least a bidirectional iterator, default-constructed @c arene::base::reverse_iterator<Iterator> 's are
/// equivalent
TYPED_TEST_P(
    ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest,
    DefaultConstructedIteratorsAreEquivalent
) {
  EXPECT_EQ(typename TestFixture::riterator{}, typename TestFixture::riterator{});
  EXPECT_EQ(typename TestFixture::const_riterator{}, typename TestFixture::const_riterator{});
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::base() returns a copy
/// @c of the underlying iterator.
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, BaseReturnsUnderlyingIterator) {
  decltype(auto) raw_itr = this->container.end();
  auto const itr = typename TestFixture::riterator{raw_itr};
  EXPECT_EQ(itr.base(), raw_itr);
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator> may be constructed from a
/// @c std::reverse_iterator<Iterator>
TYPED_TEST_P(
    ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest,
    CanBeConstructedFromStdReverseIterator
) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    STATIC_ASSERT_TRUE(std::is_convertible<typename TestFixture::std_riterator, typename TestFixture::riterator>::value
    );
    for (decltype(auto) raw_itr = this->container.end(); raw_itr != this->container.begin(); --raw_itr) {
      auto const std_itr = typename TestFixture::std_riterator{raw_itr};
      auto const itr = typename TestFixture::riterator{std_itr};
      EXPECT_EQ(itr.base(), raw_itr);
    }
  }
  {
    SCOPED_TRACE("from const iterator");
    STATIC_ASSERT_TRUE(
        std::is_convertible<typename TestFixture::const_std_riterator, typename TestFixture::const_riterator>::value
    );
    for (decltype(auto) raw_itr = this->container.cend(); raw_itr != this->container.cbegin(); --raw_itr) {
      auto const std_itr = typename TestFixture::const_std_riterator{raw_itr};
      auto const itr = typename TestFixture::const_riterator{std_itr};
      EXPECT_EQ(itr.base(), raw_itr);
    }
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c std::reverse_iterator<Iterator> may be converted to a
/// @c arene::base::reverse_iterator<Iterator>
TYPED_TEST_P(
    ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest,
    CanBeConvertedToStdReverseIterator
) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    STATIC_ASSERT_TRUE(std::is_convertible<typename TestFixture::riterator, typename TestFixture::std_riterator>::value
    );
    for (decltype(auto) raw_itr = this->container.end(); raw_itr != this->container.begin(); --raw_itr) {
      auto const itr = typename TestFixture::riterator{raw_itr};
      auto const std_itr = typename TestFixture::std_riterator{itr};
      EXPECT_EQ(std_itr.base(), raw_itr);
    }
  }
  {
    SCOPED_TRACE("from const iterator");
    STATIC_ASSERT_TRUE(
        std::is_convertible<typename TestFixture::const_riterator, typename TestFixture::const_std_riterator>::value
    );
    for (decltype(auto) raw_itr = this->container.cend(); raw_itr != this->container.cbegin(); --raw_itr) {
      auto const itr = typename TestFixture::const_riterator{raw_itr};
      auto const std_itr = typename TestFixture::const_std_riterator{itr};
      EXPECT_EQ(std_itr.base(), raw_itr);
    }
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator* is equivalent
/// to @c std::reverse_iterator<Iterator>::operator* .
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, Dereference) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    for (decltype(auto) raw_itr = this->container.end(); raw_itr != this->container.begin(); --raw_itr) {
      auto const itr = typename TestFixture::riterator{raw_itr};
      auto const std_itr = typename TestFixture::std_riterator{raw_itr};
      EXPECT_EQ(*itr, *std_itr);
    }
  }
  {
    SCOPED_TRACE("from const iterator");
    for (decltype(auto) raw_itr = this->container.cend(); raw_itr != this->container.cbegin(); --raw_itr) {
      auto const itr = typename TestFixture::const_riterator{raw_itr};
      auto const std_itr = typename TestFixture::const_std_riterator{raw_itr};
      EXPECT_EQ(*itr, *std_itr);
    }
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator-> is equivalent
/// to @c std::reverse_iterator<Iterator>::operator-> .
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, Arrow) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    for (decltype(auto) raw_itr = this->container.end(); raw_itr != this->container.begin(); --raw_itr) {
      auto const itr = typename TestFixture::riterator{raw_itr};
      auto const std_itr = typename TestFixture::std_riterator{raw_itr};
      EXPECT_EQ(itr.operator->(), std_itr.operator->());
    }
  }
  {
    SCOPED_TRACE("from const iterator");
    for (decltype(auto) raw_itr = this->container.cend(); raw_itr != this->container.cbegin(); --raw_itr) {
      auto const itr = typename TestFixture::const_riterator{raw_itr};
      auto const std_itr = typename TestFixture::const_std_riterator{raw_itr};
      EXPECT_EQ(itr.operator->(), std_itr.operator->());
    }
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator++ is equivalent
/// to @c std::reverse_iterator<Iterator>::operator++ .
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, PreIncrement) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  decltype(auto) raw_itr = this->container.end();
  auto itr = typename TestFixture::riterator{raw_itr};
  auto std_itr = typename TestFixture::std_riterator{raw_itr};
  EXPECT_EQ((++itr).base(), (++std_itr).base());
  EXPECT_EQ(itr.base(), std_itr.base());
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator++(int) is
/// equivalent to @c std::reverse_iterator<Iterator>::operator++(int) .
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, PostIncrement) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  decltype(auto) raw_itr = this->container.end();
  auto itr = typename TestFixture::riterator{raw_itr};
  auto std_itr = typename TestFixture::std_riterator{raw_itr};
  EXPECT_EQ((itr++).base(), (std_itr++).base());
  EXPECT_EQ(itr.base(), std_itr.base());
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator-- is equivalent
/// to @c std::reverse_iterator<Iterator>::operator-- .
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, PreDecrement) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  decltype(auto) raw_itr = this->container.begin();
  auto itr = typename TestFixture::riterator{raw_itr};
  auto std_itr = typename TestFixture::std_riterator{raw_itr};
  EXPECT_EQ((--itr).base(), (--std_itr).base());
  EXPECT_EQ(itr.base(), std_itr.base());
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator--(int) is
/// equivalent to @c std::reverse_iterator<Iterator>::operator--(int) .
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, PostDecrement) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  decltype(auto) raw_itr = this->container.begin();
  auto itr = typename TestFixture::riterator{raw_itr};
  auto std_itr = typename TestFixture::std_riterator{raw_itr};
  EXPECT_EQ((itr--).base(), (std_itr--).base());
  EXPECT_EQ(itr.base(), std_itr.base());
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given at least a bidirectional iterator, @c arene::base::reverse_iterator<Iterator>::operator==/!= is
/// equivalent to @c std::reverse_iterator<Iterator>::operator==/!= .
TYPED_TEST_P(ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest, Comparison) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    for (decltype(auto) raw_itr = std::prev(this->container.end()); raw_itr != std::next(this->container.begin());
         --raw_itr) {
      auto const itr = typename TestFixture::riterator{raw_itr};
      auto const itr_greater = std::next(itr);
      auto const itr_less = std::prev(itr);
      auto const std_itr = typename TestFixture::std_riterator{raw_itr};
      auto const std_itr_greater = std::next(std_itr);
      auto const std_itr_less = std::prev(std_itr);
      {
        SCOPED_TRACE("equality");
        EXPECT_EQ((itr == itr), (std_itr == std_itr));
        EXPECT_EQ((itr == itr_greater), (std_itr == std_itr_greater));
        EXPECT_EQ((itr == itr_less), (std_itr == std_itr_less));
      }
      {
        SCOPED_TRACE("inequality");
        EXPECT_EQ((itr != itr), (std_itr != std_itr));
        EXPECT_EQ((itr != itr_greater), (std_itr != std_itr_greater));
        EXPECT_EQ((itr != itr_less), (std_itr != std_itr_less));
      }
    }
  }
  {
    SCOPED_TRACE("from const iterator");
    for (decltype(auto) raw_itr = std::prev(this->container.end()); raw_itr != std::next(this->container.begin());
         --raw_itr) {
      auto const itr = typename TestFixture::const_riterator{raw_itr};
      auto const itr_greater = std::next(itr);
      auto const itr_less = std::prev(itr);
      auto const std_itr = typename TestFixture::const_std_riterator{raw_itr};
      auto const std_itr_greater = std::next(std_itr);
      auto const std_itr_less = std::prev(std_itr);
      {
        SCOPED_TRACE("equality");
        EXPECT_EQ((itr == itr), (std_itr == std_itr));
        EXPECT_EQ((itr == itr_greater), (std_itr == std_itr_greater));
        EXPECT_EQ((itr == itr_less), (std_itr == std_itr_less));
      }
      {
        SCOPED_TRACE("inequality");
        EXPECT_EQ((itr != itr), (std_itr != std_itr));
        EXPECT_EQ((itr != itr_greater), (std_itr != std_itr_greater));
        EXPECT_EQ((itr != itr_less), (std_itr != std_itr_less));
      }
    }
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

REGISTER_TYPED_TEST_SUITE_P(
    ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest,
    CanBeDefaultConstructed,
    DefaultConstructedIteratorsAreEquivalent,
    BaseReturnsUnderlyingIterator,
    CanBeConstructedFromStdReverseIterator,
    CanBeConvertedToStdReverseIterator,
    Dereference,
    Arrow,
    PostIncrement,
    PreIncrement,
    PostDecrement,
    PreDecrement,
    Comparison
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    Bidirectional,
    ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest,
    iterator_tests::bidirectional_iterator_containers,
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    RandomAccess,
    ReverseIteratorStdReverseIteratorBidirectionalOperatorEquivalenceTest,
    iterator_tests::random_access_iterator_containers,
);

template <typename T>
using ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest =
    ReverseIteratorStdReverseIteratorEquivalenceTest<T>;

TYPED_TEST_SUITE(
    ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest,
    iterator_tests::random_access_iterator_containers,
);
/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator[] is equivalent
/// to @c std::reverse_iterator<Iterator>::operator[] .
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, Index) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    decltype(auto) raw_itr = this->container.end();

    auto const itr = typename TestFixture::riterator{raw_itr};
    auto const std_itr = typename TestFixture::std_riterator{raw_itr};
    EXPECT_EQ(itr[0], std_itr[0]);
    EXPECT_EQ(itr[this->container.size() - 1], std_itr[this->container.size() - 1]);
    EXPECT_EQ(itr[this->container.size() / 2], std_itr[this->container.size() / 2]);
  }
  {
    SCOPED_TRACE("from const iterator");
    decltype(auto) raw_itr = this->container.cend();

    auto const itr = typename TestFixture::const_riterator{raw_itr};
    auto const std_itr = typename TestFixture::const_std_riterator{raw_itr};
    EXPECT_EQ(itr[0], std_itr[0]);
    EXPECT_EQ(itr[this->container.size() - 1], std_itr[this->container.size() - 1]);
    EXPECT_EQ(itr[this->container.size() / 2], std_itr[this->container.size() / 2]);
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator+(difference_type) is
/// equivalent to @c std::reverse_iterator<Iterator>::operator+(difference_type) .
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, AdditionWithDelta) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  decltype(auto) raw_itr = this->container.end();
  auto const itr = typename TestFixture::riterator{raw_itr};
  auto const std_itr = typename TestFixture::std_riterator{raw_itr};
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::riterator>::difference_type delta = 0;
       delta <= container_size;
       ++delta) {
    EXPECT_EQ((itr + delta).base(), (std_itr + delta).base());
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator-(difference_type) is
/// equivalent to @c std::reverse_iterator<Iterator>::operator-(difference_type) .
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, SubtractionWithDelta) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  decltype(auto) raw_itr = this->container.end();
  auto const itr = typename TestFixture::riterator{raw_itr};
  auto const std_itr = typename TestFixture::std_riterator{raw_itr};
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::riterator>::difference_type count = 0;
       count <= container_size;
       ++count) {
    auto const delta = container_size - count;
    EXPECT_EQ((itr - delta).base(), (std_itr - delta).base());
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator-(itr,itr) is
/// equivalent to @c std::reverse_iterator<Iterator>::operator-(itr,itr) .
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, SubtractionWithIterator) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  decltype(auto) raw_itr = this->container.end();
  auto const itr = typename TestFixture::riterator{raw_itr};
  auto const std_itr = typename TestFixture::std_riterator{raw_itr};
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::riterator>::difference_type offset = 0;
       offset <= container_size;
       ++offset) {
    auto const further_std_itr = std_itr + offset;
    auto const further_itr = itr + offset;
    EXPECT_EQ(further_itr - itr, further_std_itr - std_itr);
    EXPECT_EQ(itr - further_itr, std_itr - further_std_itr);
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator+=(difference_type) is
/// equivalent to @c std::reverse_iterator<Iterator>::operator+=(difference_type) .
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, IncrementAssign) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::riterator>::difference_type delta = 0;
       delta <= container_size;
       ++delta) {
    decltype(auto) raw_itr = this->container.end();
    auto itr = typename TestFixture::riterator{raw_itr};
    auto std_itr = typename TestFixture::std_riterator{raw_itr};
    EXPECT_EQ((itr += delta).base(), (std_itr += delta).base());
    EXPECT_EQ(itr.base(), std_itr.base());
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator>::operator-=(difference_type) is
/// equivalent to @c std::reverse_iterator<Iterator>::operator-=(difference_type) .
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, DecrementAssign) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  auto const container_size = this->container.size();
  for (typename std::iterator_traits<typename TestFixture::riterator>::difference_type count = 0;
       count <= container_size;
       ++count) {
    auto const delta = container_size - count;
    decltype(auto) raw_itr = this->container.begin();
    auto itr = typename TestFixture::riterator{raw_itr};
    auto std_itr = typename TestFixture::std_riterator{raw_itr};
    EXPECT_EQ((itr -= delta).base(), (std_itr -= delta).base());
    EXPECT_EQ(itr.base(), std_itr.base());
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator> ' relational comparison operators are
/// equivalent to @c std::reverse_iterator<Iterator> 's .
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, Comparison) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    for (decltype(auto) raw_itr = std::prev(this->container.end()); raw_itr != std::next(this->container.begin());
         --raw_itr) {
      auto const itr = typename TestFixture::riterator{raw_itr};
      auto const itr_greater = std::next(itr);
      auto const itr_less = std::prev(itr);
      auto const std_itr = typename TestFixture::std_riterator{raw_itr};
      auto const std_itr_greater = std::next(std_itr);
      auto const std_itr_less = std::prev(std_itr);
      {
        SCOPED_TRACE("equality");
        EXPECT_EQ((itr == itr), (std_itr == std_itr));
        EXPECT_EQ((itr == itr_greater), (std_itr == std_itr_greater));
        EXPECT_EQ((itr == itr_less), (std_itr == std_itr_less));
      }
      {
        SCOPED_TRACE("inequality");
        EXPECT_EQ((itr != itr), (std_itr != std_itr));
        EXPECT_EQ((itr != itr_greater), (std_itr != std_itr_greater));
        EXPECT_EQ((itr != itr_less), (std_itr != std_itr_less));
      }
      {
        SCOPED_TRACE("less");
        EXPECT_EQ((itr < itr), (std_itr < std_itr));
        EXPECT_EQ((itr < itr_greater), (std_itr < std_itr_greater));
        EXPECT_EQ((itr < itr_less), (std_itr < std_itr_less));
      }
      {
        SCOPED_TRACE("less equal");
        EXPECT_EQ((itr <= itr), (std_itr <= std_itr));
        EXPECT_EQ((itr <= itr_greater), (std_itr <= std_itr_greater));
        EXPECT_EQ((itr <= itr_less), (std_itr <= std_itr_less));
      }
      {
        SCOPED_TRACE("greater");
        EXPECT_EQ((itr > itr), (std_itr > std_itr));
        EXPECT_EQ((itr > itr_greater), (std_itr > std_itr_greater));
        EXPECT_EQ((itr > itr_less), (std_itr > std_itr_less));
      }
      {
        SCOPED_TRACE("greater equal");
        EXPECT_EQ((itr >= itr), (std_itr >= std_itr));
        EXPECT_EQ((itr >= itr_greater), (std_itr >= std_itr_greater));
        EXPECT_EQ((itr >= itr_less), (std_itr >= std_itr_less));
      }
    }
  }
  {
    SCOPED_TRACE("from const iterator");
    for (decltype(auto) raw_itr = std::prev(this->container.end()); raw_itr != std::next(this->container.begin());
         --raw_itr) {
      auto const itr = typename TestFixture::const_riterator{raw_itr};
      auto const itr_greater = std::next(itr);
      auto const itr_less = std::prev(itr);
      auto const std_itr = typename TestFixture::const_std_riterator{raw_itr};
      auto const std_itr_greater = std::next(std_itr);
      auto const std_itr_less = std::prev(std_itr);
      {
        SCOPED_TRACE("equality");
        EXPECT_EQ((itr == itr), (std_itr == std_itr));
        EXPECT_EQ((itr == itr_greater), (std_itr == std_itr_greater));
        EXPECT_EQ((itr == itr_less), (std_itr == std_itr_less));
      }
      {
        SCOPED_TRACE("inequality");
        EXPECT_EQ((itr != itr), (std_itr != std_itr));
        EXPECT_EQ((itr != itr_greater), (std_itr != std_itr_greater));
        EXPECT_EQ((itr != itr_less), (std_itr != std_itr_less));
      }
      {
        SCOPED_TRACE("less");
        EXPECT_EQ((itr < itr), (std_itr < std_itr));
        EXPECT_EQ((itr < itr_greater), (std_itr < std_itr_greater));
        EXPECT_EQ((itr < itr_less), (std_itr < std_itr_less));
      }
      {
        SCOPED_TRACE("less equal");
        EXPECT_EQ((itr <= itr), (std_itr <= std_itr));
        EXPECT_EQ((itr <= itr_greater), (std_itr <= std_itr_greater));
        EXPECT_EQ((itr <= itr_less), (std_itr <= std_itr_less));
      }
      {
        SCOPED_TRACE("greater");
        EXPECT_EQ((itr > itr), (std_itr > std_itr));
        EXPECT_EQ((itr > itr_greater), (std_itr > std_itr_greater));
        EXPECT_EQ((itr > itr_less), (std_itr > std_itr_less));
      }
      {
        SCOPED_TRACE("greater equal");
        EXPECT_EQ((itr >= itr), (std_itr >= std_itr));
        EXPECT_EQ((itr >= itr_greater), (std_itr >= std_itr_greater));
        EXPECT_EQ((itr >= itr_less), (std_itr >= std_itr_less));
      }
    }
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

/// @test Given random_access iterator, @c arene::base::reverse_iterator<Iterator> ' relational comparison operators can
/// be invoked against an instance of @c std::reverse_iterator<Iterator> created from the same underlying iterator.
TYPED_TEST(ReverseIteratorStdReverseIteratorRandomAccessOperatorEquivalenceTest, ComparisonToStdIterator) {
#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
  {
    SCOPED_TRACE("from iterator");
    for (decltype(auto) raw_itr = std::prev(this->container.end()); raw_itr != std::next(this->container.begin());
         --raw_itr) {
      auto const itr = typename TestFixture::riterator{raw_itr};
      auto const itr_greater = std::next(itr);
      auto const itr_less = std::prev(itr);
      auto const std_itr = typename TestFixture::std_riterator{raw_itr};
      auto const std_itr_greater = std::next(std_itr);
      auto const std_itr_less = std::prev(std_itr);
      {
        SCOPED_TRACE("equality");
        EXPECT_EQ((itr == std_itr), (std_itr == std_itr));
        EXPECT_EQ((itr == std_itr_greater), (std_itr == std_itr_greater));
        EXPECT_EQ((itr == std_itr_less), (std_itr == std_itr_less));
        EXPECT_EQ((std_itr == itr), (std_itr == std_itr));
        EXPECT_EQ((std_itr_greater == itr), (std_itr == std_itr_greater));
        EXPECT_EQ((std_itr_less == itr), (std_itr == std_itr_less));
      }
      {
        SCOPED_TRACE("inequality");
        EXPECT_EQ((itr != std_itr), (std_itr != std_itr));
        EXPECT_EQ((itr != std_itr_greater), (std_itr != std_itr_greater));
        EXPECT_EQ((itr != std_itr_less), (std_itr != std_itr_less));
        EXPECT_EQ((std_itr != itr), (std_itr != std_itr));
        EXPECT_EQ((std_itr_greater != itr), (std_itr != std_itr_greater));
        EXPECT_EQ((std_itr_less != itr), (std_itr != std_itr_less));
      }
      {
        SCOPED_TRACE("less");
        EXPECT_EQ((itr < std_itr), (std_itr < std_itr));
        EXPECT_EQ((itr < std_itr_greater), (std_itr < std_itr_greater));
        EXPECT_EQ((itr < std_itr_less), (std_itr < std_itr_less));
        EXPECT_EQ((std_itr < itr), (std_itr < std_itr));
        EXPECT_EQ((std_itr < itr_greater), (std_itr < std_itr_greater));
        EXPECT_EQ((std_itr < itr_less), (std_itr < std_itr_less));
      }
      {
        SCOPED_TRACE("less equal");
        EXPECT_EQ((itr <= std_itr), (std_itr <= std_itr));
        EXPECT_EQ((itr <= std_itr_greater), (std_itr <= std_itr_greater));
        EXPECT_EQ((itr <= std_itr_less), (std_itr <= std_itr_less));
        EXPECT_EQ((std_itr <= itr), (std_itr <= std_itr));
        EXPECT_EQ((std_itr <= itr_greater), (std_itr <= std_itr_greater));
        EXPECT_EQ((std_itr <= itr_less), (std_itr <= std_itr_less));
      }
      {
        SCOPED_TRACE("greater");
        EXPECT_EQ((itr > std_itr), (std_itr > std_itr));
        EXPECT_EQ((itr > std_itr_greater), (std_itr > std_itr_greater));
        EXPECT_EQ((itr > std_itr_less), (std_itr > std_itr_less));
        EXPECT_EQ((std_itr > itr), (std_itr > std_itr));
        EXPECT_EQ((std_itr > itr_greater), (std_itr > std_itr_greater));
        EXPECT_EQ((std_itr > itr_less), (std_itr > std_itr_less));
      }
      {
        SCOPED_TRACE("greater equal");
        EXPECT_EQ((itr >= std_itr), (std_itr >= std_itr));
        EXPECT_EQ((itr >= std_itr_greater), (std_itr >= std_itr_greater));
        EXPECT_EQ((itr >= std_itr_less), (std_itr >= std_itr_less));
        EXPECT_EQ((std_itr >= itr), (std_itr >= std_itr));
        EXPECT_EQ((std_itr >= itr_greater), (std_itr >= std_itr_greater));
        EXPECT_EQ((std_itr >= itr_less), (std_itr >= std_itr_less));
      }
    }
  }
  {
    SCOPED_TRACE("from const iterator");
    for (decltype(auto) raw_itr = std::prev(this->container.end()); raw_itr != std::next(this->container.begin());
         --raw_itr) {
      auto const itr = typename TestFixture::const_riterator{raw_itr};
      auto const itr_greater = std::next(itr);
      auto const itr_less = std::prev(itr);
      auto const std_itr = typename TestFixture::const_std_riterator{raw_itr};
      auto const std_itr_greater = std::next(std_itr);
      auto const std_itr_less = std::prev(std_itr);
      {
        SCOPED_TRACE("equality");
        EXPECT_EQ((itr == std_itr), (std_itr == std_itr));
        EXPECT_EQ((itr == std_itr_greater), (std_itr == std_itr_greater));
        EXPECT_EQ((itr == std_itr_less), (std_itr == std_itr_less));
        EXPECT_EQ((std_itr == itr), (std_itr == std_itr));
        EXPECT_EQ((std_itr_greater == itr), (std_itr == std_itr_greater));
        EXPECT_EQ((std_itr_less == itr), (std_itr == std_itr_less));
      }
      {
        SCOPED_TRACE("inequality");
        EXPECT_EQ((itr != std_itr), (std_itr != std_itr));
        EXPECT_EQ((itr != std_itr_greater), (std_itr != std_itr_greater));
        EXPECT_EQ((itr != std_itr_less), (std_itr != std_itr_less));
        EXPECT_EQ((std_itr != itr), (std_itr != std_itr));
        EXPECT_EQ((std_itr_greater != itr), (std_itr != std_itr_greater));
        EXPECT_EQ((std_itr_less != itr), (std_itr != std_itr_less));
      }
      {
        SCOPED_TRACE("less");
        EXPECT_EQ((itr < std_itr), (std_itr < std_itr));
        EXPECT_EQ((itr < std_itr_greater), (std_itr < std_itr_greater));
        EXPECT_EQ((itr < std_itr_less), (std_itr < std_itr_less));
        EXPECT_EQ((std_itr < itr), (std_itr < std_itr));
        EXPECT_EQ((std_itr < itr_greater), (std_itr < std_itr_greater));
        EXPECT_EQ((std_itr < itr_less), (std_itr < std_itr_less));
      }
      {
        SCOPED_TRACE("less equal");
        EXPECT_EQ((itr <= std_itr), (std_itr <= std_itr));
        EXPECT_EQ((itr <= std_itr_greater), (std_itr <= std_itr_greater));
        EXPECT_EQ((itr <= std_itr_less), (std_itr <= std_itr_less));
        EXPECT_EQ((std_itr <= itr), (std_itr <= std_itr));
        EXPECT_EQ((std_itr <= itr_greater), (std_itr <= std_itr_greater));
        EXPECT_EQ((std_itr <= itr_less), (std_itr <= std_itr_less));
      }
      {
        SCOPED_TRACE("greater");
        EXPECT_EQ((itr > std_itr), (std_itr > std_itr));
        EXPECT_EQ((itr > std_itr_greater), (std_itr > std_itr_greater));
        EXPECT_EQ((itr > std_itr_less), (std_itr > std_itr_less));
        EXPECT_EQ((std_itr > itr), (std_itr > std_itr));
        EXPECT_EQ((std_itr > itr_greater), (std_itr > std_itr_greater));
        EXPECT_EQ((std_itr > itr_less), (std_itr > std_itr_less));
      }
      {
        SCOPED_TRACE("greater equal");
        EXPECT_EQ((itr >= std_itr), (std_itr >= std_itr));
        EXPECT_EQ((itr >= std_itr_greater), (std_itr >= std_itr_greater));
        EXPECT_EQ((itr >= std_itr_less), (std_itr >= std_itr_less));
        EXPECT_EQ((std_itr >= itr), (std_itr >= std_itr));
        EXPECT_EQ((std_itr >= itr_greater), (std_itr >= std_itr_greater));
        EXPECT_EQ((std_itr >= itr_less), (std_itr >= std_itr_less));
      }
    }
  }
#else
  GTEST_SKIP() << "No std::reverse_iterator in Arene Base stdlib";
#endif
}

template <typename T>
class ReverseIteratorOperationEquivalenceConstexprTest : public ::testing::Test {
 public:
  using riterator = reverse_iterator<decltype(std::cbegin(std::declval<T&>()))>;
};

using constexpr_container_types = ::testing::Types<int[10], int const[10], char[10], char const[10]>;

TYPED_TEST_SUITE(ReverseIteratorOperationEquivalenceConstexprTest, constexpr_container_types, );

/// @test Validates @c arene::base::reverse_iterator<Iterator>::base() works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, baseReturnsUnderlyingIterator) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(typename TestFixture::riterator{container.cend()}.base(), container.cend());
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator* works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, Dereference) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(*typename TestFixture::riterator{container.cend()}, *(container.cend() - 1));
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator-> works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, Arrow) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(*(typename TestFixture::riterator{container.cend()}.operator->()), *(container.cend() - 1));
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::[] works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, Index) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(typename TestFixture::riterator{container.cend()}[0], (container.cend() - 1)[0]);
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

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator++ works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, PreIncrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_post_increment(typename TestFixture::riterator{container.cend()}).base(),
      do_post_decrement(container.cend())
  );
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator++(int) works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, PostIncrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_pre_increment(typename TestFixture::riterator{container.cend()}).base(),
      do_pre_decrement(container.cend())
  );
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator-- works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, PreDecrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_post_decrement(typename TestFixture::riterator{container.cbegin() + 1}).base(),
      do_post_increment(container.cbegin() + 1)
  );
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator--(int) works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, PostDecrement) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_pre_decrement(typename TestFixture::riterator{container.cbegin() + 1}).base(),
      do_pre_increment(container.cbegin() + 1)
  );
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator+(difference_type) works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, AdditionWithDelta) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ((typename TestFixture::riterator{container.cend()} + 1).base(), container.cend() - 1);
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator-(difference_type) works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, SubtractionWithDelta) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ((typename TestFixture::riterator{container.cbegin() + 1} - 1).base(), container.cbegin() + 1 + 1);
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator-(itr,itr) works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, SubtractionWithIterator) {
  constexpr test_container<TypeParam> container{};
  constexpr typename TestFixture::riterator::difference_type offset = 1;
  STATIC_ASSERT_EQ(
      typename TestFixture::riterator{container.cend()} - (typename TestFixture::riterator{container.cend()} + offset),
      -offset
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

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator+= works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, IncrementAssign) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_increment_assign(typename TestFixture::riterator{container.cend()}, 1).base(),
      do_decrement_assign(container.cend(), 1)
  );
}

/// @test Validates @c arene::base::reverse_iterator<Iterator>::operator-= works in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, DecrementAssign) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      do_decrement_assign(typename TestFixture::riterator{container.cbegin()}, 1).base(),
      do_increment_assign(container.cbegin(), 1)
  );
}

/// @test Validates @c arene::base::reverse_iterator<Iterator> 's comparison operators work in constexpr.
TYPED_TEST(ReverseIteratorOperationEquivalenceConstexprTest, Comparison) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(
      typename TestFixture::riterator{container.cend()},
      typename TestFixture::riterator{container.cend()}
  );
  STATIC_ASSERT_NE(
      typename TestFixture::riterator{container.cend()},
      typename TestFixture::riterator{container.cend()} + 1
  );
  STATIC_ASSERT_LT(
      typename TestFixture::riterator{container.cend()},
      typename TestFixture::riterator{container.cend()} + 1
  );
  STATIC_ASSERT_LE(
      typename TestFixture::riterator{container.cend()},
      typename TestFixture::riterator{container.cend()} + 1
  );
  STATIC_ASSERT_GT(
      typename TestFixture::riterator{container.cend()} + 1,
      typename TestFixture::riterator{container.cend()}
  );
  STATIC_ASSERT_GE(
      typename TestFixture::riterator{container.cend()} + 1,
      typename TestFixture::riterator{container.cend()}
  );
}

/// @test Given an iterator, @c arene::base::reverse_iterator<Iterator> may be constructed from an iterator convertible
/// to @c Iterator .
TEST(ReverseIteratorConversionConstructionTest, CanBeConstructedIfOtherReverseIteratorHasConvertibleItrType) {
  std::string data = "foo";
  ASSERT_EQ(
      reverse_iterator<std::string::const_iterator>{reverse_iterator<std::string::iterator>(data.end())},
      reverse_iterator<std::string::const_iterator>{data.cend()}
  );
  ASSERT_EQ(
      reverse_iterator<std::string::const_iterator>{reverse_iterator<std::string::iterator>(data.begin())},
      reverse_iterator<std::string::const_iterator>{data.cbegin()}
  );
}

template <typename T>
class MakeReverseIteratorTest : public ::testing::Test {};

TYPED_TEST_SUITE(MakeReverseIteratorTest, constexpr_container_types, );

/// @test Given an input iterator of type @c Iterator , @c arene::base::make_reverse_iterator 's return type is
/// @c arene::base::reverse_iterator<Iterator>
TYPED_TEST(MakeReverseIteratorTest, TypeIsReversedIteratorToInputType) {
  constexpr test_container<TypeParam> container{};
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::make_reverse_iterator(container.cbegin())),
      reverse_iterator<decltype(container.cbegin())>>();
}

/// @test Given an input iterator @c arene::base::make_reverse_iterator 's return value's @c base() is equal to the
/// input iterator .
TYPED_TEST(MakeReverseIteratorTest, ReturnsReverseIteratorConstructedFromInputIterator) {
  constexpr test_container<TypeParam> container{};
  STATIC_ASSERT_EQ(::arene::base::make_reverse_iterator(container.cbegin()).base(), container.cbegin());
  STATIC_ASSERT_EQ(::arene::base::make_reverse_iterator(container.cend()).base(), container.cend());
}

// NOLINTEND(hicpp-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic)

}  // namespace
