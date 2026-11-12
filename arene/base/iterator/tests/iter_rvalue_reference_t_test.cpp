// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/iter_rvalue_reference_t.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/iter_move.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"

namespace {

using ::arene::base::iter_rvalue_reference_t;
namespace iterator_tests = ::arene::base::iterator_tests;
namespace type_lists = ::arene::base::type_lists;

using standard_iterator_types =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, iterator_tests::iterator_t>;

template <typename T>
class IterRvalueReferenceTStandardIteratorsTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterRvalueReferenceTStandardIteratorsTest, standard_iterator_types, );

/// @test Given a standard iterator `Itr`, when `iter_rvalue_reference_t<Itr>` is instantiated, then it is the result
/// of `ranges::iter_move` applied to `Itr`.
TYPED_TEST(IterRvalueReferenceTStandardIteratorsTest, IsIterMoveResult) {
  using iterator = TypeParam;
  ::testing::StaticAssertTypeEq<
      iter_rvalue_reference_t<iterator>,
      decltype(::arene::base::ranges::iter_move(std::declval<iterator&>()))>();
}

namespace adl {
struct specialized_iterator {
  std::int32_t value;
  std::int16_t moved_value;

  auto operator*() -> std::int32_t& { return value; }
};

auto iter_move(specialized_iterator& itr) -> std::int16_t& { return itr.moved_value; }

struct invalidly_specialized_iterator {
  auto operator*() -> std::int32_t& { return value; }
  std::int32_t value;
};
ARENE_MAYBE_UNUSED auto iter_move(invalidly_specialized_iterator&) -> void {}

}  // namespace adl

struct lvalue_dereference_iterator {
  auto operator*() -> std::int32_t&;
};

struct rvalue_dereference_iterator {
  auto operator*() -> std::int32_t&&;
};

struct const_dereference_iterator {
  auto operator*() -> std::int32_t&;
  auto operator*() const -> std::int32_t const&;
};

struct value_dereference_iterator {
  auto operator*() -> std::int32_t;
};

struct rvalue_only_dereference_iterator {
  auto operator*() && -> std::int32_t&&;
};

/// @test Given an iterator with an ADL `iter_move` customization, when `iter_rvalue_reference_t` is instantiated,
/// then it is the customized result type.
TEST(IterRvalueReferenceTCustomTypesTest, UsesAdlIterMoveResult) {
  ::testing::StaticAssertTypeEq<iter_rvalue_reference_t<adl::specialized_iterator>, std::int16_t&>();
  adl::specialized_iterator itr{42, 7};
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  EXPECT_EQ(&moved, &itr.moved_value);
}

/// @test Given an iterator whose dereference result is an lvalue reference, when `iter_rvalue_reference_t` is
/// instantiated, then it is the moved result type.
TEST(IterRvalueReferenceTCustomTypesTest, MovesLvalueDereferenceResult) {
  ::testing::StaticAssertTypeEq<iter_rvalue_reference_t<lvalue_dereference_iterator>, std::int32_t&&>();
}

/// @test Given an iterator whose dereference result is an rvalue reference, when `iter_rvalue_reference_t` is
/// instantiated, then it preserves the dereference result type.
TEST(IterRvalueReferenceTCustomTypesTest, PreservesRvalueDereferenceResult) {
  ::testing::StaticAssertTypeEq<iter_rvalue_reference_t<rvalue_dereference_iterator>, std::int32_t&&>();
}

/// @test Given an iterator with const and non-const dereference overloads, when `iter_rvalue_reference_t` is
/// instantiated with a const-qualified iterator, then it uses the const dereference result.
TEST(IterRvalueReferenceTCustomTypesTest, UsesConstDereferenceResult) {
  using iterator = const_dereference_iterator;
  ::testing::StaticAssertTypeEq<iter_rvalue_reference_t<iterator const>, std::int32_t const&&>();
}

/// @test Given an iterator whose dereference result is a value, when `iter_rvalue_reference_t` is instantiated, then
/// it preserves the value result type.
TEST(IterRvalueReferenceTCustomTypesTest, PreservesValueDereferenceResult) {
  ::testing::StaticAssertTypeEq<iter_rvalue_reference_t<value_dereference_iterator>, std::int32_t>();
}

template <typename Itr>
using use_iter_rvalue_reference_t = iter_rvalue_reference_t<Itr>;

using invalid_iterator_types = type_lists::concat_t<
    iterator_tests::types_which_do_not_satisfy_is_dereferenceable,
    ::testing::Types<adl::invalidly_specialized_iterator, rvalue_only_dereference_iterator>>;

template <typename T>
class IterRvalueReferenceTInvalidTypesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterRvalueReferenceTInvalidTypesTest, invalid_iterator_types, );

/// @test Given a type which does not satisfy `is_dereferenceable_v<Itr>`, or which does but which has an `iter_move`
/// specialization whose return type does not satisfy `can_reference_v`, when `iter_rvalue_reference_t` is instantiated,
/// then the alias is ill-formed.
TYPED_TEST(IterRvalueReferenceTInvalidTypesTest, IsIllFormed) {
  STATIC_ASSERT_FALSE((::arene::base::substitution_succeeds<use_iter_rvalue_reference_t, TypeParam>));
}

}  // namespace
