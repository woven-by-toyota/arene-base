// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_UTF_UTF8_VALIDATION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_UTF_UTF8_VALIDATION_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/byte/byte.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/strings/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace utf8_validation_detail {
/// @brief Class to implement a state machine for UTF-8 validation
class validator {
  // parasoft-begin-suppress AUTOSAR-A5_1_1-a	"False positive: these literals constants are used"
  /// @brief The minimum value of a UTF-8 continuation byte
  static constexpr byte min_continuation_byte{to_byte(std::uint8_t{0X80})};

  // parasoft-begin-suppress AUTOSAR-M0_1_3-c	"False positive: 'max_continuation_byte' is used"
  /// @brief The maximum value of a UTF-8 continuation byte
  static constexpr byte max_continuation_byte{to_byte(std::uint8_t{0XBF})};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

  /// @brief The minimum value of a UTF-8 lead byte indicating a valid 2-byte sequence
  static constexpr byte two_byte_initial_smallest{to_byte(std::uint8_t{0XC2})};

  /// @brief The minimum value of a UTF-8 lead byte indicating a 3-byte sequence
  static constexpr byte three_byte_min_initial_byte{to_byte(std::uint8_t{0XE0})};

  /// @brief The minimum value of a UTF-8 lead byte indicating a 3-byte sequence high enough to not be an over-long
  /// encoding
  static constexpr byte three_byte_min_initial_not_overlong{to_byte(std::uint8_t{0XE1})};

  /// @brief The minimum value of a UTF-8 lead byte indicating a 3-byte sequence that could be the start of an encoding
  /// of a UTF-16 surrogate pair (which is invalid UTF-8)
  static constexpr byte three_byte_initial_surrogate_byte{to_byte(std::uint8_t{0XED})};

  /// @brief The minimum value of the second byte of a 3-byte UTF-8 encoding starting with 0XE0 for it not to be an
  /// over-long encoding
  static constexpr byte three_byte_min_valid_second_not_overlong_byte{to_byte(std::uint8_t{0XA0})};

  /// @brief The minimum valid of the second byte of a 3-byte UTF-8 encoding starting with 0XED that would indicate this
  /// is part of an encoding of a UTF-16 surrogate pair (which is invalid UTF-8)
  static constexpr byte three_byte_min_second_surrogate_byte{to_byte(std::uint8_t{0XA0})};

  /// @brief The minumum value of a UTF-8 lead byte indicating a 4-byte sequence
  static constexpr byte four_byte_initial_marker{to_byte(std::uint8_t{0XF0})};
  /// @brief The minumum value of a UTF-8 lead byte indicating a 4-byte sequence
  static constexpr byte four_byte_initial_low_marker{to_byte(std::uint8_t{0XF0})};
  /// @brief The highest valid value of a UTF-8 lead byte for a 4-byte sequence
  static constexpr byte four_byte_initial_high_byte{to_byte(std::uint8_t{0XF4})};
  /// @brief The highest valid value of a UTF-8 lead byte
  static constexpr byte maximum_initial_byte{to_byte(std::uint8_t{0XF4})};
  /// @brief The minimum valid value of the second byte of a UTF-8 4-byte sequence starting with 0XF0 for it not to be
  /// the start of an over-long encoding
  static constexpr byte four_byte_low_min_valid_second_byte{to_byte(std::uint8_t{0X90})};
  // parasoft-end-suppress AUTOSAR-A5_1_1-a

  /// @brief The internal states of the state machine
  enum class state_type : std::uint8_t {
    initial_byte,
    final_byte,
    three_byte_second_high,
    three_byte_second_surrogate,
    three_byte_second_low,
    four_byte_second,
    four_byte_second_low,
    four_byte_second_high,
    four_byte_third,
  };

  /// @brief The current state of the state machine
  state_type state_{state_type::initial_byte};

  /// @brief Check the supplied byte for a valid initial byte in an encoded sequence, and update the state machine
  ///        accordingly.
  /// @param value The byte to check
  /// @return true if @c value is valid as an initial byte
  /// @return false otherwise.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls) : AUTOSAR M3-9-1 requires this to match
  constexpr auto check_initial_byte(byte const value) noexcept -> bool;

 public:
  /// @brief Check the supplied byte to see if it is the valid next byte in the UTF-8 encoded sequence being checked,
  ///        and update the state machine accordingly.
  /// @param value The byte to check
  /// @return true if @c value is valid as the next byte
  /// @return false otherwise.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls) : AUTOSAR M3-9-1 requires this to match
  constexpr auto check_next_byte(byte const value) noexcept -> bool;

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a	"False positive: does return a value"
  // parasoft-begin-suppress CERT_C-MSC37-a	  "False positive: does return a value"
  // parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: does return a value"
  // parasoft-begin-suppress AUTOSAR-M9_3_3-a	"False positive: cannot be 'static', uses non-static data member"
  /// @brief Check if it's OK to end the UTF-8 encoded string after the current
  ///        set of checked bytes.
  /// @return @c true if all bytes processed so far are valid, and the sequence
  /// of validated bytes is not part way through a multi-byte UTF-8 encoded
  /// charpoint, @c false otherwise
  constexpr auto is_valid_end() const noexcept -> bool { return state_ == state_type::initial_byte; }
  // parasoft-end-suppress AUTOSAR-M9_3_3-a
  // parasoft-end-suppress CERT_CPP-MSC52-a
  // parasoft-end-suppress CERT_C-MSC37-a
  // parasoft-end-suppress AUTOSAR-A8_4_2-a
};

