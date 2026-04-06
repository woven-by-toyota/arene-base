// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_OUTPUT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_OUTPUT_HPP_

// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stddef.h>

#include "testlibs/minitest/libc.hpp"
#include "testlibs/minitest/utility_types.hpp"  // IWYU pragma: export

namespace testing {

[[noreturn]] auto panic(char const* file, line_number line, char const* msg) -> void;

/// @brief write output to stdout
class console_output {
 public:
  static auto failure(test_context const& context, source_location const& location, assertion_strings const& assertion)
      -> void;
  static auto skip(test_context const& context, source_location const& location) -> void;
  static auto success(test_context const& context) -> void;
  static auto prologue() -> void;
  static auto epilogue(::size_t success_count, ::size_t fail_count, ::size_t skip_count) -> void;
  static auto suite_change(char const* previous, char const* next) -> void;
  static auto flush() -> void;
};

/// @brief write output to an xml file *and also* to stdout
class xml_file_and_console_output {
  minitest::file_handle file_{nullptr};

 public:
  ~xml_file_and_console_output();
  xml_file_and_console_output() = default;

  xml_file_and_console_output(xml_file_and_console_output const&) = delete;
  xml_file_and_console_output(xml_file_and_console_output&&) = delete;
  auto operator=(xml_file_and_console_output const&) -> xml_file_and_console_output& = delete;
  auto operator=(xml_file_and_console_output&&) -> xml_file_and_console_output& = delete;

  auto failure(test_context const& context, source_location const& location, assertion_strings const& assertion) const
      -> void;

  auto skip(test_context const& context, source_location const& location) const -> void;

  auto success(test_context const& context) const -> void;

  auto prologue() const -> void;

  auto epilogue(::size_t success_count, ::size_t fail_count, ::size_t skip_count) const -> void;

  auto suite_change(char const* prev, char const* next) const -> void;

  auto open_file(char const* filename) -> void;

  auto close_file() -> void;

  auto flush() -> void;
};

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_OUTPUT_HPP_
