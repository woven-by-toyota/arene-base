// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/fill.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/strings/string_view.hpp"

namespace {

using ::arene::base::inline_string;
using ::arene::base::null_terminated_string_view;
using ::arene::base::string_view;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

template <std::size_t Size>
constexpr auto make_character_buffer(char fill_with) noexcept -> arene::base::array<char, Size + 1UL> {
  static_assert(Size + 1UL > 0UL, "Size must not wrap");
  arene::base::array<char, Size + 1UL> buffer{};
  arene::base::fill(buffer.begin(), buffer.end(), fill_with);
  buffer[Size] = '\0';
  return buffer;
}

constexpr arene::base::array<char, default_size + 2UL> too_long_buffer = make_character_buffer<default_size + 1UL>('a');
constexpr null_terminated_string_view too_long_ntsv{too_long_buffer.data()};
constexpr string_view too_long_view{too_long_buffer.data(), default_size + 1UL};

/// @test Attempting to construct an `inline_string` with a length exceeding the capacity terminates the program with a
/// precondition violation.
TEST(InlineStringCtorsDeathTest, ConstructingFromTooLongLengthValueIsPreconditionViolation) {
  static constexpr auto exceeding_len = default_size + 1U;
  static constexpr char chr{'b'};
  ASSERT_DEATH((inline_string<default_size>{exceeding_len, chr}), "Precondition");
}

/// @test Attempting to construct an `inline_string` from a NUL-terminated string, a `string_view`, or a larger-capacity
/// `inline_string` that is longer than the capacity of the `inline_string` being constructed terminates the program
/// with a precondition violation
TEST(InlineStringCtorsDeathTest, ConstructingFromOverLongSourceIsPreconditionViolation) {
  ASSERT_DEATH(inline_string<default_size>{too_long_ntsv.c_str()}, "Precondition");
  ASSERT_DEATH(inline_string<default_size>{too_long_view}, "Precondition");
  inline_string<default_size + 10> too_long_to_copy{};
  too_long_to_copy.resize(decltype(too_long_to_copy)::max_size(), 'f');
  ASSERT_DEATH(inline_string<default_size>{too_long_to_copy}, "Precondition");
}

/// @test Invoking the index operator on an `inline_string` with an index greater than the size of the string terminates
/// the program with a precondition violation
TEST(InlineStringIndexOperatorDeathTest, IfIndexGreaterThanSizeIsPreconditionViolation) {
  inline_string<default_size> str{"hello"};
  ASSERT_DEATH(str[str.size() + 1] = '0', "index <= this->size()");
  auto const& const_str = str;
  ASSERT_DEATH(std::ignore = const_str[const_str.size() + 1], "index <= this->size()");
}

template <typename T>
class InlineStringIteratorDeathTest : public ::testing::Test {};

using nostdlib_iterator_types =
    ::testing::Types<inline_string<default_size>::iterator, inline_string<default_size>::const_iterator>;

TYPED_TEST_SUITE(InlineStringIteratorDeathTest, nostdlib_iterator_types, );

/// @test Attempting to perform any operation other than assignment and destruction on a default-constructed iterator
/// terminates the program with a precondition violation
TYPED_TEST(InlineStringIteratorDeathTest, OperatorsOnDefaultConstructedIteratorArePreconditionViolations) {
  TypeParam default_ctor_itr = TypeParam{};
  TypeParam& non_const_default_ctor_itr = default_ctor_itr;
  ASSERT_DEATH(*non_const_default_ctor_itr, "Precondition");
  ASSERT_DEATH(std::ignore = non_const_default_ctor_itr.operator->(), "Precondition");
  ASSERT_DEATH(non_const_default_ctor_itr[0], "Precondition");
  ASSERT_DEATH(++non_const_default_ctor_itr, "Precondition");
  ASSERT_DEATH(non_const_default_ctor_itr++, "Precondition");
  ASSERT_DEATH(--non_const_default_ctor_itr, "Precondition");
  ASSERT_DEATH(non_const_default_ctor_itr--, "Precondition");
  ASSERT_DEATH(non_const_default_ctor_itr += 1, "Precondition");
  ASSERT_DEATH(non_const_default_ctor_itr -= 1, "Precondition");
  ASSERT_DEATH(non_const_default_ctor_itr + 1, "Precondition");
  ASSERT_DEATH(non_const_default_ctor_itr - 1, "Precondition");
  TypeParam const& const_default_ctor_itr = default_ctor_itr;
  ASSERT_DEATH(*const_default_ctor_itr, "Precondition");
  ASSERT_DEATH(std::ignore = const_default_ctor_itr.operator->(), "Precondition");
  ASSERT_DEATH(const_default_ctor_itr[0], "Precondition");
  ASSERT_DEATH(const_default_ctor_itr + 1, "Precondition");
  ASSERT_DEATH(const_default_ctor_itr - 1, "Precondition");
  inline_string<default_size> valid{"foo"};
  ASSERT_DEATH(valid.begin() - TypeParam{}, "Precondition");
  ASSERT_DEATH(TypeParam{} - valid.begin(), "Precondition");
}

/// @brief A sink function with a specific type to force the argument to convert to it; used to get the array
/// constructor of @c StringType without causing a decay to pointer.
template <typename StringType>
constexpr auto force_implicit_conversion(StringType) noexcept -> void {}

/// @test Constructing an `inline_string` from a smaller char array with no null terminator compiles but crashes.
TEST(InlineStringDeathTests, SmallCharArrayWithoutNull) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const short_char_array[] = {'r', 'e', 'd'};

