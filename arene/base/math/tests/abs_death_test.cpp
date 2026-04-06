// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/math/abs.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"

namespace {
template <typename T>
class AbsIntegerTypedTest : public testing::Test {
 public:
  using type = std::remove_reference_t<T>;
};

using integer_types = arene::base::type_lists::remove_duplicates_t<
    ::testing::Types<std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::intmax_t>>;

TYPED_TEST_SUITE(AbsIntegerTypedTest, integer_types, );

/// @test `abs` terminates when given an argument that would underflow after conversion.
TYPED_TEST(AbsIntegerTypedTest, DeathTest) {
  using value_type = typename TestFixture::type;
  ASSERT_DEATH(
      arene::base::abs(std::numeric_limits<value_type>::min()),
      "Precondition violation: value != std::numeric_limits<Number>::min()"
  );
}

}  // namespace
