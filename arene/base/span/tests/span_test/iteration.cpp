// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file iteration.cpp
/// @brief Unit tests for span's iterator methods .
///

#include <algorithm>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/iterator.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
using Iteration = span_test::SpanTest<T>;

TYPED_TEST_SUITE_P(Iteration);
/// @test Validates that @c span<T,Extent>::begin() 's return type is @c span<T,Extent>::iterator independent of the
/// @c const qualification of the span .
TYPED_TEST_P(Iteration, BeginReturnTypeIsIteratorNoMatterConstQualification) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().begin()), typename TypeParam::iterator>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().begin()), typename TypeParam::iterator>();
}
/// @test Validates that @c span<T,Extent>::begin() is @c noexcept
TYPED_TEST_P(Iteration, BeginIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().begin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().begin()));
}
/// @test Validates that @c span<T,Extent>::end() 's return type is @c span<T,Extent>::iterator independent of the
/// @c const qualification of the span .
TYPED_TEST_P(Iteration, EndReturnTypeIsIteratorNoMatterConstQualification) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().end()), typename TypeParam::iterator>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().end()), typename TypeParam::iterator>();
}
/// @test Validates that @c span<T,Extent>::end() is @c noexcept
TYPED_TEST_P(Iteration, EndIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().end()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().end()));
}
/// @test Validates that @c span<T,Extent>::cbegin() 's return type is @c span<T,Extent>::const_iterator
TYPED_TEST_P(Iteration, CBeginReturnTypeIsConstIterator) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().cbegin()), typename TypeParam::const_iterator>();
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().cbegin()), typename TypeParam::const_iterator>();
}
/// @test Validates that @c span<T,Extent>::cbegin() is @c noexcept
TYPED_TEST_P(Iteration, CBeginIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().cbegin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().cbegin()));
}
/// @test Validates that @c span<T,Extent>::cend() 's return type is @c span<T,Extent>::const_iterator
TYPED_TEST_P(Iteration, CEndReturnTypeIsConstIterator) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().cend()), typename TypeParam::const_iterator>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().cend()), typename TypeParam::const_iterator>(
  );
}
/// @test Validates that @c span<T,Extent>::cend() is @c noexcept
TYPED_TEST_P(Iteration, CEndIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().cend()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().cend()));
}
/// @test Validates that @c span<T,Extent>::rbegin() 's return type is @c span<T,Extent>::reverse_iterator
TYPED_TEST_P(Iteration, RBeginReturnTypeIsReverseIteratorNoMatterConstQualification) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().rbegin()), typename TypeParam::reverse_iterator>();
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().rbegin()), typename TypeParam::reverse_iterator>();
}
/// @test Validates that @c span<T,Extent>::rbegin() is @c noexcept
TYPED_TEST_P(Iteration, RBeginIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().rbegin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().rbegin()));
}
/// @test Validates that @c span<T,Extent>::rend() 's return type is @c span<T,Extent>::reverse_iterator
TYPED_TEST_P(Iteration, REndReturnTypeIsReverseIteratorNoMatterConstQualification) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().rend()), typename TypeParam::reverse_iterator>();
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().rend()), typename TypeParam::reverse_iterator>();
}
/// @test Validates that @c span<T,Extent>::rend() is @c noexcept
TYPED_TEST_P(Iteration, REndIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().rend()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().rend()));
}
/// @test Validates that @c span<T,Extent>::crbegin() 's return type is @c span<T,Extent>::const_reverse_iterator
TYPED_TEST_P(Iteration, CRBeginReturnTypeIsConstReverseIterator) {
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<TypeParam&>().crbegin()), typename TypeParam::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().crbegin()),
      typename TypeParam::const_reverse_iterator>();
}
/// @test Validates that @c span<T,Extent>::crbegin() is @c noexcept
TYPED_TEST_P(Iteration, CRBeginIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().crbegin()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().crbegin()));
}
/// @test Validates that @c span<T,Extent>::crend() 's return type is @c span<T,Extent>::const_reverse_iterator
TYPED_TEST_P(Iteration, CREndReturnTypeIsConstReverseIterator) {
  ::testing::
      StaticAssertTypeEq<decltype(std::declval<TypeParam&>().crend()), typename TypeParam::const_reverse_iterator>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().crend()),
      typename TypeParam::const_reverse_iterator>();
}
/// @test Validates that @c span<T,Extent>::crend() is @c noexcept
TYPED_TEST_P(Iteration, CREndIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().crend()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().crend()));
}
/// @test Validates that @c distance(span.begin(),span.end()) is equal to @c span.ssize()
TYPED_TEST_P(Iteration, TheDistanceFromBeginToEndIsSSize) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_EQ(::arene::base::distance(the_span.begin(), the_span.end()), the_span.ssize());
  EXPECT_EQ(::arene::base::distance(the_span.end(), the_span.begin()), -the_span.ssize());
}
/// @test Validates that @c distance(span.cbegin(),span.cend()) is equal to @c span.ssize()
TYPED_TEST_P(Iteration, TheDistanceFromCBeginToCEndIsSSize) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_EQ(::arene::base::distance(the_span.cbegin(), the_span.cend()), the_span.ssize());
  EXPECT_EQ(::arene::base::distance(the_span.cend(), the_span.cbegin()), -the_span.ssize());
}
/// @test Validates that @c distance(span.rbegin(),span.rend()) is equal to @c span.ssize()
TYPED_TEST_P(Iteration, TheDistanceFromRBeginToEndIsSSize) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_EQ(::arene::base::distance(the_span.rbegin(), the_span.rend()), the_span.ssize());
  EXPECT_EQ(::arene::base::distance(the_span.rend(), the_span.rbegin()), -the_span.ssize());
}
/// @test Validates that @c distance(span.crbegin(),span.crend()) is equal to @c span.ssize()
TYPED_TEST_P(Iteration, TheDistanceFromCRBeginToCEndIsSSize) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_EQ(::arene::base::distance(the_span.crbegin(), the_span.crend()), the_span.ssize());
  EXPECT_EQ(::arene::base::distance(the_span.crend(), the_span.crbegin()), -the_span.ssize());
}
/// @test Validates that iterating from @c span.begin() to @c span.end() produces an iterator that points to each
/// element in the sequence the span is a view onto, in order.
TYPED_TEST_P(Iteration, IteratingFromBeginToEndIsEquivalentToIteratingAcrossBackingDataForEquivalentDistance) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  std::vector<typename TypeParam::pointer> expected_element_addresses;
  for (auto current = this->backing_data.begin();
       current != ::arene::base::next(this->backing_data.begin(), the_span.ssize());
       ++current) {
    expected_element_addresses.push_back(&*current);
  }
  std::vector<typename TypeParam::pointer> iterated_element_addresses;
  for (auto current = the_span.begin(); current != the_span.end(); ++current) {
    iterated_element_addresses.push_back(&*current);
  }
  EXPECT_THAT(iterated_element_addresses, ::testing::ElementsAreArray(expected_element_addresses));
}
/// @test Validates that iterating from @c span.cbegin() to @c span.cend() produces an iterator that points to each
/// element in the sequence the span is a view onto, in order.
TYPED_TEST_P(Iteration, IteratingFromCBeginToCEndIsEquivalentToIteratingAcrossBackingDataForEquivalentDistance) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  std::vector<typename TypeParam::const_pointer> expected_element_addresses;
  for (auto current = this->backing_data.begin();
       current != ::arene::base::next(this->backing_data.begin(), the_span.ssize());
       ++current) {
    expected_element_addresses.push_back(&*current);
  }
  std::vector<typename TypeParam::const_pointer> iterated_element_addresses;
  for (auto current = the_span.cbegin(); current != the_span.cend(); ++current) {
    iterated_element_addresses.push_back(&*current);
  }
  EXPECT_THAT(iterated_element_addresses, ::testing::ElementsAreArray(expected_element_addresses));
}
/// @test Validates that iterating from @c span.rbegin() to @c span.rend() produces an iterator that points to each
/// element in the sequence the span is a view onto, in reverse order.
TYPED_TEST_P(Iteration, IteratingFromRBeginToREndIsEquivalentToReverseIteratingAcrossBackingDataForEquivalentDistance) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  std::vector<typename TypeParam::pointer> expected_element_addresses;
  for (auto current = this->backing_data.begin();
       current != ::arene::base::next(this->backing_data.begin(), the_span.ssize());
       ++current) {
    expected_element_addresses.push_back(&*current);
  }
  std::vector<typename TypeParam::pointer> iterated_element_addresses;
  for (auto current = the_span.rbegin(); current != the_span.rend(); ++current) {
    iterated_element_addresses.push_back(&*current);
  }
  std::reverse(expected_element_addresses.begin(), expected_element_addresses.end());
  EXPECT_THAT(iterated_element_addresses, ::testing::ElementsAreArray(expected_element_addresses));
}
/// @test Validates that iterating from @c span.crbegin() to @c span.crend() produces an iterator that points to each
/// element in the sequence the span is a view onto, in reverse order.
TYPED_TEST_P(
    Iteration,
    IteratingFromCRBeginToCREndIsEquivalentToReverseIteratingAcrossBackingDataForEquivalentDistance
) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  std::vector<typename TypeParam::const_pointer> expected_element_addresses;
  for (auto current = this->backing_data.begin();
       current != ::arene::base::next(this->backing_data.begin(), the_span.ssize());
       ++current) {
    expected_element_addresses.push_back(&*current);
  }
  std::vector<typename TypeParam::const_pointer> iterated_element_addresses;
  for (auto current = the_span.crbegin(); current != the_span.crend(); ++current) {
    iterated_element_addresses.push_back(&*current);
  }
  std::reverse(expected_element_addresses.begin(), expected_element_addresses.end());
  EXPECT_THAT(iterated_element_addresses, ::testing::ElementsAreArray(expected_element_addresses));
}

