// parasoft-suppress ALL "False Positive: there is no executable code on line 1"
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: this header intentionally defines both an implementation
// class and variable template API."

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_CONSTRUCT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_CONSTRUCT_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes/maybe_unused.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace construct_detail {

/// @brief Implementation of Construct function object.
/// @tparam To The type to construct.
template <typename To>
// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters."
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
class construct_fn {
 public:
  /// @brief Construct an object of type To from the given arguments.
  /// @tparam Args The types of the arguments to construct To from.
  /// @param args The arguments to construct To from.
  /// @return To An object of type @c To constructed from the given arguments as if via
  ///         @c To(std::forward<Args>(args)...) .
  template <
      typename... Args,
      base::constraints<std::enable_if_t<std::is_constructible<To, Args&&...>::value>> = nullptr>
  constexpr auto operator()(Args&&... args) const noexcept(std::is_nothrow_constructible<To, Args&&...>::value) -> To {
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a-2 "False positive: C-style cast not used."
    // NOLINTNEXTLINE(google-readability-casting) constructor call, not a c-style cast
    return To(std::forward<Args>(args)...);
    // parasoft-end-suppress AUTOSAR-A5_2_2-a-2
  }
};
// parasoft-end-suppress AUTOSAR-M2_10_1-a-2
// parasoft-end-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters."
}  // namespace construct_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a CERT_CPP-DCL59-a "Per-TU references all refer to the same object"
// NOLINTNEXTLINE(google-build-namespaces) explicitly want per-TU instantiations
namespace {
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) FP, there is no non-const global variable here.
/// @brief Construct an object of type To from the given arguments.
/// @tparam To The type of object to construct.
/// @copydoc construct_detail::construct_fn::operator()
template <typename To>
ARENE_MAYBE_UNUSED constexpr auto const& construct = construct_detail::construct_fn<To>{};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace
// parasoft-end-suppress AUTOSAR-M7_3_3-a CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A2_8_1-a-2
// parasoft-end-suppress CERT_C-EXP37-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_CONSTRUCT_HPP_
