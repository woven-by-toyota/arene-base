// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file type_list_test.cpp
/// @brief Provides unit tests to validate export of content in type_list.hpp
///

#include "arene/base/type_list.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `apply_all` is declared
TEST(TypeList, ApplyAll) { using arene::base::type_lists::apply_all; }

/// @test `apply_all_t` is declared
TEST(TypeList, ApplyAllT) { using arene::base::type_lists::apply_all_t; }

/// @test `apply_each` is declared
TEST(TypeList, ApplyEach) { using arene::base::type_lists::apply_each; }

/// @test `apply_each_t` is declared
TEST(TypeList, ApplyEachT) { using arene::base::type_lists::apply_each_t; }

/// @test `at` is declared
TEST(TypeList, At) { using arene::base::type_lists::at; }

/// @test `at_t` is declared
TEST(TypeList, AtT) { using arene::base::type_lists::at_t; }

/// @test `concat` is declared
TEST(TypeList, Concat) { using arene::base::type_lists::concat; }

/// @test `concat_t` is declared
TEST(TypeList, ConcatT) { using arene::base::type_lists::concat_t; }

/// @test `concat_unique_t` is declared
TEST(TypeList, ConcatUniqueT) { using arene::base::type_lists::concat_unique_t; }

/// @test `contains` is declared
TEST(TypeList, Contains) { using arene::base::type_lists::contains; }

/// @test `contains_t` is declared
TEST(TypeList, ContainsT) { using arene::base::type_lists::contains_t; }

/// @test `contains_v` is declared
TEST(TypeList, ContainsV) { using arene::base::type_lists::contains_v; }

/// @test `drop` is declared
TEST(TypeList, Drop) { using arene::base::type_lists::drop; }

/// @test `drop_t` is declared
TEST(TypeList, DropT) { using arene::base::type_lists::drop_t; }

/// @test `filter` is declared
TEST(TypeList, Filter) { using arene::base::type_lists::filter; }

/// @test `filter_t` is declared
TEST(TypeList, FilterT) { using arene::base::type_lists::filter_t; }

/// @test `flat_map` is declared
TEST(TypeList, FlatMap) { using arene::base::type_lists::flat_map; }

/// @test `flat_map_t` is declared
TEST(TypeList, FlatMapT) { using arene::base::type_lists::flat_map_t; }

/// @test `flatten` is declared
TEST(TypeList, Flatten) { using arene::base::type_lists::flatten; }

/// @test `flatten_t` is declared
TEST(TypeList, FlattenT) { using arene::base::type_lists::flatten_t; }

/// @test `index_of_t` is declared
TEST(TypeList, IndexOfT) { using arene::base::type_lists::index_of_t; }

/// @test `index_of_v` is declared
TEST(TypeList, IndexOfV) { using arene::base::type_lists::index_of_v; }

/// @test `index_of` is declared
TEST(TypeList, IndexOf) { using arene::base::type_lists::index_of; }

/// @test `remove_duplicates` is declared
TEST(TypeList, RemoveDuplicates) { using arene::base::type_lists::remove_duplicates; }

/// @test `remove_duplicates_t` is declared
TEST(TypeList, RemoveDuplicatesT) { using arene::base::type_lists::remove_duplicates_t; }

/// @test `remove_nth` is declared
TEST(TypeList, RemoveNth) { using arene::base::type_lists::remove_nth; }

/// @test `remove_nth_t` is declared
TEST(TypeList, RemoveNthT) { using arene::base::type_lists::remove_nth_t; }

/// @test `take` is declared
TEST(TypeList, Take) { using arene::base::type_lists::take; }

/// @test `take_t` is declared
TEST(TypeList, TakeT) { using arene::base::type_lists::take_t; }

/// @test `size` is declared
TEST(TypeList, Size) { using arene::base::type_lists::size; }

/// @test `size_v` is declared
TEST(TypeList, SizeV) { using arene::base::type_lists::size_v; }

/// @test `size_t` is declared
TEST(TypeList, SizeT) { using arene::base::type_lists::size_t; }

/// @test `empty` is declared
TEST(TypeList, Empty) { using arene::base::type_lists::empty; }

/// @test `empty_v` is declared
TEST(TypeList, EmptyV) { using arene::base::type_lists::empty_v; }

/// @test `empty_t` is declared
TEST(TypeList, EmptyT) { using arene::base::type_lists::empty_t; }

/// @test `sort` is declared
TEST(TypeList, Sort) { using arene::base::type_lists::sort; }

/// @test `sort_t` is declared
TEST(TypeList, SortT) { using arene::base::type_lists::sort_t; }

/// @test `type_list` is declared
TEST(TypeList, TypeList) { using arene::base::type_list; }

}  // namespace
