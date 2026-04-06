// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts/detail/stacktrace/noop/impl.hpp"

#include <array>
#include <cstdio>
#include <utility>

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

/// @test Given the noop backend, ::arene::base::contracts_detail::native_handle_t is `void*`.
TEST(NativeHandleT, IsVoidPtr) { ::testing::StaticAssertTypeEq<native_handle_t, void*>(); }

/// @test Given the noop backend, ::arene::base::contracts_detail::stacktrace_is_noop is `true`.
TEST(StacktraceIsNoop, IsTrue) { STATIC_ASSERT_TRUE(stacktrace_is_noop); }

struct TraceFixture : ::testing::Test {
  static constexpr std::size_t max_depth{10U};
  std::array<native_handle_t, max_depth> trace{};
};

struct CurrentStackTraceImplTest : TraceFixture {};

/// @test Given the noop backend, when `current_stack_trace_impl` is invoked the return is always `0`, and the input
/// buffer is unmodified.
TEST_F(CurrentStackTraceImplTest, AlwaysReturnsEmptyTrace) {
  auto const frames_captured = current_stack_trace_impl(trace.data(), max_depth, 0U);
  EXPECT_EQ(frames_captured, 0U);
  EXPECT_THAT(trace, ::testing::Each(::testing::Eq(nullptr)));
}

struct SymbolizeStacktraceToImplTest : TraceFixture {};

/// @test Given the noop backend, when `symbolize_stack_trace_to_impl()` is invoked with a `nullptr` file
/// parameter, then it is a noop.
TEST_F(SymbolizeStacktraceToImplTest, IsNoopIfInputFileIsNullptr) {
  EXPECT_NO_FATAL_FAILURE(symbolize_stack_trace_to_impl(nullptr, trace.data(), max_depth));
}

/// @test Given the noop backend, when `symbolize_stack_trace_to_impl` is invoked with a valid file and some trace data,
/// the result is an unmodified file.
TEST_F(SymbolizeStacktraceToImplTest, DoesNothing) {
  auto const tmp_dir = ::arene::base::testing::filesystem::create_temporary_directory();
  auto const file_path = tmp_dir.path() + "/log";
  auto* file = std::fopen(file_path.c_str(), "w");
  if (file == nullptr) {
    GTEST_FAIL() << "couldn't open file";
  }
  // Symbolizers generally write out "no symbol found" for invalid symbols, so giving it 10 nullptrs should be fine to
  // validate the symbolizer isn't actually symbolizing.
  symbolize_stack_trace_to_impl(file, trace.data(), max_depth);
  std::fseek(file, 0, SEEK_END);
  EXPECT_EQ(std::ftell(file), 0);
  std::fclose(file);
}

}  // namespace