  using string_type = arene::base::inline_string<sizeof(short_char_array) + 2UL>;
  ASSERT_DEATH(force_implicit_conversion<string_type>(short_char_array), "Precondition");
}

/// @test Constructing an `inline_string` from a same-size char array with no null terminator compiles but crashes.
TEST(InlineStringDeathTests, SameSizeCharArrayWithoutNull) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const exact_char_array[] = {'g', 'r', 'e', 'e', 'n'};

  using string_type = arene::base::inline_string<sizeof(exact_char_array)>;
  ASSERT_DEATH(force_implicit_conversion<string_type>(exact_char_array), "Precondition");
}

/// @test Constructing an `inline_string` from a one-too-large char array with no null terminator compiles but crashes.
TEST(InlineStringDeathTests, MaxSizeCharArrayWithoutNull) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char const long_char_array[] = {'y', 'e', 'l', 'l', 'o', 'w'};

  using string_type = arene::base::inline_string<sizeof(long_char_array) - 1UL>;
  ASSERT_DEATH(force_implicit_conversion<string_type>(long_char_array), "Precondition");
}

/// Constexpr wrappers around inline_string<N>::append()
/// @returns A copy of the input string that has had `append(args...)` called on it.
/// @{
template <typename Rhs>
constexpr auto do_append(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs.append(rhs);
  return lhs;
}

constexpr auto do_append(inline_string<default_size> lhs, string_view rhs, inline_string<default_size>::size_type pos)
    -> inline_string<default_size> {
  lhs.append(rhs, pos);
  return lhs;
}

constexpr auto do_append(
    inline_string<default_size> lhs,
    string_view rhs,
    inline_string<default_size>::size_type pos,
    inline_string<default_size>::size_type count
) -> inline_string<default_size> {
  lhs.append(rhs, pos, count);
  return lhs;
}

constexpr auto do_append(inline_string<default_size> lhs, inline_string<default_size>::size_type count, char chr)
    -> inline_string<default_size> {
  lhs.append(count, chr);
  return lhs;
}
/// @}

/// @test Invoking the `append` member function with a pointer to a NUL-terminated string that is longer than the
/// difference between the `inline_string`'s size and capacity terminates the program with a precondition violation
TEST(InlineStringAppendDeathTests, CStringPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  constexpr auto* world = " world";
  ASSERT_DEATH(do_append(full, world), "Precondition");
}

/// @test Invoking the `append` member function with a `string_view` that is longer than the difference between the
/// `inline_string`'s size and capacity terminates the program with a precondition violation
TEST(InlineStringAppendDeathTests, StringViewPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  constexpr string_view world = " world";
  ASSERT_DEATH(do_append(full, world), "Precondition");
}

/// @test Invoking `append` with a `string_view` and a position terminates the program with a precondition violation if
/// the substring of the `string_view` starting at the specified position is longer than the difference between the
/// existing size and the capacity of the `inline_string`.
TEST(InlineStringAppendDeathTests, StringViewPosPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  constexpr string_view world = " world";
  ASSERT_DEATH(do_append(full, world, 0), "Precondition");
}

