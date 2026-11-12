// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/iter_const_reference_t.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/indirectly_readable_traits.hpp"
#include "arene/base/iterator/iter_reference_t.hpp"
#include "arene/base/iterator/iter_value_t.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/add_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/add_rvalue_reference.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_traits/common_reference.hpp"

namespace {
struct no_common_reference_iterator;
struct no_value_type_iterator;

using ::arene::base::common_reference_t;
using ::arene::base::iter_const_reference_t;
using ::arene::base::iter_reference_t;
using ::arene::base::iter_value_t;
namespace iterator_tests = ::arene::base::iterator_tests;
namespace type_lists = ::arene::base::type_lists;

using standard_iterator_types =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, iterator_tests::iterator_t>;
using const_standard_iterator_types = type_lists::apply_each_t<standard_iterator_types, std::add_const_t>;
using lvalue_standard_iterator_types = type_lists::apply_each_t<standard_iterator_types, std::add_lvalue_reference_t>;
using rvalue_standard_iterator_types = type_lists::apply_each_t<standard_iterator_types, std::add_rvalue_reference_t>;
using all_standard_iterator_types = type_lists::concat_unique_t<
    standard_iterator_types,
    const_standard_iterator_types,
    lvalue_standard_iterator_types,
    rvalue_standard_iterator_types>;

template <typename T>
class IterConstReferenceTCommonReferenceTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(IterConstReferenceTCommonReferenceTest);

/// @test Given an indirectly readable iterator `Itr`, when `iter_const_reference_t<Itr>` is instantiated, then it is
/// the common reference of `iter_value_t<Itr> const&&` and `iter_reference_t<Itr>`.
TYPED_TEST_P(IterConstReferenceTCommonReferenceTest, IsCommonReferenceOfValueAndReference) {
  ::testing::StaticAssertTypeEq<
      iter_const_reference_t<TypeParam>,
      common_reference_t<iter_value_t<TypeParam> const&&, iter_reference_t<TypeParam>>>();
}

REGISTER_TYPED_TEST_SUITE_P(IterConstReferenceTCommonReferenceTest, IsCommonReferenceOfValueAndReference);

INSTANTIATE_TYPED_TEST_SUITE_P(
    StandardIterators,
    IterConstReferenceTCommonReferenceTest,
    all_standard_iterator_types,
);

struct rvalue_dereference_iterator {
  using value_type = int;

  auto operator*() const -> int&&;
};

struct value_dereference_iterator {
  using value_type = int;

  auto operator*() const -> int;
};

using non_lvalue_dereference_iterator_types = ::testing::Types<rvalue_dereference_iterator, value_dereference_iterator>;

INSTANTIATE_TYPED_TEST_SUITE_P(
    NonLvalueDereferenceIterators,
    IterConstReferenceTCommonReferenceTest,
    non_lvalue_dereference_iterator_types,
);

template <typename Itr>
using use_iter_const_reference_t = iter_const_reference_t<Itr>;

template <typename T>
class IterConstReferenceTInvalidTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(IterConstReferenceTInvalidTest);

/// @test Given an iterator type `Itr` that is not indirectly readable, when `iter_const_reference_t<Itr>` is
/// substituted, then substitution fails because the alias is constrained to indirectly readable types.
TYPED_TEST_P(IterConstReferenceTInvalidTest, IsIllFormed) {
  STATIC_ASSERT_FALSE((::arene::base::substitution_succeeds<use_iter_const_reference_t, TypeParam>));
}

REGISTER_TYPED_TEST_SUITE_P(IterConstReferenceTInvalidTest, IsIllFormed);

struct no_value_type_iterator {
  auto operator*() const -> int&;
};

using invalid_iterator_types = type_lists::concat_unique_t<
    iterator_tests::types_which_do_not_satisfy_is_dereferenceable,
    ::testing::Types<no_value_type_iterator>>;

INSTANTIATE_TYPED_TEST_SUITE_P(InvalidIterators, IterConstReferenceTInvalidTest, invalid_iterator_types, );

struct no_common_reference_iterator {
  using value_type = int;

  struct reference {};

  auto operator*() const -> reference;
};

using no_common_reference_iterator_types = ::testing::Types<no_common_reference_iterator>;

INSTANTIATE_TYPED_TEST_SUITE_P(NoCommonReference, IterConstReferenceTInvalidTest, no_common_reference_iterator_types, );
}  // namespace
