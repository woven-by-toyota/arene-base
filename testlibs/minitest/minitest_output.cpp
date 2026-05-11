// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest_output.hpp"

#include "testlibs/minitest/cstring.hpp"
#include "testlibs/minitest/libc.hpp"

namespace {

using ::testing::minitest::print_number;
using ::testing::minitest::print_string;

auto print_xml_escaped_string(testing::minitest::file_handle stream, char const* str) noexcept -> void {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  char temp_string[2];
  temp_string[1] = '\0';

  // https://www.w3.org/TR/xml/#syntax
  while (!::testing::cstring::empty(str)) {
    switch (*str) {
      case '&':
        print_string(stream, "&amp;");
        break;
      case '\'':
        print_string(stream, "&apos;");
        break;
      case '"':
        print_string(stream, "&quot;");
        break;
      case '<':
        print_string(stream, "&lt;");
        break;
      case '>':
        print_string(stream, "&gt;");
        break;
      default:
        temp_string[0] = *str;
        print_string(stream, static_cast<char const*>(temp_string));
        break;
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    ++str;
  }
}

auto print_xml_testcase_attributes(testing::minitest::file_handle stream, testing::test_context const& context) noexcept
    -> void {
  print_string(stream, "testcase name=\"");
  print_xml_escaped_string(stream, context.test_case_name);
  print_string(stream, "\" file=\"");
  print_xml_escaped_string(stream, context.file_name);
  print_string(stream, "\" line=\"");
  print_number(stream, context.line);
  print_string(stream, "\"");

  if (!::testing::cstring::empty(context.type_param_string)) {
    print_string(stream, " type_param=\"");
    print_xml_escaped_string(stream, context.type_param_string);
    print_string(stream, "\"");
  }
}

auto print_failure_header(testing::minitest::file_handle stream, testing::source_location const& location) -> void {
  print_string(stream, location.file);
  print_string(stream, ":");
  print_number(stream, location.line);
  print_string(stream, ": Error: Assertion failed");
}

auto print_failure_content(
    testing::minitest::file_handle stream,
    testing::test_context const& context,
    testing::source_location const& location,
    testing::assertion_strings const& assertion
) -> void {
  print_failure_header(stream, location);
  print_string(stream, " ");
  print_string(stream, assertion.arg1);
  print_string(stream, assertion.description);
  print_string(stream, assertion.arg2);
  print_string(stream, " for ");
  print_string(stream, context.suite_name);
  print_string(stream, "::");
  print_string(stream, context.test_case_name);
  if (!::testing::cstring::empty(context.type_param_string)) {
    print_string(stream, " ");
    print_string(stream, context.type_param_string);
  }
}

auto print_skip_header(testing::minitest::file_handle stream, testing::source_location const& location) -> void {
  print_string(stream, location.file);
  print_string(stream, ":");
  print_number(stream, location.line);
  print_string(stream, ": Skipped");
}

auto print_skip_content(
    testing::minitest::file_handle stream,
    testing::test_context const& context,
    testing::source_location const& location
) -> void {
  print_skip_header(stream, location);
  print_string(stream, " for ");
  print_string(stream, context.suite_name);
  print_string(stream, "::");
  print_string(stream, context.test_case_name);
  if (!::testing::cstring::empty(context.type_param_string)) {
    print_string(stream, " ");
    print_string(stream, context.type_param_string);
  }
}

}  // namespace

auto testing::console_output::failure(
    test_context const& context,
    source_location const& location,
    assertion_strings const& assertion
) -> void {
  print_failure_content(testing::minitest::get_stdout(), context, location, assertion);
  print_string("\n");
}

auto testing::console_output::skip(test_context const& context, source_location const& location) -> void {
  print_skip_content(testing::minitest::get_stdout(), context, location);
  print_string("\n");
}

auto testing::console_output::success(test_context const& context) -> void { static_cast<void>(context); }

auto testing::console_output::prologue() -> void {}

auto testing::console_output::epilogue(::size_t success_count, ::size_t fail_count, ::size_t skip_count) -> void {
  print_number((success_count + fail_count + skip_count));
  print_string(" tests run. ");
  print_number(success_count);
  print_string(" PASSED. ");
  print_number(fail_count);
  print_string(" FAILED");
  if (skip_count > 0UL) {
    print_string(". ");
    print_number(skip_count);
    print_string(" SKIPPED");
  }
  print_string("\n");
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto testing::console_output::suite_change(char const* prev, char const* next) -> void {
  static_cast<void>(prev);
  static_cast<void>(next);
}

auto testing::console_output::flush() -> void { testing::minitest::flush_file(testing::minitest::get_stdout()); }

testing::xml_file_and_console_output::~xml_file_and_console_output() { close_file(); }

auto testing::xml_file_and_console_output::failure(
    test_context const& context,
    source_location const& location,
    assertion_strings const& assertion
) const -> void {
  if (file_) {
    print_string(file_, "<");
    print_xml_testcase_attributes(file_, context);
    print_string(file_, ">\n");

    print_string(file_, "<failure message=\"");
    print_failure_header(file_, location);
    print_string(file_, R"(" type="">)");

    print_string(file_, "<![CDATA[");
    print_failure_content(file_, context, location, assertion);
    print_string(file_, "]]>");

    print_string(file_, "</failure>\n");

    print_string(file_, "</testcase>\n");
  }

  testing::console_output::failure(context, location, assertion);
}

auto testing::xml_file_and_console_output::skip(test_context const& context, source_location const& location) const
    -> void {
  if (file_) {
    print_string(file_, "<");
    print_xml_testcase_attributes(file_, context);
    print_string(file_, ">\n");

    print_string(file_, "<skipped message=\"");
    print_skip_header(file_, location);
    print_string(file_, "\">");

    print_string(file_, "<![CDATA[");
    print_skip_content(file_, context, location);
    print_string(file_, "]]>");

    print_string(file_, "</skipped>\n");

    print_string(file_, "</testcase>\n");
  }

  testing::console_output::skip(context, location);
}

auto testing::xml_file_and_console_output::success(test_context const& context) const -> void {
  if (file_) {
    print_string(file_, "<");
    print_xml_testcase_attributes(file_, context);
    print_string(file_, "/>\n");
  }

  testing::console_output::success(context);
}

auto testing::xml_file_and_console_output::prologue() const -> void {
  if (file_) {
    print_string(file_, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    print_string(file_, "<testsuites name=\"AllTests\">\n");
  }

  testing::console_output::prologue();
}

auto testing::xml_file_and_console_output::epilogue(::size_t success_count, ::size_t fail_count, ::size_t skip_count)
    const -> void {
  if (file_) {
    print_string(file_, "</testsuites>\n");
  }

  testing::console_output::epilogue(success_count, fail_count, skip_count);
}

auto testing::xml_file_and_console_output::suite_change(char const* prev, char const* next) const -> void {
  if (file_) {
    if (!::testing::cstring::empty(prev)) {
      print_string(file_, "</testsuite>\n");
    }

    if (!::testing::cstring::empty(next)) {
      print_string(file_, "<testsuite name=\"");
      print_string(file_, next);
      print_string(file_, "\">\n");
    }
  }

  testing::console_output::suite_change(prev, next);
}

auto testing::xml_file_and_console_output::open_file(char const* filename) -> void {
  close_file();

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  if (auto file = testing::minitest::open_file(filename, "w")) {
    file_ = file;
  } else {
    print_string("ERROR: Unable to open XML_OUTPUT_FILE: ");
    print_string(filename);
    print_string("\n");
  }
}

auto testing::xml_file_and_console_output::close_file() -> void {
  if (file_) {
    testing::minitest::close_file(file_);
  }
}

auto testing::xml_file_and_console_output::flush() -> void {
  if (file_) {
    testing::minitest::flush_file(file_);
  }
  testing::console_output::flush();
}

auto testing::panic(char const* file, line_number line, char const* msg) -> void {
  auto stderr = testing::minitest::get_stderr();
  print_string(stderr, file);
  print_string(stderr, ":");
  print_number(stderr, line);
  print_string(stderr, ": error: MINITEST PANIC [");
  print_string(stderr, msg);
  print_string(stderr, "] \n");
  testing::minitest::abort_program();
}
