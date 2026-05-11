// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_TESTS_SPAN_TEST_EQUIVALENT_SPAN_TO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_TESTS_SPAN_TEST_EQUIVALENT_SPAN_TO_HPP_

#include <gmock/gmock.h>

#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace arene {
namespace base {
namespace tests {
namespace span_test {

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
MATCHER_P(EqDataRange, expected, "size and data are equal") {  //
  return arg.size() == expected.size() &&                      //
         arg.data() == expected.data();
}

/// @brief Matcher to check if two spans are "equivalent", since spans lack comparison operators.
/// @param expected the expected span
/// @return A matcher which will will pass if the input value's @c data() and @c size() are equal to @c expected 's.
template <typename T, std::size_t Extent>
inline auto equivalent_span_to(span<T, Extent> const& expected) {
  return EqDataRange(expected);
}

}  // namespace span_test
}  // namespace tests
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SPAN_TESTS_SPAN_TEST_EQUIVALENT_SPAN_TO_HPP_
