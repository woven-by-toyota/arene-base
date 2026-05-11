// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/tuple/tests/tuple_cat.hpp"  // IWYU pragma: keep

// IWYU pragma: no_include "testlibs/minitest/minitest.hpp"

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/tuple/tests/value_category_probe.hpp"
#include "arene/base/tuple/tuple_cat.hpp"

namespace {

// These cases are exercised only in the arene flavor: the in-tree
// std::tuple_cat wrapper rejects non-std::tuple inputs per the C++ standard.

/// @test tuple_cat accepts arene::base::array
TEST(TupleCat, AreneBaseArrayInput) {
  auto result = arene::base::tuple_cat(arene::base::array<int, 2>{1, 2}, arene::base::array<int, 2>{3, 4});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, int, int, int>>();
  EXPECT_EQ(std::get<0>(result), 1);
  EXPECT_EQ(std::get<1>(result), 2);
  EXPECT_EQ(std::get<2>(result), 3);
  EXPECT_EQ(std::get<3>(result), 4);
}

using ::arene::base::tuple_testing::input_category;
using ::arene::base::tuple_testing::value_category_probe;

/// @test tuple_cat dispatches to the lvalue-ref @c get overload for lvalue inputs
TEST(TupleCat, LValueInputDispatchesToLValueGet) {
  value_category_probe probe{};
  auto const result = arene::base::tuple_cat(probe);
  EXPECT_EQ(std::get<0>(result), input_category::lvalue_ref);
}

/// @test tuple_cat dispatches to the const-lvalue-ref @c get overload for const lvalue inputs
TEST(TupleCat, ConstLValueInputDispatchesToConstLValueGet) {
  value_category_probe const probe{};
  auto const result = arene::base::tuple_cat(probe);
  EXPECT_EQ(std::get<0>(result), input_category::const_lvalue_ref);
}

/// @test tuple_cat dispatches to the rvalue-ref @c get overload for rvalue inputs
TEST(TupleCat, RValueInputDispatchesToRValueGet) {
  auto const result = arene::base::tuple_cat(value_category_probe{});
  EXPECT_EQ(std::get<0>(result), input_category::rvalue_ref);
}

/// @test tuple_cat dispatches to the const-rvalue-ref @c get overload for const rvalue inputs
TEST(TupleCat, ConstRValueInputDispatchesToConstRValueGet) {
  value_category_probe const probe{};
  // NOLINTNEXTLINE(hicpp-move-const-arg) const rvalue by design
  auto const result = arene::base::tuple_cat(std::move(probe));
  EXPECT_EQ(std::get<0>(result), input_category::const_rvalue_ref);
}

}  // namespace
