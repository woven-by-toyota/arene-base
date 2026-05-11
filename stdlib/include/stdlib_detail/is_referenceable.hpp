// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_REFERENCEABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_REFERENCEABLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/is_object.hpp"
#include "stdlib/include/stdlib_detail/is_reference.hpp"

namespace std {
namespace internal {

/// @brief Internal type trait to check if a type is a function type without a cv-qualifier or ref-qualifier.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_function_type_without_cvref_v = false;

/// @brief Internal type trait to check if a type is a function type without a cv-qualifier or ref-qualifier.
/// @tparam Result The result of the function type
/// @tparam Args The arguments of the function type
template <typename Result, typename... Args>
extern constexpr bool is_function_type_without_cvref_v<Result(Args...)> = true;

/// @brief Internal type trait to check if a type is a referenceable type
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_referenceable_v =
    is_object_v<Type> || is_reference_v<Type> || is_function_type_without_cvref_v<Type>;
}  // namespace internal
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_REFERENCEABLE_HPP_
