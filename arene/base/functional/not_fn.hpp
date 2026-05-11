// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_NOT_FN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_NOT_FN_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/functional/perfect_forward_call_wrapper.hpp"
#include "arene/base/stdlib_choice/decay.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace not_fn_detail {

/// @brief Implementation helper for @c arene::base::not_fn intended to be used as a policy for
///        @c arene::base::functional_detail::perfect_forwarding_call_wrapper .
class not_fn_policy {
 public:
  // parasoft-begin-suppress CERT_C-EXP37-a "BoundArgsTupleT is unnamed as it is unused."
  /// @brief Invokes the provided function as expression-equivalent to @c !func(call_args...) .
  ///
  /// @tparam FuncT The type of the callable to invoke.
  /// @tparam BoundArgsTupleT The type of a tuple holding the bound arguments. Must be empty.
  /// @tparam CallArgsT The types of the call arguments to invoke the callable with.
  /// @param func The invocable to invoke.
  /// @param call_args The set of call arguments to pass to the invocable.
  /// @return bool Expression-equivalent to @c !func(call_args...) .
  template <typename FuncT, typename BoundArgsTupleT, typename... CallArgsT>
  constexpr auto operator()(FuncT&& func, BoundArgsTupleT&&, CallArgsT&&... call_args) const
      noexcept((noexcept(!::arene::base::invoke(std::forward<FuncT>(func), std::forward<CallArgsT>(call_args)...))))
          -> decltype(!::arene::base::invoke(std::forward<FuncT>(func), std::forward<CallArgsT>(call_args)...)) {
    return !::arene::base::invoke(std::forward<FuncT>(func), std::forward<CallArgsT>(call_args)...);
  }
  // parasoft-end-suppress CERT_C-EXP37-a "BoundArgsTupleT is unnamed as it is unused."
};

/// @brief Simplifies correctly setting up the call wrapper and policy.
///
/// @tparam BoundFunc The type of the function to bind, post decaying.
/// @note This needs to be a struct rather than just a type alias or else gcc crashes on all versions between 8 and 12.
template <typename BoundFunc>
class not_fn_t : public functional_detail::perfect_forward_call_wrapper<not_fn_policy, BoundFunc> {
 public:
  using functional_detail::perfect_forward_call_wrapper<not_fn_policy, BoundFunc>::perfect_forward_call_wrapper;
};

/// @brief Produces a call wrapper which inverts the result of an N-arity predicate .
class not_fn_impl {
 public:
  /// @brief Produces a call wrapper which inverts the result of an N-arity predicate .
  ///
  /// @tparam BoundFunc The type of the callable to create a wrapper for. Must satisfy
  //          @c std::is_constructible<std::decay_t<BoundFunc>,BoundFunc> and
  //          @c std::is_move_constructible<std::decay_t<BoundFunc>> or else the program is ill-formed.
  /// @param bound_func The callable to create a wrapper for. It will be perfect-forwarded into the call wrapper.
  /// @return An implementation defined callable object whose call operator consumes a set of arguments @c call_args .
  ///         <c> not_fn(bound_func)(call_args...) </c> is expression-equivalent to
  ///         <c> !invoke(bound_func, call_args...) </c> . The @c noexcept -ness of @c bound_func is
  ///         preserved by the call operator of the returned object. The call operator does not participate in overload
  ///         resolution if @c !bound_func(call_args...) is not well formed.
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: there is no function pointer decay/conversion."
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: static cannot be applied, this is not a member function."
  template <typename BoundFunc>
  constexpr auto operator()(BoundFunc&& bound_func) const
      noexcept(std::is_nothrow_constructible<std::decay_t<BoundFunc>, BoundFunc>::value)
          -> not_fn_detail::not_fn_t<std::decay_t<BoundFunc>> {
    static_assert(
        std::is_constructible<std::decay_t<BoundFunc>, BoundFunc&&>::value,
        "Bound function must be constructible from input."
    );
    static_assert(
        std::is_move_constructible<std::decay_t<BoundFunc>>::value,
        "Bound function must be move-constructible."
    );
    return not_fn_detail::not_fn_t<std::decay_t<BoundFunc>>{std::forward<BoundFunc>(bound_func)};
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a "False positive: static cannot be applied, this is not a member function."
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: there is no function pointer decay/conversion."
};

}  // namespace not_fn_detail

/// @def arene::base::not_fn
/// @copydoc arene::base::not_fn_detail::not_fn_impl::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(not_fn_detail::not_fn_impl, not_fn);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_NOT_FN_HPP_
