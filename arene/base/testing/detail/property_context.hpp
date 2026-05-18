// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DETAIL_PROPERTY_CONTEXT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DETAIL_PROPERTY_CONTEXT_HPP_

#include "arene/base/optional/optional.hpp"  // IWYU pragma: export
#include "arene/base/testing/property_error.hpp"

namespace arene {
namespace base {
namespace testing {
namespace detail {

/// @brief access the global property-testing outcome context
///
/// Returns a reference to an optional containing the current @c property_error
/// associated with the active property-based test. The context is used by
/// property-assertion macros (e.g. @c ARENE_PROP_ASSERT, @c
/// ARENE_PROP_ASSERT_FALSE) to record the result, source location, and stringified
/// expressions of an errorneous assertion.
///
/// @return reference to the global @c optional<property_error> holding the
/// current property-assertion state
/// @note Access to this reference is not thread safe
auto property_context() noexcept -> optional<property_error>&;

}  // namespace detail
}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_DETAIL_PROPERTY_CONTEXT_HPP_
