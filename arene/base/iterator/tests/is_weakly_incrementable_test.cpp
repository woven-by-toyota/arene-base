// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/is_weakly_incrementable.hpp"

#include <cstdint>

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/iterator/incrementable_traits.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

using ::arene::base::is_weakly_incrementable;
using ::arene::base::is_weakly_incrementable_v;
namespace type_lists = ::arene::base::type_lists;
namespace iterator_tests = ::arene::base::iterator_tests;

template <typename T>
class WeaklyIncrementableStandardIterators : public ::testing::Test {};

using all_const_containers =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, std::add_const_t>;

using all_standard_iterator_types = type_lists::remove_duplicates_t<type_lists::apply_each_t<
    type_lists::concat_t<iterator_tests::at_least_forward_iterator_containers, all_const_containers>,
    iterator_tests::iterator_t>>;

TYPED_TEST_SUITE(WeaklyIncrementableStandardIterators, all_standard_iterator_types, );

/// @test Given an iterator `Itr` where all of the requirements of `weakly_incrementable` are satisfied, then
/// `weakly_incrementable<Itr>` is true.
TYPED_TEST(WeaklyIncrementableStandardIterators, IsTrue) {
  STATIC_ASSERT_TRUE(is_weakly_incrementable<TypeParam>::value);
  STATIC_ASSERT_TRUE(is_weakly_incrementable_v<TypeParam>);
}

struct specialized_incrementable_iterator {
  auto operator++() -> specialized_incrementable_iterator& { return *this; }
  auto operator++(int) -> specialized_incrementable_iterator { return *this; }
};

}  // namespace

template <>
struct arene::base::incrementable_traits<specialized_incrementable_iterator> {
  using difference_type = std::int64_t;
};

namespace {

/// @test Given an iterator `Itr` without a specialization of `iterator_traits` but with a specialization of
/// `incrementable_traits`, then `weakly_incrementable<Itr>` is true.
TEST(IncrementableTraitsSpecializedIteratorTypeTest, IsWeaklyIncrementable) {
  STATIC_ASSERT_TRUE(is_weakly_incrementable<specialized_incrementable_iterator>::value);
  STATIC_ASSERT_TRUE(is_weakly_incrementable_v<specialized_incrementable_iterator>);
}

template <typename T, typename U = T>
struct pre_increment_signature {
  auto operator++() -> U&;
};

template <typename T>
struct post_increment_signature {
  auto operator++(int) -> T;
};

template <typename DiffT = std::ptrdiff_t>
struct difference_type_signature {
  using difference_type = DiffT;
};

// NOLINTBEGIN(fuchsia-multiple-inheritance) CRTP mixin classes are excluded
struct non_movable_itr
    : pre_increment_signature<non_movable_itr>
    , post_increment_signature<non_movable_itr>
    , difference_type_signature<> {
  using pre_increment_signature<non_movable_itr>::operator++;
  using post_increment_signature<non_movable_itr>::operator++;

  non_movable_itr() = default;
  ~non_movable_itr() = default;
  non_movable_itr(non_movable_itr const&) = delete;
  auto operator=(non_movable_itr const&) -> non_movable_itr& = delete;
  non_movable_itr(non_movable_itr&&) = delete;
  auto operator=(non_movable_itr&&) -> non_movable_itr& = delete;
};

struct no_pre_increment_iterator
    : post_increment_signature<no_pre_increment_iterator>
    , difference_type_signature<> {};

struct no_post_increment_iterator
    : pre_increment_signature<no_post_increment_iterator>
    , difference_type_signature<> {};

struct wrong_pre_increment_return_iterator
    : pre_increment_signature<wrong_pre_increment_return_iterator, int>
    , post_increment_signature<wrong_pre_increment_return_iterator>
    , difference_type_signature<> {
  using pre_increment_signature<wrong_pre_increment_return_iterator, int>::operator++;
  using post_increment_signature<wrong_pre_increment_return_iterator>::operator++;
};

struct non_signed_difference_iterator
    : pre_increment_signature<non_signed_difference_iterator>
    , post_increment_signature<non_signed_difference_iterator>
    , difference_type_signature<std::size_t> {
  using pre_increment_signature<non_signed_difference_iterator>::operator++;
  using post_increment_signature<non_signed_difference_iterator>::operator++;
};

struct non_integral_difference_iterator
    : pre_increment_signature<non_integral_difference_iterator>
    , post_increment_signature<non_integral_difference_iterator>
    , difference_type_signature<double> {
  using pre_increment_signature<non_integral_difference_iterator>::operator++;
  using post_increment_signature<non_integral_difference_iterator>::operator++;
};

struct no_difference_type_iterator
    : pre_increment_signature<no_difference_type_iterator>
    , post_increment_signature<no_difference_type_iterator> {
  using pre_increment_signature<no_difference_type_iterator>::operator++;
  using post_increment_signature<no_difference_type_iterator>::operator++;
};
// NOLINTEND(fuchsia-multiple-inheritance) CRTP mixin classes are excluded

using non_weakly_incrementable_iterators = ::testing::Types<
    non_movable_itr,
    no_pre_increment_iterator,
    no_post_increment_iterator,
    wrong_pre_increment_return_iterator,
    non_signed_difference_iterator,
    non_integral_difference_iterator,
    no_difference_type_iterator>;

template <typename Itr>
class NonWeaklyIncrementableTest : public ::testing::Test {};

TYPED_TEST_SUITE(NonWeaklyIncrementableTest, non_weakly_incrementable_iterators, );

/// @test Given an iterator type `Itr` which does not satisfy one or more of the needed constraints of
/// `weakly_incrementable`, then `weakly_incrementable<Itr>` is false.
TYPED_TEST(NonWeaklyIncrementableTest, IsNotWeaklyIncrementable) {
  STATIC_ASSERT_FALSE(is_weakly_incrementable<TypeParam>::value);
  STATIC_ASSERT_FALSE(is_weakly_incrementable_v<TypeParam>);
}

}  // namespace
