// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_STDEXCEPT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_STDEXCEPT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: all functions have trailing return specifiers."

// IWYU pragma: private, include <stdexcept>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/detail/raw_c_string.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
namespace std {

namespace stdexcept_detail {

///
/// @brief A simple string class that manages a static storage size C-style string.
/// @warning This class is intended only for the narrow usecase of being used inside an exception type, where it is
/// guaranteed there will always be some message to store. It does not handle construction from nullptr.
///
class simple_string {
  /// @brief the maximum length of the string
  static constexpr std::size_t storage_size{256U};

  // parasoft-begin-suppress AUTOSAR-A18_1_1-a "internal stdlib has no std::array backport."
  /// @brief The internal storage for the string data.
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) needs to work without any extra dependencies.
  arene::base::detail::character data_[storage_size]{};
  // parasoft-end-suppress AUTOSAR-A18_1_1-a

 public:
  /// @brief Construct a new simple string object
  /// @param str The data to initialize the string with. It will be copied into this string.
  /// @post @c c_str() will return a cstring for which @c strcmp(c_str(),str)==0 .
  // parasoft-begin-suppress AUTOSAR-A3_1_5-a "This function is intended to be inlined"
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  explicit simple_string(arene::base::detail::raw_c_string const str) noexcept {
    // parasoft-end-suppress AUTOSAR-A7_1_3-a
    // parasoft-end-suppress AUTOSAR-A3_1_5-a
    // Note that this stops 1 character before storage_size so that data_ will be null-terminated.
    for (std::size_t copied_count{}; copied_count + 1U < storage_size; ++copied_count) {
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "Either indexing or pointer arithmetic is necessary here"
      if (str[copied_count] == '\0') {
        break;
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) Needs either indexing or pointer arithmetic
      data_[copied_count] = str[copied_count];
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
    }

    // The rest of data_ is already initialized to '\0' so there's no need to fill in the rest of it.
  }
  /// @brief dtor
  ~simple_string() noexcept = default;
  /// @brief copy ctor
  /// @param other The other string to copy from
  /// @post @c c_str() will return a cstring for which @c strcmp(c_str(),other.c_str())==0 .
  simple_string(simple_string const& other) noexcept = default;
  /// @brief copy assignment
  /// @param other The other string to copy from
  /// @return simple_string& @c *this post assignment
  /// @post @c c_str() will return a cstring for which @c strcmp(c_str(),other.c_str())==0 .
  auto operator=(simple_string const& other) noexcept -> simple_string& = default;
  /// @brief move ctor
  /// @param other The other string to move from
  /// @post @c c_str() will return a cstring for which @c strcmp(c_str(),other.c_str())==0 with @c other 's state before
  ///       move. @c other 's state post-move is unspecified.
  simple_string(simple_string&& other) noexcept = default;
  /// @brief move assignment
  /// @param other The other string to move from
  /// @return simple_string& @c *this post assignment
  /// @post @c c_str() will return a cstring for which @c strcmp(c_str(),other.c_str())==0 with @c other 's state before
  ///       move. @c other 's state post-move is unspecified.
  auto operator=(simple_string&& other) noexcept -> simple_string& = default;
  /// @brief Returns a pointer to the internal C-style string.
  /// @return arene::base::detail::raw_c_string A pointer to the internal C-style string.
  auto c_str() const noexcept -> arene::base::detail::raw_c_string {
    return static_cast<arene::base::detail::raw_c_string>(data_);
  }
};
}  // namespace stdexcept_detail

// parasoft-begin-suppress AUTOSAR-A12_8_6-a "Public assignment operators are required by the C++ standard for these
// types."

/// @brief The base class of all exceptions in the stdlib.
class exception {
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"
  /// @brief The default @c what() message.
  static constexpr arene::base::detail::raw_c_string default_exception_message{"std::exception"};
  // parasoft-end-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"

 public:
  /// @brief default ctor
  /// @post @c what() returns an implementation defined message.
  exception() = default;
  /// @brief default move ctor
  /// @param move The rvalue reference to move from.
  exception(exception&& move) = default;
  /// @brief default move assignment
  /// @param move The rvalue reference to move from.
  auto operator=(exception&& move) -> exception& = default;
  /// @brief default copy ctor
  /// @param copy The lvalue reference to copy from.
  exception(exception const& copy) = default;
  /// @brief default copy assignment
  /// @param copy The lvalue reference to copy from.
  auto operator=(exception const& copy) -> exception& = default;
  /// @brief default dtor
  virtual ~exception() = default;
  /// @brief Access the exception's message.
  /// @return arene::base::detail::raw_c_string A c-string containing the message content.
  virtual auto what() const noexcept -> arene::base::detail::raw_c_string { return default_exception_message; }
};

/// @brief An exception base class dealing with logic errors, which typically are programming defects.
/// @see https://en.cppreference.com/w/cpp/error/logic_error.html
class logic_error : exception {
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"
  /// @brief The default @c what() message.
  static constexpr arene::base::detail::raw_c_string default_logic_error_message{"std::logic_error"};
  // parasoft-end-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"

