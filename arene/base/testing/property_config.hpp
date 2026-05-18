// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_CONFIG_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_CONFIG_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief configuration for property run
///
/// Controls how many successful trials must be observed before a property
/// is considered to hold, how many discarded trials are tolerated before
/// giving up, and the seed used to initialize the property's PRNG.
struct property_config {
  std::size_t required_successes;
  std::size_t max_discards;
  std::uint64_t seed;
};

}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_CONFIG_HPP_
