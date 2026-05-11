// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file integer_sequences_test.cpp
/// @brief Validates that expected symbols are exported from integer_sequences.hpp
///

#include "arene/base/integer_sequences.hpp"

#include <gtest/gtest.h>

namespace {
/// @test Ensure that `integer_sequence_unique_elements` is declared
TEST(IntegerSequences, IntegerSequenceUniqueElements) { using ::arene::base::integer_sequence_unique_elements; }
/// @test Ensure that `integer_sequence_count_of_v` is declared
TEST(IntegerSequences, IntegerSequenceCountOfV) { using ::arene::base::integer_sequence_count_of_v; }

/// @test Ensure that `integer_sequence_count_of` is declared
TEST(IntegerSequences, IntegerSequenceCountOf) { using ::arene::base::integer_sequence_count_of; }

/// @test Ensure that `integer_sequence_index_of_v` is declared
TEST(IntegerSequences, IntegerSequenceIndexOfV) { using ::arene::base::integer_sequence_index_of_v; }

/// @test Ensure that `integer_sequence_index_of` is declared
TEST(IntegerSequences, IntegerSequenceIndexOf) { using ::arene::base::integer_sequence_index_of; }

/// @test Ensure that `integer_sequence_contains_v` is declared
TEST(IntegerSequences, IntegerSequenceContainsV) { using ::arene::base::integer_sequence_contains_v; }

/// @test Ensure that `integer_sequence_element_v` is declared
TEST(IntegerSequences, IntegerSequenceElementV) { using ::arene::base::integer_sequence_element_v; }

/// @test Ensure that `integer_sequence_element` is declared
TEST(IntegerSequences, IntegerSequenceElement) { using ::arene::base::integer_sequence_element; }

/// @test Ensure that `integer_sequence_cat` is declared
TEST(IntegerSequences, IntegerSequenceCat) { using ::arene::base::integer_sequence_cat; }

/// @test Ensure that `sequential_values` is declared
TEST(IntegerSequences, SequentialValues) { using ::arene::base::sequential_values; }

/// @test Ensure that `make_index_sequence_repeat_n` is declared
TEST(IntegerSequences, MakeIndexSequenceRepeatN) { using ::arene::base::make_index_sequence_repeat_n; }

}  // namespace
