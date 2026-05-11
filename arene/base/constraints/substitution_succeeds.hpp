// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONSTRAINTS_SUBSTITUTION_SUCCEEDS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONSTRAINTS_SUBSTITUTION_SUCCEEDS_HPP_

// IWYU pragma: private, include "arene/base/constraints.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: A '@brief' tag is present"
/// @brief An alias for @c void that accepts any number of type parameters, used for substitution checks
template <class...>
using void_t = void;
// parasoft-end-suppress AUTOSAR-A2_7_3-a

namespace substitution_succeeds_impl {

/// @brief A helper @c substitution_succeeds template used for checking if substitution
/// into a template alias succeeds. This turns substitution successes and
/// failures into compile-time @c true and @c false values for testing, similar
/// to the "detection idiom" of library fundamentals TS 2
/// (https://wg21.link/n4806). The primary usage of this facilitiy is for tests
/// to make sure that code specifies its template constraints correctly.
///
/// @c substitution_succeeds_impl is an implementation-detail of
/// @c substitution_succeeds. The default definition is picked when substitution
/// into @c Template with @c P... fails.
///
///
template <class, template <class...> class Template, class... P>
extern constexpr bool substitution_succeeds = false;

/// @brief This partial specialization is picked when substitution into @c Template
/// with
/// @c P... succeeds.
///
///
template <template <class...> class Template, class... P>
extern constexpr bool substitution_succeeds<void_t<Template<P...>>, Template, P...> = true;
}  // namespace substitution_succeeds_impl

/// @brief Determines if a template can be instantiated successfully, similar to C++20's @c requires clause.
/// @return true if @c Template<P...> successfully undergoes substitution when in a SFINAE context, otherwise @c false.
///
/// This is generally used to help emulate "concepts" in a pattern such as the following example which detects if a type
/// has a member function named @c foo() :
/// @snippet docs/examples/constraints_examples.cpp substitution_succeeds_usage_example
///
template <template <class...> class Template, class... P>
extern constexpr bool substitution_succeeds = substitution_succeeds_impl::substitution_succeeds<void, Template, P...>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONSTRAINTS_SUBSTITUTION_SUCCEEDS_HPP_
