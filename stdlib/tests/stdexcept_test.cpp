// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/stdlib_detail/stdexcept.hpp"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/strings/algorithm/length.hpp"
#include "arene/base/type_list/concat.hpp"
#include "stdlib/include/stdlib_detail/is_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_copy_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_move_assignable.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test `std::exception` is default constructible.
CONSTEXPR_TEST(Exception, IsDefaultConstructible) { CONSTEXPR_ASSERT(std::is_constructible<std::exception>::value); }
/// @test `std::exception` is copy constructible.
CONSTEXPR_TEST(Exception, IsCopyConstructible) {
  CONSTEXPR_ASSERT(std::is_constructible<std::exception, std::exception const&>::value);
}
/// @test `std::exception` is move constructible.
CONSTEXPR_TEST(Exception, IsMoveConstructible) {
  CONSTEXPR_ASSERT(std::is_constructible<std::exception, std::exception&&>::value);
}
/// @test `std::exception` is copy assignable.
CONSTEXPR_TEST(Exception, IsCopyAssignable) { CONSTEXPR_ASSERT(std::is_copy_assignable<std::exception>::value); }
/// @test `std::exception` is move assignable.
CONSTEXPR_TEST(Exception, IsMoveAssignable) { CONSTEXPR_ASSERT(std::is_move_assignable<std::exception>::value); }
/// @test Given an instance of `std::exception`, when `what()` is invoked, then it returns `"std::exception"`.
TEST(Exception, WhatReturnsStockText) {
  auto const exp = std::exception{};
  ASSERT_NE(exp.what(), nullptr);
  ASSERT_TRUE(testing::cstring::equal(exp.what(), "std::exception"));
}

using runtime_derived_errors = ::testing::Types<std::runtime_error>;
using logic_derived_errors = ::testing::Types<std::logic_error, std::out_of_range>;

using message_containing_errors = ::arene::base::type_lists::concat_t<runtime_derived_errors, logic_derived_errors>;

template <typename T>
class MessageContainingErrorTest : public ::testing::Test {};

TYPED_TEST_SUITE(MessageContainingErrorTest, message_containing_errors, );
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, then `E` is default constructible.
TYPED_TEST(MessageContainingErrorTest, IsDefaultConstructible) {
  CONSTEXPR_ASSERT(std::is_constructible<TypeParam>::value);
}
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, then `E` is copy constructible.
TYPED_TEST(MessageContainingErrorTest, IsCopyConstructible) {
  CONSTEXPR_ASSERT(std::is_constructible<TypeParam, TypeParam const&>::value);
}
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, then `E` is move constructible.
TYPED_TEST(MessageContainingErrorTest, IsMoveConstructible) {
  CONSTEXPR_ASSERT(std::is_constructible<TypeParam, TypeParam&&>::value);
}
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, then `E` is copy assignable.
TYPED_TEST(MessageContainingErrorTest, IsCopyAssignable) {
  CONSTEXPR_ASSERT(std::is_copy_assignable<TypeParam>::value);
}
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, then `E` is move assignable.
TYPED_TEST(MessageContainingErrorTest, IsMoveAssignable) {
  CONSTEXPR_ASSERT(std::is_move_assignable<TypeParam>::value);
}
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, when `E` is default constructed, then `what()` returns stock message text.
TYPED_TEST(MessageContainingErrorTest, WhenDefaultConstructedWhatIsStockText) {
  auto const exp = TypeParam{};
  ASSERT_NE(exp.what(), nullptr);
  // We can't access the actual content since it's type dependent, but we can check the prefix this way.
  ASSERT_TRUE(testing::cstring::starts_with(exp.what(), "std::"));
}

/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, when `E` is constructed with a message string `msg`, then `strcmp(e.what(), msg)` returns 0.
TYPED_TEST(MessageContainingErrorTest, WhenConstructedWithMessageWhatReturnsMessage) {
  constexpr auto message = "some error message";
  auto const exp = TypeParam{message};
  ASSERT_NE(exp.what(), nullptr);
  ASSERT_TRUE(testing::cstring::equal(exp.what(), message));
}
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, when `E` is constructed with `nullptr`, then then `what()` returns stock message text.
TYPED_TEST(MessageContainingErrorTest, WhenConstructedFromNullptrReturnsStockMessage) {
  auto const exp = TypeParam{nullptr};
  ASSERT_NE(exp.what(), nullptr);
  ASSERT_TRUE(testing::cstring::starts_with(exp.what(), "std::"));
}

