// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file static_properties.cpp
/// @brief Unit tests for span's static member definitions, ex @c span<T,Extent>::value_type .
///

#include <gtest/gtest.h>

#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/add_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/add_pointer.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
using StaticProperties = span_test::SpanTest<T>;

TYPED_TEST_SUITE_P(StaticProperties);

/// @test Validates that @c span<T,Extent>::extent is @c Extent .
TYPED_TEST_P(StaticProperties, ExtentIsTParamExtent) {
  STATIC_ASSERT_EQ(TypeParam::extent, TestFixture::template_extent());
  STATIC_ASSERT_EQ(TypeParam::extent(), TestFixture::template_extent());
}
/// @test Validates that @c span<T,Extent>::element_type is @c T .
TYPED_TEST_P(StaticProperties, ElementTypeIsTemplateType) {
  ::testing::StaticAssertTypeEq<typename TypeParam::element_type, typename TestFixture::template_type>();
}
/// @test Validates that @c span<T,Extent>::value_type is @c remove_cv_ref<T> .
TYPED_TEST_P(StaticProperties, ValueTypeIsCVRefRemovedTemplateType) {
  ::testing::StaticAssertTypeEq<
      typename TypeParam::value_type,
      ::arene::base::remove_cvref_t<typename TestFixture::template_type>>();
}
/// @test Validates that @c span<T,Extent>::size_type is @c std::size_t .
TYPED_TEST_P(StaticProperties, SizeTypeIsStdSizeT) {
  ::testing::StaticAssertTypeEq<typename TypeParam::size_type, std::size_t>();
}
/// @test Validates that @c span<T,Extent>::pointer is @c T* .
TYPED_TEST_P(StaticProperties, PointerIsPointerToTemplateType) {
  ::testing::StaticAssertTypeEq<typename TypeParam::pointer, std::add_pointer_t<typename TestFixture::template_type>>();
}
/// @test Validates that @c span<T,Extent>::const_pointer is <c>T const*</c>.
TYPED_TEST_P(StaticProperties, ConstPointerIsPointerToTemplateType) {
  ::testing::StaticAssertTypeEq<
      typename TypeParam::const_pointer,
      std::add_pointer_t<typename TestFixture::template_type const>>();
}
/// @test Validates that @c span<T,Extent>::reference is @c T& .
TYPED_TEST_P(StaticProperties, ReferenceIsReferenceToTemplateType) {
  ::testing::StaticAssertTypeEq<
      typename TypeParam::reference,
      std::add_lvalue_reference_t<typename TestFixture::template_type>>();
}
/// @test Validates that @c span<T,Extent>::const_reference is <c>T const&</c> .
TYPED_TEST_P(StaticProperties, ConstReferenceIsConstReferenceToTemplateType) {
  ::testing::StaticAssertTypeEq<
      typename TypeParam::const_reference,
      std::add_lvalue_reference_t<typename TestFixture::template_type const>>();
}
/// @test Validates that @c span<T,Extent>::iterator 's @c iterator_category is @c std::random_access_iterator_tag ,
/// that its @c value_type is @c span<T,Extent>::value_type , and that its @c reference is @c span<T,Extent>::reference.
TYPED_TEST_P(StaticProperties, IteratorIsARandomAccessIteratorToTemplateType) {
  using iterator_traits = std::iterator_traits<typename TypeParam::iterator>;
  ::testing::StaticAssertTypeEq<typename iterator_traits::iterator_category, std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::value_type, typename TypeParam::value_type>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::reference, typename TypeParam::reference>();
}
/// @test Validates that @c span<T,Extent>::const_iterator 's @c iterator_category is @c std::random_access_iterator_tag
/// , that its @c value_type is @c span<T,Extent>::value_type , and that its @c reference is
/// @c span<T,Extent>::const_reference.
TYPED_TEST_P(StaticProperties, ConstIteratorIsARandomAccessIteratorToConstTemplateType) {
  using iterator_traits = std::iterator_traits<typename TypeParam::const_iterator>;
  ::testing::StaticAssertTypeEq<typename iterator_traits::iterator_category, std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::value_type, typename TypeParam::value_type>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::reference, typename TypeParam::const_reference>();
}
/// @test Validates that @c span<T,Extent>::reverse_iterator 's @c iterator_category is
/// @c std::random_access_iterator_tag , that its @c value_type is @c span<T,Extent>::value_type , and that its
/// @c reference is @c span<T,Extent>::reference.
TYPED_TEST_P(StaticProperties, ReverseIteratorIsARandomAccessIteratorToTemplateType) {
  using iterator_traits = std::iterator_traits<typename TypeParam::reverse_iterator>;
  ::testing::StaticAssertTypeEq<typename iterator_traits::iterator_category, std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::value_type, typename TypeParam::value_type>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::reference, typename TypeParam::reference>();
}
/// @test Validates that @c span<T,Extent>::const_reverse_iterator 's @c iterator_category is
/// @c std::random_access_iterator_tag , that its @c value_type is @c span<T,Extent>::value_type , and that its
/// @c reference is @c span<T,Extent>::const_reference.
TYPED_TEST_P(StaticProperties, ConstReverseIteratorIsARandomAccessIteratorToConstTemplateType) {
  using iterator_traits = std::iterator_traits<typename TypeParam::const_reverse_iterator>;
  ::testing::StaticAssertTypeEq<typename iterator_traits::iterator_category, std::random_access_iterator_tag>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::value_type, typename TypeParam::value_type>();
  ::testing::StaticAssertTypeEq<typename iterator_traits::reference, typename TypeParam::const_reference>();
}

REGISTER_TYPED_TEST_SUITE_P(
    StaticProperties,
    ExtentIsTParamExtent,
    ElementTypeIsTemplateType,
    ValueTypeIsCVRefRemovedTemplateType,
    SizeTypeIsStdSizeT,
    PointerIsPointerToTemplateType,
    ConstPointerIsPointerToTemplateType,
    ReferenceIsReferenceToTemplateType,
    ConstReferenceIsConstReferenceToTemplateType,
    IteratorIsARandomAccessIteratorToTemplateType,
    ConstIteratorIsARandomAccessIteratorToConstTemplateType,
    ReverseIteratorIsARandomAccessIteratorToTemplateType,
    ConstReverseIteratorIsARandomAccessIteratorToConstTemplateType
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, StaticProperties, span_test::all_spans, );

}  // namespace
