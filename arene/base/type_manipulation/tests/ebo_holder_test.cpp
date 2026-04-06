// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/ebo_holder.hpp"

#include <cstdint>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {
struct first_field_tag {};
struct second_field_tag {};

template <typename T1, typename T2>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
class compressed_pair
    : arene::base::ebo_holder<first_field_tag, T1>
    , arene::base::ebo_holder<second_field_tag, T2> {
 public:
  auto first() -> T1& { return this->get_value(first_field_tag{}); }
  auto second() -> T2& { return this->get_value(second_field_tag{}); }

  // other members
};

struct empty_class {};
struct other_empty_class {};

class derived : compressed_pair<empty_class, other_empty_class> {
  std::int32_t i_ = 0;

 public:
  auto get_i() -> std::int32_t& { return i_; }
};

/// @test `ebo_holder` allows the use of the empty base optimization to reduce the size of an object
TEST(EBO, CompressedPairUseEBO) {
  STATIC_ASSERT_EQ(sizeof(std::pair<empty_class, other_empty_class>), 2);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<empty_class, other_empty_class>), 1);
  STATIC_ASSERT_EQ(sizeof(std::pair<std::int32_t, empty_class>), 8);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<std::int32_t, empty_class>), 4);
  STATIC_ASSERT_EQ(sizeof(std::pair<empty_class, std::int32_t>), 8);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<empty_class, std::int32_t>), 4);
  STATIC_ASSERT_EQ(sizeof(std::pair<std::int32_t, std::int32_t>), 8);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<std::int32_t, std::int32_t>), 8);
  STATIC_ASSERT_EQ(sizeof(derived), 4);
}
}  // namespace
