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

}  // namespace
