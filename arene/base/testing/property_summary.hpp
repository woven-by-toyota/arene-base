// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_SUMMARY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_SUMMARY_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/property_error.hpp"
// IWYU pragma: no_include "testlibs/minitest/assertion_strings.hpp"
// IWYU pragma: no_include "testlibs/minitest/source_location.hpp"

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

  /// @brief stream a human-readable report of a @c property_summary
  /// @tparam Ostream output stream type supporting @c operator<< for strings,
  ///   integers, and booleans
  /// @param out destination stream
  /// @param summary the summary to format
  /// @return reference to @c out, to support chaining
  ///
  /// @attention Output formatting may change at any time. If consistent output
  /// formatting is needed, please implement a newtype formatter.
  template <class Ostream, constraints<typename Ostream::char_type, typename Ostream::traits_type> = nullptr>
  friend constexpr auto operator<<(Ostream& out, property_summary const& summary) -> Ostream& {
    out                                                       //
        << "PROPERTY " << (summary.failed ? "FAIL" : "PASS")  //
        << "\nseed: " << summary.seed                         //
        << "\nsuccesses: " << summary.successes               //
        << "\ndiscards: " << summary.discards;

    if (summary.failed) {
      if (summary.first_failure.has_value()) {
        auto const& error = *summary.first_failure;
        out << "\nproperty not satisfied [" << error.location.file << ":" << error.location.line  //
            << "]\nARENE_PROP_ASSERT(" << error.strings.arg1 << ")";
      } else {
        out << "\nreached max discards";
      }
    }

    return out;
  }
};

}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_PROPERTY_SUMMARY_HPP_
