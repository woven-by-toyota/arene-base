// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/filesystem/detail/encode_counter.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/compare.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/span.hpp"
#include "arene/base/string_view.hpp"

namespace {

using ::arene::base::inline_string;
using ::arene::base::string_view;
using ::arene::base::filesystem::detail::encode_counter;

using namespace ::arene::base::literals;  // NOLINT(google-build-using-namespace) literals are excepted

constexpr std::size_t valid_string_size = 6;
using valid_string_buffer = inline_string<valid_string_size>;

/// Simple wrapper to hold an expected and actual couplet
// NOLINTNEXTLINE(bugprone-exception-escape)
class values_t {
 public:
  values_t(std::uint32_t value, string_view expected)
      : value_(value),
        expected_(expected) {}

  auto value() const noexcept { return value_; }
  auto expected() const noexcept -> auto const& { return expected_; }

  friend auto operator<<(std::ostream& lhs, values_t const& rhs) -> std::ostream& {
    return lhs << "value: " << rhs.value() << ", expected: '" << rhs.expected().c_str() << "'";
  }

 private:
  std::uint32_t value_;
  valid_string_buffer expected_;
};

class EncodeCounterTest : public ::testing::TestWithParam<values_t> {};

/// @test `encode_counter` writes a base-64-encoded value of the counter to the span
TEST_P(EncodeCounterTest, WritesBase64EncodedValueToInputSpan) {
  valid_string_buffer output_buffer;
  output_buffer.resize(valid_string_size);
  encode_counter(GetParam().value(), output_buffer);
  ASSERT_EQ(output_buffer, GetParam().expected());
}

INSTANTIATE_TEST_SUITE_P(
    EncodeCounterTestValidBuffer,
    EncodeCounterTest,
    ::testing::Values(
        values_t{0, "000000"_asv},
        values_t{1, "100000"_asv},
        values_t{10, "a00000"_asv},
        values_t{63, "-00000"_asv},
        values_t{64, "010000"_asv},
        values_t{64 * 64 + 10, "a01000"_asv},
        values_t{std::numeric_limits<std::uint32_t>::max(), "-----3"_asv}
    )
);

using too_small_string_buffer = inline_string<5>;
using too_large_string_buffer = inline_string<7>;

/// @test If the buffer passed to `encode_counter` is too small, then the program is terminated with a precondition
/// violation
TEST(EncodeCounterDeathTest, TooSmallBufferIsPreconditionViolation) {
  too_small_string_buffer output_buffer;
  ASSERT_DEATH(encode_counter(0, output_buffer), "Precondition");
}

/// @test If the buffer passed to `encode_counter` is too large, then the program is terminated with a precondition
/// violation
TEST(EncodeCounterDeathTest, TooLargeBufferIsPreconditionViolation) {
  too_large_string_buffer output_buffer;
  ASSERT_DEATH(encode_counter(0, output_buffer), "Precondition");
}

}  // namespace
