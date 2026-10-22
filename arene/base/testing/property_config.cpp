// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/property_config.hpp"

#include "arene/base/contracts/contract.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/strings/string_view.hpp"

// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdlib.h>

namespace arene {
namespace base {
namespace testing {

namespace {

constexpr auto default_required_successes = std::size_t{5000U};
constexpr auto default_max_discards = std::size_t{100U};
constexpr auto default_seed = std::uint64_t{0U};

/// @brief thin wrapper around @c ::getenv
/// @param name environment variable name (must be null-terminated, which
///     @c null_terminated_string_view guarantees)
/// @return value as a @c string_view, or @c nullopt if unset
auto read_env(arene::base::null_terminated_string_view name) noexcept
    -> arene::base::optional<arene::base::string_view> {
  char const* const raw = ::getenv(name.c_str());  // NOLINT(concurrency-mt-unsafe)
  if (raw == nullptr) {
    return arene::base::nullopt;
  }
  return arene::base::optional<arene::base::string_view>{arene::base::string_view{raw}};
}

/// @brief parse a decimal ASCII string into a @c std::uint64_t
/// @param text string to parse
/// @return the parsed value
/// @pre The string must not be empty, must consist of only characters '0' through '9', and must not have a leading '0'
/// unless the string is exactly "0". The value represented by the string must not exceed @c
/// std::numeric_limits<std::uint64_t>::max().
constexpr auto parse_uint64_decimal(arene::base::string_view text) noexcept -> std::uint64_t {
  constexpr auto decimal_base = std::uint64_t{10U};

  ARENE_PRECONDITION(!text.empty());
  ARENE_PRECONDITION((text.size() == 1U) || (text.front() != '0'));

  constexpr auto max_value = std::numeric_limits<std::uint64_t>::max();
  auto value = std::uint64_t{0U};

  for (auto const character : text) {
    ARENE_PRECONDITION((character >= '0') && (character <= '9'));

    auto const digit = static_cast<std::uint64_t>(character - '0');
    ARENE_PRECONDITION(value <= ((max_value - digit) / decimal_base));
    value = (value * decimal_base) + digit;
  }

  return value;
}

/// @brief narrow a @c std::uint64_t to @c std::size_t
/// @param value value to narrow
/// @return @p value as a @c std::size_t
/// @pre The value must not overflow when narrowed to @c std::size_t
auto to_size(std::uint64_t value) noexcept -> std::size_t {
  ARENE_PRECONDITION(value <= std::numeric_limits<std::size_t>::max());
  return static_cast<std::size_t>(value);
}

}  // namespace

auto read_property_config() noexcept -> property_config {
  return property_config{
      read_env(required_successes_env_name)
          .transform(parse_uint64_decimal)
          .transform(to_size)
          .value_or(default_required_successes),
      read_env(max_discards_env_name).transform(parse_uint64_decimal).transform(to_size).value_or(default_max_discards),
      read_env(seed_env_name).transform(parse_uint64_decimal).value_or(default_seed),
  };
}

}  // namespace testing
}  // namespace base
}  // namespace arene
