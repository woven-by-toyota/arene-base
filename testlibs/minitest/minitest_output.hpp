// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_OUTPUT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_OUTPUT_HPP_

// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stddef.h>

#include "testlibs/minitest/assertion_strings.hpp"  // IWYU pragma: export
#include "testlibs/minitest/libc.hpp"
#include "testlibs/minitest/source_location.hpp"  // IWYU pragma: export
#include "testlibs/minitest/test_context.hpp"     // IWYU pragma: export

namespace testing {

[[noreturn]] auto panic(char const* file, line_number line, char const* msg) -> void;

class output {
 public:
  output() = default;
  output(output const&) = delete;
  output(output&&) = delete;
  auto operator=(output const&) -> output& = delete;
  auto operator=(output&&) -> output& = delete;
  virtual ~output() = default;

  virtual auto failure(test_context const& context, source_location const& loc, assertion_strings const& assertion)
      -> void = 0;
  virtual auto skip(test_context const& context, source_location const& loc) -> void = 0;
  virtual auto success(test_context const& context) -> void = 0;

  virtual auto prologue() -> void = 0;
  virtual auto epilogue(::size_t success_count, ::size_t fail_count, ::size_t skip_count) -> void = 0;

  virtual auto suite_change(char const* previous_suite, char const* current_suite) -> void = 0;

  virtual auto flush() -> void = 0;
};

/// @brief write output to stdout
class console_output : public output {
 public:
  console_output() = default;
  console_output(console_output const&) = delete;
  console_output(console_output&&) = delete;
  auto operator=(console_output const&) -> console_output& = delete;
  auto operator=(console_output&&) -> console_output& = delete;
  ~console_output() override = default;

  auto failure(test_context const& context, source_location const& location, assertion_strings const& assertion)
      -> void override;
  auto skip(test_context const& context, source_location const& location) -> void override;
  auto success(test_context const& context) -> void override;
  auto prologue() -> void override;
  auto epilogue(::size_t success_count, ::size_t fail_count, ::size_t skip_count) -> void override;
  auto suite_change(char const* previous, char const* next) -> void override;
  auto flush() -> void override;
};

/// @brief write output to an xml file *and also* to stdout
class xml_file_and_console_output : public console_output {
  minitest::file_handle file_{nullptr};

 public:
  ~xml_file_and_console_output() override;
  xml_file_and_console_output() = default;

  xml_file_and_console_output(xml_file_and_console_output const&) = delete;
  xml_file_and_console_output(xml_file_and_console_output&&) = delete;
  auto operator=(xml_file_and_console_output const&) -> xml_file_and_console_output& = delete;
  auto operator=(xml_file_and_console_output&&) -> xml_file_and_console_output& = delete;

  auto failure(test_context const& context, source_location const& location, assertion_strings const& assertion)
      -> void override;
  auto skip(test_context const& context, source_location const& location) -> void override;
  auto success(test_context const& context) -> void override;
  auto prologue() -> void override;
  auto epilogue(::size_t success_count, ::size_t fail_count, ::size_t skip_count) -> void override;
  auto suite_change(char const* prev, char const* next) -> void override;
  auto flush() -> void override;

  auto open_file(char const* filename) -> void;
  auto close_file() -> void;
};

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_MINITEST_OUTPUT_HPP_
