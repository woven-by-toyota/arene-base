#include "testlibs/minitest/minitest.hpp"

TEST(Test, TrailingElseBindsToOuterIf) {
  auto hit = false;

  // NOLINTBEGIN(hicpp-braces-around-statements)
  // NOLINTNEXTLINE(readability-simplify-boolean-expr)
  if (false)
    ASSERTION_UNDER_TEST;
  else
    hit = true;
  // NOLINTEND(hicpp-braces-around-statements)

  ASSERT_TRUE(hit);
}
