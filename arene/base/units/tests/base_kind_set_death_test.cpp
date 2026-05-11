#include <gtest/gtest.h>

#include "arene/base/units/base_kind_set.hpp"

namespace {

using arene::base::units_detail::base_kind_exponents;

/// @test use of negative exponent values is a precondition violation
TEST(BaseExponentDeathTest, WithNegativeExponentValues) {
  ASSERT_DEATH((base_kind_exponents{-1, 0}), "Precondition");
  ASSERT_DEATH((base_kind_exponents{0, -1}), "Precondition");
  ASSERT_DEATH((base_kind_exponents{-1, -1}), "Precondition");
}

}  // namespace
