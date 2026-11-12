// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/iter_value_t.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/indirectly_readable_traits.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"

struct explicitly_specialized_indirectly_readable_type {
  struct value {};
};

template <>
class arene::base::indirectly_readable_traits<explicitly_specialized_indirectly_readable_type> {
 public:
  using value_type = explicitly_specialized_indirectly_readable_type::value;
};

namespace {

using ::arene::base::iter_value_t;
namespace iterator_tests = ::arene::base::iterator_tests;
namespace type_lists = ::arene::base::type_lists;
using ::arene::base::indirectly_readable_traits;

using standard_iterator_types =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, iterator_tests::iterator_t>;

template <typename T>
class IterValueTStandardIteratorsTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterValueTStandardIteratorsTest, standard_iterator_types, );

/// @test Given a standard iterator `Itr`, when `iter_value_t<Itr>` is instantiated, then it is the iterator's value
/// type as defined by `indirectly_readable_traits`.
TYPED_TEST(IterValueTStandardIteratorsTest, MatchesIndirectlyReadableTraitsValueType) {
  ::testing::StaticAssertTypeEq<iter_value_t<TypeParam>, typename indirectly_readable_traits<TypeParam>::value_type>();
}

/// @test Given a const-qualified standard iterator, when `iter_value_t<Itr const>` is instantiated, then it is the
/// iterator's value type as defined by `indirectly_readable_traits`.
TYPED_TEST(IterValueTStandardIteratorsTest, MatchesConstQualifiedIteratorValueType) {
  ::testing::
      StaticAssertTypeEq<iter_value_t<TypeParam const>, typename indirectly_readable_traits<TypeParam>::value_type>();
}

/// @test Given a type with an explicit specialization of `indirectly_readable_traits`, when `iter_value_t` is
/// instantiated with cvref-qualified input, then it is the specialized `value_type`.
TEST(IterValueTExplicitlySpecializedTypeTest, UsesExplicitIndirectlyReadableTraitsSpecialization) {
  using specialized_type = explicitly_specialized_indirectly_readable_type;
  using expected_value_type = specialized_type::value;
  ::testing::StaticAssertTypeEq<iter_value_t<specialized_type>, expected_value_type>();
  ::testing::StaticAssertTypeEq<iter_value_t<specialized_type const&>, expected_value_type>();
  ::testing::StaticAssertTypeEq<iter_value_t<specialized_type&&>, expected_value_type>();
}

struct value_type_only {
  using value_type = value_type_only;
};

struct element_type_only {
  using element_type = element_type_only;
};

struct value_type_and_element_type {
  using value_type = value_type_and_element_type;
  using element_type = value_type_and_element_type;
};

using default_indirectly_readable_traits_types =
    ::testing::Types<value_type_only, element_type_only, value_type_and_element_type>;

template <typename T>
class IterValueTDefaultIndirectlyReadableTraitsTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterValueTDefaultIndirectlyReadableTraitsTest, default_indirectly_readable_traits_types, );

/// @test Given a type `T` which does not explicitly specialize `indirectly_readable_traits` but for which the
/// default implementation of `indirectly_readable_traits` provides `value_type`, then `iter_value_t<T>` is
/// `indirectly_readable_traits<T>::value_type`.
TYPED_TEST(IterValueTDefaultIndirectlyReadableTraitsTest, MatchesDefaultIndirectlyReadableTraitsValueType) {
  ::testing::StaticAssertTypeEq<iter_value_t<TypeParam>, typename indirectly_readable_traits<TypeParam>::value_type>();
}

struct no_value_or_element_type_iterator {};

struct mismatched_value_and_element_type {
  using value_type = mismatched_value_and_element_type;
  using element_type = double;
};

template <typename Itr>
using use_iter_value_t = iter_value_t<Itr>;

using invalid_iterator_types =
    ::testing::Types<void, int, no_value_or_element_type_iterator, mismatched_value_and_element_type>;

template <typename T>
class IterValueTInvalidTypesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterValueTInvalidTypesTest, invalid_iterator_types, );

/// @test Given a type without an explicit specialization of `indirectly_readable_traits` and for which the default
/// specialization of `indirectly_readable_traits` does not provide a `value_type`, when `iter_value_t` is instantiated,
/// then the alias is ill-formed.
TYPED_TEST(IterValueTInvalidTypesTest, HasNoValueType) {
  STATIC_ASSERT_FALSE((::arene::base::substitution_succeeds<use_iter_value_t, TypeParam>));
}

}  // namespace
