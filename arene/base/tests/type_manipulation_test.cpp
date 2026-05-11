// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file type_manipulation_test.cpp
/// @brief Provides unit tests to validate export of content in type_manipulation.hpp
///

#include <gtest/gtest.h>

#include "arene/base/type_manipulation/consume_values.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"
#include "arene/base/type_manipulation/give_qualifiers_to.hpp"
#include "arene/base/type_manipulation/implicit_constructor_base.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_manipulation/repeat_type.hpp"
#include "arene/base/type_manipulation/smallest_integer_for.hpp"
#include "arene/base/type_manipulation/static_if.hpp"

namespace {

/// @test `consume_values` is declared
TEST(TypeManipulation, ConsumeValues) { using arene::base::consume_values; }

/// @test `ebo_holder` is declared
TEST(TypeManipulation, EboHolder) { using arene::base::ebo_holder; }

/// @test `give_qualifiers_to` is declared
TEST(TypeManipulation, GiveQualifiersTo) { using arene::base::give_qualifiers_to; }

/// @test `implicit_constructor_base` is declared
TEST(TypeManipulation, ImplicitConstructorBase) { using arene::base::implicit_constructor_base; }

/// @test `non_constructible_dummy` is declared
TEST(TypeManipulation, NonConstructibleDummy) { using arene::base::non_constructible_dummy; }

/// @test `repeat_type` is declared
TEST(TypeManipulation, RepeatType) { using arene::base::repeat_type; }

/// @test `smallest_integer_for` is declared
TEST(TypeManipulation, SmallestIntegerFor) {
  using arene::base::smallest_signed_integer_for;
  using arene::base::smallest_unsigned_integer_for;
}

/// @test `static_if` is declared
TEST(TypeManipulation, StaticIf) { using arene::base::static_if; }

}  // namespace
