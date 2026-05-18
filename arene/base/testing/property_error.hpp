// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_ERROR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_ERROR_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "testlibs/minitest/assertion_strings.hpp"
#include "testlibs/minitest/source_location.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief error result of evaluating a property assertion
///
/// Stores the error status of a property along with contextual information
/// about where the assertion occurred and the textual components of the assertion.
// NOLINTNEXTLINE(hicpp-member-init)
struct property_error {
  enum class error_code : std::uint8_t { failure, discard };

  error_code error;
  ::testing::source_location location;
  ::testing::assertion_strings strings;
};

}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_ERROR_HPP_