/// @test Invoking `append` with a `string_view` a position and a count terminates the program with a precondition
/// violation if the substring of the `string_view` starting at the specified position and of the specified size is
/// longer than the difference between the existing size and the capacity of the `inline_string`.
TEST(InlineStringAppendDeathTests, StringViewPosCountPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  constexpr string_view world = " world";
  ASSERT_DEATH(do_append(full, world, 0, world.size()), "Precondition");
}

/// @test Invoking `append` with a single character when `size` is equal to the capacity of the string terminates the
/// program with a precondition violation
TEST(InlineStringAppendDeathTests, CharPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  ASSERT_DEATH(do_append(full, 'f'), "Precondition");
}

/// @test Invoking `append` with a count and a single character terminates the program if the count would cause the
/// string to exceed its capacity
TEST(InlineStringAppendDeathTests, CharRepeatingPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  ASSERT_DEATH(do_append(full, 1, 'f'), "Precondition");
}

/// @test Invoking `append` with an `inline_string` that is longer than the available space terminates the program with
/// a precondition violation.
TEST(InlineStringAppendDeathTests, InlineStringMaybeFitsPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  inline_string<default_size> const world{" world"};
  ASSERT_DEATH(do_append(full, world), "Precondition");
}

/// @test Invoking `append` with an initializer list of characters longer than the available space in the string
/// terminates the program with a precondition violation.
TEST(InlineStringAppendDeathTests, InitializerListPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  std::initializer_list<char> const world{' ', 'w', 'o', 'r', 'l', 'd'};
  ASSERT_DEATH(do_append(full, world), "Precondition");
}

/// Constexpr wrapper around inline_string<N>::+=()
/// @returns A copy of the input string that has had `+= rhs` called on it.
template <typename Rhs>
constexpr auto do_plus_assign(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs += rhs;
  return lhs;
}

/// @test Using `+=` to append a pointer to a NUL-terminated string to an `inline_string` terminates the program with a
/// precondition violation if the NUL-terminated string is longer than the available space.
TEST(InlineStringPlusAssignDeathTests, CStringPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  constexpr auto* world = " world";
  ASSERT_DEATH(do_plus_assign(full, world), "Precondition");
}

/// @test Using `+=` to append a `string_view` to an `inline_string` terminates the program with a precondition
/// violation if the `string_view` is longer than the available space.
TEST(InlineStringPlusAssignDeathTests, StringViewPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  constexpr string_view world = " world";
  ASSERT_DEATH(do_plus_assign(full, world), "Precondition");
}

/// @test Using `+=` to append an `inline_string` to an `inline_string` terminates the program with a precondition
/// violation if the LHS string does not have enough space for the characters in the RHS string.
TEST(InlineStringPlusAssignDeathTests, InlineStringMaybeFitsPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  inline_string<default_size> const world{" world"};
  ASSERT_DEATH(do_plus_assign(full, world), "Precondition");
}

/// @test Using `+=` to append an initializer list of characters to an `inline_string` terminates the program if there
/// is not enough space in the string for the characters in the initializer list
TEST(InlineStringPlusAssignDeathTests, InitializerListPreconditionIfNoRoom) {
  inline_string<default_size> full;
  full.resize(default_size, 'f');
  std::initializer_list<char> const world{' ', 'w', 'o', 'r', 'l', 'd'};
  ASSERT_DEATH(do_plus_assign(full, world), "Precondition");
}

/// constexpr wrapper for inline_string<default_size>::operator=
/// @return A copy of the input string with rhs assigned to it.
template <typename Rhs>
constexpr auto do_assignment(inline_string<default_size> lhs, Rhs rhs) -> inline_string<default_size> {
  lhs = rhs;
  return lhs;
}

/// @test Assigning to an `inline_string` from a pointer to a NUL-terminated string that is longer than the capacity of
/// the `inline_string` terminates the program with a precondition violation
TEST(InlineStringAssignmentDeathTest, CStrLongerThanMaxLengthIsPreconditionViolation) {
  ASSERT_DEATH(do_assignment("hello", too_long_ntsv.c_str()), "Precondition");
}
/// @test Assigning to an `inline_string` from a `string_view` that is longer than the capacity of the `inline_string`
/// terminates the program with a precondition violation
TEST(InlineStringAssignmentDeathTest, StringViewLongerThanMaxLengthIsPreconditionViolation) {
  ASSERT_DEATH(do_assignment("hello", too_long_view), "Precondition");
}
/// Constexpr around inline_string<default_size>.resize()
/// @return A copy of the input string with `resize(args..)` called on it.
/// @{
constexpr auto do_resize(inline_string<default_size> str, inline_string<default_size>::size_type new_size)
    -> inline_string<default_size> {
  str.resize(new_size);
  return str;
}
constexpr auto do_resize(inline_string<default_size> str, inline_string<default_size>::size_type new_size, char chr)
    -> inline_string<default_size> {
  str.resize(new_size, chr);
  return str;
}
/// @}

