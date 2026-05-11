#include "arene/base/units/detail/sum.hpp"

#include <gtest/gtest.h>

namespace {

/// @test @c units_detail::sum calculates a sum
TEST(UnitsDetailSum, BasicTest) {
  ASSERT_EQ(0U, arene::base::units_detail::sum({}));
  ASSERT_EQ(3U, arene::base::units_detail::sum({1, 1, 1}));
  ASSERT_EQ(6U, arene::base::units_detail::sum({1, 2, 3}));
}

}  // namespace
