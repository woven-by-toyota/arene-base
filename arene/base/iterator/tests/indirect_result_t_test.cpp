// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/indirect_result_t.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/iterator/iter_reference_t.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using ::arene::base::indirect_result_t;
using ::arene::base::invoke_result_t;
using ::arene::base::iter_reference_t;
using ::arene::base::substitution_succeeds;
namespace iterator_tests = ::arene::base::iterator_tests;
namespace type_lists = ::arene::base::type_lists;

using standard_iterator_types =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, iterator_tests::iterator_t>;

template <typename T>
class IndirectResultTStandardIteratorTest : public ::testing::Test {};

TYPED_TEST_SUITE(IndirectResultTStandardIteratorTest, standard_iterator_types, );

struct forwarding_callable {
  template <typename Arg>
  auto operator()(Arg&&) const -> Arg&&;
};

/// @test Given a standard iterator and a forwarding callable, when `indirect_result_t` is instantiated, then it is
/// exactly `invoke_result_t<F, iter_reference_t<Is>...>`.
TYPED_TEST(IndirectResultTStandardIteratorTest, MatchesInvokeResultForForwardingCallable) {
  ::testing::StaticAssertTypeEq<
      indirect_result_t<forwarding_callable, TypeParam>,
      invoke_result_t<forwarding_callable, iter_reference_t<TypeParam>>>();
}

struct lvalue_callable_result {};
struct const_lvalue_callable_result {};
struct rvalue_callable_result {};
struct const_rvalue_callable_result {};

struct qualification_sensitive_callable {
  template <typename Arg>
  auto operator()(Arg&&) & -> lvalue_callable_result;

  template <typename Arg>
  auto operator()(Arg&&) const& -> const_lvalue_callable_result;

  template <typename Arg>
  auto operator()(Arg&&) && -> rvalue_callable_result;

  template <typename Arg>
  auto operator()(Arg&&) const&& -> const_rvalue_callable_result;
};

/// @test Given a standard iterator and a callable with cvref-qualified overloads, when `indirect_result_t` is
/// instantiated with each callable qualification, then each result is exactly `invoke_result_t<F,
/// iter_reference_t<Is>...>` for the corresponding qualification.
TYPED_TEST(IndirectResultTStandardIteratorTest, PreservesCallableCvrefQualification) {
  ::testing::StaticAssertTypeEq<
      indirect_result_t<qualification_sensitive_callable&, TypeParam>,
      invoke_result_t<qualification_sensitive_callable&, iter_reference_t<TypeParam>>>();
  ::testing::StaticAssertTypeEq<
      indirect_result_t<qualification_sensitive_callable const&, TypeParam>,
      invoke_result_t<qualification_sensitive_callable const&, iter_reference_t<TypeParam>>>();
  ::testing::StaticAssertTypeEq<
      indirect_result_t<qualification_sensitive_callable&&, TypeParam>,
      invoke_result_t<qualification_sensitive_callable&&, iter_reference_t<TypeParam>>>();
  ::testing::StaticAssertTypeEq<
      indirect_result_t<qualification_sensitive_callable const&&, TypeParam>,
      invoke_result_t<qualification_sensitive_callable const&&, iter_reference_t<TypeParam>>>();
}

struct value_result {};

struct value_callable {
  template <typename Arg>
  auto operator()(Arg&&) const -> value_result;
};

/// @test Given a standard iterator and a value-returning callable, when `indirect_result_t` is instantiated, then it
/// is exactly `invoke_result_t<F, iter_reference_t<Is>...>`.
TYPED_TEST(IndirectResultTStandardIteratorTest, MatchesInvokeResultForValueCallable) {
  ::testing::StaticAssertTypeEq<
      indirect_result_t<value_callable, TypeParam>,
      invoke_result_t<value_callable, iter_reference_t<TypeParam>>>();
}

struct void_callable {
  template <typename Arg>
  auto operator()(Arg&&) const -> void;
};

