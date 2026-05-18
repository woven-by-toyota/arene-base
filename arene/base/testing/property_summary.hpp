// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_SUMMARY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_SUMMARY_HPP_

#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/property_error.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief result of a property run
///
/// Aggregates the statistics gathered during a property run along with
/// the first counterexample encountered, if any.
struct property_summary {
  std::size_t successes;
  std::size_t discards;
  bool failed;
  std::uint64_t seed;
  optional<property_error> first_failure;
};

}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_SUMMARY_HPP_
