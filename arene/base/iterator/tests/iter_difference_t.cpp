// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/iter_difference_t.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/iterator/incrementable_traits.hpp"
#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/add_const.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

namespace {

using ::arene::base::iter_difference_t;
namespace type_lists = ::arene::base::type_lists;
namespace iterator_tests = ::arene::base::iterator_tests;

template <typename T>
class IterDifferenceTStandardIterators : public ::testing::Test {};

using pointer_iterators = ::testing::Types<int*, int const*>;

using all_const_containers =
    type_lists::apply_each_t<iterator_tests::at_least_forward_iterator_containers, std::add_const_t>;

using all_standard_iterator_types = type_lists::apply_each_t<
    type_lists::concat_t<iterator_tests::at_least_forward_iterator_containers, all_const_containers>,
    iterator_tests::iterator_t>;

using all_valid_iterator_types = type_lists::remove_duplicates_t<
    type_lists::
        concat_t<pointer_iterators, iterator_tests::at_least_forward_iterator_containers, all_const_containers>>;

TYPED_TEST_SUITE(IterDifferenceTStandardIterators, all_standard_iterator_types, );

/// @test Given an iterator `Itr` where `iterator_traits<Itr>` is specialized, then `iter_difference_t<Itr>` is the same
/// as `typename std::iterator_traits<Itr>::difference_type`.
TYPED_TEST(IterDifferenceTStandardIterators, IsIteratorTraitsDifferenceType) {
  ::testing::
      StaticAssertTypeEq<iter_difference_t<TypeParam>, typename std::iterator_traits<TypeParam>::difference_type>();
}

struct specialized_incrementable_iterator {};

}  // namespace

template <>
struct arene::base::incrementable_traits<specialized_incrementable_iterator> {
  using difference_type = std::int64_t;
};

namespace {

/// @test Given an iterator `Itr` without a specialization of `iterator_traits` but with a specialization of
/// `incrementable_traits`, then `iter_difference_t<Itr>` is the same as the `difference_type` specified in the
/// specialization of `incrementable_traits`.
TEST(IncrementableTraitsSpecializedIteratorTypeTest, DifferenceTypeIsSubtractionResult) {
  ::testing::StaticAssertTypeEq<
      iter_difference_t<specialized_incrementable_iterator>,
      typename ::arene::base::incrementable_traits<specialized_incrementable_iterator>::difference_type>();
}

}  // namespace