/// @test Given a standard iterator and a void-returning callable, when `indirect_result_t` is instantiated, then it
/// is exactly `invoke_result_t<F, iter_reference_t<Is>...>`.
TYPED_TEST(IndirectResultTStandardIteratorTest, MatchesInvokeResultForVoidCallable) {
  ::testing::StaticAssertTypeEq<
      indirect_result_t<void_callable, TypeParam>,
      invoke_result_t<void_callable, iter_reference_t<TypeParam>>>();
}

template <typename First, typename Second>
struct binary_result {};

struct binary_callable {
  template <typename First, typename Second>
  auto operator()(First&&, Second&&) const -> binary_result<First, Second>;
};

/// @test Given two indirectly readable iterator types and a binary callable, when `indirect_result_t` is instantiated,
/// then it is exactly `invoke_result_t<F, iter_reference_t<Is>...>` in argument order.
TEST(IndirectResultTMultipleIteratorsTest, MatchesInvokeResultForBinaryCallable) {
  using first_iterator = int*;
  using second_iterator = int const*;
  ::testing::StaticAssertTypeEq<
      indirect_result_t<binary_callable, first_iterator, second_iterator>,
      invoke_result_t<binary_callable, iter_reference_t<first_iterator>, iter_reference_t<second_iterator>>>();
}

struct nullary_result {};

struct nullary_callable {
  auto operator()() const -> nullary_result;
};

/// @test Given a nullary callable and no iterator types, when `indirect_result_t` is instantiated, then it is exactly
/// `invoke_result_t<F>`.
TEST(IndirectResultTNoIteratorsTest, MatchesInvokeResultForNullaryCallable) {
  ::testing::StaticAssertTypeEq<indirect_result_t<nullary_callable>, invoke_result_t<nullary_callable>>();
}

template <typename T>
class IndirectResultTInvalidIteratorTest : public ::testing::Test {};

TYPED_TEST_SUITE(IndirectResultTInvalidIteratorTest, iterator_tests::types_which_do_not_satisfy_is_dereferenceable, );

/// @test Given an iterator that is not indirectly readable, when `indirect_result_t` is instantiated, then substitution
/// fails.
TYPED_TEST(IndirectResultTInvalidIteratorTest, RejectsNonIndirectlyReadableIterator) {
  STATIC_ASSERT_FALSE((substitution_succeeds<indirect_result_t, forwarding_callable, TypeParam>));
}

/// @test Given a pack containing one valid and one invalid iterator, when `indirect_result_t` is instantiated, then
/// substitution fails.
TYPED_TEST(IndirectResultTInvalidIteratorTest, RejectsInvalidIteratorInAnyPosition) {
  STATIC_ASSERT_FALSE((substitution_succeeds<indirect_result_t, binary_callable, int*, TypeParam>));
}

struct dereferenceable_without_value_type_iterator {
  auto operator*() const -> int&;
};

/// @test Given a dereferenceable iterator without a `value_type`, when `indirect_result_t` is instantiated, then
/// substitution fails because the iterator is not indirectly readable.
TEST(IndirectResultTInvalidIteratorTest, RejectsDereferenceableNonIndirectlyReadableIterator) {
  STATIC_ASSERT_TRUE((substitution_succeeds<iter_reference_t, dereferenceable_without_value_type_iterator>));
  STATIC_ASSERT_FALSE(
      (substitution_succeeds<indirect_result_t, forwarding_callable, dereferenceable_without_value_type_iterator>)
  );
}

struct non_invocable_callable {};

/// @test Given an indirectly readable iterator and a non-invocable callable, when `indirect_result_t` is instantiated,
/// then substitution fails.
TEST(IndirectResultTInvalidCallableTest, RejectsNonInvocableCallable) {
  STATIC_ASSERT_FALSE((substitution_succeeds<indirect_result_t, non_invocable_callable, int*>));
}

/// @test Given a callable whose arity does not match the iterator pack, when `indirect_result_t` is instantiated, then
/// substitution fails.
TEST(IndirectResultTInvalidCallableTest, RejectsCallableWithWrongArity) {
  STATIC_ASSERT_FALSE((substitution_succeeds<indirect_result_t, forwarding_callable, int*, int const*>));
  STATIC_ASSERT_FALSE((substitution_succeeds<indirect_result_t, binary_callable, int*>));
}

}  // namespace
