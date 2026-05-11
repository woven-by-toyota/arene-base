// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdexcept>

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/inline_string_reference.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using ::arene::base::const_inline_string_reference;
using ::arene::base::inline_string;
using ::arene::base::inline_string_reference;
using ::arene::base::is_invocable_v;

// A simple default size to use in tests
constexpr std::size_t default_size = 123;

/// @test Given a non-const `inline_string_reference`, then `at` is invocable with `inline_string_reference::size_type`.
TEST(InlineStringRefAt, IsNonConstInvocable) {
  STATIC_ASSERT_TRUE((is_invocable_v<
                      decltype(&inline_string_reference::at<>),
                      inline_string_reference&,
                      inline_string_reference::size_type>));
}

/// @test Given a const `inline_string_reference`, then `at` is invocable with `inline_string_reference::size_type`.
TEST(InlineStringRefAt, IsConstInvocable) {
  STATIC_ASSERT_TRUE((is_invocable_v<
                      decltype(&inline_string_reference::at<>),
                      inline_string_reference const&,
                      inline_string_reference::size_type>));
}

/// @test Given `inline_string_ref ref`, when `ref.at(idx)` is invoked with `idx<ref.size()`, then the result is
/// equivalent to `ref[idx]`.
CONSTEXPR_TEST(InlineStringRefAt, IndexInRangeIsEquivalentToIndexOperator) {
  inline_string<default_size> str{"hello"};
  inline_string_reference const ref(str);
  CONSTEXPR_ASSERT_EQ(ref.at(0), ref[0]);
  CONSTEXPR_ASSERT_EQ(ref.at(2), ref[2]);
  CONSTEXPR_ASSERT_EQ(ref.at(ref.size() - 1), ref[ref.size() - 1]);

  ref.at(0) = 'j';
  CONSTEXPR_ASSERT_EQ(ref.at(0), ref[0]);
}

/// @test Given `inline_string_ref ref`, when `ref.at(idx)` is invoked with `idx>=ref.size()`, then `std::out_of_range`
/// is thrown.
TEST(InlineStringRefAt, IndexOutOfRangeThrows) {
  inline_string<default_size> str{"hello"};
  inline_string_reference const ref(str);
  ASSERT_THROW(ref.at(ref.size()), std::out_of_range);
  ASSERT_THROW(ref.at(ref.size() + 1), std::out_of_range);
}

/// @test Given a non-const `const_inline_string_reference`, then `at` is invocable with
/// `inline_string_reference::size_type`.
TEST(ConstInlineStringRefAt, IsNonConstInvocable) {
  STATIC_ASSERT_TRUE((is_invocable_v<
                      decltype(&const_inline_string_reference::at<>),
                      const_inline_string_reference&,
                      const_inline_string_reference::size_type>));
}

/// @test Given a const `const_inline_string_reference`, then `at` is invocable with
/// `inline_string_reference::size_type`.
TEST(ConstInlineStringRefAt, IsConstInvocable) {
  STATIC_ASSERT_TRUE((is_invocable_v<
                      decltype(&const_inline_string_reference::at<>),
                      const_inline_string_reference const&,
                      const_inline_string_reference::size_type>));
}

/// @test Given `const_inline_string_ref ref`, when `ref.at(idx)` is invoked with `idx<ref.size()`, then the result is
/// equivalent to `ref[idx]`.
CONSTEXPR_TEST(ConstInlineStringRefAt, IndexInRangeIsEquivalentToIndexOperator) {
  inline_string<default_size> const str{"hello"};
  const_inline_string_reference const ref(str);
  CONSTEXPR_ASSERT_EQ(ref.at(0), ref[0]);
  CONSTEXPR_ASSERT_EQ(ref.at(2), ref[2]);
  CONSTEXPR_ASSERT_EQ(ref.at(ref.size() - 1), ref[ref.size() - 1]);
}

/// @test Given `const_inline_string_ref ref`, when `ref.at(idx)` is invoked with `idx>=ref.size()`, then
/// `std::out_of_range` is thrown.
TEST(ConstInlineStringRefAt, IndexOutOfRangeThrows) {
  inline_string<default_size> const str{"hello"};
  const_inline_string_reference const ref(str);
  ASSERT_THROW(ref.at(ref.size()), std::out_of_range);
  ASSERT_THROW(ref.at(ref.size() + 1), std::out_of_range);
}

}  // namespace
