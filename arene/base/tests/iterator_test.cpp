// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file iterator_test.cpp
/// @brief Validates that expected symbols are exported from iterator.hpp
///

#include "arene/base/iterator.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `advance` is declared
TEST(IteratorExports, Advance) { using ::arene::base::advance; }
/// @test `distance` is declared
TEST(IteratorExports, Distance) { using ::arene::base::distance; }
/// @test `next` is declared
TEST(IteratorExports, Next) { using ::arene::base::next; }
/// @test `prev` is declared
TEST(IteratorExports, Prev) { using ::arene::base::prev; }

/// @test `reverse_iterator` is declared
TEST(IteratorExports, ReverseIterator) { using ::arene::base::reverse_iterator; }
/// @test `make_reverse_iterator` is declared
TEST(IteratorExports, MakeReverseIterator) { using ::arene::base::make_reverse_iterator; }

/// @test `is_weakly_incrementable` is declared
TEST(IteratorExports, WeaklyIncrementable) { using ::arene::base::is_weakly_incrementable; }
/// @test `iter_difference_t` is declared
TEST(IteratorExports, IterDifferenceT) { using ::arene::base::iter_difference_t; }
/// @test `incrementable_traits` is declared
TEST(IteratorExports, IncrementableTraits) { using ::arene::base::incrementable_traits; }
/// @test `indirectly_readable_traits` is declared
TEST(IteratorExports, IndirectlyReadableTraits) { using ::arene::base::indirectly_readable_traits; }
/// @test `is_indirectly_readable` is declared
TEST(IteratorExports, IsIndirectlyReadable) { using ::arene::base::is_indirectly_readable; }
/// @test `is_indirectly_readable_v` is declared
TEST(IteratorExports, IsIndirectlyReadableV) { using ::arene::base::is_indirectly_readable_v; }
/// @test `indirect_result_t` is declared
TEST(IteratorExports, IndirectResultT) { using ::arene::base::indirect_result_t; }
/// @test `iter_reference_t` is declared
TEST(IteratorExports, IterReferenceT) { using ::arene::base::iter_reference_t; }
/// @test `iter_rvalue_reference_t` is declared
TEST(IteratorExports, IterRvalueReferenceT) { using ::arene::base::iter_rvalue_reference_t; }
/// @test `iter_const_reference_t` is declared
TEST(IteratorExports, IterConstReferenceT) { using ::arene::base::iter_const_reference_t; }
/// @test `iter_value_t` is declared
TEST(IteratorExports, IterValueT) { using ::arene::base::iter_value_t; }

/// @test `iter_move` is declared
TEST(IteratorExports, IterMove) { using ::arene::base::ranges::iter_move; }
}  // namespace
