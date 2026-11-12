// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/is_indirectly_readable.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/add_lvalue_reference.hpp"
#include "arene/base/stdlib_choice/add_rvalue_reference.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace adl {

struct no_referenceable_iter_move_iterator {
  using value_type = int;

  auto operator*() const -> int&;
};

ARENE_MAYBE_UNUSED auto iter_move(no_referenceable_iter_move_iterator const&) -> void;

struct dereference_result_mismatch_iterator {
  using value_type = int;

  auto operator*() -> int&;
  auto operator*() const -> int const&;
};

ARENE_MAYBE_UNUSED auto iter_move(dereference_result_mismatch_iterator const&) -> int&&;

struct iter_move_result_mismatch_iterator {
  using value_type = int;

  auto operator*() const -> int&;
};

ARENE_MAYBE_UNUSED auto iter_move(iter_move_result_mismatch_iterator&) -> int&&;
ARENE_MAYBE_UNUSED auto iter_move(iter_move_result_mismatch_iterator const&) -> int const&&;

template <typename Value, typename Reference, typename RvalueReference>
struct iterator_with_specified_types {
  using value_type = Value;

  auto operator*() const -> Reference;
};

template <typename Value, typename Reference, typename RvalueReference>
auto iter_move(iterator_with_specified_types<Value, Reference, RvalueReference> const&) -> RvalueReference;

}  // namespace adl

namespace {

using ::arene::base::is_indirectly_readable;
using ::arene::base::is_indirectly_readable_v;
namespace iterator_tests = ::arene::base::iterator_tests;
namespace type_lists = ::arene::base::type_lists;

using standard_iterator_types =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, iterator_tests::iterator_t>;
using const_standard_iterator_types = type_lists::apply_each_t<standard_iterator_types, std::add_const_t>;
using lvalue_standard_iterator_types = type_lists::apply_each_t<standard_iterator_types, std::add_lvalue_reference_t>;
using const_lvalue_standard_iterator_types =
    type_lists::apply_each_t<const_standard_iterator_types, std::add_lvalue_reference_t>;
using rvalue_standard_iterator_types = type_lists::apply_each_t<standard_iterator_types, std::add_rvalue_reference_t>;
using const_rvalue_standard_iterator_types =
    type_lists::apply_each_t<const_standard_iterator_types, std::add_rvalue_reference_t>;
using indirectly_readable_iterator_types = type_lists::concat_unique_t<
    standard_iterator_types,
    const_standard_iterator_types,
    lvalue_standard_iterator_types,
    const_lvalue_standard_iterator_types,
    rvalue_standard_iterator_types,
    const_rvalue_standard_iterator_types>;

template <typename T>
class IndirectlyReadableStandardIteratorTest : public ::testing::Test {};

TYPED_TEST_SUITE(IndirectlyReadableStandardIteratorTest, indirectly_readable_iterator_types, );

/// @test Given a standard iterator with any cvref qualification, when `is_indirectly_readable_v` is queried, then it
/// is true.
TYPED_TEST(IndirectlyReadableStandardIteratorTest, IsTrue) { STATIC_ASSERT_TRUE(is_indirectly_readable_v<TypeParam>); }

namespace invalid_iterator_types {

struct no_value_type_iterator {
  auto operator*() const -> int&;
};

struct no_reference_type_iterator {
  using value_type = int;
};

struct non_const_dereferenceable_iterator {
  using value_type = int;

  auto operator*() -> int&;
};

struct unrelated_value_type {};
struct unrelated_reference_type {};
struct unrelated_rvalue_reference_type {};

using missing_associated_type_iterators = type_lists::concat_unique_t<
    iterator_tests::types_which_do_not_satisfy_is_dereferenceable,
    ::testing::Types<no_value_type_iterator, no_reference_type_iterator>>;
using invalid_iter_move_iterators = ::testing::Types<adl::no_referenceable_iter_move_iterator>;
using mismatched_result_iterators =
    ::testing::Types<adl::dereference_result_mismatch_iterator, adl::iter_move_result_mismatch_iterator>;
using no_common_reference_iterators = ::testing::Types<
    adl::
        iterator_with_specified_types<unrelated_value_type, unrelated_reference_type, unrelated_rvalue_reference_type>>;
using non_indirectly_readable_iterator_types = type_lists::concat_unique_t<
    missing_associated_type_iterators,
    invalid_iter_move_iterators,
    mismatched_result_iterators,
    no_common_reference_iterators,
    ::testing::Types<non_const_dereferenceable_iterator>>;

}  // namespace invalid_iterator_types

template <typename T>
class NonIndirectlyReadableIteratorTest : public ::testing::Test {};

TYPED_TEST_SUITE(NonIndirectlyReadableIteratorTest, invalid_iterator_types::non_indirectly_readable_iterator_types, );

/// @test Given an iterator that fails at least one indirectly-readable constraint, when `is_indirectly_readable_v` is
/// queried, then it is false.
TYPED_TEST(NonIndirectlyReadableIteratorTest, IsFalse) { STATIC_ASSERT_FALSE(is_indirectly_readable_v<TypeParam>); }

using all_iterator_types = type_lists::
    concat_unique_t<indirectly_readable_iterator_types, invalid_iterator_types::non_indirectly_readable_iterator_types>;

template <typename T>
class IndirectlyReadableVTest : public ::testing::Test {};

TYPED_TEST_SUITE(IndirectlyReadableVTest, all_iterator_types, );

/// @test Given any iterator type `Itr`, when `is_indirectly_readable_v<Itr>` is evaluated, then it equals
/// `is_indirectly_readable<Itr>::value`.
TYPED_TEST(IndirectlyReadableVTest, MatchesIsIndirectlyReadableValue) {
  STATIC_ASSERT_EQ(is_indirectly_readable_v<TypeParam>, is_indirectly_readable<TypeParam>::value);
}

}  // namespace
