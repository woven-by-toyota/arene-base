// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_HPP_

// IWYU pragma: private, include "arene/base/detail/exceptions.hpp"
// IWYU pragma: friend "arene/base/detail/exceptions/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
#include "arene/base/detail/exceptions/mixin_at_exceptions_enabled.hpp"
#else
#include "arene/base/detail/exceptions/mixin_at_exceptions_disabled.hpp"
#endif

namespace arene {
namespace base {
namespace detail {

///
/// @brief A CRTP mixin class for conditionally injecting a throwing bounds-checked @c at() member function.
///
/// @tparam ContainerType the type of the container class to mix into. Must provide a member function @c size() which is
/// comparable to @c IndexType , and must provide an @c operator[] overload which consumes @c IndexType .
/// @tparam IndexType the type of the index used for the @c at() function.
///
/// If @c are_exceptions_enabled_v is true, this mixin provides a member function
/// @c at(IndexType) that performs bounds checking and throws @c std::out_of_range if the index is out of range.
/// Otherwise, it is a no-op. This allows container-like classes to conditionally provide @c at() in a manner that works
/// with @c -fno-exceptions .
template <typename ContainerType, typename IndexType>
using mixin_at = mixin_at_impl<ContainerType, IndexType>;

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_MIXIN_AT_HPP_