  /// @brief Storage for the message held in the exception.
  stdexcept_detail::simple_string message_{default_logic_error_message};

 public:
  /// @brief default ctor
  /// @post @c what() returns @c "std::logic_error" .
  logic_error() = default;
  /// @brief Construct a new logic error object
  /// @param msg The message to hold in the exception.
  /// @post @c strcmp(what(),msg)==0
  explicit logic_error(arene::base::detail::raw_c_string const msg) noexcept
      : exception(),
        message_((msg != nullptr) ? msg : default_logic_error_message) {}
  /// @brief default dtor
  ~logic_error() override = default;
  /// @brief default move ctor
  /// @param move The rvalue reference to move from.
  logic_error(logic_error&& move) = default;
  /// @brief default copy ctor
  /// @param copy The lvalue reference to copy from.
  logic_error(logic_error const& copy) = default;
  /// @brief default move assignment
  /// @param move The rvalue reference to move from.
  auto operator=(logic_error&& move) -> logic_error& = default;
  /// @brief default copy assignment
  /// @param copy The lvalue reference to copy from.
  auto operator=(logic_error const& copy) -> logic_error& = default;
  /// @brief Access the exception's message.
  /// @return arene::base::detail::raw_c_string A c-string containing the message content.
  auto what() const noexcept -> arene::base::detail::raw_c_string override { return message_.c_str(); }
};

/// @brief An exception type representing a failure to validate bounds preconditions on a function call.
/// @see https://en.cppreference.com/w/cpp/error/out_of_range.html
class out_of_range : logic_error {
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"
  /// @brief The default @c what() message.
  static constexpr arene::base::detail::raw_c_string default_out_of_range_message{"std::out_of_range"};
  // parasoft-end-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"

 public:
  /// @brief default ctor
  /// @post @c what() returns @c "std::out_of_range" .
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: it delegates to another constructor."
  out_of_range() noexcept
      : out_of_range(default_out_of_range_message) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a "False positive: it delegates to another constructor."
  /// @brief Construct a new logic error object
  /// @param msg The message to hold in the exception.
  /// @post @c strcmp(what(),msg)==0
  explicit out_of_range(arene::base::detail::raw_c_string const msg) noexcept
      : logic_error((msg != nullptr) ? msg : default_out_of_range_message) {}
  /// @brief default dtor
  ~out_of_range() override = default;
  /// @brief default move ctor
  /// @param move The rvalue reference to move from.
  out_of_range(out_of_range&& move) = default;
  /// @brief default copy ctor
  /// @param copy The lvalue reference to copy from.
  out_of_range(out_of_range const& copy) = default;
  /// @brief default move assignment
  /// @param move The rvalue reference to move from.
  auto operator=(out_of_range&& move) -> out_of_range& = default;
  /// @brief default copy assignment
  /// @param copy The lvalue reference to copy from.
  auto operator=(out_of_range const& copy) -> out_of_range& = default;

  /// @brief alias in underlying @c what()
  using logic_error::what;
};

/// @brief An exception type representing a failure at runtime that is generally not representative of a programming
/// defect.
/// @see https://en.cppreference.com/w/cpp/error/runtime_error.html
class runtime_error : exception {
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"
  /// @brief The default error message
  static constexpr arene::base::detail::raw_c_string default_runtime_error_message{"std::runtime_error"};
  // parasoft-end-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"
  /// @brief Storage for the message held in the exception.
  stdexcept_detail::simple_string message_{default_runtime_error_message};

 public:
  /// @brief default ctor
  /// @post @c what() returns @c "std::runtime_error" .
  runtime_error() = default;
  /// @brief Construct a new runtime error object
  /// @param msg The message to hold in the exception.
  /// @post @c strcmp(what(),msg)==0
  explicit runtime_error(arene::base::detail::raw_c_string const msg) noexcept
      : exception(),
        message_((msg != nullptr) ? msg : default_runtime_error_message) {}
  /// @brief default dtor
  ~runtime_error() override = default;
  /// @brief default move ctor
  /// @param move The rvalue reference to move from.
  runtime_error(runtime_error&& move) = default;
  /// @brief default copy ctor
  /// @param copy The lvalue reference to copy from.
  runtime_error(runtime_error const& copy) = default;
  /// @brief default move assignment
  /// @param move The rvalue reference to move from.
  auto operator=(runtime_error&& move) -> runtime_error& = default;
  /// @brief default copy assignment
  /// @param copy The lvalue reference to copy from.
  auto operator=(runtime_error const& copy) -> runtime_error& = default;
  /// @brief Access the exception's message.
  /// @return arene::base::detail::raw_c_string A c-string containing the message content.
  auto what() const noexcept -> arene::base::detail::raw_c_string override { return message_.c_str(); }
};

// parasoft-end-suppress AUTOSAR-A12_8_6-a "Public assignment operators are required by the C++ standard for these
// types."

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_STDEXCEPT_HPP_
