// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONSTRAINTS_CONSTRAINTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONSTRAINTS_CONSTRAINTS_HPP_

// IWYU pragma: private, include "arene/base/constraints.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {
namespace constraints_impl {

/// @brief Implementation-detail to force arguments of @c Constraints to be a part of
/// the mangling of function templates for which they are requirements.
template <class...>
struct constraints {
  /// @brief The type that is actually used for the @c constraints template parameters
  using type = decltype(nullptr);
};

}  // namespace constraints_impl

// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: the typedef *is* preceded by a comment with @brief"
/// @brief Helper used to express template constraints in a consistent way using @c std::enable_if_t.
///
/// Use with @c std::enable_if_t to group N short-circuited constraints without N top-level template
/// parameters and without @c std::conjunction . This preserves error
/// information while also minimizing extraneous template parameters. If we
/// eventually move all users to C++20, we can remove this and use
/// language-level concepts.
///
/// Usage Example:
/// @snippet docs/examples/constraints_examples.cpp contrained_function_example
///
/// @note @c constraints is an alias to @c std::nullptr_t for two reasons.
///   1) It is unlikely that someone invoking the function-template will
///      accidentally pass an explicit "nullptr" as a template argument the
///      place that the constraints appear in the template parameter list.
///   2) There is only one possible value for a @c std::nullptr_t argument, so
///      the choice for default argument value being @c nullptr is not
///      subjective.
///
/// Users should prefer using @c constraints to form the template parameter
/// kind as in the example, as opposed to using its result as a default argument
/// for a class template parameter. This is so that constraints in overload sets
/// do not conflict with each other as if redeclarations that differ only by
/// default arguments.
template <class... EnableIfs>
using constraints = typename constraints_impl::constraints<EnableIfs...>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONSTRAINTS_CONSTRAINTS_HPP_