/// @brief Check the supplied byte for a valid initial byte in an encoded sequence, and update the state machine
///        accordingly.
/// @param value The byte to check
/// @return true if @c value is valid as an initial byte
/// @return false otherwise.
constexpr auto validator::check_initial_byte(byte const value) noexcept -> bool {
  if (value > maximum_initial_byte) {
    return false;
  }
  if (value == four_byte_initial_high_byte) {
    state_ = state_type::four_byte_second_high;
  } else if (value == four_byte_initial_low_marker) {
    state_ = state_type::four_byte_second_low;
  } else if (value >= four_byte_initial_marker) {
    state_ = state_type::four_byte_second;
  } else if (value == three_byte_initial_surrogate_byte) {
    state_ = state_type::three_byte_second_surrogate;
  } else if (value >= three_byte_min_initial_not_overlong) {
    state_ = state_type::three_byte_second_high;
  } else if (value >= three_byte_min_initial_byte) {
    state_ = state_type::three_byte_second_low;
  } else if (value >= two_byte_initial_smallest) {
    state_ = state_type::final_byte;
  } else if (value >= min_continuation_byte) {
    return false;
  } else {
    ;  // no-op
  }
  return true;
}

/// @brief Check the supplied byte to see if it is the valid next byte in the UTF-8 encoded sequence being checked,
///        and update the state machine accordingly.
/// @param value The byte to check
/// @return true if @c value is valid as the next byte
/// @return false otherwise.
constexpr auto validator::check_next_byte(byte const value) noexcept -> bool {
  if ((state_ != state_type::initial_byte) && ((value < min_continuation_byte) || (value > max_continuation_byte))) {
    return false;
  }

  // parasoft-begin-suppress AUTOSAR-M0_1_1-g	"False positive: no statement or expression is placed outside case and
  // default body"
  //
  // parasoft-begin-suppress AUTOSAR-M6_4_3-d "False positive: there is a case clause in switch statement"
  // parasoft-begin-suppress AUTOSAR-A6_4_1-a "False positive: there are more than two case labels"
  // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated as permitted by M6-4-3 Permit #1"
  // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated as permitted by M6-4-5 Permit #1"
  switch (state_) {
    case state_type::initial_byte:
      return check_initial_byte(value);
    case state_type::three_byte_second_low:
      if (value < three_byte_min_valid_second_not_overlong_byte) {
        return false;
      }
      state_ = state_type::final_byte;
      break;
    case state_type::three_byte_second_surrogate:
      if (value >= three_byte_min_second_surrogate_byte) {
        return false;
      }
      // fall through
    case state_type::three_byte_second_high:
    case state_type::four_byte_third:
      state_ = state_type::final_byte;
      break;
    case state_type::four_byte_second_high:
      if (value >= four_byte_low_min_valid_second_byte) {
        return false;
      }
      state_ = state_type::four_byte_third;
      break;
    case state_type::four_byte_second_low:
      if (value < four_byte_low_min_valid_second_byte) {
        return false;
      }
      // fall through
    case state_type::four_byte_second:
      state_ = state_type::four_byte_third;
      break;
    case state_type::final_byte:
      state_ = state_type::initial_byte;
      break;
    default:
      ARENE_INVARIANT_UNREACHABLE("Corrupt UTF-8 validator");
  }
  // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
  // parasoft-end-suppress AUTOSAR-A6_4_1-a
  // parasoft-end-suppress AUTOSAR-M6_4_3-d
  // parasoft-end-suppress AUTOSAR-M0_1_1-g
  return true;
}
}  // namespace utf8_validation_detail

// parasoft-begin-suppress AUTOSAR-A8_4_2-a "False positive: does return a value"
// parasoft-begin-suppress CERT_C-MSC37-a   "False positive: does return a value"
// parasoft-begin-suppress CERT_CPP-MSC52-a "False positive: does return a value"
/// @brief Check if a string is valid UTF-8.
///
/// This rejects any incomplete encoding sequences, any overlong encodings (e.g. 7 bit encoded in 4 bytes), any
/// encodings of UTF-16 surrogate pair code points, and any encoding that would decode to a codepoint beyond the max
/// Unicode codepoint of 0X10FFFF.
///
/// @param str The string to check.
/// @return true if the string is valid UTF-8
/// @return false otherwise
inline constexpr auto is_valid_utf8(string_view str) noexcept -> bool {
  utf8_validation_detail::validator validator;
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b	"False positive: This represents a character not a number"
  for (char const chr : str) {
    if (!validator.check_next_byte(to_byte(static_cast<unsigned char>(chr)))) {
      return false;
    }
  }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b
  return validator.is_valid_end();
}
// parasoft-end-suppress AUTOSAR-A8_4_2-a
// parasoft-end-suppress CERT_C-MSC37-a
// parasoft-end-suppress CERT_CPP-MSC52-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_UTF_UTF8_VALIDATION_HPP_