/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, when `E` is copy-constructed from another `E` instance, then `strcmp(e1.what(), e2.what())`
/// returns 0.
TYPED_TEST(MessageContainingErrorTest, CopyConstructingCopiesMessageWithoutAlteringOriginal) {
  auto const copied_from_exception = TypeParam{"some error message"};
  auto const copied_to_exception = TypeParam{copied_from_exception};
  ASSERT_NE(copied_from_exception.what(), nullptr);
  ASSERT_NE(copied_to_exception.what(), nullptr);
  ASSERT_TRUE(testing::cstring::equal(copied_to_exception.what(), copied_from_exception.what()));
}
/// @test Given an instance, `e1` of exception type `E` derived from `std::exception` which has a constructor consuming
/// a c-string to use as an error message, when `e1` is copy-assigned from another `E` instance e2, then
/// `strcmp(e1.what(), e2.what())` returns 0.
TYPED_TEST(MessageContainingErrorTest, CopyAssigningReplacesMessageWithoutAlteringAssigned) {
  auto copied_assigned_to_exception = TypeParam{"original message"};
  constexpr auto copied_from_message = "some new message";
  auto const copy_assigned_from_message = TypeParam{copied_from_message};
  copied_assigned_to_exception = copy_assigned_from_message;
  ASSERT_NE(copied_assigned_to_exception.what(), nullptr);
  ASSERT_NE(copy_assigned_from_message.what(), nullptr);
  ASSERT_TRUE(testing::cstring::equal(copied_assigned_to_exception.what(), copy_assigned_from_message.what()));
  ASSERT_TRUE(testing::cstring::equal(copy_assigned_from_message.what(), copied_from_message));
}
/// @test Given an instance, `e1` of exception type `E` derived from `std::exception` which has a constructor consuming
/// a c-string to use as an error message, when `e1` is copy-assigned from itself, then
/// the result is a no-op.
TYPED_TEST(MessageContainingErrorTest, CopySelfAssignmentDoesNotLeak) {
  auto copy_assigned_to_exception = TypeParam{"original message"};
  copy_assigned_to_exception = static_cast<TypeParam const&>(copy_assigned_to_exception);
  ASSERT_NE(copy_assigned_to_exception.what(), nullptr);
}
/// @test Given an exception type `E` derived from `std::exception` which has a constructor consuming a c-string to use
/// as an error message, when `E` is move-constructed from another `E` instance `e2`, then `strcmp(e1.what(),
/// e2.what())` where `e2`'s state is as if before the move, returns 0.
TYPED_TEST(MessageContainingErrorTest, MoveConstructingCopiesMessage) {
  constexpr auto message = "some error message";
  auto moved_from_exception = TypeParam{message};
  auto const moved_to_exception = TypeParam{std::move(moved_from_exception)};
  ASSERT_NE(moved_to_exception.what(), nullptr);
  ASSERT_TRUE(testing::cstring::equal(moved_to_exception.what(), message));
}
/// @test Given an instance, `e1` of exception type `E` derived from `std::exception` which has a constructor consuming
/// a c-string to use as an error message, when `e1` is move-assigned from another `E` instance e2, then
/// `strcmp(e1.what(), e2.what())` where `e2`'s state is as if before the move, returns 0.
TYPED_TEST(MessageContainingErrorTest, MoveAssigningReplacesMessageWithAssigned) {
  auto move_assigned_to_exception = TypeParam{"some original message"};
  constexpr auto message = "some error message";
  auto move_assigned_from_message = TypeParam{message};
  move_assigned_to_exception = std::move(move_assigned_from_message);
  ASSERT_NE(move_assigned_to_exception.what(), nullptr);
  ASSERT_TRUE(testing::cstring::equal(move_assigned_to_exception.what(), message));
}
/// @test Given an instance, `e1` of exception type `E` derived from `std::exception` which has a constructor consuming
/// a c-string to use as an error message, when `e1` is move-assigned from itself, then
/// the result is a no-op.
TYPED_TEST(MessageContainingErrorTest, MoveSelfAssignmentDoesNotLeak) {
  auto move_assigned_to_exception = TypeParam{"original message"};
  ARENE_IGNORE_START();
  ARENE_IGNORE_WSELFMOVE("Test code: self-assignment is on purpose");
  move_assigned_to_exception = std::move(move_assigned_to_exception);
  ARENE_IGNORE_END();
  ASSERT_NE(move_assigned_to_exception.what(), nullptr);
}

/// @test Constructing an exception from a very long input message causes the message to be truncated.
TYPED_TEST(MessageContainingErrorTest, VeryLongMessagesAreCutOff) {
  constexpr auto very_long_message =
      "this_message_has_10_of_every_letter_"
      "AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFFGGGGGGGGGGHHHHHHHHHHIIIIIIIIIIJJJJJJJJJJKKKKKKKKKK"
      "LLLLLLLLLLMMMMMMMMMMNNNNNNNNNNOOOOOOOOOOPPPPPPPPPPQQQQQQQQQQRRRRRRRRRRSSSSSSSSSSTTTTTTTTTTUUUUUUUUUUVVVVVVVVVV"
      "WWWWWWWWWWXXXXXXXXXXYYYYYYYYYYZZZZZZZZZZ";

  auto exception = TypeParam{very_long_message};
  ASSERT_EQ(arene::base::string_length(exception.what()), 255);
  ASSERT_FALSE(testing::cstring::equal(very_long_message, exception.what()));
  ASSERT_TRUE(testing::cstring::starts_with(very_long_message, exception.what()));
}

}  // namespace