REGISTER_TYPED_TEST_SUITE_P(
    Iteration,
    BeginReturnTypeIsIteratorNoMatterConstQualification,
    BeginIsNoexcept,
    EndReturnTypeIsIteratorNoMatterConstQualification,
    EndIsNoexcept,
    CBeginReturnTypeIsConstIterator,
    CBeginIsNoexcept,
    CEndReturnTypeIsConstIterator,
    CEndIsNoexcept,
    RBeginReturnTypeIsReverseIteratorNoMatterConstQualification,
    RBeginIsNoexcept,
    REndReturnTypeIsReverseIteratorNoMatterConstQualification,
    REndIsNoexcept,
    CRBeginReturnTypeIsConstReverseIterator,
    CRBeginIsNoexcept,
    CREndReturnTypeIsConstReverseIterator,
    CREndIsNoexcept,
    TheDistanceFromBeginToEndIsSSize,
    TheDistanceFromCBeginToCEndIsSSize,
    TheDistanceFromRBeginToEndIsSSize,
    TheDistanceFromCRBeginToCEndIsSSize,
    IteratingFromBeginToEndIsEquivalentToIteratingAcrossBackingDataForEquivalentDistance,
    IteratingFromCBeginToCEndIsEquivalentToIteratingAcrossBackingDataForEquivalentDistance,
    IteratingFromRBeginToREndIsEquivalentToReverseIteratingAcrossBackingDataForEquivalentDistance,
    IteratingFromCRBeginToCREndIsEquivalentToReverseIteratingAcrossBackingDataForEquivalentDistance

);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, Iteration, span_test::all_spans, );

}  // namespace
