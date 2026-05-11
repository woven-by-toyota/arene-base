// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

#include "testlibs/minitest/cstring.hpp"

// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stddef.h>

#include "testlibs/minitest/libc.hpp"
#include "testlibs/minitest/minitest_output.hpp"

namespace testing {

struct typed_test_pos_check_dummy {};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
test_base* test_base::first_test = nullptr;

namespace {

enum class test_status { passed, skipped, failed };

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
test_status current_test_status = test_status::passed;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
test_context current_context{};

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects,cppcoreguidelines-avoid-non-const-global-variables)
xml_file_and_console_output output{};
}  // namespace

auto assertion_failure(char const* file, line_number line, assertion_strings const& assertion) noexcept -> bool {
  current_test_status = test_status::failed;

  output.failure(current_context, source_location{file, line}, assertion);
  return true;
}

auto skip_test(char const* file, line_number line) noexcept -> bool {
  current_test_status = test_status::skipped;

  output.skip(current_context, source_location{file, line});
  return true;
}

auto test_base::run_tests() -> bool {
  output.prologue();

  test_context previous_context{};
  ::size_t success_count{0};
  ::size_t skip_count{0};
  ::size_t fail_count{0};
  auto* test = first_test;

  while (test != nullptr) {
    current_context = test->get_context();

    if (!cstring::equal(previous_context.suite_name, current_context.suite_name)) {
      output.suite_change(previous_context.suite_name, current_context.suite_name);
    }

    current_test_status = test_status::passed;

#ifdef __EXCEPTIONS
    try {
      test->SetUp();
      test->do_test();
      test->TearDown();
    } catch (...) {
      assertion_failure(__FILE__, __LINE__, {"", "Uncaught exception thrown from test body"});
    }
#else
    test->SetUp();
    test->do_test();
    test->TearDown();
#endif

    switch (current_test_status) {
      case test_status::passed:
        ++success_count;
        output.success(current_context);
        break;
      case test_status::skipped:
        ++skip_count;
        break;
      case test_status::failed:
        ++fail_count;
        break;
    }

    test = test->next_;
    previous_context = current_context;
  }

  output.suite_change(previous_context.suite_name, "");
  output.epilogue(success_count, fail_count, skip_count);
  return fail_count == 0;
}

auto test_base::flush_output() noexcept -> void { output.flush(); }

}  // namespace testing

auto main() -> int {
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  if (auto const* path = ::testing::minitest::get_env_var("XML_OUTPUT_FILE")) {
    ::testing::output.open_file(path);
  }

  if (!::testing::test_base::run_tests()) {
    ::testing::minitest::exit_program(1);
  }
}
