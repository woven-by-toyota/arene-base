// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_ARE_EXCEPTIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_ARE_EXCEPTIONS_ENABLED_HPP_

///
/// @file are_exceptions_enabled.hpp
/// @brief Provides a trait for programmatically querying if exceptions are enabled.
///

// IWYU pragma: private, include "arene/base/detail/exceptions.hpp"
// IWYU pragma: friend "arene/base/detail/exceptions/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
#include "arene/base/detail/exceptions/are_exceptions_enabled_enabled.hpp"  // IWYU pragma: export
#else
#include "arene/base/detail/exceptions/are_exceptions_enabled_disabled.hpp"  // IWYU pragma: export
#endif
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace detail {

namespace are_exceptions_enabled_detail {
/// @brief Trait for testing if exceptions are enabled.
/// @return true if @c ARENE_EXCEPTIONS_ENABLED is defined, otherwise false.
template <typename = std::nullptr_t>
extern constexpr bool are_exceptions_enabled_v{are_exceptions_enabled::value};
}  // namespace are_exceptions_enabled_detail

/// @brief Trait for testing if exceptions are enabled.
/// @return true if @c ARENE_EXCEPTIONS_ENABLED is defined, otherwise false.
// parasoft-begin-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"
ARENE_MAYBE_UNUSED static constexpr bool const&
    are_exceptions_enabled_v{are_exceptions_enabled_detail::are_exceptions_enabled_v<>};
// parasoft-end-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_ARE_EXCEPTIONS_ENABLED_HPP_
