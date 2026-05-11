// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/integer_sequences.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"

namespace {

/// @test `make_index_sequence_repeat_n` with Count=0 yields an empty sequence
TEST(IndexSequenceRepeat, CountZeroYieldsEmptySequence) {
  ::testing::StaticAssertTypeEq<arene::base::make_index_sequence_repeat_n<0, 0>, std::index_sequence<>>();
  ::testing::StaticAssertTypeEq<arene::base::make_index_sequence_repeat_n<7, 0>, std::index_sequence<>>();
}

/// @test `make_index_sequence_repeat_n` with Count=1 yields a single-element sequence
TEST(IndexSequenceRepeat, CountOneYieldsSingletonSequence) {
  ::testing::StaticAssertTypeEq<arene::base::make_index_sequence_repeat_n<0, 1>, std::index_sequence<0>>();
  ::testing::StaticAssertTypeEq<arene::base::make_index_sequence_repeat_n<5, 1>, std::index_sequence<5>>();
}

/// @test `make_index_sequence_repeat_n` repeats Value exactly Count times
TEST(IndexSequenceRepeat, RepeatsValueCountTimes) {
  ::testing::StaticAssertTypeEq<arene::base::make_index_sequence_repeat_n<3, 4>, std::index_sequence<3, 3, 3, 3>>();
  ::testing::StaticAssertTypeEq<arene::base::make_index_sequence_repeat_n<0, 5>, std::index_sequence<0, 0, 0, 0, 0>>();
}

}  // namespace
