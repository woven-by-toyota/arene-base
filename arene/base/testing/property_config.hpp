// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_CONFIG_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_CONFIG_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief environment variable name controlling @c property_config::required_successes
constexpr auto required_successes_env_name =
    arene::base::null_terminated_string_view{"ARENE_PROP_PARAM_REQUIRED_SUCCESSES"};
/// @brief environment variable name controlling @c property_config::max_discards
constexpr auto max_discards_env_name = arene::base::null_terminated_string_view{"ARENE_PROP_PARAM_MAX_DISCARDS"};
/// @brief environment variable name controlling @c property_config::seed
constexpr auto seed_env_name = arene::base::null_terminated_string_view{"ARENE_PROP_PARAM_SEED"};

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

/// @brief obtain the runtime configuration for property-based tests
/// @return a @c property_config used by @c run_property
///
/// Values are read from these environment variables (as unsigned decimal
/// integers):
/// * @c ARENE_PROP_PARAM_REQUIRED_SUCCESSES controls @c required_successes
///   (default @c 100)
/// * @c ARENE_PROP_PARAM_MAX_DISCARDS controls @c max_discards (default
///   @c 10)
/// * @c ARENE_PROP_PARAM_SEED controls @c seed (default @c 0)
///
/// When a variable is unset or fails to parse as a decimal unsigned 64-bit
/// integer, the default is used.
auto read_property_config() noexcept -> property_config;

}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_CONFIG_HPP_
