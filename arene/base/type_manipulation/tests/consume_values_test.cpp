// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/consume_values.hpp"

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace {

using ::arene::base::consume_values;

struct some_user_type {
  int a;
};

using types = ::testing::Types<int, double*, std::string, some_user_type>;

template <typename T>
struct ConsumeValuesTypeConsumption : ::testing::Test {};

TYPED_TEST_SUITE(ConsumeValuesTypeConsumption, types, );

/// @test `consume_values` can be invoked with a list of any number of values of any type
/// @tparam TypeParam The type to put in the list
TYPED_TEST(ConsumeValuesTypeConsumption, WorksForArbitraryTypesAndArbitraryLengths) {
  consume_values({TypeParam{}});
  consume_values({TypeParam{}, TypeParam{}, TypeParam{}});
  consume_values({TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{},
                  TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{},
                  TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{},
                  TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{},
                  TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}});
}

/// @test Invoking `consume_values` does not copy or move the supplied values
TEST(ConsumeValuesValueConsumption, ConsumptionIsANoOp) {
  int was_copied_or_moved = 0;
  struct fail_if_copied_or_moved {
    int& count_;
    explicit fail_if_copied_or_moved(int& was_copied_or_moved)
        : count_(was_copied_or_moved) {}
    ~fail_if_copied_or_moved() = default;
    fail_if_copied_or_moved(fail_if_copied_or_moved const& other)
        : count_(other.count_) {
      ++count_;
    }
    fail_if_copied_or_moved(fail_if_copied_or_moved&&) = delete;

    auto operator=(fail_if_copied_or_moved const&) -> fail_if_copied_or_moved& {
      ++count_;
      return *this;
    }
    auto operator=(fail_if_copied_or_moved&&) -> fail_if_copied_or_moved& = delete;
  };

  consume_values(
      {fail_if_copied_or_moved{was_copied_or_moved},
       fail_if_copied_or_moved{was_copied_or_moved},
       fail_if_copied_or_moved{was_copied_or_moved}}
  );
  EXPECT_EQ(was_copied_or_moved, 0);
}

/// @test `consume_values` can be invoked with a mix of types
TEST(ConsumeValuesValueConsumption, DistinctTypes) {
  consume_values({"hello", 1, 23, 47U, 3.4, std::string("wibble")});
}

}  // namespace
