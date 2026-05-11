// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts/detail/stacktrace/stacktrace.hpp"

#include <array>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/filesystem/temporary_directory.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/string_view.hpp"
#include "arene/base/testing/filesystem/create_temporary_directory.hpp"

namespace {

using arene::base::contracts_detail::native_handle_t;
using arene::base::contracts_detail::stacktrace;
using arene::base::contracts_detail::stacktrace_is_noop;

struct TraceFixture : ::testing::Test {};

struct StackTraceCurrentTest : TraceFixture {
  template <typename Rng>
  auto subrange_from_till(Rng&& rng, std::size_t start, std::size_t end) {
    return std::vector<native_handle_t>(
        rng.begin() + static_cast<std::ptrdiff_t>(start),
        rng.begin() + static_cast<std::ptrdiff_t>(end)
    );
  }
};

/// @test Given any backend, when `stacktrace<>::current()` is invoked it is not an error.
TEST_F(StackTraceCurrentTest, CanBeCalledWithoutErrorEvenIfNoop) { EXPECT_NO_FATAL_FAILURE(stacktrace<>::current()); }

/// @test Given any backend, when `stacktrace<>::current()` is invoked, then the returned object's
/// `stacktrace<>::size` corresponds to the stack depth at the site of invocation, and the entires from
/// `stacktrace::begin()` to `stacktrace::end()` are all non-null.
TEST_F(StackTraceCurrentTest, ReturnsFrameCountCorrespondingToFrameDepth) {
  auto const trace = stacktrace<>::current();
  if (!stacktrace_is_noop) {
    EXPECT_GT(trace.size(), 0);
  } else {
    EXPECT_EQ(trace.size(), 0U);
  }
  EXPECT_EQ(trace.size(), std::distance(trace.begin(), trace.end()));
  EXPECT_THAT(trace, ::testing::Each(::testing::NotNull()));
}

/// @test Given any backend, when `stacktrace<>::current()` is invoked with a count of frames to skip, `N>0`,
/// then the returned object is equivalent to the subset of the trace where `N==0` with the first `N` entires skipped.
TEST_F(StackTraceCurrentTest, SkippingFramesSkipsTheFirstNFramesInStack) {
  {
    SCOPED_TRACE("0 to N<max_depth frames");
    std::array<stacktrace<>, stacktrace<>::max_size> traces{};
    for (std::size_t num_skips = 0; num_skips < traces.size(); ++num_skips) {
      auto& trace_entry = traces.at(num_skips);
      trace_entry = stacktrace<>::current(num_skips);

      auto const& baseline = traces.front();
      auto const expected_size = num_skips < baseline.size() ? baseline.size() - num_skips : 0;
      ASSERT_EQ(trace_entry.size(), expected_size);
      EXPECT_THAT(
          trace_entry,
          ::testing::ElementsAreArray(
              subrange_from_till(baseline, std::min(num_skips, baseline.size()), baseline.size())
          )
      );
    }
  }
}

/// @test Given any backend, when `stacktrace<>::current()` is invoked with a count of frames to skip, `N`, if
/// `N` exceeds the actual stack depth than an empty stack trace is returned.
TEST_F(StackTraceCurrentTest, SkippingMoreFramesThanStackDepthResultsInEmptyTrace) {
  {
    SCOPED_TRACE("N==max_depth frames");
    auto const skip_all = stacktrace<>::current(stacktrace<>::max_size);

    EXPECT_THAT(skip_all, ::testing::IsEmpty());
  }

  {
    SCOPED_TRACE("N>max_depth frames");
    auto const skip_more_than_all = stacktrace<>::current(std::numeric_limits<std::size_t>::max());
    EXPECT_THAT(skip_more_than_all, ::testing::IsEmpty());
  }
}

struct SymbolizeStackTraceToTest : TraceFixture {};

/// @test Given any backend, when `symbolize_stack_trace_to()` is invoked with a `nullptr` file parameter, then it is a
/// noop.
TEST_F(SymbolizeStackTraceToTest, IsNoopIfInputFileIsNullptr) {
  auto const trace = stacktrace<>::current();
  EXPECT_NO_FATAL_FAILURE(symbolize_stack_trace_to(nullptr, trace));
}

/// @test Given a non-noop backend, when `symbolize_stack_trace_to()` is invoked with a non-empty stacktrace and a valid
/// file pointer, then the file will contain backend-dependent symbolized representation of the input stacktrace.
TEST_F(SymbolizeStackTraceToTest, WritesSymbolizedContentToFile) {
  if (stacktrace_is_noop) {
    GTEST_SKIP() << "Only meaningful if stacktrace is not a noop";
  }
  auto const tmp_dir = ::arene::base::testing::filesystem::create_temporary_directory();
  auto const file_path = tmp_dir.path() + "/log";
  auto* file = std::fopen(file_path.c_str(), "wb");
  if (file == nullptr) {
    GTEST_FAIL() << "couldn't open file";
  }
  auto const trace = stacktrace<>::current();
  symbolize_stack_trace_to(file, trace);
  std::fclose(file);
  std::fstream fsfile{file_path.c_str()};
  std::string const symbolized_content{std::istreambuf_iterator<char>{fsfile}, {}};
  static_assert(
      arene::base::string_view{__FILE__}.ends_with("stacktrace_test.cpp"),
      "Test assumes that this file is called \"stacktrace_test.cpp\"."
  );
  EXPECT_THAT(symbolized_content, ::testing::ContainsRegex("stacktrace_test"));
}

}  // namespace
