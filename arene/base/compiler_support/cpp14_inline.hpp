// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_CPP14_INLINE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_CPP14_INLINE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {
namespace detail {

/// @brief A helper for C++14 inline variable emulation.
///
/// @tparam ObjectT The type of the object to create.
template <typename ObjectT>
struct cpp14_inline_static_const {
  /// @brief The instantiated object.
  static constexpr ObjectT value{};
};

template <typename ObjectT>
constexpr ObjectT cpp14_inline_static_const<ObjectT>::value;

}  // namespace detail
}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "This macro handles replacement depending on C++ version"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-M16_0_6-a "'type' and 'name' cannot be parenthesized in these contexts"
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."

// parasoft-begin-suppress AUTOSAR-A2_7_2-a "False positive: no commented-out code, this is documentation"
/// @def ARENE_CPP14_INLINE_VARIABLE
/// @brief Emulates C++17's inline variable declarations for function objects.
///
/// @param type The type of the function object to instantiate
/// @param name The name the instantiated object should have in the containing namespace.
///
/// This macro is used to correctly declare the instantiation of function objects in a way compatible with both C++14
/// and >= C++17 contexts, inspired by Eric Niebler's original paper here:
/// https://ericniebler.com/2014/10/21/customization-point-design-in-c11-and-beyond/
///
/// In a <= C++17 context, the type is simply declared as <c> inline constexpr type name{} </c>. In C++14 contexts,
/// where @c inline variable declarations are not permitted, the same effect is achieved by indirecting through the
/// @c arene::base::detail::cpp14_inline_static_const helper template. The usage looks like so:
///
/// \snippet docs/examples/compiler_support_examples.cpp cpp14_inline_variable_fo
///
#if ARENE_IS_ON(ARENE_STD_INLINE_VARIABLES)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) This cannot be a constexpr function, it must be a macro.
#define ARENE_CPP14_INLINE_VARIABLE(type, name) \
  ARENE_MAYBE_UNUSED inline constexpr type name {}
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) This cannot be a constexpr function, it must be a macro.
#define ARENE_CPP14_INLINE_VARIABLE(type, name)                                                            \
  namespace { /* NOLINT(google-build-namespaces) explicitly want per-TU instantiations*/                   \
  /* NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, bugprone-macro-parentheses) */     \
  ARENE_MAYBE_UNUSED constexpr auto& name = ::arene::base::detail::cpp14_inline_static_const<type>::value; \
  }                                                                                                        \
  ARENE_REQUIRE_SEMICOLON
#endif

// parasoft-end-suppress AUTOSAR-A16_0_1-d "This macro handles replacement depending on C++ version"
// parasoft-end-suppress AUTOSAR-A16_0_1-a "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-end-suppress AUTOSAR-M16_0_6-a "'type' and 'name' cannot be parenthesized in these contexts"
// parasoft-end-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-end-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_CPP14_INLINE_HPP_
