// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts/detail/stacktrace/execinfo/impl.hpp"

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
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::contracts_detail::current_stack_trace_impl;
using ::arene::base::contracts_detail::native_handle_t;
using ::arene::base::contracts_detail::stacktrace_is_noop;
using ::arene::base::contracts_detail::symbolize_stack_trace_to_impl;

/// @test Given the execinfo-based backend, ::arene::base::contracts_detail::native_handle_t is `void*`.
TEST(NativeHandleT, IsVoidPtr) { ::testing::StaticAssertTypeEq<native_handle_t, void*>(); }

/// @test Given the execinfo-based backend, ::arene::base::contracts_detail::stacktrace_is_noop is `false`.
TEST(StacktraceIsNoop, IsFalse) { STATIC_ASSERT_FALSE(stacktrace_is_noop); }

struct TraceFixture : ::testing::Test {
  static constexpr std::size_t max_depth{10U};
  struct trace_entry {
    std::size_t trace_size{};
    std::array<native_handle_t, max_depth> trace{};
  };
};

struct CurrentStackTraceImplTest : TraceFixture {
  template <typename Rng>
  auto subrange_from_till(Rng&& rng, std::size_t start, std::size_t end) {
    return std::vector<native_handle_t>(
        rng.begin() + static_cast<std::ptrdiff_t>(start),
        rng.begin() + static_cast<std::ptrdiff_t>(end)
    );
  }
};

/// @test Given the execinfo-based backend, when `current_stack_trace_impl` is invoked with a null pointer to a buffer
/// and the depth of that buffer, then it is a noop and 0 is returned.
TEST_F(CurrentStackTraceImplTest, IfFramesIsNullptrIsNoop) {
  auto const trace_size = current_stack_trace_impl(nullptr, max_depth, 0U);
  EXPECT_EQ(trace_size, 0);
}

/// @test Given the execinfo-based backend, when `current_stack_trace_impl` is invoked with a valid pointer to a buffer
/// and the depth of that buffer, then the returned value is the count of frames captured, and the entires from the
/// start of the buffer to that offset are all non-null.
TEST_F(CurrentStackTraceImplTest, ReturnsFrameCountCorrespondingToFrameDepth) {
  trace_entry trace;
  trace.trace_size = current_stack_trace_impl(trace.trace.data(), max_depth, 0U);
  EXPECT_GT(trace.trace_size, 0);
  EXPECT_THAT(subrange_from_till(trace.trace, 0U, trace.trace_size), ::testing::Each(::testing::NotNull()));
}

/// @test Given the execinfo-based backend, when `current_stack_trace_impl` is invoked with a valid pointer to a buffer
/// and the depth of that buffer and a number of frames to skip `N>0`, then the returned value is the count of frames
/// captured, and the entires from the start of the buffer to that offset are equal to the subrange of the buffer
/// captured when `N==0` starting at offset `N`.
TEST_F(CurrentStackTraceImplTest, SkippingFramesSkipsTheFirstNFramesInStack) {
  {
    SCOPED_TRACE("0 to N<max_depth frames");
    std::array<trace_entry, max_depth> traces{};
    for (std::size_t num_skips = 0; num_skips < max_depth; ++num_skips) {
      auto& entry = traces.at(num_skips);
      entry.trace_size = current_stack_trace_impl(entry.trace.data(), max_depth, num_skips);

      auto const& baseline = traces.front();
      EXPECT_EQ(entry.trace_size, baseline.trace_size - num_skips);
      EXPECT_THAT(
          subrange_from_till(entry.trace, 0U, entry.trace_size),
          ::testing::ElementsAreArray(subrange_from_till(baseline.trace, num_skips, baseline.trace_size))
      );
    }
  }

  {
    SCOPED_TRACE("N==max_depth frames");
    auto skip_all_frames = std::array<native_handle_t, max_depth>{};
    auto const skip_all_size = current_stack_trace_impl(skip_all_frames.data(), max_depth, max_depth);

    EXPECT_EQ(skip_all_size, 0U);
  }

  {
    SCOPED_TRACE("N>max_depth frames");
    auto skip_more_than_all_frames = std::array<native_handle_t, max_depth>{};
    auto const skip_more_than_all_size =
        current_stack_trace_impl(skip_more_than_all_frames.data(), max_depth, std::numeric_limits<std::size_t>::max());
    EXPECT_EQ(skip_more_than_all_size, 0U);
  }
}

struct SymbolizeStackTraceToImplTest : TraceFixture {
  SymbolizeStackTraceToImplTest() { trace.trace_size = current_stack_trace_impl(trace.trace.data(), max_depth, 0); }
  trace_entry trace;
};

/// @test Given the execinfo-based backend, when `symbolize_stack_trace_to_impl()` is invoked with a `nullptr` file
/// parameter, then it is a noop.
TEST_F(SymbolizeStackTraceToImplTest, IsNoopIfInputFileIsNullptr) {
  EXPECT_NO_FATAL_FAILURE(symbolize_stack_trace_to_impl(nullptr, trace.trace.data(), trace.trace_size));
}

/// @test Given the execinfo-based backend, when `symbolize_stack_trace_to_impl()` is invoked with a `nullptr` file
/// parameter, then it is a noop.
TEST_F(SymbolizeStackTraceToImplTest, IsNoopIfInputFramesIsNullptr) {
  auto const tmp_dir = ::arene::base::testing::filesystem::create_temporary_directory();
  auto const file_path = tmp_dir.path() + "/log";
  auto* file = std::fopen(file_path.c_str(), "wb");
  if (file == nullptr) {
    GTEST_FAIL() << "couldn't open file";
  }
  EXPECT_NO_FATAL_FAILURE(symbolize_stack_trace_to_impl(file, nullptr, trace.trace_size));

  std::fclose(file);
  std::fstream fsfile{file_path.c_str()};
  std::string const symbolized_content{std::istreambuf_iterator<char>{fsfile}, {}};
  EXPECT_THAT(symbolized_content, ::testing::IsEmpty());
}

/// @test Given the execinfo-based backend, when `symbolize_stack_trace_to()` is invoked with a non-empty stacktrace and
/// a valid file pointer, then the file will contain backend-dependent symbolized representation of the input
/// stacktrace.
TEST_F(SymbolizeStackTraceToImplTest, WritesSymbolizedContentToFile) {
  auto const tmp_dir = ::arene::base::testing::filesystem::create_temporary_directory();
  auto const file_path = tmp_dir.path() + "/log";
  auto* file = std::fopen(file_path.c_str(), "wb");
  if (file == nullptr) {
    GTEST_FAIL() << "couldn't open file";
  }

  symbolize_stack_trace_to_impl(file, trace.trace.data(), trace.trace_size);
  std::fclose(file);
  std::fstream fsfile{file_path.c_str()};
  std::string const symbolized_content{std::istreambuf_iterator<char>{fsfile}, {}};
  EXPECT_THAT(symbolized_content, ::testing::ContainsRegex("stacktrace/execinfo/tests"));
}

}  // namespace