/// @test Invoking `resize` with a size greater than the capacity (with or without a fill character) terminates the
/// program with a precondition violation
TEST(InlineStringResizeDeathTests, ToGreaterThanMaxSizeIsPreconditionViolation) {
  ASSERT_DEATH(do_resize("hello", default_size + 1), "Precondition");
  ASSERT_DEATH(do_resize("hello", default_size + 100), "Precondition");
  ASSERT_DEATH(do_resize("hello", default_size + 1, 'a'), "Precondition");
  ASSERT_DEATH(do_resize("hello", default_size + 100, 'a'), "Precondition");
}

/// @test Invoking `front` on an empty string terminates the program with a precondition violation
TEST(InlineStringFrontDeathTest, OfEmptyStringIsPreconditionViolation) {
  constexpr inline_string<default_size> str{};
  ASSERT_DEATH(std::ignore = str.front(), "!empty()");
}

/// @test Invoking `back` on an empty string terminates the program with a precondition violation
TEST(InlineStringBackDeathTest, OfEmptyStringIsPreconditionViolation) {
  constexpr inline_string<default_size> str{};
  ASSERT_DEATH(std::ignore = str.back(), "!empty()");
}

/// @test Invoking `insert` with an offset and a `string_view` where the offset is beyond the end of the string
/// terminates the program with a precondition violation
TEST(InlineStringInsertDeathTest, BeyondEndIsPreconditionViolation) {
  inline_string<default_size> inline_str{"abcdef"};
  ASSERT_DEATH(inline_str.insert(inline_str.size() + 1, string_view{"XXXXYYYYZZZZ"}), "Precondition");
}

/// @test Invoking `insert` with an offset and a `string_view` where the string view is longer than the available free
/// space terminates the program with a precondition violation
TEST(InlineStringInsertDeathTest, TooLongStringIsPreconditionViolation) {
  inline_string<default_size> inline_str{"abcdef"};
  inline_str.resize(decltype(inline_str)::max_size() - 3, 'f');
  ASSERT_DEATH(inline_str.insert(inline_str.size() - 3, string_view{"XXXXYYYYZZZZ"}), "Precondition");
}

/// @test Invoking `pop_back` on an empty string terminates the program with a precondition violation
TEST(InlineStringPopBackDeathTest, PopbackOnEmptyIsPreconditionViolation) {
  inline_string<default_size> inline_str{};
  ASSERT_DEATH(inline_str.pop_back(), "!empty()");
}

/// @test Invoking `replace` with a position that is beyond the end of the string terminates the program with a
/// precondition violation
TEST(InlineStringReplaceDeathTest, IfPosOutOfRangePreconditionViolation) {
  inline_string<default_size> inline_str{"abcdefg"};
  ASSERT_DEATH(inline_str.replace(inline_str.size() + 1, 2, string_view{"XYZ"}), "Precondition");
}

/// @test Invoking `replace` with arguments such that the resulting string would exceed the capacity terminates the
/// program with a precondition violation
TEST(InlineStringReplaceDeathTest, IfInsertionWouldOverflowIsPreconditionViolation) {
  inline_string<default_size> too_small_str{"abcdefg"};
  too_small_str.resize(decltype(too_small_str)::max_size() - 1, 'f');
  ASSERT_DEATH(too_small_str.replace(too_small_str.max_size() - 1, 2, string_view{"XYZ"}), "Precondition");
}

/// @test Concatenating an `inline_string` and a `string_view` with `+` terminates the program with a precondition
/// violation if the resulting string is longer than the capacity
TEST(InlineStringOperatorPlusDeathTest, WithStringViewIfResultantStringIsOverCapacityIsPreconditionViolation) {
  inline_string<default_size> already_full{};
  already_full.resize(default_size - 2, 'f');
  ASSERT_DEATH(std::ignore = already_full + string_view{" world"}, "Precondition");
  ASSERT_DEATH(std::ignore = string_view{" world"} + already_full, "Precondition");
}

}  // namespace
