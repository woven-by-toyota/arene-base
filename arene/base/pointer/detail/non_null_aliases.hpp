// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_NON_NULL_ALIASES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_NON_NULL_ALIASES_HPP_

// IWYU pragma: private, include "arene/base/pointer.hpp"

#include <memory>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// forward declaration
template <typename P>
class non_null;

///
/// @brief Alias for a @c non_null which holds a @c std::shared_ptr
///
/// @tparam T The type of the pointed to element
///
template <typename T>
using non_null_shared_ptr = non_null<std::shared_ptr<T>>;

///
/// @brief A version of @c std::make_shared which returns @c non_null_shared_ptr<T>
///
/// @tparam T The pointed-to type
/// @tparam ArgTypes Types of the args to be forwarded to the constructor of @c T
/// @param args Arguments to be forwarded to the constructor of @c T
/// @return non_null_unique_ptr<T> A @c non_null_shared_ptr<T> which has been created by a @c std::shared_ptr<T>
///         produced as if by calling @c std::make_shared
///
template <typename T, typename... ArgTypes>
ARENE_NODISCARD constexpr auto make_shared(ArgTypes&&... args) -> non_null_shared_ptr<T> {
  return non_null_shared_ptr<T>(std::make_shared<T>(std::forward<ArgTypes>(args)...));
}

///
/// @brief Alias for a @c non_null which holds a @c std::unique_ptr
///
/// @tparam T The type of the pointed to element
/// @tparam Deleter The deleter for the element
///
template <typename T, typename Deleter = std::default_delete<T>>
using non_null_unique_ptr = non_null<std::unique_ptr<T, Deleter>>;

///
/// @brief A version of @c std::make_unique which returns @c non_null_unique_ptr<T>
///
/// @tparam T The pointed-to type
/// @tparam ArgTypes Types of the args to be forwarded to the constructor of @c T
/// @param args Arguments to be forwarded to the constructor of @c T
/// @return non_null_unique_ptr<T> A @c non_null_unique_ptr<T> which has been created by a @c std::unique_ptr<T>
///         produced as if by calling @c std::make_unique
///
template <typename T, typename... ArgTypes>
ARENE_NODISCARD constexpr auto make_unique(ArgTypes&&... args) -> non_null_unique_ptr<T> {
  return non_null_unique_ptr<T>(std::make_unique<T>(std::forward<ArgTypes>(args)...));
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_NON_NULL_ALIASES_HPP_
