// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/libc.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {

[[noreturn]] auto exit_with_message(int exit_code, char const* message) noexcept {
  ::testing::minitest::print_string(::testing::minitest::get_stderr(), message);
  ::testing::minitest::exit_program(exit_code);
}

/// @brief Fill the given @c string with @c count copies of <c>fill_with</c>; doesn't write a null, so be careful
constexpr auto fill_string_with_copies_of_char(char* string, char fill_with, ::size_t count) noexcept -> void {
  for (::size_t i{}; i < count; ++i) {
    string[i] = fill_with;
  }
}

}  // namespace

TEST(DeathTest, Correct) { ASSERT_DEATH(exit_with_message(1, "crash"), "ras"); }

TEST(DeathTest, NoCrashFails) { ASSERT_DEATH(0, ""); }

TEST(DeathTest, ExitSuccessFails) { ASSERT_DEATH(exit_with_message(0, "crash"), "ras"); }

TEST(DeathTest, EmptyStringFoundInLongMessage) { ASSERT_DEATH(exit_with_message(2, "ghjlskhvbawluvhnawoi;fh"), ""); }

TEST(DeathTest, EmptyStringFoundInNoMessage) { ASSERT_DEATH(::testing::minitest::exit_program(3), ""); }

TEST(DeathTest, WrongMessageNotFound) { ASSERT_DEATH(exit_with_message(4, "ghjlskhvbawluvhnawoi;fh"), "oops"); }

TEST(DeathTest, WrongMessageNotFoundInNoMessage) { ASSERT_DEATH(::testing::minitest::exit_program(5), "hmmm"); }

TEST(DeathTest, TwoDeathTestsInOneCase) {
  ASSERT_DEATH(exit_with_message(6, "first"), "");
  ASSERT_DEATH(exit_with_message(7, "second"), "");
}

TEST(DeathTest, TargetMessageCrossingMultipleChunks) {
  // target_message is 400 copies of 'o' and then a null terminator
  char target_message[401UL]{};
  fill_string_with_copies_of_char(static_cast<char*>(target_message), 'o', 400UL);

  // actual_message is 900 copies of 'x', then 400 copies of 'o', then another 300 copies of 'x' and a null terminator
  char actual_message[1601UL]{};
  fill_string_with_copies_of_char(static_cast<char*>(actual_message), 'x', 1600UL);
  fill_string_with_copies_of_char(static_cast<char*>(actual_message) + 900UL, 'o', 400UL);

  // This one should pass
  ASSERT_DEATH(exit_with_message(8, static_cast<char*>(actual_message)), static_cast<char*>(target_message));

  // Writing an 'x' in the middle of the actual_message should cause the assertion to fail
  actual_message[1000UL] = 'x';
  ASSERT_DEATH(exit_with_message(9, static_cast<char*>(actual_message)), static_cast<char*>(target_message));
}

TEST(DeathTest, VeryLongTargetMessageIsRejected) {
  char target_message[601UL]{};
  fill_string_with_copies_of_char(static_cast<char*>(target_message), 'a', 600UL);
  ASSERT_DEATH(exit_with_message(10, "that's too many 'a's"), static_cast<char*>(target_message));
}
