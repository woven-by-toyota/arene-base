// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/incrementable_traits.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"

namespace {
struct difference_type_itr;
struct non_incrementable_type;
struct only_subtractable_iterator;
struct specialized_incrementable_iterator;

using ::arene::base::incrementable_traits;

template <typename T>
using use_incrementable_traits_difference_type = typename incrementable_traits<T>::difference_type;

template <typename T>
constexpr bool has_incrementable_traits_difference_type =
    ::arene::base::substitution_succeeds<use_incrementable_traits_difference_type, T>;

struct non_incrementable_type {};
using non_incrementable_types = ::testing::Types<
    void,
    void*,
    void const*,
    non_incrementable_type,
    non_incrementable_type const,
    int&,
    int&&,
    int const&,
    int const&&>;

/// @test Given a type which has no member typedef `difference_type` and no subtraction operator `T`, when
/// `incrementable_traits<T>` is instantiated, then it has no member typedef `difference_type`.
TEST(IncrementableTraitsInvalidType, ATypeWithNoRequiredFacilitiesHasNoDifferenceType) {
  STATIC_ASSERT_FALSE(has_incrementable_traits_difference_type<non_incrementable_type>);
  STATIC_ASSERT_FALSE(has_incrementable_traits_difference_type<non_incrementable_type const>);
}

using pointer_types = ::testing::Types<int*, non_incrementable_type*, int const*, non_incrementable_type const*>;
using const_pointer_types = ::arene::base::type_lists::apply_each_t<pointer_types, std::add_const_t>;
using all_pointer_types = ::arene::base::type_lists::concat_t<pointer_types, const_pointer_types>;

template <typename Itr>
class IncrementableTraitsPointerTypesTypeTest : public ::testing::Test {};

TYPED_TEST_SUITE(IncrementableTraitsPointerTypesTypeTest, all_pointer_types, );

/// @test Given a pointer type `T*` where `T` satisfies `std::is_object<T>`, when `incrementable_traits<T*>` is
/// instantiated, then it has a member typedef `difference_type`.
TYPED_TEST(IncrementableTraitsPointerTypesTypeTest, HasDifferenceType) {
  STATIC_ASSERT_TRUE(has_incrementable_traits_difference_type<TypeParam>);
}

/// @test Given a pointer type `T*` where `T` satisfies `std::is_object<T>`, when `incrementable_traits<T*>` is
/// instantiated, then its member typedef `difference_type` is `std::ptrdiff_t`.
TYPED_TEST(IncrementableTraitsPointerTypesTypeTest, DifferenceTypeIsPtrdiffT) {
  ::testing::StaticAssertTypeEq<typename incrementable_traits<TypeParam>::difference_type, std::ptrdiff_t>();
}

struct only_subtractable_iterator {};

ARENE_MAYBE_UNUSED auto operator-(only_subtractable_iterator const&, only_subtractable_iterator const&) -> int {
  return 0;
}

using subtraction_only_iterator_types = ::testing::Types<int, only_subtractable_iterator>;
using const_subtraction_only_iterator_types =
    ::arene::base::type_lists::apply_each_t<subtraction_only_iterator_types, std::add_const_t>;

using all_subtraction_only_iterator_types =
    ::arene::base::type_lists::concat_t<subtraction_only_iterator_types, const_subtraction_only_iterator_types>;

template <typename Itr>
class IncrementableTraitsSubtractionOnlyTypeTest : public ::testing::Test {};

TYPED_TEST_SUITE(IncrementableTraitsSubtractionOnlyTypeTest, all_subtraction_only_iterator_types, );

/// @test Given a subtraction-only iterator type, when `incrementable_traits` is instantiated, then it has a member
/// typedef `difference_type`.
TYPED_TEST(IncrementableTraitsSubtractionOnlyTypeTest, HasDifferenceType) {
  STATIC_ASSERT_TRUE(has_incrementable_traits_difference_type<TypeParam>);
}

/// @test Given a subtraction-only iterator type, when `incrementable_traits` is instantiated, then its member typedef
/// `difference_type` is `decltype(std::declval<TypeParam>() - std::declval<TypeParam>())`.
TYPED_TEST(IncrementableTraitsSubtractionOnlyTypeTest, DifferenceTypeIsSubtractionResult) {
  ::testing::StaticAssertTypeEq<
      typename incrementable_traits<TypeParam>::difference_type,
      decltype(std::declval<TypeParam>() - std::declval<TypeParam>())>();
}

struct difference_type_itr {
  using difference_type = int;
};

using difference_type_iterator_types = ::testing::Types<difference_type_itr, difference_type_itr const>;

template <typename Itr>
class IncrementableTraitsStandardIteratorTypeTest : public ::testing::Test {};

TYPED_TEST_SUITE(IncrementableTraitsStandardIteratorTypeTest, difference_type_iterator_types, );

/// @test Given an iterator with a public member typedef `difference_type`, when `incrementable_traits` is
/// instantiated, then it has a member typedef `difference_type`.
TYPED_TEST(IncrementableTraitsStandardIteratorTypeTest, HasDifferenceType) {
  STATIC_ASSERT_TRUE(has_incrementable_traits_difference_type<TypeParam>);
}

/// @test Given an iterator with a public member typedef `difference_type`, then its member typedef
/// `difference_type` is `difference_type`.
TYPED_TEST(IncrementableTraitsStandardIteratorTypeTest, DifferenceTypeIsSubtractionResult) {
  ::testing::StaticAssertTypeEq<
      typename incrementable_traits<TypeParam>::difference_type,
      typename TypeParam::difference_type>();
}

struct specialized_incrementable_iterator {};

}  // namespace

template <>
struct arene::base::incrementable_traits<specialized_incrementable_iterator> {
  using difference_type = std::int64_t;
};

namespace {

/// @test Given an iterator with a public member typedef `difference_type`, when `incrementable_traits` is
/// instantiated, then it has a member typedef `difference_type`.
TEST(IncrementableTraitsSpecializedIteratorTypeTest, HasDifferenceType) {
  STATIC_ASSERT_TRUE(has_incrementable_traits_difference_type<specialized_incrementable_iterator>);
}

/// @test Given an iterator with a specialization of `incrementable_traits`, then its member typedef
/// `difference_type` is the one specified in the specialization of `incrementable_traits`.
TEST(IncrementableTraitsSpecializedIteratorTypeTest, DifferenceTypeIsSubtractionResult) {
  ::testing::StaticAssertTypeEq<
      typename incrementable_traits<specialized_incrementable_iterator>::difference_type,
      typename ::arene::base::incrementable_traits<specialized_incrementable_iterator>::difference_type>();
}

}  // namespace
