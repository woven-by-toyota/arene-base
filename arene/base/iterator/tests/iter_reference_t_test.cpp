// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/iter_reference_t.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"

namespace {

using ::arene::base::iter_reference_t;
namespace iterator_tests = ::arene::base::iterator_tests;
namespace type_lists = ::arene::base::type_lists;

using standard_iterator_types =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, iterator_tests::iterator_t>;

template <typename T>
class IterReferenceTStandardIteratorsTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterReferenceTStandardIteratorsTest, standard_iterator_types, );

/// @test Given a standard iterator `Itr`, when `iter_reference_t<Itr>` is instantiated, then it is the result of
/// dereferencing `Itr`.
TYPED_TEST(IterReferenceTStandardIteratorsTest, IsDereferenceResult) {
  using iterator = TypeParam;
  ::testing::StaticAssertTypeEq<iter_reference_t<iterator>, decltype(*std::declval<iterator&>())>();
}

struct rvalue_dereference_iterator {
  auto operator*() -> int&&;
};

struct proxy_dereference_iterator {
  struct proxy {};

  auto operator*() -> proxy;
};

struct const_dereference_iterator {
  auto operator*() -> int&;
  auto operator*() const -> int const&;
};

using custom_iterator_types = ::testing::Types<rvalue_dereference_iterator, proxy_dereference_iterator>;

template <typename T>
class IterReferenceTCustomTypesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterReferenceTCustomTypesTest, custom_iterator_types, );

/// @test Given a dereferenceable type, when `iter_reference_t` is instantiated, then it preserves the exact
/// dereference result type.
TYPED_TEST(IterReferenceTCustomTypesTest, PreservesDereferenceResult) {
  ::testing::StaticAssertTypeEq<iter_reference_t<TypeParam>, decltype(*std::declval<TypeParam&>())>();
}

/// @test Given an iterator with const and non-const dereference overloads, when `iter_reference_t` is instantiated
/// with a const-qualified iterator, then it uses the const dereference result.
TEST(IterReferenceTCustomTypesTest, UsesConstDereferenceResult) {
  using iterator = const_dereference_iterator;
  ::testing::StaticAssertTypeEq<iter_reference_t<iterator const>, int const&>();
}

struct rvalue_only_dereference_iterator {
  auto operator*() && -> int&&;
};

template <typename Itr>
using use_iter_reference_t = iter_reference_t<Itr>;

/// @test Given an iterator with only an rvalue-qualified dereference operator, when `iter_reference_t` is
/// instantiated, then the alias is ill-formed because the iterator is used as an lvalue.
TEST(IterReferenceTInvalidCustomTypesTest, RejectsRvalueQualifiedDereference) {
  STATIC_ASSERT_FALSE((::arene::base::substitution_succeeds<use_iter_reference_t, rvalue_only_dereference_iterator>));
}

template <typename T>
class IterReferenceTInvalidTypesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterReferenceTInvalidTypesTest, iterator_tests::types_which_do_not_satisfy_is_dereferenceable, );

/// @test Given a type that cannot be dereferenced, when `iter_reference_t` is instantiated, then the alias is
/// ill-formed.
TYPED_TEST(IterReferenceTInvalidTypesTest, IsIllFormed) {
  STATIC_ASSERT_FALSE((::arene::base::substitution_succeeds<use_iter_reference_t, TypeParam>));
}

}  // namespace
