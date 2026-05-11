// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_OPTIONAL_RESETTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_OPTIONAL_RESETTER_HPP_

// IWYU pragma: private, include "arene/base/optional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "False Positive: header exports optional, used below"
#include "arene/base/optional/optional.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {

// IWYU pragma: begin_keep
template <typename T>
class optional;
// IWYU pragma: end_keep

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: '@brief' tags are present for all declarations"
/// @brief A simple class that resets an @c optional value in its destructor, unless dismissed.
/// @tparam Value the type of the @c optional's data
template <typename Value>
class optional_resetter {
  /// @brief The optional to reset
  optional<Value>* op_;

 public:
  // parasoft-begin-suppress AUTOSAR-A2_7_3-b "False positive: An '@param' tag is present for 'opt'"
  /// @brief Construct an instance and specify the @c optional object to reset
  /// @param opt The optional to reset
  explicit optional_resetter(optional<Value>& opt) noexcept
      : op_(&opt) {}
  // parasoft-end-suppress AUTOSAR-A2_7_3-b

  /// @brief Not copyable
  optional_resetter(optional_resetter const&) = delete;
  /// @brief Not copyable
  auto operator=(optional_resetter const&) -> optional_resetter& = delete;
  /// @brief Not movable
  optional_resetter(optional_resetter&&) = delete;
  /// @brief Not movable
  auto operator=(optional_resetter&&) -> optional_resetter& = delete;
  /// @brief Reset the @c optional if not dismissed
  ~optional_resetter() {
    if (op_ != nullptr) {
      op_->reset();
    }
  }
  /// @brief Dismiss the resetter, so it no longer resets the @c optional in the
  /// destructor
  void dismiss() noexcept { op_ = nullptr; }
};
// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_OPTIONAL_RESETTER_HPP_
